import os
import time

import mindspore as ms
import mindspore.ops.operations.kungfu_comm_ops as kfops
import numpy as np
from kungfu.python import current_rank, propose_new_size
from mindspore.train.serialization import save_checkpoint


class KungFuSummaryCallback(ms.train.callback.Callback):
    def __init__(self, summary_path):
        super().__init__()

        self._summary_path = summary_path

    def begin(self, run_context):
        with open(self._summary_path, "w") as summary_file:
            summary_file.write("{},{}\n".format("step", "loss"))

    def step_end(self, run_context):
        cb_params = run_context.original_args()
        step = cb_params.cur_step_num
        output = cb_params.net_outputs
        loss = np.mean(output[0].asnumpy())

        with open(self._summary_path, "a") as summary_file:
            summary_file.write("{},{}\n".format(step, loss))


class CheckpointCallback(ms.train.callback.Callback):
    def __init__(self, elastic_state, model, path):
        self._model = model
        self._path = path
        self._elastic_state = elastic_state
        self._every_progress = 1600

    def save_checkpoint(self):
        progress = self._elastic_state._progress
        rank = current_rank()
        ckpt_name = "model-{}-{}.ckpt".format(rank, progress)
        if progress % self._every_progress == 0 and progress > 0:
            save_checkpoint(self._model.train_network,
                            os.path.join(self._path, ckpt_name))


    def step_end(self, run_context):
        self.save_checkpoint()

    def end(self, run_context):
        self.save_checkpoint()


def write_checkpoint(model, path, rank, step):
    ckpt_name = "model-{}-{}.ckpt".format(rank, step)
    save_checkpoint(model.train_network,
                    os.path.join(path, ckpt_name))


class DebugCallback(ms.train.callback.Callback):
    def __init__(self, model):
        self._model = model
        self._first_time = True

    def check_for_change(self):
        changed, detached = kfpy.resize()
        return changed

    def step_begin(self, run_context):
        rank = kfpy.current_local_rank()
        size = kfpy.current_cluster_size()
        cb_params = run_context.original_args()
        step = cb_params.cur_step_num
        print("ElasticCallback step_begin {}".format(step))

        if size == 2 and self._first_time:
            self._first_time = False
            write_checkpoint(self._model, "./checkpoint", rank, step)


class DebugTwoCallback(ms.train.callback.Callback):
    def __init__(self, model):
        self._model = model

    def step_begin(self, run_context):
        cb_params = run_context.original_args()
        step = cb_params.cur_step_num

        params = list(self._model.train_network.get_parameters())
        for param in params:
            if param.name == "global_step":
                print("step {}, global step {}".format(step, param.asnumpy()))


def ckpt(es):
    return 'progress-%010d.log' % (es._progress)

def save_progress(es, progress):
    with open(ckpt(es), 'w') as f:
        f.write('%d\n' % (progress))

class ElasticScheduleCallback(ms.train.callback.Callback):
    def __init__(self, es, schedule, model):
        self._es = es
        self._schedule = schedule
        self._rank = current_rank()

        if self._rank == 0:
            print('starting from progress %d' % (self._es._progress))

        self._proc_start = int(os.getenv('KUNGFU_PROC_START_TIMESTAMP'))
        self._local_step = 0

        self._model = model

    def step_begin(self, run_context):
        if self._rank == 0:
            print('running progress %d' % (self._es._progress))

        if self._rank == 0 and self._local_step == 0:
            d = time.time() - self._proc_start
            print('first step BEGIN after reload took %.fs' % (d))

        self._step_begin_ts = time.time()

    def step_end(self, run_context):
        progress = self._es._progress
        step_took = time.time() - self._step_begin_ts

        self._local_step += 1
        if self._rank == 0:
            if self._local_step == 1:
                d = time.time() - self._proc_start
                print('first step END after reload took %.fs' % (d))
            print('progress %d took %.fs' % (progress, step_took))

        if self._es._progress in self._schedule:
            if current_rank() == 0:
                new_size = self._schedule[progress]
                propose_new_size(new_size)

                save_checkpoint(self._model.train_network,
                                "./checkpoint/model.ckpt")

    def end(self, run_context):
        progress = self._es._progress
        if self._rank == 0:
            save_progress(self._es, progress)
            print('stopping at progress %d' % (progress))

