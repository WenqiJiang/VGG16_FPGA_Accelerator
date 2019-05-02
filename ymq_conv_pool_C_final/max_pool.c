#include <stdio.h>
#include <stdlib.h>

void max_pool_forward(float* A, float* x, int pool_size, int stride, 
int batch, int height, int width, int channels){
    int new_height = (int)(((height - pool_size) / stride) + 1);
    int new_width = (int)(((width - pool_size) / stride) + 1);
    
    //float[] A = (float) malloc(sizeof(float) * batch * new_height * new_width * channels);
    for (int bt = 0; bt < batch; bt++){
        for (int row = 0; row < new_height; row++){
            for (int col = 0; col < new_width; col++){
                for (int cn = 0; cn < channels; cn++){
                    for (int i = 0; i < pool_size; i++){
                        for (int j = 0; j < pool_size; j++){
                            if (x[channels * width * height * bt + channels * width * (row * stride + i) + 
                            channels * (col * stride + j) + cn] > 
                            A[channels * new_width * new_height * bt + channels * new_width * row + channels * col + cn]){
                                A[channels * new_width * new_height * bt + channels * new_width * row + channels * col + cn] =
                                x[channels * width * height * bt + channels * width * (row * stride + i) + 
                                    channels * (col * stride + j) + cn];
                            }
                        }
                    }
                }
            }
        }
    }
    // return A;
}


