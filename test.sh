CUDA_VISIBLE_DEVICES=7 python3 test.py \
--eval_data "/lustre/home/shyan/code/EPT-recognition/SCUT-EPT/testing/EPT" \
--Transformation None --FeatureExtraction ResNet --SequenceModeling BiLSTM --Prediction Attn \
--saved_model "/lustre/home/shyan/code/EPT-recognition/saved_models/None-ResNet-BiLSTM-Attn-Seed1111/iter_160000.pth"

