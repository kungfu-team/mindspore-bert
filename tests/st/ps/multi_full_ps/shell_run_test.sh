#!/bin/bash
# Copyright 2020 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ============================================================================

execute_path=$(pwd)
self_path=$(dirname $0)
export MS_SCHED_NUM=1
DEVICE_TARGET=$1
export MS_WORKER_NUM=$2
export MS_SERVER_NUM=$3
export MS_SCHED_HOST=$4
export MS_SCHED_PORT=$5

export MS_ROLE=MS_SCHED
for((i=0;i<1;i++));
do
  rm -rf ${execute_path}/sched_$i/
  mkdir ${execute_path}/sched_$i/
  cd ${execute_path}/sched_$i/ || exit
  python ${self_path}/../test_multi_full_ps.py --device_target=$DEVICE_TARGET &
done

export MS_ROLE=MS_PSERVER
for((i=0;i<$MS_SERVER_NUM;i++));
do
  rm -rf ${execute_path}/server_$i/
  mkdir ${execute_path}/server_$i/
  cd ${execute_path}/server_$i/ || exit
  export RANK_ID=$i
  export DEVICE_ID=$i
  python ${self_path}/../test_multi_full_ps.py --device_target=$DEVICE_TARGET &
done

export MS_ROLE=MS_WORKER
if [ $DEVICE_TARGET == "Ascend" ];then
for((i=0;i<$MS_WORKER_NUM;i++));
do
  rm -rf ${execute_path}/worker_$i/
  mkdir ${execute_path}/worker_$i/
  cd ${execute_path}/worker_$i/ || exit
  export RANK_ID=$i
  export DEVICE_ID=$i
  python ${self_path}/../test_multi_full_ps.py --device_target=$DEVICE_TARGET &
done
fi
if [ $DEVICE_TARGET == "GPU" ];then
  rm -rf ${execute_path}/worker/
  mkdir ${execute_path}/worker/
  cd ${execute_path}/worker/ || exit
  mpirun -n $MS_WORKER_NUM python ${self_path}/../test_multi_full_ps.py --device_target=$DEVICE_TARGET &
fi

wait $!
exit $?
