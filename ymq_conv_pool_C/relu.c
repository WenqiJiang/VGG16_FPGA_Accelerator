#include <stdio.h>
#include <stdlib.h>

void relu_forward(float* out, float* x, int x_dim1, int x_dim2, int x_dim3, int x_dim4){
    for (int i = 0; i < x_dim1 * x_dim2 * x_dim3 * x_dim4; i++){
        if (x[i] > 0){
            out[i] = x[i];
        }
    }
}