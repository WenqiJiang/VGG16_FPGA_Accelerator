
#define IMG_DIM1 1
#define IMG_DIM2 226
#define IMG_DIM3 226
#define IMG_DIM4 3
#define IMG_NUM IMG_DIM1 * IMG_DIM2 * IMG_DIM3 * IMG_DIM4

#define POOL_SIZE 2
#define POOL_STRIDE 2
#define CONV_STRIDE 1
#define PAD 1
#define BATCH 1

#define FIL1_N 64
#define FIL1_H 3
#define FIL1_W 3
#define FIL1_C 3
#define FIL1_NUM FIL1_N * FIL1_H * FIL1_W * FIL1_C

#define FIL2_N 64
#define FIL2_H 3
#define FIL2_W 3
#define FIL2_C 64
#define FIL2_NUM FIL2_N * FIL2_H * FIL2_W * FIL2_C

//max_pool

#define FIL3_N 128
#define FIL3_H 3
#define FIL3_W 3
#define FIL3_C 64
#define FIL3_NUM FIL3_N * FIL3_H * FIL3_W * FIL3_C

#define FIL4_N 128
#define FIL4_H 3
#define FIL4_W 3
#define FIL4_C 128
#define FIL4_NUM FIL4_N * FIL4_H * FIL4_W * FIL4_C

//max_pool

#define FIL5_N 256
#define FIL5_H 3
#define FIL5_W 3
#define FIL5_C 128
#define FIL5_NUM FIL5_N * FIL5_H * FIL5_W * FIL5_C

#define FIL6_N 256
#define FIL6_H 3
#define FIL6_W 3
#define FIL6_C 256
#define FIL6_NUM FIL6_N * FIL6_H * FIL6_W * FIL6_C

#define FIL7_N 256
#define FIL7_H 3
#define FIL7_W 3
#define FIL7_C 256
#define FIL7_NUM FIL7_N * FIL7_H * FIL7_W * FIL7_C

//max_pool

#define FIL8_N 512
#define FIL8_H 3
#define FIL8_W 3
#define FIL8_C 256
#define FIL8_NUM FIL8_N * FIL8_H * FIL8_W * FIL8_C

#define FIL9_N 512
#define FIL9_H 3
#define FIL9_W 3
#define FIL9_C 512
#define FIL9_NUM FIL9_N * FIL9_H * FIL9_W * FIL9_C

#define FIL10_N 512
#define FIL10_H 3
#define FIL10_W 3
#define FIL10_C 512
#define FIL10_NUM FIL10_N * FIL10_H * FIL10_W * FIL10_C

//max_pool

#define FIL11_N 512
#define FIL11_H 3
#define FIL11_W 3
#define FIL11_C 512
#define FIL11_NUM FIL11_N * FIL11_H * FIL11_W * FIL11_C

#define FIL12_N 512
#define FIL12_H 3
#define FIL12_W 3
#define FIL12_C 512
#define FIL12_NUM FIL12_N * FIL12_H * FIL12_W * FIL12_C

#define FIL13_N 512
#define FIL13_H 3
#define FIL13_W 3
#define FIL13_C 512
#define FIL13_NUM FIL13_N * FIL13_H * FIL13_W * FIL13_C

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