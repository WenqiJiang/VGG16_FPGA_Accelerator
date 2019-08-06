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
/*
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
*/
#define BIAS_LAYER1 64*2
#define BIAS_LAYER2 BIAS_LAYER1+64*2
#define BIAS_LAYER3 BIAS_LAYER2+128*2
#define BIAS_LAYER4 BIAS_LAYER3+128*2
#define BIAS_LAYER5 BIAS_LAYER4+256*2
#define BIAS_LAYER6 BIAS_LAYER5+256*2
#define BIAS_LAYER7 BIAS_LAYER6+256*2
#define BIAS_LAYER8 BIAS_LAYER7+512*2
#define BIAS_LAYER9 BIAS_LAYER8+512*2
#define BIAS_LAYER10 BIAS_LAYER9+512*2
#define BIAS_LAYER11 BIAS_LAYER10+512*2
#define BIAS_LAYER12 BIAS_LAYER11+512*2
#define BIAS_LAYER13 BIAS_LAYER12+512*2

#define NUM_WEIGHT1 3*64*9

#define DATA_NUM_WEIGHT 14710464
#define DATA_NUM_BIAS   4224
#define DATA_NUM_INPUT  150528
#define DATA_NUM_INPUT_PAD 153228

// FC layer
#define FC_INPUT_SIZE 25088
#define FC_OUTPUT_SIZE 1000
#define FC1_DIM1 25088
#define FC1_DIM2 4096
#define FC2_DIM1 4096
#define FC2_DIM2 4096
#define FC3_DIM1 4096
#define FC3_DIM2 1000

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

void sdram(int addr_shift, int num, unsigned short *value);
int load_weight(int weight_addr, int data_size);
void load_input(int send_input_addr, int addr_shift, int data_size);
void load_bias(int send_bias_addr, int rcv_bias_addr);
void point_enable(int output_enable, int pool_enable, int relu_enable, int partial_enable, int conv_num, int layer, int output_addr_onchip);
void output_back(int onchip_addr, int sdram_addr, int data_size);
void addr_shift(int *shift, int base, int width, int input_channel);
void conv_relu(int input_channel, int output_channel, int width, int conv_num, int weight_addr, int bias_addr, int input_addr, 
               int output_addr_sdram);
void conv_pool(int input_channel, int output_channel, int width, int conv_num, int weight_addr, int bias_addr, int input_addr, 
               int output_addr_sdram);
void point_cal(int output_enable, int pool_enable, int relu_enable, int point_pos, int width, int channel, int in, int out, 
               int weight_bias, int output_addr_onchip_rcv, int input_addr, int output_addr_sdram, int bias_addr, int conv_num);
void padding_row(int attach_padding, int width, int output_sdram_addr);
void retrieve(unsigned short *result, int width, int output_channel, int output_sdram_addr);
void fixed_to_dec(unsigned short * data, int data_num, float* output, int point);