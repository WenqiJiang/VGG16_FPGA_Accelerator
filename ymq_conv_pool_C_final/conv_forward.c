
// #include <stdio.h>
#include <stdlib.h>
// #include "relu.h"

float get_sum(float* w, float* x_pad, int ft, int w2, int w3, int w4,
 int bt, int i, int j, int stride, int x_pad1, int x_pad2, int x_pad3, int x_pad4){
    float sum = 0;
    for (int fh = 0; fh < w2; fh++){
        for (int fw = 0; fw < w3; fw++){
            for (int cf = 0; cf < w4; cf++){
                sum += w[w4 * w3 * w2 * ft + w4 * w3 * fh + w4 *fw + cf]
                * x_pad[x_pad4 * x_pad3 * x_pad2 * bt + x_pad4 * x_pad3 * (i * stride + fh) + x_pad4 * (j * stride + fw) + cf]; 
            }
        }
    }
    return sum;
}

void conv_forward(float* A, float* x, float* w, float* b, int pad, int stride, 
int batch, int height, int width, int channels, int num_of_filters, int filter_height, 
int filter_width, int channels_f) // A is output
{

    int new_height = height;
    int new_width = width;
    // float[] A = (float) malloc(sizeof(float) * batch * new_height * new_width * num_of_filters);
    float* x_pad = (float*) calloc(batch * (height + 2 * pad) * (width + 2 * pad) * channels, sizeof(float));

    for (int bt = 0; bt < batch; bt++){
        for (int i = 0; i < height; i++){
            for (int j = 0; j < width; j++){
                for (int cn = 0; cn < channels; cn++){
                    x_pad[channels * (width + 2 * pad) * (height + 2 * pad) * bt + channels * (width + 2 * pad) * (i + pad) + channels * (j + pad) + cn] =
                    x[channels * width * height * bt + channels * width * i + channels * j + cn];
                }
            }
        }
    }
    for (int bt = 0; bt < batch; bt++) {
        for (int i = 0; i < new_height; i++){
            for (int j = 0; j < new_width; j++){
                for (int ft = 0; ft < num_of_filters; ft++){
                    A[num_of_filters * new_width * new_height * bt + num_of_filters * new_width * i + num_of_filters * j + ft] = 
                    b[ft] + get_sum(w, x_pad, ft, filter_height, filter_width, channels_f, bt, 
                    i, j, stride, batch, height + 2 * pad, width + 2 * pad, channels);
                }
            }
        }
    }

    free(x_pad);
}

// w2 -> filter_height, w3 -> filter_width, w4 -> channels_f
// x_pad1 -> batch, x_pad2 -> height + 2 * pad, x_pad3 -> width + 2 * pad; x_pad4 -> channels
