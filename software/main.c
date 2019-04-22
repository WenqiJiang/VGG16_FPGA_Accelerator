///////////////////////////////////////
// DMA controller test
// Send data from HPS DDR3
// to FPGA On-chip Memory
// compile with
// gcc hps_v1.c -o hps_v1  -O3
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


#define ITER 4*1024*1024
#define ITER2 1024
/* ----------------------------------*/
//#define H2F_AXI_MASTER_BASE    0xC0000000
#define F2H_SDRAM_MASTER_BASE  0x00000000
// On-chip Memory on FPGA
// main bus; scratch RAM
//#define FPGA_ONCHIP_BASE       0xC0001000
#define FPGA_ONCHIP_BASE       0xC0000000
#define FPGA_ONCHIP_SPAN       0x04000000
// h2f bus
// RAM FPGA port s2
// main bus address 0x0000_1000
volatile unsigned int * dram_ptr = NULL;   //ocpram = on-chip-ram; ptr = pointer
void *dram_virtual_base;

/* ----------------------------------*/
// axi_lw_bus; DMA address
#define HW_REGS_BASE           0xff200000
#define HW_REGS_SPAN           0x00005000
#define DMA0                   0x00000000   //This address needs to be confirmed!!!!
#define DMA1                   0x00000020
#define DMA_STATUS_OFFSET      0x00
#define DMA_READ_ADDR_OFFSET   0x04         //NOTE: DATASHEET says 1 for this addr
#define DMA_WRTE_ADDR_OFFSET   0x08
#define DMA_LENGTH_OFFSET      0x012
#define DMA_CNTL_OFFSET        0x024
// h2f_axi_lw_bus base
void *h2f_lw_virtual_base;
// HPS_TO_FPGA DMA0 address = 0
volatile unsigned int * DMA0_status_ptr = NULL;
volatile unsigned int * DMA0_read_ptr   = NULL;
volatile unsigned int * DMA0_write_ptr  = NULL;
volatile unsigned int * DMA0_length_ptr = NULL;
volatile unsigned int * DMA0_cntl_ptr   = NULL;


/* ----------------------------------*/
// HPS DDR3 base and span
//#define HPS_DDR3_BASE          0x00000000
//#define HPS_DDR3_SPAN          0x3fffffff
//#define HPS_DDR3_SIZE          200*1024*1024        //200 
// HPS DDR3 memory
//volatile unsigned int *hps_ddr3_ptr = NULL;
//void *hps_ddr3_virtual_base;

// WAIT looks nicer than just braces
#define WAIT {}

int fd;

// timer variables
struct timeval t1, t2;
double elapsedTime;

int temp;

int main(void) {
    // Declare volatile pointers to I/O registers (volatile 	
	// means that IO load and store instructions will be used 	
	// to access these pointer locations, 
	// instead of regular memory loads and stores)

    /* ----------------------------------*/
    // get FPGA address
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
    printf("axi_lw successful\n");
    //DMA0 registers address shift to physical address
    DMA0_status_ptr = (unsigned int *) (h2f_lw_virtual_base);
    DMA0_read_ptr   = (unsigned int *) (h2f_lw_virtual_base + DMA_READ_ADDR_OFFSET);
    DMA0_write_ptr  = (unsigned int *) (h2f_lw_virtual_base + DMA_WRTE_ADDR_OFFSET);
    DMA0_length_ptr = (unsigned int *) (h2f_lw_virtual_base + DMA_LENGTH_OFFSET);
    DMA0_cntl_ptr   = (unsigned int *) (h2f_lw_virtual_base + DMA_CNTL_OFFSET);
    //DMA registers address shift to physical address
/*    DMA1_status_ptr = (unsigned int *) (h2f_lw_virtual_base                        + DMA1);
    DMA1_read_ptr   = (unsigned int *) (h2f_lw_virtual_base + DMA_READ_ADDR_OFFSET + DMA1);
    DMA1_write_ptr  = (unsigned int *) (h2f_lw_virtual_base + DMA_WRTE_ADDR_OFFSET + DMA1);
    DMA1_length_ptr = (unsigned int *) (h2f_lw_virtual_base + DMA_LENGTH_OFFSET    + DMA1);
    DMA1_cntl_ptr   = (unsigned int *) (h2f_lw_virtual_base + DMA_CNTL_OFFSET      + DMA1);
    printf("Address allocating for DMA0 & DMA1 successful");
*/    /* ----------------------------------*/
    // FPGA on-chip memory s2 port
    dram_virtual_base = mmap(NULL, FPGA_ONCHIP_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, FPGA_ONCHIP_BASE);
    
    if (dram_virtual_base == MAP_FAILED) {
        printf("ERROR: mmap(h2f) failed...\n");
        close(fd);
        return 1;
    }
    // Get address that maps to the DRAM buffer
    dram_ptr = (unsigned int *) (dram_virtual_base);

    // Write data into DRAM on FPGA
    printf("SDRAM virtual address is %p\n\n", dram_ptr);
    
    for (int i=0; i < 16*1024*1024; i++){
        *(dram_ptr + i) = 2141;
    }
    
    gettimeofday(&t1, NULL);
    for (int i=0; i < ITER; i++){
        *(dram_ptr + i) = i;
    }
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_usec - t1.tv_usec) / 1000;
    printf("HPS write SDRAM T=%.0f mSec \n", elapsedTime);

    temp = *(dram_ptr + 0xa);
    printf("十进制 %d   16进制 %x   \n", temp, temp);
    temp = *(dram_ptr + 10);
    printf("十进制 %d   16进制 %x   \n", temp, temp);

    printf("Write data into SDRAM successful\n");
    printf("Data at %p\n", dram_ptr);
    printf("===============================\n\r");

    // DMA0 transfer from HPS_DDR3->FPGA_On_chip_memory
    // set up DMA0
    // from https://www.intel.com/content/dam/www/programmable/us/en/pdfs/literature/ug/ug_embedded_ip.pdf
    // version: 19.1
    // section 29.4.3: Table 283: Control Regesiter bits
    // Stop (initialize) the dma at first
    *(DMA0_status_ptr) = 0x00000000;
    // read bus_master gets data from DRAM addr = 0x0000_0000, setted by Qsys
    *(DMA0_status_ptr + 1) = 0x00000000;
    // write bus_master for FPGA on-chip memory is mapped to 0x0800_0000, setted by Qsys
    *(DMA0_status_ptr + 2) = 0x08000000;
    // 1 word = 4 bytes
    *(DMA0_status_ptr + 3) = 2 * ITER2;
    // From table 283:
    // set bit 1 for half-WORD transfer, set 0,1,2 digit to 1 according to data-width
	// set bit 3 to start DMA0
	// set bit 7 to stop on byte-count   
    gettimeofday(&t1, NULL);
    *(DMA0_status_ptr + 6) = 0b10001010;
    printf("Start transfer from SDRAM to On-chip Memory\n");
    while ((*(DMA0_status_ptr) & 0x010) == 0) {
        WAIT;
    }
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_usec - t1.tv_usec);
    printf("SDRAM write to On-chip T=%.0f uSec \n", elapsedTime);
    printf("===============================\n\r");

    temp = *(dram_ptr + 8*1024*1024);
    printf("data at 0x03000000: %d\n", temp);

    // DMA0 transfer from HPS_DDR3->FPGA_On_chip_memory
    // set up DMA0
    // from https://www.intel.com/content/dam/www/programmable/us/en/pdfs/literature/ug/ug_embedded_ip.pdf
    // version: 19.1
    // section 29.4.3: Table 283: Control Regesiter bits
    *(DMA0_status_ptr) = 0;
    // read bus_master gets data from DRAM addr = 0x0000_0000, setted by Qsys
    *(DMA0_status_ptr + 1) = 0x08000000;
    // write bus_master for FPGA on-chip memory is mapped to 0x0800_0000, setted by Qsys
    *(DMA0_status_ptr + 2) = 0x02000000;
    // 1 word = 4 bytes
    *(DMA0_status_ptr + 3) = 2 * ITER2;
    // From table 283:
    //
    // set bit 2 for WORD transfer
	// set bit 3 to start DMA0
	// set bit 7 to stop on byte-count
    printf("Starting transfer from On-chip Memory to SDRAM\n");
    gettimeofday(&t1, NULL);
    *(DMA0_status_ptr + 6) = 0b10001010;
    while ((*(DMA0_status_ptr) & 0x010) == 0) {
        WAIT;
    }
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_usec - t1.tv_usec);
    printf("On-chip write to SDRAM T=%.0f uSec \n", elapsedTime);
    
    temp = *(dram_ptr + 8*1024*1024+1);
    printf("Data after at 0x03000000 is %d\n", temp);

}
