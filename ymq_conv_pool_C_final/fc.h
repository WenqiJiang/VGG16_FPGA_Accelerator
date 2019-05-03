void fc(float* input_feature_map, float* bias, float* kernel, float* output_feature_map,
        int fc_output_size, int fc_input_size, int fc_batch_size);
void argmax(float* input, int* result, int sm_batch_size, int sm_class_size);