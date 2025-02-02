#!/bin/bash

export CUDA_VISIBLE_DEVICES=2,3

RANK_SIZE=2
EPOCH_SIZE=1
DATA_DIR="/data/squad1/train.tf_record"
SCHEMA_DIR="/data/squad1/squad_schema.json"


. /home/marcel/Elasticity/Repository/kungfu-mindspore/ld_library_path.sh
export LD_LIBRARY_PATH=$(ld_library_path /home/marcel/Elasticity/Repository/kungfu-mindspore/mindspore)

/home/marcel/KungFu/kungfu/bin/kungfu-run \
    -np $RANK_SIZE \
    -logfile kungfu-run.log \
    -logdir ./log \
    -port-range 10500-11000 \
    python run_squad_kungfu.py  \
        --device_target="GPU" \
        --distribute="true" \
        --do_train="true" \
        --do_eval="true" \
        --device_id=0 \
        --epoch_num=${EPOCH_SIZE} \
        --num_class=2 \
        --train_data_shuffle="true" \
        --eval_data_shuffle="false" \
        --train_batch_size=8 \
        --eval_batch_size=1 \
        --vocab_file_path="/data/bert/bert_uncased_L-12_H-768_A-12/vocab.txt" \
        --save_finetune_checkpoint_path="./checkpoint" \
        --load_pretrain_checkpoint_path="/data/bert/bert_base_squad.ckpt" \
        --train_data_file_path=${DATA_DIR} \
        --eval_json_path="/data/squad1/dev-v1.1.json" \
        --schema_file_path=${SCHEMA_DIR} > squad.log 2>&1
