#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "conv_forward.h"
#include "max_pool.h"
#include "relu.h"
#include "fc.h"
#include "utils.h"

#define IMG_DIM1 1
#define IMG_DIM2 224
#define IMG_DIM3 224
#define IMG_DIM4 3

#define POOL_SIZE 2
#define POOL_STRIDE 2
#define CONV_STRIDE 1
#define PAD 1
#define BATCH 1

#define FIL1_N 64
#define FIL1_H 3
#define FIL1_W 3
#define FIL1_C 3

#define FIL2_N 64
#define FIL2_H 3
#define FIL2_W 3
#define FIL2_C 64

//max_pool

#define FIL3_N 128
#define FIL3_H 3
#define FIL3_W 3
#define FIL3_C 64

#define FIL4_N 128
#define FIL4_H 3
#define FIL4_W 3
#define FIL4_C 128

//max_pool

#define FIL5_N 256
#define FIL5_H 3
#define FIL5_W 3
#define FIL5_C 128

#define FIL6_N 256
#define FIL6_H 3
#define FIL6_W 3
#define FIL6_C 256

#define FIL7_N 256
#define FIL7_H 3
#define FIL7_W 3
#define FIL7_C 256

//max_pool

#define FIL8_N 512
#define FIL8_H 3
#define FIL8_W 3
#define FIL8_C 256

#define FIL9_N 512
#define FIL9_H 3
#define FIL9_W 3
#define FIL9_C 512

#define FIL10_N 512
#define FIL10_H 3
#define FIL10_W 3
#define FIL10_C 512

//max_pool

#define FIL11_N 512
#define FIL11_H 3
#define FIL11_W 3
#define FIL11_C 512

#define FIL12_N 512
#define FIL12_H 3
#define FIL12_W 3
#define FIL12_C 512

#define FIL13_N 512
#define FIL13_H 3
#define FIL13_W 3
#define FIL13_C 512

//max_pool

//output dimensions
#define INPUT_B 1
#define INPUT_H 224
#define INPUT_W 224
#define INPUT_C 3

#define CONV1_B 1
#define CONV1_H 224
#define CONV1_W 224
#define CONV1_C 64

#define CONV2_B 1
#define CONV2_H 224
#define CONV2_W 224 
#define CONV2_C 64

#define POOL_1_H 112
#define POOL_1_W 112

#define CONV3_B 1
#define CONV3_H 112
#define CONV3_W 112
#define CONV3_C 128

#define CONV4_B 1
#define CONV4_H 112
#define CONV4_W 112
#define CONV4_C 128

#define POOL_2_H 56
#define POOL_2_W 56

#define CONV5_B 1
#define CONV5_H 56
#define CONV5_W 56
#define CONV5_C 256

#define CONV6_B 1
#define CONV6_H 56
#define CONV6_W 56
#define CONV6_C 256

#define CONV7_B 1
#define CONV7_H 56
#define CONV7_W 56
#define CONV7_C 256

#define POOL_3_H 28
#define POOL_3_W 28

#define CONV8_B 1
#define CONV8_H 28
#define CONV8_W 28
#define CONV8_C 512

#define CONV9_B 1
#define CONV9_H 28
#define CONV9_W 28
#define CONV9_C 512

#define CONV10_B 1
#define CONV10_H 28
#define CONV10_W 28
#define CONV10_C 512

#define POOL_4_H 14
#define POOL_4_W 14

#define CONV11_B 1
#define CONV11_H 14
#define CONV11_W 14
#define CONV11_C 512

#define CONV12_B 1
#define CONV12_H 14
#define CONV12_W 14
#define CONV12_C 512

#define CONV13_B 1
#define CONV13_H 14
#define CONV13_W 14
#define CONV13_C 512

#define POOL_5_H 7
#define POOL_5_W 7

// FC layer
#define FC_INPUT_SIZE 25088
#define FC_OUTPUT_SIZE 1000
#define FC1_DIM1 25088
#define FC1_DIM2 4096
#define FC2_DIM1 4096
#define FC2_DIM2 4096
#define FC3_DIM1 4096
#define FC3_DIM2 1000

void show(float* A, int dim1, int dim2, int dim3, int dim4){
    for (int d1 = 0; d1 < dim1; d1++){
        for (int d2 = 0; d2 < dim2; d2++){
            for (int d3 = 0; d3 < dim3; d3++){
                for (int d4 = 0; d4 < dim4; d4++){
                    printf("%f\n ", A[dim4 * dim3 * dim2 * d1 + dim4 * dim3 * d2 + dim4 * d3 + d4]);
                }
            }
        }
    }
}
int main(){
    printf("hello!\n");
    
    // show(b1,  FIL1_N, 1,1,1);
    // block 1
    // output, input, weights, bias, input's 4 dimensions, weight's 4 dimensions
    float* input_img = (float*) malloc(sizeof(float) * IMG_DIM1 * IMG_DIM2 * IMG_DIM3 * IMG_DIM4);
    load_data("persian_cat.txt", input_img, IMG_DIM1 * IMG_DIM2 * IMG_DIM3 * IMG_DIM4);
    float* w1  = (float*) malloc(sizeof(float) * FIL1_N  * FIL1_H  * FIL1_W  * FIL1_C);
    float* b1  = (float*) malloc(sizeof(float) * FIL1_N);
    load_data("weights_txt/block1_conv1_kernel.txt", w1,  FIL1_N   * FIL1_H   * FIL1_W   * FIL1_C);
    load_data("weights_txt/block1_conv1_bias.txt", b1,  FIL1_N);
    float* conv1_output = (float*) malloc(sizeof(float) * BATCH * CONV1_H * CONV1_W * CONV1_C);
    float* conv1_output_relu = (float*) malloc(sizeof(float) * CONV1_B * CONV1_H * CONV1_W * CONV1_C);

    clock_t conv_start = clock();
    float conv_clock = 0;

    conv_forward(conv1_output, input_img, w1, b1, PAD, CONV_STRIDE, 
    IMG_DIM1, IMG_DIM2, IMG_DIM3, IMG_DIM4, 
    FIL1_N, FIL1_H, FIL1_W, FIL1_C);
    conv_clock += clock() - conv_start;
    free(input_img);
    free(w1);
    free(b1);
    printf("conv1 ended\n");
    relu_forward(conv1_output_relu, conv1_output, BATCH, CONV1_H, CONV1_W, CONV1_C);
    free(conv1_output);
    printf("relu1 ended\n");
    // show(conv1_output_relu, BATCH, CONV1_H, CONV1_W, CONV1_C);

    float* w2  = (float*) malloc(sizeof(float) * FIL2_N  * FIL2_H  * FIL2_W  * FIL2_C);
    float* b2  = (float*) malloc(sizeof(float) * FIL2_N);
    load_data("weights_txt/block1_conv2_kernel.txt", w2,  FIL2_N   * FIL2_H   * FIL2_W   * FIL2_C);
    load_data("weights_txt/block1_conv2_bias.txt", b2,  FIL2_N);
    float* conv2_output = (float*) malloc(sizeof(float) * BATCH * CONV2_H * CONV2_W * CONV2_C);
    float* conv2_output_relu = (float*) malloc(sizeof(float) * BATCH * CONV2_H * CONV2_W * CONV2_C);
    conv_start = clock();
    conv_forward(conv2_output, conv1_output_relu, w2, b2, PAD, CONV_STRIDE, 
    BATCH, CONV1_H, CONV1_W, CONV1_C, 
    FIL2_N, FIL2_H, FIL2_W, FIL2_C);
    conv_clock += clock() - conv_start;
    free(conv1_output_relu);
    free(w2);
    free(b2);
    printf("conv2 ended\n");
    relu_forward(conv2_output_relu, conv2_output, BATCH, CONV2_H, CONV2_W, CONV2_C);
    free(conv2_output);
    printf("relu2 ended\n");

    float* pool1_output = (float*) malloc(sizeof(float) * BATCH * POOL_1_H * POOL_1_W * FIL2_N);
    // output, input, input's 4 dimensions
    max_pool_forward(pool1_output, conv2_output_relu, POOL_SIZE, POOL_STRIDE, 
    BATCH, CONV2_H, CONV2_W, CONV2_C);  
    free(conv2_output_relu);
    printf("pool1 ended\n");

    // block 2
    float* w3  = (float*) malloc(sizeof(float) * FIL3_N  * FIL3_H  * FIL3_W  * FIL3_C);
    float* b3  = (float*) malloc(sizeof(float) * FIL3_N);
    load_data("weights_txt/block2_conv1_kernel.txt", w3,  FIL3_N   * FIL3_H   * FIL3_W   * FIL3_C);
    load_data("weights_txt/block2_conv1_bias.txt", b3,  FIL3_N);
    float* conv3_output = (float*) malloc(sizeof(float) * BATCH * CONV3_H * CONV3_W * CONV3_C);
    float* conv3_output_relu = (float*) malloc(sizeof(float) * BATCH * CONV3_H * CONV3_W * CONV3_C);
    conv_start = clock();
    conv_forward(conv3_output, pool1_output, w3, b3, PAD, CONV_STRIDE, 
    BATCH, POOL_1_H, POOL_1_W, FIL2_N,  
    FIL3_N, FIL3_H, FIL3_W, FIL3_C);
    conv_clock += clock() - conv_start;
    free(pool1_output);
    free(w3);
    free(b3);
    printf("conv3 ended\n");
    relu_forward(conv3_output_relu, conv3_output, BATCH, CONV3_H, CONV3_W, CONV3_C);
    free(conv3_output);
    printf("relu3 ended\n");

    float* w4  = (float*) malloc(sizeof(float) * FIL4_N  * FIL4_H  * FIL4_W  * FIL4_C);
    float* b4  = (float*) malloc(sizeof(float) * FIL4_N);
    load_data("weights_txt/block2_conv2_kernel.txt", w4,  FIL4_N   * FIL4_H   * FIL4_W   * FIL4_C);
    load_data("weights_txt/block2_conv2_bias.txt", b4,  FIL4_N);
    float* conv4_output = (float*) malloc(sizeof(float) * BATCH * CONV4_H * CONV4_W * CONV4_C);
    float* conv4_output_relu = (float*) malloc(sizeof(float) * BATCH * CONV4_H * CONV4_W * CONV4_C);
    conv_start = clock();
    conv_forward(conv4_output, conv3_output_relu, w4, b4, PAD, CONV_STRIDE, 
    BATCH, CONV3_H, CONV3_W, CONV3_C, 
    FIL4_N, FIL4_H, FIL4_W, FIL4_C);
    conv_clock += clock() - conv_start;
    free(conv3_output_relu);
    free(w4);
    free(b4);
    printf("conv4 ended\n");
    relu_forward(conv4_output_relu, conv4_output, BATCH, CONV3_H, CONV4_W, CONV4_C);
    free(conv4_output);
    printf("relu4 ended\n");

    float* pool2_output = (float*) malloc(sizeof(float) * BATCH * POOL_2_H * POOL_2_W * FIL4_N);
    max_pool_forward(pool2_output, conv4_output_relu, POOL_SIZE, POOL_STRIDE, 
    BATCH, CONV4_H, CONV4_W, CONV4_C); 
    free(conv4_output_relu);
    printf("pool2 ended\n"); 

    // block 3
    float* w5  = (float*) malloc(sizeof(float) * FIL5_N  * FIL5_H  * FIL5_W  * FIL5_C);
    float* b5  = (float*) malloc(sizeof(float) * FIL5_N);
    load_data("weights_txt/block3_conv1_kernel.txt", w5,  FIL5_N   * FIL5_H   * FIL5_W   * FIL5_C);
    load_data("weights_txt/block3_conv1_bias.txt", b5,  FIL5_N);
    float* conv5_output = (float*) malloc(sizeof(float) * BATCH * CONV5_H * CONV5_W * CONV5_C);
    float* conv5_output_relu = (float*) malloc(sizeof(float) * BATCH * CONV5_H * CONV5_W * CONV5_C);
    conv_start = clock();
    conv_forward(conv5_output, pool2_output, w5, b5, PAD, CONV_STRIDE, 
    BATCH, POOL_2_H, POOL_2_W, FIL4_N,  
    FIL5_N, FIL5_H, FIL5_W, FIL5_C);
    conv_clock += clock() - conv_start;
    free(pool2_output);
    free(w5);
    free(b5);
    printf("conv5 ended\n");
    relu_forward(conv5_output_relu, conv5_output, BATCH, CONV5_H, CONV5_W, CONV5_C);
    free(conv5_output);
    printf("relu5 ended\n");

    float* w6  = (float*) malloc(sizeof(float) * FIL6_N  * FIL6_H  * FIL6_W  * FIL6_C);
    float* b6  = (float*) malloc(sizeof(float) * FIL6_N);
    load_data("weights_txt/block3_conv2_kernel.txt", w6,  FIL6_N   * FIL6_H   * FIL6_W   * FIL6_C);
    load_data("weights_txt/block3_conv2_bias.txt", b6,  FIL6_N);
    float* conv6_output = (float*) malloc(sizeof(float) * BATCH * CONV6_H * CONV6_W * CONV6_C);
    float* conv6_output_relu = (float*) malloc(sizeof(float) * BATCH * CONV6_H * CONV6_W * CONV6_C);
    conv_start = clock();
    conv_forward(conv6_output, conv5_output_relu, w6, b6, PAD, CONV_STRIDE, 
    BATCH, CONV5_H, CONV5_W, CONV5_C, 
    FIL6_N, FIL6_H, FIL6_W, FIL6_C);
    conv_clock += clock() - conv_start;
    free(conv5_output_relu);
    free(w6);
    free(b6);
    printf("conv6 ended\n");
    relu_forward(conv6_output_relu, conv6_output, BATCH, CONV6_H, CONV6_W, CONV6_C);
    free(conv6_output);
    printf("relu6 ended\n");

    float* w7  = (float*) malloc(sizeof(float) * FIL7_N  * FIL7_H  * FIL7_W  * FIL7_C);
    float* b7  = (float*) malloc(sizeof(float) * FIL7_N);
    load_data("weights_txt/block3_conv3_kernel.txt", w7,  FIL7_N   * FIL7_H   * FIL7_W   * FIL7_C);
    load_data("weights_txt/block3_conv3_bias.txt", b7,  FIL7_N);
    float* conv7_output = (float*) malloc(sizeof(float) * BATCH * CONV7_H * CONV7_W * CONV7_C);
    float* conv7_output_relu = (float*) malloc(sizeof(float) * BATCH * CONV7_H * CONV7_W * CONV7_C);
    conv_start = clock();
    conv_forward(conv7_output, conv6_output_relu, w7, b7, PAD, CONV_STRIDE, 
    BATCH, CONV6_H, CONV6_W, CONV6_C, 
    FIL7_N, FIL7_H, FIL7_W, FIL7_C);
    conv_clock += clock() - conv_start;
    free(conv6_output_relu);
    free(w7);
    free(b7);
    printf("conv7 ended\n");
    relu_forward(conv7_output_relu, conv7_output, BATCH, CONV7_H, CONV7_W, CONV7_C);
    free(conv7_output);
    printf("relu7 ended\n");

    float* pool3_output = (float*) malloc(sizeof(float) * BATCH * POOL_3_H * POOL_3_W * FIL7_N);
    max_pool_forward(pool3_output, conv7_output_relu, POOL_SIZE, POOL_STRIDE, 
    BATCH, CONV7_H, CONV7_W, CONV7_C); 
    free(conv7_output_relu);
    printf("pool3 ended\n");  

    // block 4
    float* w8  = (float*) malloc(sizeof(float) * FIL8_N  * FIL8_H  * FIL8_W  * FIL8_C);    
    float* b8  = (float*) malloc(sizeof(float) * FIL8_N);
    load_data("weights_txt/block4_conv1_kernel.txt", w8,  FIL8_N   * FIL8_H   * FIL8_W   * FIL8_C);
    load_data("weights_txt/block4_conv1_bias.txt", b8,  FIL8_N);
    float* conv8_output = (float*) malloc(sizeof(float) * BATCH * CONV8_H * CONV8_W * CONV8_C);
    float* conv8_output_relu = (float*) malloc(sizeof(float) * BATCH * CONV8_H * CONV8_W * CONV8_C);
    conv_start = clock();
    conv_forward(conv8_output, pool3_output, w8, b8, PAD, CONV_STRIDE, 
    BATCH, POOL_3_H, POOL_3_W, FIL7_N,  
    FIL8_N, FIL8_H, FIL8_W, FIL8_C);
    conv_clock += clock() - conv_start;
    free(pool3_output);
    free(w8);
    free(b8);
    printf("conv8 ended\n");
    relu_forward(conv8_output_relu, conv8_output, BATCH, CONV8_H, CONV8_W, CONV8_C);
    free(conv8_output);
    printf("relu8 ended\n");

    float* w9  = (float*) malloc(sizeof(float) * FIL9_N  * FIL9_H  * FIL9_W  * FIL9_C);   
    float* b9  = (float*) malloc(sizeof(float) * FIL9_N);
    load_data("weights_txt/block4_conv2_kernel.txt", w9,  FIL9_N   * FIL9_H   * FIL9_W   * FIL9_C);
    load_data("weights_txt/block4_conv2_bias.txt", b9,  FIL9_N);
    float* conv9_output = (float*) malloc(sizeof(float) * BATCH * CONV9_H * CONV9_W * CONV9_C);
    float* conv9_output_relu = (float*) malloc(sizeof(float) * BATCH * CONV9_H * CONV9_W * CONV9_C);
    conv_start = clock();
    conv_forward(conv9_output, conv8_output_relu, w9, b9, PAD, CONV_STRIDE, 
    BATCH, CONV8_H, CONV8_W, CONV8_C, 
    FIL9_N, FIL9_H, FIL9_W, FIL9_C);
    conv_clock += clock() - conv_start;
    free(conv8_output_relu);
    free(w9);
    free(b9);
    printf("conv9 ended\n");
    relu_forward(conv9_output_relu, conv9_output, BATCH, CONV9_H, CONV9_W, CONV9_C);
    free(conv9_output);
    printf("relu9 ended\n");

    float* w10 = (float*) malloc(sizeof(float) * FIL10_N * FIL10_H * FIL10_W * FIL10_C);    
    float* b10 = (float*) malloc(sizeof(float) * FIL10_N);
    load_data("weights_txt/block4_conv3_kernel.txt", w10, FIL10_N  * FIL10_H  * FIL10_W  * FIL10_C);
    load_data("weights_txt/block4_conv3_bias.txt", b10, FIL10_N);
    float* conv10_output = (float*) malloc(sizeof(float) * BATCH * CONV10_H * CONV10_W * CONV10_C);
    float* conv10_output_relu = (float*) malloc(sizeof(float) * BATCH * CONV10_H * CONV10_W * CONV10_C);
    conv_start = clock();
    conv_forward(conv10_output, conv9_output_relu, w10, b10, PAD, CONV_STRIDE, 
    BATCH, CONV9_H, CONV9_W, CONV9_C, 
    FIL10_N, FIL10_H, FIL10_W, FIL10_C);
    conv_clock += clock() - conv_start;
    free(conv9_output_relu);
    free(w10);
    free(b10);
    printf("conv10 ended\n");
    relu_forward(conv10_output_relu, conv10_output, BATCH, CONV10_H, CONV10_W, CONV10_C);
    free(conv10_output);
    printf("relu10 ended\n");

    float* pool4_output = (float*) malloc(sizeof(float) * BATCH * POOL_4_H * POOL_4_W * FIL10_N);
    max_pool_forward(pool4_output, conv10_output_relu, POOL_SIZE, POOL_STRIDE, 
    BATCH, CONV10_H, CONV10_W, CONV10_C);  
    free(conv10_output_relu);
    printf("pool4 ended\n");  

    // block 5
    float* w11 = (float*) malloc(sizeof(float) * FIL11_N * FIL11_H * FIL11_W * FIL11_C);
    float* b11 = (float*) malloc(sizeof(float) * FIL11_N);
    load_data("weights_txt/block5_conv1_kernel.txt", w11, FIL11_N  * FIL11_H  * FIL11_W  * FIL11_C);
    load_data("weights_txt/block5_conv1_bias.txt", b11, FIL11_N);
    float* conv11_output = (float*) malloc(sizeof(float) * BATCH * CONV11_H * CONV11_W * CONV11_C);
    float* conv11_output_relu = (float*) malloc(sizeof(float) * BATCH * CONV11_H * CONV11_W * CONV11_C);  
    conv_start = clock();
    conv_forward(conv11_output, pool4_output, w11, b11, PAD, CONV_STRIDE, 
    BATCH, POOL_4_H, POOL_4_W, FIL10_N,  
    FIL11_N, FIL11_H, FIL11_W, FIL11_C);
    conv_clock += clock() - conv_start;
    free(pool4_output);
    free(w11);
    free(b11);
    printf("conv11 ended\n");
    relu_forward(conv11_output_relu, conv11_output, BATCH, CONV11_H, CONV11_W, CONV11_C);
    free(conv11_output);
    printf("relu11 ended\n");

    float* w12 = (float*) malloc(sizeof(float) * FIL12_N * FIL12_H * FIL12_W * FIL12_C);
    float* b12 = (float*) malloc(sizeof(float) * FIL12_N);
    load_data("weights_txt/block5_conv2_kernel.txt", w12, FIL12_N  * FIL12_H  * FIL12_W  * FIL12_C);
    load_data("weights_txt/block5_conv2_bias.txt", b12, FIL12_N);
    float* conv12_output = (float*) malloc(sizeof(float) * BATCH * CONV12_H * CONV12_W * CONV12_C);
    float* conv12_output_relu = (float*) malloc(sizeof(float) * BATCH * CONV12_H * CONV12_W * CONV12_C);
    conv_start = clock();
    conv_forward(conv12_output, conv11_output_relu, w12, b12, PAD, CONV_STRIDE, 
    BATCH, CONV11_H, CONV11_W, CONV11_C, 
    FIL12_N, FIL12_H, FIL12_W, FIL12_C);
    conv_clock += clock() - conv_start;
    free(conv11_output_relu);
    free(w12);
    free(b12);
    printf("conv12 ended\n");
    relu_forward(conv12_output_relu, conv12_output, BATCH, CONV12_H, CONV12_W, CONV12_C);
    free(conv12_output);
    printf("relu12 ended\n");

    float* w13 = (float*) malloc(sizeof(float) * FIL13_N * FIL13_H * FIL13_W * FIL13_C);
    float* b13 = (float*) malloc(sizeof(float) * FIL13_N);
    load_data("weights_txt/block5_conv3_kernel.txt", w13, FIL13_N  * FIL13_H  * FIL13_W  * FIL13_C);
    load_data("weights_txt/block5_conv3_bias.txt", b13, FIL13_N);
    float* conv13_output = (float*) malloc(sizeof(float) * BATCH * CONV13_H * CONV13_W * CONV13_C);
    float* conv13_output_relu = (float*) malloc(sizeof(float) * BATCH * CONV13_H * CONV13_W * CONV13_C);
    conv_start = clock();
    conv_forward(conv13_output, conv12_output_relu, w13, b13, PAD, CONV_STRIDE, 
    BATCH, CONV12_H, CONV12_W, CONV12_C, 
    FIL13_N, FIL13_H, FIL13_W, FIL13_C);
    conv_clock += clock() - conv_start;
    free(conv12_output_relu);
    free(w13);
    free(b13);
    printf("conv13 ended\n");
    relu_forward(conv13_output_relu, conv13_output, BATCH, CONV13_H, CONV13_W, CONV13_C);
    free(conv13_output);
    printf("relu13 ended\n");

    float* pool5_output = (float*) malloc(sizeof(float) * BATCH * POOL_5_H * POOL_5_W * FIL13_N);
    max_pool_forward(pool5_output, conv13_output_relu, POOL_SIZE, POOL_STRIDE, 
    BATCH, CONV13_H, CONV13_W, FIL13_N);  
    free(conv13_output_relu);
    printf("pool5 ended\n");

    //show(pool5_output, BATCH, POOL_5_H, POOL_5_W, FIL13_N);
    float conv_time = conv_clock / CLOCKS_PER_SEC;
    // fc layer
    printf("Starting FC Layer:");
    // load_data("fc_input_flatten.txt", input_feature_map, FC_INPUT_SIZE);
    float* fc1_kernel = (float*) malloc(sizeof(float) * FC1_DIM1 * FC1_DIM2);
    float* fc1_bias = (float*) malloc(sizeof(float) * FC1_DIM2);
    float* fc1_feature_map = (float*) malloc(sizeof(float) * FC1_DIM2);
    load_data("weights_txt/fc1_kernel.txt", fc1_kernel, FC1_DIM1 * FC1_DIM2);
    load_data("weights_txt/fc1_bias.txt", fc1_bias, FC1_DIM2);
    clock_t fc_start = clock();
    fc(pool5_output, fc1_bias, fc1_kernel, fc1_feature_map,
        FC1_DIM2, FC1_DIM1, 1);
    free(pool5_output);
    free(fc1_kernel);
    free(fc1_bias);
    float fc_clock = clock() - fc_start;
    
    float* fc2_kernel = (float*) malloc(sizeof(float) * FC2_DIM1 * FC2_DIM2);
    float* fc2_bias = (float*) malloc(sizeof(float) * FC2_DIM2);
    float* fc2_feature_map = (float*) malloc(sizeof(float) * FC2_DIM2);
    load_data("weights_txt/fc2_kernel.txt", fc2_kernel, FC2_DIM1 * FC2_DIM2);
    load_data("weights_txt/fc2_bias.txt", fc2_bias, FC2_DIM2);
    fc_start = clock();
    fc(fc1_feature_map, fc2_bias, fc2_kernel, fc2_feature_map,
        FC2_DIM2, FC2_DIM1, 1);
    free(fc1_feature_map);
    free(fc2_kernel);
    free(fc2_bias);
    fc_clock += clock() - fc_start;

    float* fc3_kernel = (float*) malloc(sizeof(float) * FC3_DIM1 * FC3_DIM2);
    float* fc3_bias = (float*) malloc(sizeof(float) * FC3_DIM2);
    float* output_feature_map = (float*) malloc(sizeof(float) * FC_OUTPUT_SIZE);
    load_data("weights_txt/predictions_kernel.txt", fc3_kernel, FC3_DIM1 * FC3_DIM2);
    load_data("weights_txt/predictions_bias.txt", fc3_bias, FC3_DIM2);
    fc_start = clock();
    fc(fc2_feature_map, fc3_bias, fc3_kernel, output_feature_map,
        FC3_DIM2, FC3_DIM1, 1);
    free(fc2_feature_map);
    free(fc3_kernel);
    free(fc3_bias);
   
   	int result[1];
    argmax(output_feature_map, result, 1, FC_OUTPUT_SIZE);
    clock_t fc_end = clock();
    fc_clock += clock() - fc_start;
    float fc_time = fc_clock / CLOCKS_PER_SEC;
    printf("argmax: \n");
    printf("%d", result[0]);
    
    printf("time consumed:\nconv:\t%f\tfc:\t%f\t\npercentage:\nconv:\t%f\tfc:\t%f",
           conv_time, fc_time, conv_time / (conv_time + fc_time), fc_time / (conv_time + fc_time));

    return 0;
}
