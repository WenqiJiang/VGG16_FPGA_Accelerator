#include <stdio.h>
#include <stdlib.h>
#include "conv_forward.h"
#include "max_pool.h"
#include "relu.h"
#include "fc.h"
#include "utils.h"

#define IMG_DIM1 1
#define IMG_DIM2 224
#define IMG_DIM3 224
#define IMG_DIM4 3

#define pool_size 2
#define pool_stride 2
#define conv_stride 1
#define pad 1
#define batch 1
#define output_size 25088 

#define conv1_num_f 64
#define conv1_f_height 3
#define conv1_f_width 3
#define conv1_channels 3

#define conv2_num_f 64
#define conv2_f_height 3
#define conv2_f_width 3
#define conv2_channels 64

//max_pool

#define conv3_num_f 128
#define conv3_f_height 3
#define conv3_f_width 3
#define conv3_channels 64

#define conv4_num_f 128
#define conv4_f_height 3
#define conv4_f_width 3
#define conv4_channels 128

//max_pool

#define conv5_num_f 256
#define conv5_f_height 3
#define conv5_f_width 3
#define conv5_channels 128

#define conv6_num_f 256
#define conv6_f_height 3
#define conv6_f_width 3
#define conv6_channels 256

#define conv7_num_f 256
#define conv7_f_height 3
#define conv7_f_width 3
#define conv7_channels 256

//max_pool

#define conv8_num_f 512
#define conv8_f_height 3
#define conv8_f_width 3
#define conv8_channels 256

#define conv9_num_f 512
#define conv9_f_height 3
#define conv9_f_width 3
#define conv9_channels 512

#define conv10_num_f 512
#define conv10_f_height 3
#define conv10_f_width 3
#define conv10_channels 512

//max_pool

#define conv11_num_f 512
#define conv11_f_height 3
#define conv11_f_width 3
#define conv11_channels 512

#define conv12_num_f 512
#define conv12_f_height 3
#define conv12_f_width 3
#define conv12_channels 512

#define conv13_num_f 512
#define conv13_f_height 3
#define conv13_f_width 3
#define conv13_channels 512

//max_pool

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
    float* input_img = (float*) malloc(sizeof(float) * IMG_DIM1 * IMG_DIM2 * IMG_DIM3 * IMG_DIM4);
    //float* output = (float*) malloc(sizeof(float) * output_size);
    float* w1  = (float*) malloc(sizeof(float) * conv1_num_f  * conv1_f_height  * conv1_f_width  * conv1_channels);
    float* w2  = (float*) malloc(sizeof(float) * conv2_num_f  * conv2_f_height  * conv2_f_width  * conv2_channels);
    float* w3  = (float*) malloc(sizeof(float) * conv3_num_f  * conv3_f_height  * conv3_f_width  * conv3_channels);
    float* w4  = (float*) malloc(sizeof(float) * conv4_num_f  * conv4_f_height  * conv4_f_width  * conv4_channels);
    float* w5  = (float*) malloc(sizeof(float) * conv5_num_f  * conv5_f_height  * conv5_f_width  * conv5_channels);
    float* w6  = (float*) malloc(sizeof(float) * conv6_num_f  * conv6_f_height  * conv6_f_width  * conv6_channels);
    float* w7  = (float*) malloc(sizeof(float) * conv7_num_f  * conv7_f_height  * conv7_f_width  * conv7_channels);
    float* w8  = (float*) malloc(sizeof(float) * conv8_num_f  * conv8_f_height  * conv8_f_width  * conv8_channels);
    float* w9  = (float*) malloc(sizeof(float) * conv9_num_f  * conv9_f_height  * conv9_f_width  * conv9_channels);
    float* w10 = (float*) malloc(sizeof(float) * conv10_num_f * conv10_f_height * conv10_f_width * conv10_channels);
    float* w11 = (float*) malloc(sizeof(float) * conv11_num_f * conv11_f_height * conv11_f_width * conv11_channels);
    float* w12 = (float*) malloc(sizeof(float) * conv12_num_f * conv12_f_height * conv12_f_width * conv12_channels);
    float* w13 = (float*) malloc(sizeof(float) * conv13_num_f * conv13_f_height * conv13_f_width * conv13_channels);
    float* b1  = (float*) malloc(sizeof(float) * conv1_num_f);
    float* b2  = (float*) malloc(sizeof(float) * conv2_num_f);
    float* b3  = (float*) malloc(sizeof(float) * conv3_num_f);
    float* b4  = (float*) malloc(sizeof(float) * conv4_num_f);
    float* b5  = (float*) malloc(sizeof(float) * conv5_num_f);
    float* b6  = (float*) malloc(sizeof(float) * conv6_num_f);
    float* b7  = (float*) malloc(sizeof(float) * conv7_num_f);
    float* b8  = (float*) malloc(sizeof(float) * conv8_num_f);
    float* b9  = (float*) malloc(sizeof(float) * conv9_num_f);
    float* b10 = (float*) malloc(sizeof(float) * conv10_num_f);
    float* b11 = (float*) malloc(sizeof(float) * conv11_num_f);
    float* b12 = (float*) malloc(sizeof(float) * conv12_num_f);
    float* b13 = (float*) malloc(sizeof(float) * conv13_num_f);
    printf("finished weights and bias malloc.\n");

// block 1
    int conv1_new_height = (int)(((IMG_DIM2 - conv1_f_height + 2 * pad) / conv_stride) + 1);
    int conv1_new_width  = (int)(((IMG_DIM3 - conv1_f_width  + 2 * pad) / conv_stride) + 1);
    float* conv1_output = (float*) malloc(sizeof(float) * batch * conv1_new_height * conv1_new_width * conv1_num_f);
    float* conv1_output_relu = (float*) malloc(sizeof(float) * batch * conv1_new_height * conv1_new_width * conv1_num_f);
    int conv2_new_height = (int)(((conv1_new_height - conv2_f_height + 2 * pad) / conv_stride) + 1);
    int conv2_new_width  = (int)(((conv1_new_width  - conv2_f_width + 2 * pad) / conv_stride) + 1);
    float* conv2_output = (float*) malloc(sizeof(float) * batch * conv2_new_height * conv2_new_width * conv2_num_f);
    float* conv2_output_relu = (float*) malloc(sizeof(float) * batch * conv2_new_height * conv2_new_width * conv2_num_f);

    int pool1_new_height = (int)(((conv2_new_height - pool_size) / pool_stride) + 1);
    int pool1_new_width  = (int)(((conv2_new_width  - pool_size) / pool_stride) + 1);
    float* pool1_output = (float*) malloc(sizeof(float) * batch * pool1_new_height * pool1_new_width * conv2_num_f);

// block 2
    int conv3_new_height = (int)(((conv2_new_height - conv3_f_height + 2 * pad) / conv_stride) + 1);
    int conv3_new_width  = (int)(((conv2_new_width  - conv3_f_width + 2 * pad) / conv_stride) + 1);
    float* conv3_output = (float*) malloc(sizeof(float) * batch * conv3_new_height * conv3_new_width * conv3_num_f);
    float* conv3_output_relu = (float*) malloc(sizeof(float) * batch * conv3_new_height * conv3_new_width * conv3_num_f);

    int conv4_new_height = (int)(((conv3_new_height - conv4_f_height + 2 * pad) / conv_stride) + 1);
    int conv4_new_width  = (int)(((conv3_new_width  - conv4_f_width + 2 * pad) / conv_stride) + 1);
    float* conv4_output = (float*) malloc(sizeof(float) * batch * conv4_new_height * conv4_new_width * conv4_num_f);
    float* conv4_output_relu = (float*) malloc(sizeof(float) * batch * conv4_new_height * conv4_new_width * conv4_num_f);

    int pool2_new_height = (int)(((conv4_new_height - pool_size) / pool_stride) + 1);
    int pool2_new_width  = (int)(((conv4_new_width  - pool_size) / pool_stride) + 1);
    float* pool2_output = (float*) malloc(sizeof(float) * batch * pool2_new_height * pool2_new_width * conv4_num_f);

// block 3
    int conv5_new_height = (int)(((conv4_new_height - conv5_f_height + 2 * pad) / conv_stride) + 1);
    int conv5_new_width  = (int)(((conv4_new_width  - conv5_f_width + 2 * pad) / conv_stride) + 1);
    float* conv5_output = (float*) malloc(sizeof(float) * batch * conv5_new_height * conv5_new_width * conv5_num_f);
    float* conv5_output_relu = (float*) malloc(sizeof(float) * batch * conv5_new_height * conv5_new_width * conv5_num_f);

    int conv6_new_height = (int)(((conv5_new_height - conv6_f_height + 2 * pad) / conv_stride) + 1);
    int conv6_new_width  = (int)(((conv5_new_width  - conv6_f_width + 2 * pad) / conv_stride) + 1);
    float* conv6_output = (float*) malloc(sizeof(float) * batch * conv6_new_height * conv6_new_width * conv6_num_f);
    float* conv6_output_relu = (float*) malloc(sizeof(float) * batch * conv6_new_height * conv6_new_width * conv6_num_f);

    int conv7_new_height = (int)(((conv6_new_height - conv7_f_height + 2 * pad) / conv_stride) + 1);
    int conv7_new_width  = (int)(((conv6_new_width  - conv7_f_width + 2 * pad) / conv_stride) + 1);
    float* conv7_output = (float*) malloc(sizeof(float) * batch * conv7_new_height * conv7_new_width * conv7_num_f);
    float* conv7_output_relu = (float*) malloc(sizeof(float) * batch * conv7_new_height * conv7_new_width * conv7_num_f);

    int pool3_new_height = (int)(((conv7_new_height - pool_size) / pool_stride) + 1);
    int pool3_new_width  = (int)(((conv7_new_width  - pool_size) / pool_stride) + 1);
    float* pool3_output = (float*) malloc(sizeof(float) * batch * pool3_new_height * pool3_new_width * conv7_num_f);

// block 4
    int conv8_new_height = (int)(((conv7_new_height - conv8_f_height + 2 * pad) / conv_stride) + 1);
    int conv8_new_width  = (int)(((conv7_new_width  - conv8_f_width + 2 * pad) / conv_stride) + 1);
    float* conv8_output = (float*) malloc(sizeof(float) * batch * conv8_new_height * conv8_new_width * conv8_num_f);
    float* conv8_output_relu = (float*) malloc(sizeof(float) * batch * conv8_new_height * conv8_new_width * conv8_num_f);

    int conv9_new_height = (int)(((conv8_new_height - conv9_f_height + 2 * pad) / conv_stride) + 1);
    int conv9_new_width  = (int)(((conv8_new_width  - conv9_f_width + 2 * pad) / conv_stride) + 1);
    float* conv9_output = (float*) malloc(sizeof(float) * batch * conv9_new_height * conv9_new_width * conv9_num_f);
    float* conv9_output_relu = (float*) malloc(sizeof(float) * batch * conv9_new_height * conv9_new_width * conv9_num_f);

    int conv10_new_height = (int)(((conv9_new_height - conv10_f_height + 2 * pad) / conv_stride) + 1);
    int conv10_new_width  = (int)(((conv9_new_width  - conv10_f_width + 2 * pad) / conv_stride) + 1);
    float* conv10_output = (float*) malloc(sizeof(float) * batch * conv10_new_height * conv10_new_width * conv10_num_f);
    float* conv10_output_relu = (float*) malloc(sizeof(float) * batch * conv10_new_height * conv10_new_width * conv10_num_f);

    int pool4_new_height = (int)(((conv10_new_height - pool_size) / pool_stride) + 1);
    int pool4_new_width  = (int)(((conv10_new_width  - pool_size) / pool_stride) + 1);
    float* pool4_output = (float*) malloc(sizeof(float) * batch * pool4_new_height * pool4_new_width * conv10_num_f);
 
// block 5
    int conv11_new_height = (int)(((conv10_new_height - conv11_f_height + 2 * pad) / conv_stride) + 1);
    int conv11_new_width  = (int)(((conv10_new_width  - conv11_f_width + 2 * pad) / conv_stride) + 1);
    float* conv11_output = (float*) malloc(sizeof(float) * batch * conv11_new_height * conv11_new_width * conv11_num_f);
    float* conv11_output_relu = (float*) malloc(sizeof(float) * batch * conv11_new_height * conv11_new_width * conv11_num_f);

    int conv12_new_height = (int)(((conv11_new_height - conv12_f_height + 2 * pad) / conv_stride) + 1);
    int conv12_new_width  = (int)(((conv11_new_width  - conv12_f_width + 2 * pad) / conv_stride) + 1);
    float* conv12_output = (float*) malloc(sizeof(float) * batch * conv12_new_height * conv12_new_width * conv12_num_f);
    float* conv12_output_relu = (float*) malloc(sizeof(float) * batch * conv12_new_height * conv12_new_width * conv12_num_f);

    int conv13_new_height = (int)(((conv12_new_height - conv13_f_height + 2 * pad) / conv_stride) + 1);
    int conv13_new_width  = (int)(((conv12_new_width  - conv13_f_width + 2 * pad) / conv_stride) + 1);
    float* conv13_output = (float*) malloc(sizeof(float) * batch * conv13_new_height * conv13_new_width * conv13_num_f);
    float* conv13_output_relu = (float*) malloc(sizeof(float) * batch * conv13_new_height * conv13_new_width * conv13_num_f);

    int pool5_new_height = (int)(((conv13_new_height - pool_size) / pool_stride) + 1);
    int pool5_new_width  = (int)(((conv13_new_width  - pool_size) / pool_stride) + 1);
    float* pool5_output = (float*) malloc(sizeof(float) * batch * pool5_new_height * pool5_new_width * conv13_num_f);
    printf("finished output malloc.\n");

    load_data("persian_cat.txt", input_img, IMG_DIM1 * IMG_DIM2 * IMG_DIM3 * IMG_DIM4);
    load_data("weights_txt/block1_conv1_kernel.txt", w1,  conv1_num_f   * conv1_f_height   * conv1_f_width   * conv1_channels);
    load_data("weights_txt/block1_conv2_kernel.txt", w2,  conv2_num_f   * conv2_f_height   * conv2_f_width   * conv2_channels);
    load_data("weights_txt/block2_conv1_kernel.txt", w3,  conv3_num_f   * conv3_f_height   * conv3_f_width   * conv3_channels);
    load_data("weights_txt/block2_conv2_kernel.txt", w4,  conv4_num_f   * conv4_f_height   * conv4_f_width   * conv4_channels);
    load_data("weights_txt/block3_conv1_kernel.txt", w5,  conv5_num_f   * conv5_f_height   * conv5_f_width   * conv5_channels);
    load_data("weights_txt/block3_conv2_kernel.txt", w6,  conv6_num_f   * conv6_f_height   * conv6_f_width   * conv6_channels);
    load_data("weights_txt/block3_conv3_kernel.txt", w7,  conv7_num_f   * conv7_f_height   * conv7_f_width   * conv7_channels);
    load_data("weights_txt/block4_conv1_kernel.txt", w8,  conv8_num_f   * conv8_f_height   * conv8_f_width   * conv8_channels);
    load_data("weights_txt/block4_conv2_kernel.txt", w9,  conv9_num_f   * conv9_f_height   * conv9_f_width   * conv9_channels);
    load_data("weights_txt/block4_conv3_kernel.txt", w10, conv10_num_f  * conv10_f_height  * conv10_f_width  * conv10_channels);
    load_data("weights_txt/block5_conv1_kernel.txt", w11, conv11_num_f  * conv11_f_height  * conv11_f_width  * conv11_channels);
    load_data("weights_txt/block5_conv2_kernel.txt", w12, conv12_num_f  * conv12_f_height  * conv12_f_width  * conv12_channels);
    load_data("weights_txt/block5_conv3_kernel.txt", w13, conv13_num_f  * conv13_f_height  * conv13_f_width  * conv13_channels);

    load_data("weights_txt/block1_conv1_bias.txt", b1,  conv1_num_f);
    load_data("weights_txt/block1_conv2_bias.txt", b2,  conv2_num_f);
    load_data("weights_txt/block2_conv1_bias.txt", b3,  conv3_num_f);
    load_data("weights_txt/block2_conv2_bias.txt", b4,  conv4_num_f);
    load_data("weights_txt/block3_conv1_bias.txt", b5,  conv5_num_f);
    load_data("weights_txt/block3_conv2_bias.txt", b6,  conv6_num_f);
    load_data("weights_txt/block3_conv3_bias.txt", b7,  conv7_num_f);
    load_data("weights_txt/block4_conv1_bias.txt", b8,  conv8_num_f);
    load_data("weights_txt/block4_conv2_bias.txt", b9,  conv9_num_f);
    load_data("weights_txt/block4_conv3_bias.txt", b10, conv10_num_f);
    load_data("weights_txt/block5_conv1_bias.txt", b11, conv11_num_f);
    load_data("weights_txt/block5_conv2_bias.txt", b12, conv12_num_f);
    load_data("weights_txt/block5_conv3_bias.txt", b13, conv13_num_f);

    printf("input and weights loaded!\n");
    

    // block 1
    // output, input, weights, bias, input's 4 dimensions, weight's 4 dimensions
    conv_forward(conv1_output, input_img, w1, b1, pad, conv_stride, 
    IMG_DIM1, IMG_DIM2, IMG_DIM3, IMG_DIM4, 
    conv1_num_f, conv1_f_height, conv1_f_width, conv1_channels);
    printf("conv1 ended\n");
    relu_forward(conv1_output_relu, conv1_output, batch, conv1_new_height, conv1_new_width, conv1_num_f);
    printf("relu1 ended\n");

    conv_forward(conv2_output, conv1_output_relu, w2, b2, pad, conv_stride, 
    batch, conv1_new_height, conv1_new_width, conv1_num_f, 
    conv2_num_f, conv2_f_height, conv2_f_width, conv2_channels);
    printf("conv2 ended\n");
    relu_forward(conv2_output_relu, conv2_output, batch, conv2_new_height, conv2_new_width, conv2_num_f);
    printf("relu2 ended\n");

    // output, input, input's 4 dimensions
    max_pool_forward(pool1_output, conv2_output_relu, pool_size, pool_stride, 
    batch, conv2_new_height, conv2_new_width, conv2_num_f);  
    printf("pool1 ended\n");

    // block 2
    conv_forward(conv3_output, pool1_output, w3, b3, pad, conv_stride, 
    batch, pool1_new_height, pool1_new_width, conv2_num_f,  
    conv3_num_f, conv3_f_height, conv3_f_width, conv3_channels);
    printf("conv3 ended\n");
    relu_forward(conv3_output_relu, conv3_output, batch, conv3_new_height, conv3_new_width, conv3_num_f);
    printf("relu3 ended\n");

    conv_forward(conv4_output, conv3_output_relu, w4, b4, pad, conv_stride, 
    batch, conv3_new_height, conv3_new_width, conv3_num_f, 
    conv4_num_f, conv4_f_height, conv4_f_width, conv4_channels);
    printf("conv4 ended\n");
    relu_forward(conv4_output_relu, conv4_output, batch, conv4_new_height, conv4_new_width, conv4_num_f);
    printf("relu4 ended\n");

    max_pool_forward(pool2_output, conv4_output_relu, pool_size, pool_stride, 
    batch, conv4_new_height, conv4_new_width, conv4_num_f); 
    printf("pool2 ended\n"); 

    // block 3
    conv_forward(conv5_output, pool2_output, w5, b5, pad, conv_stride, 
    batch, pool2_new_height, pool2_new_width, conv4_num_f,  
    conv5_num_f, conv5_f_height, conv5_f_width, conv5_channels);
    printf("conv5 ended\n");
    relu_forward(conv5_output_relu, conv5_output, batch, conv5_new_height, conv5_new_width, conv5_num_f);
    printf("relu5 ended\n");

    conv_forward(conv6_output, conv5_output_relu, w6, b6, pad, conv_stride, 
    batch, conv5_new_height, conv5_new_width, conv5_num_f, 
    conv6_num_f, conv6_f_height, conv6_f_width, conv6_channels);
    printf("conv6 ended\n");
    relu_forward(conv6_output_relu, conv6_output, batch, conv6_new_height, conv6_new_width, conv6_num_f);
    printf("relu6 ended\n");

    conv_forward(conv7_output, conv6_output_relu, w7, b7, pad, conv_stride, 
    batch, conv6_new_height, conv6_new_width, conv6_num_f, 
    conv7_num_f, conv7_f_height, conv7_f_width, conv7_channels);
    printf("conv7 ended\n");
    relu_forward(conv7_output_relu, conv7_output, batch, conv7_new_height, conv7_new_width, conv7_num_f);
    printf("relu7 ended\n");

    max_pool_forward(pool3_output, conv7_output_relu, pool_size, pool_stride, 
    batch, conv7_new_height, conv7_new_width, conv7_num_f); 
    printf("pool3 ended\n");  

    // block 4
    conv_forward(conv8_output, pool3_output, w8, b8, pad, conv_stride, 
    batch, pool3_new_height, pool3_new_width, conv7_num_f,  
    conv8_num_f, conv8_f_height, conv8_f_width, conv8_channels);
    printf("conv8 ended\n");
    relu_forward(conv8_output_relu, conv8_output, batch, conv8_new_height, conv8_new_width, conv8_num_f);
    printf("relu8 ended\n");

    conv_forward(conv9_output, conv8_output_relu, w9, b9, pad, conv_stride, 
    batch, conv8_new_height, conv8_new_width, conv8_num_f, 
    conv9_num_f, conv9_f_height, conv9_f_width, conv9_channels);
    printf("conv9 ended\n");
    relu_forward(conv9_output_relu, conv9_output, batch, conv9_new_height, conv9_new_width, conv9_num_f);
    printf("relu9 ended\n");

    conv_forward(conv10_output, conv9_output_relu, w10, b10, pad, conv_stride, 
    batch, conv9_new_height, conv9_new_width, conv9_num_f, 
    conv10_num_f, conv10_f_height, conv10_f_width, conv10_channels);
    printf("conv10 ended\n");
    relu_forward(conv10_output_relu, conv10_output, batch, conv10_new_height, conv10_new_width, conv10_num_f);
    printf("relu10 ended\n");

    max_pool_forward(pool4_output, conv10_output_relu, pool_size, pool_stride, 
    batch, conv10_new_height, conv10_new_width, conv10_num_f);  
    printf("pool4 ended\n");  

    // block 5
    conv_forward(conv11_output, pool4_output, w11, b11, pad, conv_stride, 
    batch, pool4_new_height, pool4_new_width, conv10_num_f,  
    conv11_num_f, conv11_f_height, conv11_f_width, conv11_channels);
    printf("conv11 ended\n");
    relu_forward(conv11_output_relu, conv11_output, batch, conv11_new_height, conv11_new_width, conv11_num_f);
    printf("relu11 ended\n");

    conv_forward(conv12_output, conv11_output_relu, w12, b12, pad, conv_stride, 
    batch, conv11_new_height, conv11_new_width, conv11_num_f, 
    conv12_num_f, conv12_f_height, conv12_f_width, conv12_channels);
    printf("conv12 ended\n");
    relu_forward(conv12_output_relu, conv12_output, batch, conv12_new_height, conv12_new_width, conv12_num_f);
    printf("relu12 ended\n");

    conv_forward(conv13_output, conv12_output_relu, w13, b13, pad, conv_stride, 
    batch, conv12_new_height, conv12_new_width, conv12_num_f, 
    conv13_num_f, conv13_f_height, conv13_f_width, conv13_channels);
    printf("conv13 ended\n");
    relu_forward(conv13_output_relu, conv13_output, batch, conv13_new_height, conv13_new_width, conv13_num_f);
    printf("relu13 ended\n");

    max_pool_forward(pool5_output, conv13_output_relu, pool_size, pool_stride, 
    batch, conv13_new_height, conv13_new_width, conv13_num_f);  
    printf("pool5 ended");

    show(pool5_output, batch, pool5_new_height, pool5_new_width, conv13_num_f);

    // float* fc1_kernel = (float*) malloc(sizeof(float) * FC1_DIM1 * FC1_DIM2);
    // float* fc1_bias = (float*) malloc(sizeof(float) * FC1_DIM2);
    // float* fc1_feature_map = (float*) malloc(sizeof(float) * FC1_DIM2);

    // float* fc2_kernel = (float*) malloc(sizeof(float) * FC2_DIM1 * FC2_DIM2);
    // float* fc2_bias = (float*) malloc(sizeof(float) * FC2_DIM2);
    // float* fc2_feature_map = (float*) malloc(sizeof(float) * FC2_DIM2);

    // float* fc3_kernel = (float*) malloc(sizeof(float) * FC3_DIM1 * FC3_DIM2);
    // float* fc3_bias = (float*) malloc(sizeof(float) * FC3_DIM2);
    // float* output_feature_map = (float*) malloc(sizeof(float) * FC_OUTPUT_SIZE);

    // // load_data("fc_input_flatten.txt", input_feature_map, FC_INPUT_SIZE);
    // load_data("weights_txt/fc1_kernel.txt", fc1_kernel, FC1_DIM1 * FC1_DIM2);
    // load_data("weights_txt/fc1_bias.txt", fc1_bias, FC1_DIM2);
    // load_data("weights_txt/fc2_kernel.txt", fc2_kernel, FC2_DIM1 * FC2_DIM2);
    // load_data("weights_txt/fc2_bias.txt", fc2_bias, FC2_DIM2);
    // load_data("weights_txt/predictions_kernel.txt", fc3_kernel, FC3_DIM1 * FC3_DIM2);
    // load_data("weights_txt/predictions_bias.txt", fc3_bias, FC3_DIM2);

    // fc(pool5_output, fc1_bias, fc1_kernel, fc1_feature_map,
    //     FC1_DIM2, FC1_DIM1, 1);
    // fc(fc1_feature_map, fc2_bias, fc2_kernel, fc2_feature_map,
    //     FC2_DIM2, FC2_DIM1, 1);
    // fc(fc2_feature_map, fc3_bias, fc3_kernel, output_feature_map,
    //     FC3_DIM2, FC3_DIM1, 1);
   
   	// int result[1];
    // argmax(output_feature_map, result, 1, FC_OUTPUT_SIZE);

    // printf("%d", result[0]);

    return 0;
}
