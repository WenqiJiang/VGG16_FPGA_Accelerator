///////////////////////////////////////
// DMA controller test
// Send data from HPS DDR3
// to FPGA On-chip Memory
// compile with
// gcc hps.c -o hps
//
// based on a design from
// https://github.com/robertofem/CycloneVSoC-examples/blob/master/Linux-applications/DMA_transfer_FPGA_DMAC
// & https://people.ece.cornell.edu/land/courses/ece5760/DE1_SOC/HPS_peripherials/FPGA_addr_index.html
//
// Author: Ke Xu
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


#define SDRAM 32*1024*1024
#define INIT  64*9
#define WEIGHT 64*64*9
#define LAYER1 3
#define LAYER2 64

#define DMA_WRITE_S2P       0x09000000
#define FPGA_SDRAM_HPS_AXI  0x00000000
#define WEIGHT_SRAM_HPS_AXI 0x08000000
#define INPUT_SRAM_HPS_AXI  0x08020000
#define OUTPUT_SRAM_HPS_AXI 0x08030000

#define INPUT_SRAM_DMA_WRITE 0x08000000
#define INPUT_SRAM_PE_ARRAY  0x00000000
#define OUTPUT_SRAM_DMA_READ 0x08000000
#define OUTPUT_SRAM_PE_ARRAY 0x00000000
#define PE_ARRAY_WEIGHT_SRAM 0x00000000
#define S2P_WEIGHT_SRAM      0x00000000

#define WEIGHT_SDRAM 0x00000000
#define BIAS_SDRAM   0x01c0ed80
#define INOUT_SDRAM  0x035e5d80
#define OUTPUT_SDRAM 0x03c21f80
#define INTER_SDRAM  0x03f31f80 //843904

#define WEIGHT_LAYER1 3*64*9*2
#define WEIGHT_LAYER2 WEIGHT_LAYER1+64*64*9*2
#define WEIGHT_LAYER3 WEIGHT_LAYER2+64*128*9*2
#define WEIGHT_LAYER4 WEIGHT_LAYER3+128*128*9*2
#define WEIGHT_LAYER5 WEIGHT_LAYER4+128*256*9*2
#define WEIGHT_LAYER6 WEIGHT_LAYER5+256*256*9*2
#define WEIGHT_LAYER7 WEIGHT_LAYER6+256*256*9*2
#define WEIGHT_LAYER8 WEIGHT_LAYER7+256*512*9*2
#define WEIGHT_LAYER9 WEIGHT_LAYER8+512*512*9*2
#define WEIGHT_LAYER10 WEIGHT_LAYER9+512*512*9*2
#define WEIGHT_LAYER11 WEIGHT_LAYER10+512*512*9*2
#define WEIGHT_LAYER12 WEIGHT_LAYER11+512*512*9*2
#define WEIGHT_LAYER13 WEIGHT_LAYER12+512*512*9*2

#define BIAS_LAYER1 224*224*64*2
#define BIAS_LAYER2 BIAS_LAYER1+224*224*64*2
#define BIAS_LAYER3 BIAS_LAYER2+112*112*128*2
#define BIAS_LAYER4 BIAS_LAYER3+112*112*128*2
#define BIAS_LAYER5 BIAS_LAYER4+56*56*256*2
#define BIAS_LAYER6 BIAS_LAYER5+56*56*256*2
#define BIAS_LAYER7 BIAS_LAYER6+56*56*256*2
#define BIAS_LAYER8 BIAS_LAYER7+28*28*512*2
#define BIAS_LAYER9 BIAS_LAYER8+28*28*512*2
#define BIAS_LAYER10 BIAS_LAYER9+28*28*512*2
#define BIAS_LAYER11 BIAS_LAYER10+14*14*512*2
#define BIAS_LAYER12 BIAS_LAYER11+14*14*512*2
#define BIAS_LAYER13 BIAS_LAYER12+14*14*512*2


/* ----------------------------------*/

// On-chip Memory on FPGA
// main bus; scratch RAM
//#define FPGA_ONCHIP_BASE       0xC0001000
#define FPGA_ONCHIP_BASE       0xC0000000
#define FPGA_ONCHIP_SPAN       0x0a000000
// h2f bus
// RAM FPGA port s2
// main bus address 0x0000_1000
volatile unsigned short * dram_ptr = NULL;   //ocpram = on-chip-ram; ptr = pointer
void *dram_virtual_base;

/* ----------------------------------*/
// axi_lw_bus; DMA address
#define HW_REGS_BASE           0xff200000
#define HW_REGS_SPAN           0x00005000
#define DMA                    0x00000000   //This address needs to be confirmed!!!!
#define DMA1                   0x00000020
#define DMA_STATUS_OFFSET      0x00
#define DMA_READ_ADDR_OFFSET   0x04         //NOTE: DATASHEET says 1 for this addr
#define DMA_WRTE_ADDR_OFFSET   0x08
#define DMA_LENGTH_OFFSET      0x12
#define DMA_CNTL_OFFSET        0x24
#define CONTROLLER_FSM         0x40
// h2f_axi_lw_bus base
void *h2f_lw_virtual_base;
// HPS_TO_FPGA DMA address = 0
volatile unsigned int * DMA_status_ptr = NULL;
volatile unsigned int * DMA_read_ptr   = NULL;
volatile unsigned int * DMA_write_ptr  = NULL;
volatile unsigned int * DMA_length_ptr = NULL;
volatile unsigned int * DMA_cntl_ptr   = NULL;
volatile unsigned int * CONTROLLER_status_ptr = NULL;


// WAIT looks nicer than just braces
#define WAIT {}
enum {layer1 = 224+2, layer2 = 112+2, layer3 = 56+2, layer4 = 28+2, layer5 = 14+2, layer6 = 7+2};
enum {channel1 = 3, channel2 = 64, channel3 = 128, channel4 = 256, channel5 = 512} depth;

void sdram(volatile unsigned short * dram_ptr, int amount, short value);
int load_weight(int weight_addr, int data_size);
void load_input(int send_input_addr, int addr_shift, int data_size);
void load_bias(int send_bias_addr, int rcv_bias_addr);
void point_enable(int output_enable, int pool_enable, int relu_enable, int conv_num, int layer, int output_addr_onchip);
void output_back(int onchip_addr, int sdram_addr, int data_size);
void addr_shift(int *shift, int base, int width, int input_channel);
void conv_relu(int input_channel, int output_channel, int width, int conv_num, int weight_addr, int bias_addr, int input_addr, 
               int output_addr_sdram, int output_addr_onchip_rcv, int output_addr_onchip_send);
void conv_pool(int input_channel, int output_channel, int width, int conv_num, int weight_addr, int bias_addr, int input_addr, 
               int output_addr_sdram, int output_addr_onchip_rcv, int output_addr_onchip_send);
int  point_cal(int output_enable, int pool_enable, int relu_enable, int point_pos, int width, int channel, int in, int out, 
               int bias_shift, int weight_bias, int output_addr_onchip_rcv, int input_addr, int output_addr_sdram, int bias_addr, int conv_num);
void padding_row(int width, int output_sdram_addr);
void retrieve(int *result, int width, int output_channel, int output_sdram_addr);

int fd;

// timer variables
struct timeval t1, t2;
double elapsedTime;
// timer variables for WHOLE COMUPTATION
struct timeval start, end;
double alltime;

int temp, temp2;
int conv_layer;

int main(void) {
    int fc1[7*7*512];
    // Declare volatile pointers to I/O registers (volatile 	
	// means that IO load and store instructions will be used 	
	// to access these pointer locations, 
	// instead of regular memory loads and stores)
    gettimeofday(&start, NULL);
    /* ----------------------------------*/
    // Open /dev/mem
    if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1) {
        printf("ERROR: could not open \"/dev/mem\"...\n");
        return 1;
    }
    printf("Open /dev/mem successful\n");

    /* ----------------------------------*/
    // Get virtual address which maps to physical for 
    // axi_lw_master bus
    // DMA status register
    h2f_lw_virtual_base = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HW_REGS_BASE);  
    if (h2f_lw_virtual_base == MAP_FAILED) {
        perror("Can not get address\n");
        printf("ERROR: mmap(h2p_lw) failed ...\n");
        close(fd);
        return 1;
    }
    printf("Get axi_lw address successful\n");
    // DMA registers address shift to physical address
    DMA_status_ptr = (unsigned int *) (h2f_lw_virtual_base);
    // PE State Machine address setted in Qsys
    CONTROLLER_status_ptr = (unsigned int *) (h2f_lw_virtual_base + CONTROLLER_FSM);
    
    /* ----------------------------------*/
    // Get virtual address which maps to physical for 
    // FPGA on-chip memory
    dram_virtual_base = mmap(NULL, FPGA_ONCHIP_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, FPGA_ONCHIP_BASE);
    if (dram_virtual_base == MAP_FAILED) {
        printf("ERROR: mmap(h2f) failed...\n");
        close(fd);
        return 1;
    }
    // Point virtual address to dram_ptr
    dram_ptr = (unsigned short *) (dram_virtual_base);
    

    /* ----------------------------------*/
    /* ----------------------------------*/
    printf("Start Convolution Neural Network\n");
    // Initialzie SDRAM with 1 to all cells
    sdram(dram_ptr, SDRAM, 0b1);
    // Write data into DRAM on FPGA
    //sdram(dram_ptr, INIT, 3);
    printf("===============================\n\r\n");

    printf("Start Input Convolution Layer\n");
    conv_layer = 1;

    conv_relu( 3, 64, 226, 15, WEIGHT_SDRAM,               BIAS_SDRAM,             INTER_SDRAM,          INOUT_SDRAM, OUTPUT_SRAM_PE_ARRAY, OUTPUT_SRAM_DMA_READ);
    conv_relu(64, 64, 226, 15, WEIGHT_SDRAM+WEIGHT_LAYER1, BIAS_SDRAM+BIAS_LAYER1, INOUT_SDRAM+64*226*2, INOUT_SDRAM, OUTPUT_SRAM_PE_ARRAY, OUTPUT_SRAM_DMA_READ);

    conv_pool(64,  128, 226, 15, WEIGHT_SDRAM+WEIGHT_LAYER2, BIAS_SDRAM+BIAS_LAYER2, INOUT_SDRAM, OUTPUT_SDRAM, OUTPUT_SRAM_PE_ARRAY, OUTPUT_SRAM_DMA_READ);
    conv_relu(128, 128, 114, 15, WEIGHT_SDRAM+WEIGHT_LAYER3, BIAS_SDRAM+BIAS_LAYER3, OUTPUT_SDRAM, INOUT_SDRAM, OUTPUT_SRAM_PE_ARRAY, OUTPUT_SRAM_DMA_READ);

    conv_pool(128, 256, 114, 15, WEIGHT_SDRAM+WEIGHT_LAYER4, BIAS_SDRAM+BIAS_LAYER4, INOUT_SDRAM, OUTPUT_SDRAM, OUTPUT_SRAM_PE_ARRAY, OUTPUT_SRAM_DMA_READ);
    conv_relu(256, 256, 58,  15, WEIGHT_SDRAM+WEIGHT_LAYER5, BIAS_SDRAM+BIAS_LAYER5, OUTPUT_SDRAM, INOUT_SDRAM, OUTPUT_SRAM_PE_ARRAY, OUTPUT_SRAM_DMA_READ);
    conv_relu(256, 256, 58,  15, WEIGHT_SDRAM+WEIGHT_LAYER6, BIAS_SDRAM+BIAS_LAYER6, INOUT_SDRAM, OUTPUT_SDRAM, OUTPUT_SRAM_PE_ARRAY, OUTPUT_SRAM_DMA_READ);

    conv_pool(256, 512, 58, 15, WEIGHT_SDRAM+WEIGHT_LAYER7, BIAS_SDRAM+BIAS_LAYER7, OUTPUT_SDRAM, INOUT_SDRAM, OUTPUT_SRAM_PE_ARRAY, OUTPUT_SRAM_DMA_READ);
    conv_relu(512, 512, 30, 15, WEIGHT_SDRAM+WEIGHT_LAYER8, BIAS_SDRAM+BIAS_LAYER8, INOUT_SDRAM, OUTPUT_SDRAM, OUTPUT_SRAM_PE_ARRAY, OUTPUT_SRAM_DMA_READ);
    conv_relu(512, 512, 30, 15, WEIGHT_SDRAM+WEIGHT_LAYER9, BIAS_SDRAM+BIAS_LAYER9, OUTPUT_SDRAM, INOUT_SDRAM, OUTPUT_SRAM_PE_ARRAY, OUTPUT_SRAM_DMA_READ);

    conv_pool(512, 512, 30, 15, WEIGHT_SDRAM+WEIGHT_LAYER10, BIAS_SDRAM+BIAS_LAYER10, INOUT_SDRAM, OUTPUT_SDRAM, OUTPUT_SRAM_PE_ARRAY, OUTPUT_SRAM_DMA_READ);
    conv_relu(512, 512, 16, 15, WEIGHT_SDRAM+WEIGHT_LAYER11, BIAS_SDRAM+BIAS_LAYER11, OUTPUT_SDRAM, INOUT_SDRAM, OUTPUT_SRAM_PE_ARRAY, OUTPUT_SRAM_DMA_READ);
    conv_relu(512, 512, 16, 15, WEIGHT_SDRAM+WEIGHT_LAYER12, BIAS_SDRAM+BIAS_LAYER12, INOUT_SDRAM, OUTPUT_SDRAM, OUTPUT_SRAM_PE_ARRAY, OUTPUT_SRAM_DMA_READ);

    conv_pool(512, 512, 16, 15, WEIGHT_SDRAM+WEIGHT_LAYER13, BIAS_SDRAM+BIAS_LAYER13, OUTPUT_SDRAM, INOUT_SDRAM, OUTPUT_SRAM_PE_ARRAY, OUTPUT_SRAM_DMA_READ);
 
    for (int i = 0; i < 64*226*5; i += 64) {
        temp2 = *(dram_ptr + INOUT_SDRAM/2 + i);
        //if (temp2 != 0) {
            //printf("Data at %d is %d \n", i/64, temp2);
            //break;
        //
    }

    retrieve(fc1, 9, 512, INOUT_SDRAM);
/* 
    temp2 = *(dram_ptr + 0x08000000/2 + 64*64*9-1);
    printf("weight is %d \n",temp2);
    temp2 = *(dram_ptr + 0x08000000/2 + 64*64*9+1);
    printf("bias is %d \n",temp2);
    temp2 = *(dram_ptr + 0x08020000/2 + 64);
    printf("input is %d \n",temp2);

*/
    gettimeofday(&end, NULL);
    elapsedTime = (end.tv_sec - start.tv_sec);
    printf("\nCompleted Computation in T=%.0f Sec \n", elapsedTime);
    
}

void conv_relu(int input_channel, int output_channel, int width, int conv_num, int weight_addr, int bias_addr, int input_addr, 
          int output_addr_sdram, int output_addr_onchip_rcv, int output_addr_onchip_send) {
    int corner[3];
    int weight_bias;    //After transferring weight, give bias a consecutively next address
    int result_signal;
    int padding, attach_padding;
    int sub_channel;
    int bias_shift;

    gettimeofday(&t1, NULL);
    // Initialize output On-chip Memory with 0 for padding 
    for (int i = 0; i < 64*226*226; i++) {
        *(dram_ptr + 0x08030000/2 + i) = 1;
    }
    attach_padding = input_channel == 3 ? 1 : 0;
    sub_channel = input_channel >= 64 ? 64 : 3;
    padding = (input_channel-1)/64;  // Flag for padding

    // Loop for output channel
    for (int out = 0; out < output_channel/64; out++) {
        bias_shift = 0;
        padding_row(width, output_addr_sdram + out*width*width*64*2);

        // Loop for input channel
        for (int in = 0; in < input_channel/sub_channel; in++) {
            weight_bias = load_weight(weight_addr + (in+out)*sub_channel*64*9*2, sub_channel*64*9);
            // Loop only supposed to 64-channel data
            for (int point = 0; point < width * (width-2); point++) {
                // Case that ignore rightest two column due to the base point chose in 3x3 filter is left upper corner
                // It's also the time when a row output has been finished and needed be transfer back
                if (point%width == width-2) {
                    // Partial-Sum: Case that program still struggles with partial sum, load only width-2 length cols result
                    // Address explaination: base address + first 0-padding row + former 64-channel data shift + shift a row each time
                    output_back(output_addr_onchip_send, output_addr_sdram + (attach_padding+1)*64*width*2 + out*width*width*64*2 + point/width*64*width*2, 64*width);                
                    point++;
                    continue;
                }
                else {
                    addr_shift(corner, input_addr + in*width*width*64*2 + point*sub_channel*2, width, sub_channel);
                    for (int i = 0; i < 3; i++) {
                        load_input(corner[i], i*sub_channel*3, sub_channel*3);
                    }
                    // Case that input channel, i.e. depth, larger than 64, load bias from SDRAM at first,
                    // after first 64 layers, load partial-sum results from SDRAM in its region as bias. 
                    if (in == 0) {
                        load_bias(bias_addr, weight_addr);
                        //load_bias(bias_addr + out*(width-2)*(width-2)*64*2 + 64*bias_shift*2, weight_bias);
                        //bias_shift++;
                    }
                    else {
                        load_bias(output_addr_sdram + 64*width*2 + out*width*width*64*2 + 64*2 + 64*point*2, weight_bias);
                    }

                    if (padding != in) {
                        // Address explaination: base address + address shift for 0-padding + each point position
                        // | 0*64, data[0], data[1], ... , data[width-2], 0*64 |
                        point_enable(1, 0, 0, conv_num, sub_channel, output_addr_onchip_rcv + 64*2 + point%width*64*2);
                    }
                    else {
                        point_enable(1, 0, 1, conv_num, sub_channel, output_addr_onchip_rcv + 64*2 + point%width*64*2);
                    }
                    
                    // Receive the signal from FPGA for current state
                    while (*(CONTROLLER_status_ptr + 1) == 1){
                        result_signal = *(CONTROLLER_status_ptr + 1);
                    }
                    result_signal = *(CONTROLLER_status_ptr + 1);
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
               int weight_addr, int bias_addr, int input_addr,
               int output_addr_sdram, int output_addr_onchip_rcv, 
               int output_addr_onchip_send) {
    int corner[3];
    int weight_bias;  //After transferring weight, give bias a consecutively next address
    int result_signal;
    int padding, attach_padding, width_pool;
    int sub_channel;
    int bias_shift;

    gettimeofday(&t1, NULL);
    // Initialize output On-chip Memory with 0 for padding 
    for (int i = 0; i < 64*226*226; i++) {
        *(dram_ptr + 0x08030000/2 + i) = 1;
    }
    attach_padding = input_channel == 3 ? 1 : 0;
    sub_channel = input_channel >= 64 ? 64 : 3;
    padding = (input_channel-1)/64;  // Flag for padding
    width_pool = (width-2)/2 + 2;

    // Loop for output channel
    for (int out = 0; out < output_channel/64; out++) {
        //printf("out %d\n", out);
        bias_shift = 0;
        // Padding still works with size of width due to unchanged width with partial sum
        padding_row(width, output_addr_sdram + out*width*width*64*2);

        // Loop for input channel
        for (int in = 0; in < input_channel/sub_channel; in++) {
            weight_bias = load_weight(weight_addr + (in+out)*sub_channel*64*9*2, sub_channel*64*9);
            //printf("in %d\n", in);
            // Partial-Sum: Case that program still struggles with partial sum, deal with width scale data
            // output data form: input data width in 64 channel
            if (padding != in) {
                // Loop only supposed to 64-channel data
                for (int point = 0; point < width * (width-2); point++) {
                    // Case that ignore rightest two columns due to the base point chosen in 3x3 filter is left upper corner
                    // It's also the time when a row output has been finished and needed be transfer back
                    if (point%width == width-2) { 
                        // Address explaination: base address + first 0-padding row + former 64-channel data shift + shift a row each time
                        output_back(output_addr_onchip_send, output_addr_sdram + 64*width*2 + out*width*width*64*2 + point/width*64*width*2, 64*width);                                                          
                        //output_back(output_addr_onchip_send, output_addr_sdram + (attach_padding+1)*64*width_pool*2 + out*width_pool*width_pool*64*2 + point/width*64*width_pool*2, 64*width_pool);                        
                        point++;    // increase point position to the rightest point in each row
                        continue;   // end current row
                    }
                    else {
                        bias_shift = point_cal(1, 0, 0, point, width, sub_channel, in, out, bias_shift, weight_bias, output_addr_onchip_rcv, input_addr, output_addr_sdram, bias_addr, conv_num);
                    }
                }
            }
            // Output: Case that program finishs with output, load width length cols result with padding 0 at both sides of result
            // output data form: output data width in 64 channel
            else {
                // Loop only supposed to 64-channel data
                for (int point = 0; point < width * (width-2); point += 2) {
                    // Case that ignore rightest two column due to the base point chose in 3x3 filter is left upper corner
                    // It's also the time when a row output has been finished and needed be transfer back
                    if (point%width == width-2) {
                        // Address explaination: base address + first 0-padding row + former 64-channel data shift + shift a row each time                                        
                        // Output: Case that program finishs with output, load width length cols result with padding 0 at both sides of result
                        output_back(output_addr_onchip_send, output_addr_sdram + (attach_padding+1)*64*width_pool*2 + out*width_pool*width_pool*64*2 + point/2/width*64*width_pool*2, 64*width_pool);                        
                        //point++;
                        point += width;
                        continue;
                    }
                    else {
                        point_cal(0, 1, 1, point, width, sub_channel, in, out, bias_shift, weight_bias, output_addr_onchip_rcv, input_addr, output_addr_sdram, bias_addr, conv_num);
                        point_cal(0, 1, 1, point+1, width, sub_channel, in, out, bias_shift, weight_bias, output_addr_onchip_rcv, input_addr, output_addr_sdram, bias_addr, conv_num);
                        point_cal(0, 1, 1, point+width, width, sub_channel, in, out, bias_shift, weight_bias, output_addr_onchip_rcv, input_addr, output_addr_sdram, bias_addr, conv_num);
                        point_cal(1, 1, 1, point+width+1, width, sub_channel, in, out, bias_shift, weight_bias, output_addr_onchip_rcv, input_addr, output_addr_sdram, bias_addr, conv_num);
                    }
                }
            }            

        }
        // Padding again so that last row padding has been changed during calculation
        padding_row(width_pool, output_addr_sdram + out*width_pool*width_pool*64*2);
    }
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec);
    printf("Finish %dth convolution and max pooling in T=%.0f Sec \n", conv_layer, elapsedTime);
    ++conv_layer;
}

int load_weight(int weight_addr, int data_size) {
    int weight_bias;
    //Weight Transfer
    for (int i = 0; i < data_size/64; i++) {
        // Address for series2parrallel write
        *(CONTROLLER_status_ptr + 0x04) = S2P_WEIGHT_SRAM + i * 128;
        weight_bias = S2P_WEIGHT_SRAM + (i+1) * 128;
        // DMA controller configuration
        // Read from SDRAM for weight data
        // Write to Weight On-chip Memory
        *(DMA_status_ptr)     = 0x00000000;
        *(DMA_status_ptr + 1) = weight_addr + i * 128;
        *(DMA_status_ptr + 2) = DMA_WRITE_S2P;
        *(DMA_status_ptr + 3) = 2 * 64;
        *(DMA_status_ptr + 6) = 0b10001010;
        while ((*(DMA_status_ptr) & 0x010) == 0) {WAIT;} 
    }
    //printf("Finish transfer from SDRAM into Weight On-chip Memory\n");
    return weight_bias;
}

void load_input(int send_input_addr, int addr_shift, int data_size) {
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
    *(DMA_status_ptr + 2) = INPUT_SRAM_DMA_WRITE + addr_shift*2;
    // 1 word = 4 bytes
    *(DMA_status_ptr + 3) = 2 * data_size;
    // From table 283:
    // set bit 1 for half-WORD transfer, set 0,1,2 digit to 1 according to data-width
	// set bit 3 to start DMA
    // set bit 4 to enalbe interrupt request(IRQ)
	// set bit 7 to stop on byte-count
    // Start DMA transfer
    *(DMA_status_ptr + 6) = 0b10001010;
    while ((*(DMA_status_ptr) & 0x010) == 0) { WAIT; }
    
}

/* ----------------------------------*/
// Function: Transfer bias data to weight On-chip Memory for each point calculation
void load_bias(int send_bias_addr, int rcv_bias_addr) {
    // Bias Transfer   
    // Address for series2parrallel write
    *(CONTROLLER_status_ptr + 0x04) = rcv_bias_addr;
    // DMA controller configuration
    // Read from SDRAM for bias data
    // Write to Weight On-chip Memory
    *(DMA_status_ptr)     = 0x00000000;
    *(DMA_status_ptr + 1) = send_bias_addr;
    *(DMA_status_ptr + 2) = DMA_WRITE_S2P;    // Address for dma write -- constant
    *(DMA_status_ptr + 3) = 2 * 64;
    *(DMA_status_ptr + 6) = 0b10001010;
    while ((*(DMA_status_ptr) & 0x010) == 0) {WAIT;} 
    
}

/* ----------------------------------*/
// Function: Pe Array State Machine enable signal to calculate one point with 64 layers 
void point_enable(int output_enable, int pool_enable, int relu_enable, int conv_num, 
                  int layer, int output_addr_onchip) {
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
    //Controller state machine 1: pe enable signal
    *(CONTROLLER_status_ptr + 1) = 1;

}

void output_back(int onchip_addr, int sdram_addr, int data_size) {
    // DMA controller configuration
    // Read from SDRAM for weight data
    // Write to Weight On-chip Memory
    *(DMA_status_ptr + 0) = 0x00000000;
    *(DMA_status_ptr + 1) = onchip_addr;
    *(DMA_status_ptr + 2) = sdram_addr;
    *(DMA_status_ptr + 3) = 2 * data_size;
    *(DMA_status_ptr + 6) = 0b10001010;
    while ((*(DMA_status_ptr) & 0x010) == 0) {WAIT;} 
    
}

/* ----------------------------------*/
// Function: Transfer given data to SDRAM
void sdram(volatile unsigned short * dram_ptr, int amount, short value) {
    gettimeofday(&t1, NULL);
    for (int i=0; i < amount; i++) {
        *(dram_ptr + i) = value;
    }
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec);
    printf("Finishing transfer data to SDRAM within T=%.0f Sec \n", elapsedTime);
}

/* ----------------------------------*/
// Function: 
void retrieve(int *result, int width, int output_channel, int output_sdram_addr) {
    for (int out = 0; out < output_channel/64; out++) {
        for (int j = 0; j < 64; j++) {
            for (int i = width+1; i < width*(width-1); i++) {
                if (i%width == width-2) {
                    i++;
                    continue;
                }
                *result = (dram_ptr + output_sdram_addr/2 + out*width*width*64 + i*64 + j);
                result++;
            }
        }
    }
}

void addr_shift(int *shift, int base, int width, int input_channel) {
    shift[0] = 0 + base;
    shift[1] = input_channel*width + base;
    shift[2] = input_channel*width*2 + base;
}

void padding_row(int width, int output_sdram_addr) {
    for (int i = 0; i < 64*width; i++) {
        *(dram_ptr + output_sdram_addr/2 + i) = 1;
        *(dram_ptr + output_sdram_addr/2 + width*(width-1)*64 + i) = 1;
    }
}

int point_cal(int output_enable, int pool_enable, int relu_enable, int point_pos, 
              int width, int channel, int in, int out, int bias_shift, int weight_bias, 
              int output_addr_onchip_rcv, int input_addr, int output_addr_sdram, 
              int bias_addr, int conv_num) {
    int corner[3];
    int result_signal;
    
    addr_shift(corner, input_addr + in*width*width*64*2 + point_pos*channel*2, width, channel);
    for (int i = 0; i < 3; i++) {
        load_input(corner[i], i*channel*3, channel*3);
    }
    // Case that input channel, i.e. depth, larger than 64, load bias from SDRAM at first,
    // after first 64 layers, load partial-sum results from SDRAM in its region as bias. 
    if (in == 0) {
        load_bias(bias_addr, weight_addr);
        //load_bias(bias_addr + out*(width-2)*(width-2)*64*2 + 64*bias_shift*2, weight_bias);
        //bias_shift++;
    }
    else {
        load_bias(output_addr_sdram + 64*width*2 + out*width*width*64*2 + 64*2 + 64*point_pos*2, weight_bias);
    }
    
    // Address explaination: base address + address shift for 0-padding + each point position
    // | 0*64, data[0], data[1], ... , data[width-2], 0*64 |
    // I don't change the value of width since it doesn't influence the result of modulo operation
    point_enable(output_enable, pool_enable, relu_enable, conv_num, channel, output_addr_onchip_rcv + 64*2 + (point_pos/2)%width*64*2);
    // Receive the signal from FPGA for current state
    while (*(CONTROLLER_status_ptr + 1) == 1) {
        result_signal = *(CONTROLLER_status_ptr + 1);
    }
    //result_signal = *(CONTROLLER_status_ptr + 1);
    //printf("%dth Result from PE controller: %d \n", point_pos, result_signal);
    return bias_shift;
}