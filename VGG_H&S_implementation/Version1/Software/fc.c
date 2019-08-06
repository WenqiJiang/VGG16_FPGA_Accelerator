//#include "fc.h"
//#include "types.h"
#include <stdlib.h>
#include <stdio.h>

void fc(float* input_feature_map, float* bias, float* kernel, float* output_feature_map,
        int fc_output_size, int fc_input_size, int fc_batch_size) {

    // please do INITIALIZATION before input output_feature_map
    // ------- DIMENSION SETTING  ----------

    //  input_feature_map: fc_batch_size * fc_input_size (None * 128)
    //  bias: fc_output_size (16192)
    //  kernel: fc_input_size * fc_output_size (128 * 16192)
    //  output_feature_map: fc_batch_size * fc_output_size (None * 16192)

    for (int batch_index = 0; batch_index < fc_batch_size; batch_index++) {
        // compute each sample in a batch

        for (int output_feature_map_index = 0;
                output_feature_map_index < fc_output_size;
                output_feature_map_index++) {

            // compute output_feature_map[batch_index][output_feature_map_index]
            // each output_feature_map has fc_output_size elements, compute each of them
            //  * each computation is a vector vector multiplication
            //  * vector 1: input_feature_map
            //  * vector 2: a row of weights

            // output_feature_map[batch_index][output_feature_map_index]
            int current_output_feature_map_index = batch_index * fc_output_size +
                output_feature_map_index;

            // initialize to 0
            output_feature_map[current_output_feature_map_index] = 0;

            for (int input_feature_map_index = 0;
                    input_feature_map_index < fc_input_size;
                    input_feature_map_index++) {

                // output_feature_map[batch_index][output_feature_map_index] +=
                //      input_feature_map[batch_index][input_feature_map_index] *
                //      kernel[output_feature_map_index][input_feature_map_index]

                // input_feature_map[batch_index][input_feature_map_index]
                int current_input_feature_map_index = batch_index * fc_input_size +
                    input_feature_map_index;

                // kernel[output_feature_map_index][input_feature_map_index]
                int current_kernel_index = input_feature_map_index * fc_output_size +
                    output_feature_map_index;

                // do multiplication, add to previous value
                output_feature_map[current_output_feature_map_index] +=
                    input_feature_map[current_input_feature_map_index] *
                    kernel[current_kernel_index];
            }
            // add bias: bias[current_output_feature_map_index]
            output_feature_map[current_output_feature_map_index] +=
                bias[output_feature_map_index];
        }

    }
}

// void softmax (float* input_feature_map, float* output_probability_distribution,
//                 int sm_batch_size, int sm_class_size) {
//     // please do INITIALIZATION before input output_feature_map
//     // ------- DIMENSION SETTING  ---------- *

//     // input_feature_map:                (sm_batch_size,     SM_INPUT_SIZE)
//     // output_probability_distribution:     (sm_batch_size,     SM_OUTPUT_SIZE) =
//     //                                      (sm_batch_size,     SM_INPUT_SIZE)

//     // used to cache the exponential result
//     double* input_feature_map_exp = (double*) malloc(sizeof(double) * sm_class_size);

//     for (int batch_index = 0; batch_index < sm_batch_size; batch_index++) {
//         // compute each sample in a batch

//         // compute denominator, which is the sum of exponential
//         // of each input_feature_map
//         double denominator = 0;

//         for (int input_feature_map_index = 0;
//                 input_feature_map_index < sm_class_size;
//                 input_feature_map_index++) {

//             // denominator += input_feature_map[batch_index][input_feature_map_index]
//             int current_input_feature_map_index = batch_index * sm_class_size +
//                 input_feature_map_index;
//             // compute it, cache it
//             input_feature_map_exp[input_feature_map_index] =
//                 exp((double) input_feature_map[current_input_feature_map_index]);

//             // partial sum
//             denominator += input_feature_map_exp[input_feature_map_index];
//         }

//         // now compute each output_probability_distribution
//         for (int output_probability_distribution_index = 0;
//                 output_probability_distribution_index < sm_class_size;
//                 output_probability_distribution_index++) {

//             // output_probability_distribution [batch_index][output_probability_distribution_index]
//             //   = input_feature_map_exp [output_probability_distribution_index] / denominator
//             int current_output_probability_distribution_index = batch_index * sm_class_size
//                 + output_probability_distribution_index;
//             output_probability_distribution [current_output_probability_distribution_index] =
//                 input_feature_map_exp [output_probability_distribution_index] / denominator;
//         }
//     }
//     free(input_feature_map_exp);
// }

void argmax(float* input, int* result, int sm_batch_size, int sm_class_size) {
    // input: a probability distribution (sm_batch_size, SM_OUTPUT_SIZE)
    // result: the index of each output (SM_OUTPUT_SIZE, )
    for (int batch_index = 0; batch_index < sm_batch_size; batch_index++) {
        int max_index = 0;
        float max_val = input[batch_index * sm_class_size];
        for (int find_max_index = 0; find_max_index < sm_class_size; find_max_index++) {
            // input[batch_index][find_max_index]
            int input_index = batch_index * sm_class_size + find_max_index;
            if (input[input_index] > max_val) {
                max_index = find_max_index;
                max_val = input[input_index];
            }
        }
        result[batch_index] = max_index;
    }
}

