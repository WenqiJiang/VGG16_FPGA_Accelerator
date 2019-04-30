#include <stdio.h>
#include <stdlib.h>

#include "fc.h"
#include "utils.h"

#define FC_INPUT_SIZE 25088
#define FC_OUTPUT_SIZE 1000
#define FC1_DIM1 25088
#define FC1_DIM2 4096
#define FC2_DIM1 4096
#define FC2_DIM2 4096
#define FC3_DIM1 4096
#define FC3_DIM2 1000

int main(){
    float* input_feature_map = (float*) malloc(sizeof(float) * FC_INPUT_SIZE);

    float* fc1_kernel = (float*) malloc(sizeof(float) * FC1_DIM1 * FC1_DIM2);
    float* fc1_bias = (float*) malloc(sizeof(float) * FC1_DIM2);
    float* fc1_feature_map = (float*) malloc(sizeof(float) * FC1_DIM2);


    float* fc2_kernel = (float*) malloc(sizeof(float) * FC2_DIM1 * FC2_DIM2);
    float* fc2_bias = (float*) malloc(sizeof(float) * FC2_DIM2);
    float* fc2_feature_map = (float*) malloc(sizeof(float) * FC2_DIM2);

    float* fc3_kernel = (float*) malloc(sizeof(float) * FC3_DIM1 * FC3_DIM2);
    float* fc3_bias = (float*) malloc(sizeof(float) * FC3_DIM2);
    float* output_feature_map = (float*) malloc(sizeof(float) * FC_OUTPUT_SIZE);

    load_data("fc_input_flatten.txt", input_feature_map, FC_INPUT_SIZE);
    load_data("weights_txt/fc1_kernel.txt", fc1_kernel, FC1_DIM1 * FC1_DIM2);
    load_data("weights_txt/fc1_bias.txt", fc1_bias, FC1_DIM2);
    load_data("weights_txt/fc2_kernel.txt", fc2_kernel, FC2_DIM1 * FC2_DIM2);
    load_data("weights_txt/fc2_bias.txt", fc2_bias, FC2_DIM2);
    load_data("weights_txt/predictions_kernel.txt", fc3_kernel, FC3_DIM1 * FC3_DIM2);
    load_data("weights_txt/predictions_bias.txt", fc3_bias, FC3_DIM2);

    fc(input_feature_map, fc1_bias, fc1_kernel, fc1_feature_map,
        FC1_DIM2, FC1_DIM1, 1);
    fc(fc1_feature_map, fc2_bias, fc2_kernel, fc2_feature_map,
        FC2_DIM2, FC2_DIM1, 1);
    fc(fc2_feature_map, fc3_bias, fc3_kernel, output_feature_map,
        FC3_DIM2, FC3_DIM1, 1);
   
   	int result[1];
    argmax(output_feature_map, result, 1, FC_OUTPUT_SIZE);

    printf("%d", result[0]);

    return 0;
}
