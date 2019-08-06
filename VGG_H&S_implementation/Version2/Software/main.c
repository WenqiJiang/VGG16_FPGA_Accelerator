///////////////////////////////////////
//
// Run: make
//
// based on a design from
// https://github.com/robertofem/CycloneVSoC-examples/blob/master/Linux-applications/DMA_transfer_FPGA_DMAC
// & https://people.ece.cornell.edu/land/courses/ece5760/DE1_SOC/HPS_peripherials/FPGA_addr_index.html
//
// Author: Ke Xu 
// Second Author: Wenqi Jiang (Function of FC layer)
// Date: 2019/07/21 at Columbia University
// Email: kx2141@columbia.edu
//
///////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <math.h>
#include "load_fixed.h"
#include "fc.h"
#include "utils.h"
#include "config.h"

// Temp define zone
#define WIDTH 7
#define DEPTH 512
int fd;
// timer variables
struct timeval t1, t2;
double elapsedTime;
// timer variables for PARTIAL COMPUTATION
struct timeval part1, part2;
double parttime;
// timer variables for WHOLE COMUPTATION
struct timeval start, end;
double alltime;

int temp = 0, temp2;
int conv_layer;

int main(void)
{
    // Declare volatile pointers to I/O registers (volatile
    // means that IO load and store instructions will be used
    // to access these pointer locations,
    // instead of regular memory loads and stores)
    gettimeofday(&start, NULL);
    /* ----------------------------------*/
    // Open /dev/mem
    if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1)
    {
        printf("ERROR: could not open \"/dev/mem\"...\n");
        return 1;
    }
    printf("Open /dev/mem successful\n");

    /* ----------------------------------*/
    // Get virtual address which maps to physical for
    // axi_lw_master bus
    // DMA status register
    h2f_lw_virtual_base = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HW_REGS_BASE);
    if (h2f_lw_virtual_base == MAP_FAILED)
    {
        perror("Can not get address\n");
        printf("ERROR: mmap(h2p_lw) failed ...\n");
        close(fd);
        return 1;
    }
    printf("Get axi_lw address successful\n");
    // DMA registers address shift to physical address
    DMA_status_ptr = (unsigned int *)(h2f_lw_virtual_base);
    // PE State Machine address setted in Qsys
    CONTROLLER_status_ptr = (unsigned int *)(h2f_lw_virtual_base + CONTROLLER_FSM);

    /* ----------------------------------*/
    // Get virtual address which maps to physical for
    // FPGA on-chip memory
    dram_virtual_base = mmap(NULL, FPGA_ONCHIP_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, FPGA_ONCHIP_BASE);
    if (dram_virtual_base == MAP_FAILED)
    {
        printf("ERROR: mmap(h2f) failed...\n");
        close(fd);
        return 1;
    }
    // Point virtual address to dram_ptr
    dram_ptr = (unsigned short *)(dram_virtual_base);
    /* ----------------------------------*/

    unsigned short *weight  = (unsigned short *)malloc(sizeof(unsigned short) * DATA_NUM_WEIGHT);
    unsigned short *bias    = (unsigned short *)malloc(sizeof(unsigned short) * DATA_NUM_BIAS);
    unsigned short *input   = (unsigned short *)malloc(sizeof(unsigned short) * DATA_NUM_INPUT);
    unsigned short *feature = (unsigned short *)malloc(sizeof(unsigned short) * DATA_NUM_INPUT_PAD);
    printf("Waiting load data\n");
    load_fixed("filter/weight2.txt", DATA_NUM_WEIGHT, weight);
    //printf("load weight finished\n");
    load_fixed("filter/bias.txt", DATA_NUM_BIAS, bias);
    //printf("load bias finished\n");
    load_fixed("filter/input.txt", DATA_NUM_INPUT, input);
    //printf("load input finished\n");
    printf("Load data finished\n");
    // These two loops used for padding input image before writting to SDRAM
    for (int i = 0; i < DATA_NUM_INPUT_PAD; ++i) {
        feature[i] = 0;
    }
    for (int i = 3 * 227; i < 3 * 226 * 226; i++) {
        if (i % (3 * 226) == 225 * 3) {
            i += 5;
            continue;
        }
        feature[i] = input[temp];
        temp++;
        if (temp >= DATA_NUM_INPUT)
            break;
    }
    /* ----------------------------------*/
    /* ----------------------------------*/
    printf("===============================\n");
    printf("Start Convolution Neural Network\n\n");
    // Load Weight data into sdram    
    sdram(0, DATA_NUM_WEIGHT, weight);
   // Load Bias data into sdram
    sdram(BIAS_SDRAM, DATA_NUM_BIAS, bias);
    // Load Input feature into sdram
    sdram(INTER_SDRAM, DATA_NUM_INPUT_PAD, feature);
    unsigned short initial[64*226*226] = {0};
    sdram(INOUT_SDRAM, 64*226*226, initial);
    free(weight);
    free(bias);
    free(input);
    free(feature);
    //printf("===============================\n\r\n");
    // Convoluation part on FPGA
    gettimeofday(&part1, NULL);
    printf("\nStart Input Convolution Layer\n");
    conv_layer = 1;
    //Block 1   
    conv_relu( 3, 64, 226, 0, WEIGHT_SDRAM, BIAS_SDRAM, INTER_SDRAM, INOUT_SDRAM);
    conv_pool(64, 64, 226, 1, WEIGHT_SDRAM+WEIGHT_LAYER1, BIAS_SDRAM+BIAS_LAYER1, INOUT_SDRAM+64*226*2, INOUT_SDRAM);
    //Block 2 width changed from 224 to 112
    conv_relu(64, 128, 114, 2, WEIGHT_SDRAM+WEIGHT_LAYER2, BIAS_SDRAM+BIAS_LAYER2, INOUT_SDRAM, OUTPUT_SDRAM);
    conv_pool(128, 128, 114, 3, WEIGHT_SDRAM+WEIGHT_LAYER3, BIAS_SDRAM+BIAS_LAYER3, OUTPUT_SDRAM, INOUT_SDRAM);
    //Block 3 width changed from 112 to 56    
    conv_relu(128, 256, 58, 4, WEIGHT_SDRAM+WEIGHT_LAYER4, BIAS_SDRAM+BIAS_LAYER4, INOUT_SDRAM, OUTPUT_SDRAM);
    conv_relu(256, 256, 58, 5, WEIGHT_SDRAM+WEIGHT_LAYER5, BIAS_SDRAM+BIAS_LAYER5, OUTPUT_SDRAM, INOUT_SDRAM);
    conv_pool(256, 256, 58, 6, WEIGHT_SDRAM+WEIGHT_LAYER6, BIAS_SDRAM+BIAS_LAYER6, INOUT_SDRAM, OUTPUT_SDRAM);
    //Block 4 width changed from 56 to 28
    conv_relu(256, 512, 30, 7, WEIGHT_SDRAM+WEIGHT_LAYER7, BIAS_SDRAM+BIAS_LAYER7, OUTPUT_SDRAM, INOUT_SDRAM);
    conv_relu(512, 512, 30, 8, WEIGHT_SDRAM+WEIGHT_LAYER8, BIAS_SDRAM+BIAS_LAYER8, INOUT_SDRAM, OUTPUT_SDRAM);
    conv_pool(512, 512, 30, 9, WEIGHT_SDRAM+WEIGHT_LAYER9, BIAS_SDRAM+BIAS_LAYER9, OUTPUT_SDRAM, INOUT_SDRAM);
    //Block 5 width changed from 28 to 14
    conv_relu(512, 512, 16, 10, WEIGHT_SDRAM+WEIGHT_LAYER10, BIAS_SDRAM+BIAS_LAYER10, INOUT_SDRAM, OUTPUT_SDRAM);
    conv_relu(512, 512, 16, 11, WEIGHT_SDRAM+WEIGHT_LAYER11, BIAS_SDRAM+BIAS_LAYER11, OUTPUT_SDRAM, INOUT_SDRAM);
    conv_pool(512, 512, 16, 12, WEIGHT_SDRAM+WEIGHT_LAYER12, BIAS_SDRAM+BIAS_LAYER12, INOUT_SDRAM, OUTPUT_SDRAM);

    unsigned short* conv_output = (unsigned short*) malloc(sizeof(unsigned short) * WIDTH * WIDTH * DEPTH);
    retrieve(conv_output, WIDTH+2, DEPTH, OUTPUT_SDRAM);
    float* conv = (float*) malloc(sizeof(float) * WIDTH * WIDTH * DEPTH);
    fixed_to_dec(conv_output, WIDTH*WIDTH*DEPTH, conv, 5);
    for (int i = 0; i < DEPTH*WIDTH*7; i += DEPTH) {
        //temp2 = *(dram_ptr + INOUT_SDRAM/2 + DEPTH*(WIDTH+2) + i);
        //printf("Data at %d is: %d\n",i/DEPTH, temp2);
        //printf("Data at %d is: %f\n",i/DEPTH, conv[i]);
    }   
    free(conv);

    gettimeofday(&part2, NULL);
    parttime = (part2.tv_sec - part1.tv_sec);
    printf("\nCompleted FPGA part in T=%.2f Sec \n", parttime);
    printf("===============================\n\r\n");

    /* ----------------------------------*/
    /* ----------------------------------*/
    // FC layer Part
    gettimeofday(&part1, NULL);
    printf("FC layer\n");
    float* fc1 = (float*) malloc(sizeof(float) * 7 * 7 * 512);
    fixed_to_dec(conv_output, 7*7*512, fc1, 5);
    free(conv_output);

    gettimeofday(&t1, NULL);
    printf("FC Layer1 start\n");
    float* fc1_kernel = (float*) malloc(sizeof(float) * FC1_DIM1 * FC1_DIM2);
    float* fc1_bias = (float*) malloc(sizeof(float) * FC1_DIM2);
    float* fc1_feature_map = (float*) malloc(sizeof(float) * FC1_DIM2);
    load_data("filter/fc1_kernel.txt", fc1_kernel, FC1_DIM1 * FC1_DIM2);
    load_data("filter/fc1_bias.txt", fc1_bias, FC1_DIM2);
    gettimeofday(&t2, NULL);
    elapsedTime = t2.tv_sec - t1.tv_sec;
    printf("Fc1 load weight&bias: %.2f min\n", elapsedTime/60);

    fc(fc1, fc1_bias, fc1_kernel, fc1_feature_map,
        FC1_DIM2, FC1_DIM1, 1);
    gettimeofday(&t2, NULL);
    elapsedTime = t2.tv_sec - t1.tv_sec;
    printf("Fc1 calculation: %.2f\n", elapsedTime/60);
    free(fc1);
    free(fc1_kernel);
    free(fc1_bias);
    printf("FC Layer1 end\n");
    
    gettimeofday(&t1, NULL);
    printf("FC Layer2 start\n");
    float* fc2_kernel = (float*) malloc(sizeof(float) * FC2_DIM1 * FC2_DIM2);
    float* fc2_bias = (float*) malloc(sizeof(float) * FC2_DIM2);
    float* fc2_feature_map = (float*) malloc(sizeof(float) * FC2_DIM2);
    load_data("filter/fc2_kernel.txt", fc2_kernel, FC2_DIM1 * FC2_DIM2);
    load_data("filter/fc2_bias.txt", fc2_bias, FC2_DIM2);
    gettimeofday(&t2, NULL);
    elapsedTime = t2.tv_sec - t1.tv_sec;
    printf("Fc2 load weight&bias: %.0f\n", elapsedTime);
    fc(fc1_feature_map, fc2_bias, fc2_kernel, fc2_feature_map,
        FC2_DIM2, FC2_DIM1, 1);
    gettimeofday(&t2, NULL);
    elapsedTime = t2.tv_sec - t1.tv_sec;
    printf("Fc2 calculation: %.0f\n", elapsedTime);
    free(fc1_feature_map);
    free(fc2_kernel);
    free(fc2_bias);
    printf("FC Layer2 end\n");

    gettimeofday(&t1, NULL);
    printf("FC Layer3 start\n");
    float* fc3_kernel = (float*) malloc(sizeof(float) * FC3_DIM1 * FC3_DIM2);
    float* fc3_bias = (float*) malloc(sizeof(float) * FC3_DIM2);
    float* output_feature_map = (float*) malloc(sizeof(float) * FC_OUTPUT_SIZE);
    load_data("filter/predictions_kernel.txt", fc3_kernel, FC3_DIM1 * FC3_DIM2);
    load_data("filter/predictions_bias.txt", fc3_bias, FC3_DIM2);
    gettimeofday(&t2, NULL);
    elapsedTime = t2.tv_sec - t1.tv_sec;
    printf("Fc3 load weight&bias: %.0f\n", elapsedTime);
    fc(fc2_feature_map, fc3_bias, fc3_kernel, output_feature_map,
        FC3_DIM2, FC3_DIM1, 1);
    gettimeofday(&t2, NULL);
    elapsedTime = t2.tv_sec - t1.tv_sec;
    printf("Fc3 calculation: %.0f\n", elapsedTime);
    free(fc2_feature_map);
    free(fc3_kernel);
    free(fc3_bias);
    printf("FC Layer3 end\n");
   
   	int result[1];
    argmax(output_feature_map, result, 1, FC_OUTPUT_SIZE);

    printf("argmax: \n");
    printf("%d", result[0]);

    gettimeofday(&part2, NULL);
    parttime = (part2.tv_sec - part1.tv_sec);
    printf("\nCompleted FC layer in HPS part in T=%.0f Sec \n", parttime);

    gettimeofday(&end, NULL);
    alltime = (end.tv_sec - start.tv_sec);
    printf("\nCompleted Computation in T=%.2f Min \n", alltime/60);
}

void conv_relu(int input_channel, int output_channel, int width, int conv_num,
               int weight_addr, int bias_addr, int input_addr, int output_addr_sdram)
{
    int corner[3];
    int weight_bias; //After transferring weight, give bias a consecutively next address
    int result_signal;
    int padding, attach_padding;
    int sub_channel;
    int partial_enable;

    gettimeofday(&t1, NULL);
    // Initialize output On-chip Memory with 0 for padding
    for (int i = 0; i < 64 * 226; i++)
    {
        *(dram_ptr + 0x08040000 / 2 + i) = 0;
    }
    for (int i = 0; i < 64*226*3; i++)
    {
        *(dram_ptr + 0x08020000 / 2 + i) = 0;
    }
    
    attach_padding = input_channel == 3 ? 1 : 0;
    sub_channel = input_channel >= 64 ? 64 : 3;
    padding = (input_channel - 1) / 64; // Flag for padding
    // Loop for output channel
    for (int out = 0; out < output_channel / 64; out++)
    {
        padding_row(attach_padding, width, output_addr_sdram + out * width * width * 64 * 2);
        // Loop for input channel
        for (int in = 0; in < input_channel / sub_channel; in++)
        {
            weight_bias = load_weight(weight_addr + (in + out*(input_channel/64)) * sub_channel * 64 * 9 * 2, sub_channel * 64 * 9);
            // Loop only supposed to 64-channel data
            for (int point = 0; point < width*(width - 2); point++)
            {
                if (point % width == 0) {
                    addr_shift(corner, input_addr + in * width * width * 64 * 2 + (point/width)*width*sub_channel*2, width, sub_channel);
                    for (int i = 0; i < 3; i++) {
                        load_input(corner[i], i*width*sub_channel, width*sub_channel);
                    }
                    if (input_channel == 64 && point == 226) {
                        printf("%d\n",corner[0]);
                    }
                }
                // Case that ignore rightest two column due to the base point chose in 3x3 filter is left upper corner
                // It's also the time when a row output has been finished and needed be transfer back
                if (point % width == width - 2)
                {
                    // Partial-Sum: Case that program still struggles with partial sum, load only width-2 length cols result
                    // Address explaination: base address + first 0-padding row + former 64-channel data shift + shift a row each time
                    output_back(OUTPUT_SRAM_DMA_READ, output_addr_sdram + (attach_padding + 1) * 64 * width * 2 + out * width * width * 64 * 2 + point / width * 64 * width * 2, 64 * width);
                    point++;
                    continue;
                }
                else
                {
                    // Case that input channel, i.e. depth, larger than 64, load bias from SDRAM at first,
                    // after first 64 layers, load partial-sum results from SDRAM in its region as bias.
                    if (in == 0)
                    {
                        load_bias(bias_addr, weight_bias);
                        partial_enable = 0;
                    }
                    else
                    {   
                        load_bias(output_addr_sdram + 64 * width * 2 + out * width * width * 64 * 2 + 64 * 2 + 64 * point * 2, weight_bias);
                        partial_enable = 1;
                    }  
                    if (padding != in)
                    {
                        // Address explaination: base address + address shift for 0-padding + each point position
                        // | 0*64, data[0], data[1], ... , data[width-2], 0*64 |
                        point_enable(1, 0, 0, partial_enable, (point%width)*sub_channel*2, FILTER_SIZE, sub_channel, width, conv_num, OUTPUT_SRAM_PE_ARRAY + 64 * 2 + point % width * 64 * 2);
                    }
                    else
                    {   
                        point_enable(1, 0, 1, partial_enable, (point%width)*sub_channel*2, 3, sub_channel, width, conv_num, OUTPUT_SRAM_PE_ARRAY + 64 * 2 + point % width * 64 * 2);
                    }

                    // Receive the signal from FPGA for current state
                    while (*(CONTROLLER_status_ptr + 1) == 1) {
                        result_signal = *(CONTROLLER_status_ptr + 1);
                    }
                    //result_signal = *(CONTROLLER_status_ptr + 1);
                    //printf("%dth Result from PE controller: %d \n", point, result_signal);
                }
            }
        }
    }
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec);
    printf("Finish %dth convolution in T=%.0f Sec \n", conv_layer, elapsedTime);
    ++conv_layer;
}

void conv_pool(int input_channel, int output_channel, int width, int conv_num,
               int weight_addr, int bias_addr, int input_addr, int output_addr_sdram)
{
    int corner[3];
    int weight_bias; //After transferring weight, give bias a consecutively next address
    int result_signal;
    int padding, attach_padding, width_pool;
    int sub_channel;

    gettimeofday(&t1, NULL);
    // Initialize output On-chip Memory with 0 for padding
    for (int i = 0; i < 64 * 226; i++)
    {
        *(dram_ptr + OUTPUT_SRAM_HPS_AXI / 2 + i) = 0;
    }
    for (int i = 0; i < 64 * 226 * 4; i++)
    {
        *(dram_ptr + INPUT_SRAM_HPS_AXI / 2 + i) = 0;
    }
    attach_padding = input_channel == 3 ? 1 : 0;
    sub_channel = input_channel >= 64 ? 64 : 3;
    padding = (input_channel - 1) / 64; // Flag for padding
    width_pool = (width - 2) / 2 + 2;

    // Loop for output channel
    for (int out = 0; out < output_channel / 64; out++)
    {
        //printf("out %d\n", out);
        // Padding still works with size of width due to unchanged width with partial sum
        padding_row(0, width, output_addr_sdram + out * width_pool * width_pool * 64 * 2);

        // Loop for input channel
        for (int in = 0; in < input_channel / sub_channel; in++)
        {
            weight_bias = load_weight(weight_addr + (in + out*(input_channel/64)) * sub_channel * 64 * 9 * 2, sub_channel * 64 * 9);
            // Partial-Sum: Case that program still struggles with partial sum, deal with width scale data
            // output data form: input data width in 64 channel
            if (padding != in)
            {
                // Loop only supposed to 64-channel data
                for (int point = 0; point < width * (width - 2); point++)
                {
                    if (point % width == 0) {
                        addr_shift(corner, input_addr + in * width * width * 64 * 2 + (point/width)*width*sub_channel*2, width, sub_channel);
                        for (int i=0; i < 3; i++) {
                            load_input(corner[i], i*width*sub_channel, width*sub_channel);
                        }
                    }
                    // Case that ignore rightest two columns due to the base point chosen in 3x3 filter is left upper corner
                    // It's also the time when a row output has been finished and needed be transfer back
                    if (point % width == width - 2)
                    {
                        // Address explaination: base address + first 0-padding row + former 64-channel data shift + shift a row each time
                        output_back(OUTPUT_SRAM_DMA_READ, output_addr_sdram + 64 * width * 2 + out * width_pool * width_pool * 64 * 2 + point / width * 64 * width * 2, 64 * width);
                        //output_back(output_addr_onchip_send, output_addr_sdram + (attach_padding+1)*64*width_pool*2 + out*width_pool*width_pool*64*2 + point/width*64*width_pool*2, 64*width_pool);
                        point++;  // increase point position to the rightest point in each row
                        continue; // end current row
                    }
                    else
                    {
                        point_cal(1, 0, 0, point, width, sub_channel, in, out, weight_bias, OUTPUT_SRAM_PE_ARRAY, input_addr, output_addr_sdram, bias_addr, conv_num);
                    }
                }
            }
            // Output: Case that program finishs with output, load width length cols result with padding 0 at both sides of result
            // output data form: output data width in 64 channel
            else
            {
                for (int i = 0; i < 64 * 226; i++) {
                    *(dram_ptr + OUTPUT_SRAM_HPS_AXI / 2 + i) = 0;
                }
                // Loop only supposed to 64-channel data
                for (int point = 0; point < width * (width - 3); point += 2)
                {
                    if (point % width == 0 && point % (2*width) == 0) {
                        for (int i = 0; i < 4; i++){
                            load_input(input_addr + in * width * width * 64 * 2 + (point/width+i)*width*sub_channel*2, width*sub_channel*i, width*sub_channel);
                        }
                            
                    }
                    // Case that ignore rightest two column due to the base point chose in 3x3 filter is left upper corner
                    // It's also the time when a row output has been finished and needed be transfer back
                    if (point % width == width - 2)
                    {
                        // Address explaination: base address + first 0-padding row + former 64-channel data shift + shift a row each time
                        // Output: Case that program finishs with output, load width length cols result with padding 0 at both sides of result
                        output_back(OUTPUT_SRAM_DMA_READ, output_addr_sdram + 64 * width_pool * 2 + out * width_pool * width_pool * 64 * 2
                                                          + point / 2 / width * 64 * width_pool * 2, 64 * width_pool);
                        point += width;
                        continue;
                    }
                    else
                    {
                        point_cal(0, 1, 1, point, width, sub_channel, in, out, weight_bias, OUTPUT_SRAM_PE_ARRAY, input_addr, output_addr_sdram, bias_addr, conv_num);
                        point_cal(0, 1, 1, point + 1, width, sub_channel, in, out, weight_bias, OUTPUT_SRAM_PE_ARRAY, input_addr, output_addr_sdram, bias_addr, conv_num);
                        point_cal(0, 1, 1, point + width, width, sub_channel, in, out, weight_bias, OUTPUT_SRAM_PE_ARRAY, input_addr, output_addr_sdram, bias_addr, conv_num);
                        point_cal(1, 1, 1, point + width + 1, width, sub_channel, in, out, weight_bias, OUTPUT_SRAM_PE_ARRAY, input_addr, output_addr_sdram, bias_addr, conv_num); 
                    }
                }
            }
        }
        // Padding again so that last row padding has been changed after pooling calculation
        padding_row(0, width_pool, output_addr_sdram + out * width_pool * width_pool * 64 * 2);
    }
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec);
    printf("Finish %dth convolution and max pooling in T=%.0f Sec \n", conv_layer, elapsedTime);
    ++conv_layer;
}

int load_weight(int weight_addr, int data_size)
{
    int weight_bias;
    //Weight Transfer
    for (int i = 0; i < data_size / 64; i++)
    {
        // Address for series2parrallel write
        *(CONTROLLER_status_ptr + 0x04) = S2P_WEIGHT_SRAM + i * 128;
        weight_bias = S2P_WEIGHT_SRAM + (i + 1) * 128;
        // DMA controller configuration
        // Read from SDRAM for weight data
        // Write to Weight On-chip Memory
        *(DMA_status_ptr) = 0x00000000;
        *(DMA_status_ptr + 1) = weight_addr + i * 128;
        *(DMA_status_ptr + 2) = DMA_WRITE_S2P;
        *(DMA_status_ptr + 3) = 2 * 64;
        *(DMA_status_ptr + 6) = 0b10001010;
        while ((*(DMA_status_ptr)&0x010) == 0) {WAIT;}
    }
    return weight_bias;
}

void load_input(int send_input_addr, int addr_shift, int data_size)
{
    // Input Data Transfer
    // DMA transfer from HPS_DDR3->FPGA_On_chip_memory
    // set up DMA
    // from https://www.intel.com/content/dam/www/programmable/us/en/pdfs/literature/ug/ug_embedded_ip.pdf
    // version: 19.1
    // section 29.4.3: Table 283: Control Regesiter bits
    // Stop (initialize) the dma at first
    *(DMA_status_ptr) = 0x00000000;
    // read bus_master gets data from DRAM addr = 0x0000_0000, setted by Qsys
    *(DMA_status_ptr + 1) = send_input_addr;
    // write bus_master for FPGA on-chip memory is mapped to 0x0800_0000, setted by Qsys
    *(DMA_status_ptr + 2) = INPUT_SRAM_DMA_WRITE + addr_shift * 2;
    // 1 word = 4 bytes
    *(DMA_status_ptr + 3) = 2 * data_size;
    // From table 283:
    // set bit 1 for half-WORD transfer, set 0,1,2 digit to 1 according to data-width
    // set bit 3 to start DMA
    // set bit 4 to enalbe interrupt request(IRQ)
    // set bit 7 to stop on byte-count
    // Start DMA transfer
    *(DMA_status_ptr + 6) = 0b10001010;
    while ((*(DMA_status_ptr)&0x010) == 0) {WAIT;}
}

/* ----------------------------------*/
// Function: Transfer bias data to weight On-chip Memory for each point calculation
void load_bias(int send_bias_addr, int rcv_bias_addr)
{
    // Bias Transfer
    // Address for series2parrallel write
    *(CONTROLLER_status_ptr + 0x04) = rcv_bias_addr;
    // DMA controller configuration
    // Read from SDRAM for bias data
    // Write to Weight On-chip Memory
    *(DMA_status_ptr) = 0x00000000;
    *(DMA_status_ptr + 1) = send_bias_addr;
    *(DMA_status_ptr + 2) = DMA_WRITE_S2P; // Address for dma write -- constant
    *(DMA_status_ptr + 3) = 2 * 64;
    *(DMA_status_ptr + 6) = 0b10001010;
    while ((*(DMA_status_ptr)&0x010) == 0) {WAIT;}
}

/* ----------------------------------*/
// Function: Pe Array State Machine enable signal to calculate one point with 64 layers
void point_enable(int output_enable, int pool_enable, int relu_enable, int partial_enable,
                  int addr_input, int filter_size, int layer, int layer_width, int conv_num,
                  int output_addr_onchip)
{
    //Controller state machine 0: convolution number
    *(CONTROLLER_status_ptr + 0) = conv_num;
    //Controller state machine 2: input channel
    *(CONTROLLER_status_ptr + 2) = layer;
    //Controller state machine 3: output on-chip memory address
    *(CONTROLLER_status_ptr + 3) = output_addr_onchip;
    //Controller state machine 5: whether or not relu result
    *(CONTROLLER_status_ptr + 5) = relu_enable;
    //Controller state machine 6: whether or not pooling
    *(CONTROLLER_status_ptr + 6) = pool_enable;
    //Controller state machine 7: whether or not output result
    *(CONTROLLER_status_ptr + 7) = output_enable;
    //Controller state machine 8: whether or not consider decimal
    *(CONTROLLER_status_ptr + 8) = partial_enable;
    //Controller state machine 8: whether or not consider decimal
    *(CONTROLLER_status_ptr + 9) = layer_width;
    //Controller state machine 8: whether or not consider decimal
    *(CONTROLLER_status_ptr +10) = filter_size;
    //Controller state machine 8: whether or not consider decimal
    *(CONTROLLER_status_ptr +11) = addr_input;
    //Controller state machine 1: pe enable signal
    *(CONTROLLER_status_ptr + 1) = 1;
    *(CONTROLLER_status_ptr + 1) = 0;
}

void output_back(int onchip_addr, int sdram_addr, int data_size)
{
    // DMA controller configuration
    // Read from SDRAM for weight data
    // Write to Weight On-chip Memory
    *(DMA_status_ptr + 0) = 0x00000000;
    *(DMA_status_ptr + 1) = onchip_addr;
    *(DMA_status_ptr + 2) = sdram_addr;
    *(DMA_status_ptr + 3) = 2 * data_size;
    *(DMA_status_ptr + 6) = 0b10001010;
    while ((*(DMA_status_ptr)&0x010) == 0) {WAIT;}
}

/* ----------------------------------*/
// Function: Transfer given data to SDRAM
void sdram(int addr_shift, int num, unsigned short *value)
{
    gettimeofday(&t1, NULL);
    for (int i = 0; i < num; i++)
    {
        *(dram_ptr + addr_shift/2 + i) = value[i];
    }
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec);
    printf("Finishing transfer data to SDRAM within T=%.0f Sec \n", elapsedTime);
}

/* ----------------------------------*/
// Function: Retrieve the data from SDRAM and rearrange the data with a column within output channel number
void retrieve(unsigned short *result, int width, int output_channel, int output_sdram_addr) {
    int count = 0;
    for (int i = width+1; i < width*(width-1); i++) {
        if (i % width == width - 1) {
            i++;
            continue;
        }
        for (int out = 0; out < output_channel/64; out++) {
            for (int j = 0; j < 64; j++) {
                result[count] = *(dram_ptr + output_sdram_addr/2 + i*64 + out*width*width*64 + j);
                count ++;
            } 
        }               
    }    
}

void addr_shift(int *shift, int base, int width, int input_channel)
{
    shift[0] = 0 + base;
    shift[1] = input_channel * width * 2+ base;
    shift[2] = input_channel * width * 2 * 2 + base;
}

void padding_row(int attach_padding, int width, int output_sdram_addr)
{
    for (int i = 0; i < 64 * width; i++)
    {
        *(dram_ptr + output_sdram_addr / 2 + i) = 0;
        *(dram_ptr + output_sdram_addr / 2 + width * (width - 1) * 64 + i) = 0;
    }
    if (attach_padding == 1) {
        for (int i = 0; i < 64*width; i++) {
            *(dram_ptr + output_sdram_addr / 2 + width * 64 + i) = 0;
        }
    }
}

void point_cal(int output_enable, int pool_enable, int relu_enable, int point_pos,
              int width, int channel, int in, int out, int weight_bias,
              int output_addr_onchip_rcv, int input_addr, int output_addr_sdram,
              int bias_addr, int conv_num)
{
    int corner[3];
    int result_signal;
    int point;
    int partial_enable;
    int width_pool;

    // Case that current point value is larger than width. The value need to minus width
    // in case that modulo operation could not acquire correct poisition.
    if ((point_pos/width)%2 == 1 && pool_enable == 1)
        point = point_pos - width;
    else if (pool_enable == 0){
        point = point_pos*2;
    }
    
    width_pool = (width - 2) / 2 + 2;
    // Case that input channel, i.e. depth, larger than 64, load bias from SDRAM at first,
    // after first 64 layers, load partial-sum results from SDRAM in its region as bias.
    if (in == 0)
    {
        load_bias(bias_addr, weight_bias);
        partial_enable = 0;
    }
    else
    {
        load_bias(output_addr_sdram + 64 * width * 2 + out * width_pool * width_pool * 64 * 2 + 64 * 2 + 64 * point_pos * 2, weight_bias);
        partial_enable = 1;
    }

    // Address explaination: base address + address shift for 0-padding + each point position
    // | 0*64, data[0], data[1], ... , data[width-2], 0*64 |
    // I don't change the value of width since it doesn't influence the result of modulo operation
    point_enable(output_enable, pool_enable, relu_enable, partial_enable, (point_pos%(width*2))*channel*2, FILTER_SIZE, channel, width, conv_num, output_addr_onchip_rcv + 64 * 2 + point/2 % width * 64 * 2);
    // Receive the signal from FPGA for current state
    while (*(CONTROLLER_status_ptr + 1) == 1) 
    {
        result_signal = *(CONTROLLER_status_ptr + 1);
    }
    //result_signal = *(CONTROLLER_status_ptr + 1);
    //printf("%dth Result from PE controller: %d \n", point_pos, result_signal);
}

void fixed_to_dec(unsigned short *data, int data_num, float *output, int point)
{
    int standard = 1;
    unsigned short temp;
    for (int j = 0; j < point; ++j)
    {
        standard *= 2;
    }
    for (int i = 0; i < data_num; ++i)
    {
        if (data[i] > 32768)
        {
            temp = data[i] - 32768;
            output[i] = -(float)((temp ^ 0b111111111111111) + 1) / standard;
        }
        else
        {
            output[i] = (float)data[i] / standard;
        }
    }
}