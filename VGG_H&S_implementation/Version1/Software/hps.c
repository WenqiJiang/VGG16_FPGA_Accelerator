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


#define SDRAM 4*1024*1024
#define INIT  3*9
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
#define OUTPUT_SRAM_DMA_READ 0x00000000
#define OUTPUT_SRAM_PE_ARRAY 0x00000000
#define PE_ARRAY_WEIGHT_SRAM 0x00000000
#define S2P_WEIGHT_SRAM      0x00000000


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
#define RESERVE_MEM_BASE      0x20000000
#define RESERVED_MEM_SPAN      0x20000000
// h2f_axi_lw_bus base
void *h2f_lw_virtual_base;
//reserved_mem_base
void *reserved_mem_base;
// HPS_TO_FPGA DMA address = 0
volatile unsigned int * DMA_status_ptr = NULL;
volatile unsigned int * DMA1_status_ptr = NULL;
volatile unsigned int * DMA_read_ptr   = NULL;
volatile unsigned int * DMA_write_ptr  = NULL;
volatile unsigned int * DMA_length_ptr = NULL;
volatile unsigned int * DMA_cntl_ptr   = NULL;
volatile unsigned int * CONTROLLER_status_ptr = NULL;
volatile unsigned short * reserved_mem_ptr = NULL;

// WAIT looks nicer than just braces
#define WAIT {}
void sdram(volatile unsigned short * dram_ptr, int amount, short value);
int load_weight(int weight_addr, int data_size);
void load_input(int input_addr, int data_size);
void load_bias(int send_bias_addr, int rcv_bias_addr);
void point_enable(int conv_num, int layer, int output_addr_onchip);
void output_back(int output_addr, int sdram_addr);

int fd;

// timer variables
struct timeval t1, t2;
double elapsedTime;

int temp, temp2;
int weight_bias;  //After transferring weight, give bias a consecutively next address

int main(void) {
    // Declare volatile pointers to I/O registers (volatile 	
	// means that IO load and store instructions will be used 	
	// to access these pointer locations, 
	// instead of regular memory loads and stores)

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
    reserved_mem_base = mmap(NULL, RESERVED_MEM_SPAN, (PROT_READ | PROT_WRITE),MAP_SHARED, fd, RESERVE_MEM_BASE);
    if (reserved_mem_base == MAP_FAILED){
	    perror("Can not get address\n");
	    printf("ERROR:mmap(reserved_mem)failed ...\n");
	    close(fd);
	    return 1;
    }
    printf("Get reserved_mem address successful\n");
    printf("Get axi_lw address successful\n");
    // DMA registers address shift to physical address
    DMA_status_ptr = (unsigned int *) (h2f_lw_virtual_base);
    DMA1_status_ptr = (unsigned int *) (h2f_lw_virtual_base + 0x80);
    // PE State Machine address setted in Qsys
    CONTROLLER_status_ptr = (unsigned int *) (h2f_lw_virtual_base + 0x40);
    
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
    //
    reserved_mem_ptr = (unsigned short *)(reserved_mem_base);

    /* ----------------------------------*/
    /* ----------------------------------*/
    // Initialzie SDRAM with 1 to all cells
    sdram(dram_ptr, SDRAM, 1);
    // Write data into DRAM on FPGA
    sdram(dram_ptr, INIT, 1);
    printf("===============================\n\r");
    signed short weight[27] = {7036,9017,7864,6118,7210,6693,-1005,-1333,-1067,4501,5664,5086,633,665,822,-6016,-7430,-6609,-941,-960,-833,-4296,-5417,-4673,-5735,-7946,-6857};
    signed short input[27] = {0,0,0,0,0,0,0,0,0,0,0,0,-13176,-14819,-15703,-13176,-14819,-15703,0,0,0,-13176,-14819,-15703,-13176,-14819,-15703};
    /*signed short input[27] = {0,0,0,0,0,0,0,0,0,0,0,0,-13176,10780,0,0,0,0,0,0,0,0,0,0,0,0,0};*/
    for (int i = 0; i < 27; i++){
	*(dram_ptr + i) = input[i];
	}
   /* for (int i = 0; i < 1024*1024;i++){
	*(reserved_mem_ptr  + i) = i;
	}
    for (int i = 0; i<100;i++){
	temp2 = *(reserved_mem_ptr + i);
	printf("reserver_mem at %d is %d\n",i,temp2);
	}*/

    for (int i = 0; i < 27; i++) {
       *(dram_ptr  + 64*i + 64 + 27) =weight[i];
    }
   
    for (int i = 0; i < 64; i++) {
       *(dram_ptr + i + 27 ) = 11468;
    }

    for (int i = 0; i < 64; i++) {
    //    *(dram_ptr + i + 27+64) =2;
    }
    for (int i = 0; i < 64; i++) {
      //  *(dram_ptr + i + 27+64 + 3*64*9-64) = 2;
    }

    weight_bias = load_weight(FPGA_SDRAM_HPS_AXI, 3*64*9);
    load_input(FPGA_SDRAM_HPS_AXI, 3*9);
    load_bias(FPGA_SDRAM_HPS_AXI+27*2, weight_bias);
    point_enable(0, 3, OUTPUT_SRAM_PE_ARRAY);
    output_back(OUTPUT_SRAM_DMA_READ, FPGA_SDRAM_HPS_AXI);
       
    for (int i = 0; i < 64; ++i) {
        temp2 = *(dram_ptr + i);
        printf("Data at %d is %d \n", i, temp2);
    }


    for (int i = 0; i < 28; ++i) {
        temp2 = *(dram_ptr +  0x08020000/2 + i);
        //printf("Input at %d is %d \n", i, temp2);
    }
    for (int i = 0; i < 3*64*9; i += 64) {
        temp2 = *(dram_ptr +  0x08000000/2 + i);
        //printf("weight at %d is %d \n", i, temp2);
    }
    for (int i = 0; i < 5; ++i) {
        temp2 = *(dram_ptr +  0x08000000/2 + 3*64*9 + i);
        printf("bias at %d is %d \n", i, temp2);
    }
    /*
    temp2 = *(dram_ptr + 0x08000000/2 + 3*64*9-2);
    printf("weight is %d \n",temp2);
    temp2 = *(dram_ptr + 0x08000000/2 + 3*64*9-1);
    printf("weight is %d \n",temp2);
    temp2 = *(dram_ptr + 0x08000000/2 + 3*64*9+1);
    printf("bias is %d \n",temp2); */
    
}

void dma_s2p(int conv_num, int weight_addr, int bias_addr, int input_addr, int input_channel, int output_addr_sdram, int output_addr_onchip) {}

int load_weight(int weight_addr, int data_size) {
    //Weight Transfer
    for (int i = 0; i < data_size/64; i++) {
        // Address for series2parrallel write
        *(CONTROLLER_status_ptr + 0x04) = S2P_WEIGHT_SRAM + i * 128;
        weight_bias = S2P_WEIGHT_SRAM + (i+1)*128;
        // DMA controller configuration
        // Read from SDRAM for weight data
        // Write to Weight On-chip Memory
        *(DMA_status_ptr)     = 0x00000000;
        *(DMA_status_ptr + 1) = weight_addr+ (27+64)*2 + i * 128;
        *(DMA_status_ptr + 2) = DMA_WRITE_S2P;
        *(DMA_status_ptr + 3) = 2 * 64;
        *(DMA_status_ptr + 6) = 0b10001010;
        while ((*(DMA_status_ptr) & 0x010) == 0) {WAIT;} 
    }
    printf("Finish transfer from SDRAM into Weight On-chip Memory\n");
    return weight_bias;
}

void load_input(int input_addr, int data_size) {
    // Input Data Transfer
    // DMA transfer from HPS_DDR3->FPGA_On_chip_memory
    // set up DMA
    // from https://www.intel.com/content/dam/www/programmable/us/en/pdfs/literature/ug/ug_embedded_ip.pdf
    // version: 19.1
    // section 29.4.3: Table 283: Control Regesiter bits
    // Stop (initialize) the dma at first
    *(DMA_status_ptr) = 0x00000000;
    // read bus_master gets data from DRAM addr = 0x0000_0000, setted by Qsys
    *(DMA_status_ptr + 1) = input_addr;
    // write bus_master for FPGA on-chip memory is mapped to 0x0800_0000, setted by Qsys
    *(DMA_status_ptr + 2) = INPUT_SRAM_DMA_WRITE;
    // 1 word = 4 bytes
    *(DMA_status_ptr + 3) = 2 * data_size;
    // From table 283:
    // set bit 1 for half-WORD transfer, set 0,1,2 digit to 1 according to data-width
	// set bit 3 to start DMA
    // set bit 4 to enalbe interrupt request(IRQ)
	// set bit 7 to stop on byte-count
    // Start DMA transfer
    gettimeofday(&t1, NULL);
    *(DMA_status_ptr + 6) = 0b10001010;
    while ((*(DMA_status_ptr) & 0x010) == 0) { WAIT; }
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_usec - t1.tv_usec);
    printf("Finish transfer from SDRAM to Input On-chip Memory in T=%.0f uSec \n", elapsedTime);
    printf("===============================\n\r");
}

/* ----------------------------------*/
// Function: Transfer bias data to weight On-chip Memory for each point calculation
void load_bias(int send_bias_addr, int rcv_bias_addr) {
    //Bias Transfer
    
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
void point_enable(int conv_num, int layer, int output_addr_onchip) {
    //Controller state machine 0: convolution number
    *(CONTROLLER_status_ptr + 0) = conv_num;
    //Controller state machine 2: input channel
    *(CONTROLLER_status_ptr + 2) = layer;
    //Controller state machine 3: output on-chip memory address
    *(CONTROLLER_status_ptr + 3) = output_addr_onchip;
    *(CONTROLLER_status_ptr + 7) = 1;
    *(CONTROLLER_status_ptr + 5) = 0;
    //Controller state machine 1: pe enable signal
    *(CONTROLLER_status_ptr + 1) = 1;
    

}

void output_back(int output_addr, int sdram_addr) {
    int result_signal;

    // Receive the signal from FPGA for current state
    while (*(CONTROLLER_status_ptr + 1) == 1){
        result_signal = *(CONTROLLER_status_ptr + 1);
    }
    result_signal = *(CONTROLLER_status_ptr + 1);
    printf("Result from PE controller: %d \n", result_signal);
    if (result_signal == 0) {
        // DMA controller configuration
        // Read from SDRAM for weight data
        // Write to Weight On-chip Memory
        *(DMA1_status_ptr + 0) = 0x00000000;
        *(DMA1_status_ptr + 1) = 0x08000000;
        *(DMA1_status_ptr + 2) = FPGA_SDRAM_HPS_AXI;
        *(DMA1_status_ptr + 3) = 2 * 64;
        *(DMA1_status_ptr + 6) = 0b10001010;
        while ((*(DMA1_status_ptr) & 0x010) == 0) {
            WAIT;
        } 
    }
}

/* ----------------------------------*/
// Function: Transfer given data to SDRAM
void sdram(volatile unsigned short * dram_ptr, int amount, short value) {
    gettimeofday(&t1, NULL);
    for (int i=0; i < amount; i++){
        *(dram_ptr + i) = value;
    }
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec);
    printf("Finishing transfer data to SDRAM within T=%.0f Sec \n", elapsedTime);
}
