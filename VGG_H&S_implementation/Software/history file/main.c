#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <dirent.h>
#include <inttypes.h>
#include <sys/time.h>
#include <stdbool.h>
#include <pthread.h>
#include  "hps_0.h"
#include "math.h"
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"	
#include "socal/alt_gpio.h"
#include  "disp_config.h"
#include  "lib_bitmap.h"


#define SDRAM_BASE_ADDR 0
#define ALT_VIP_SOFTWARE_RESET_N_BASE 0x00000200   //

//#define ALT_STM_OFST (0xfc000000)
//#define ALT_LWFPGASLVS_OFST (0xff200000)  // axi_lw_master

#define ALT_AXI_FPGASLVS_OFST (0xC0000000)  // axi_master
#define HW_FPGA_AXI_SPAN (0x40000000)  // Bridge span
#define HW_FPGA_AXI_MASK ( HW_FPGA_AXI_SPAN - 1 )

#define ALT_GPIO1_BASE_OFST   (0xFF709000)

#define HW_REGS_BASE (ALT_STM_OFST)
#define HW_REGS_SPAN (0x04000000 )
#define HW_REGS_MASK (HW_REGS_SPAN - 1 )

#define HW_DMA0_BASE (ALT_STM_OFST)
#define HW_DMA0_SPAN (0X04000000)
#define HW_DMA0_MASK (HW_DMA0_SPAN - 1)


#define FILEADDRESS "/dev/mem"  //This place actually sets in the DDR3 on HPS part

static volatile unsigned long *h2p_lw_axi_addr=NULL;
static volatile unsigned long *dma_salve_addr=NULL;


int main(int argc,char ** argv) {
    void *lw_axi_virtual_base;
    int fd;

    if ((fd = open(FILEADDRESS, (O_RDWR | O_SYNC))) == -1) {
        printf("ERROR: could not open file \n");
        return (1);
    }

    lw_axi_virtual_base = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HW_DMA0_BASE);
    if (lw_axi_virtual_base == MAP_FAILED) {
        printf("ERROR: mmap() failed ...\n");
        close(fd);
        return(1);
    }

    h2p_lw_axi_addr = lw_axi_virtual_base + ((unsigned long)(ALT_LWFPGASLVS_OFST) & (unsigned long)(HW_DMA0_MASK));

    memcpy((void *) lw_axi_addr, (void *)  )

    if( munmap( lw_axi_virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	 }
	 close( fd );
	 return( 0 );

}

