CUDA_VISIBLE_DEVICES=4,5,6,7 python3 train.py  --valid_data data_lmdb_release/validation \
--Transformation None --FeatureExtraction ResNet --SequenceModeling BiLSTM --Prediction Attn --train_data "/lustre/home/shyan/code/EPT-recognition/SCUT-EPT/training"  --batch_size 10 --saved_model "/lustre/home/shyan/code/EPT-recognition/saved_models/None-ResNet-BiLSTM-Attn-Seed1111/iter_26000.pth" 
