// ==================================================================
// Copyright (c) 2013 by Terasic Technologies Inc.
// ==================================================================
//
// Modified 2019 by Stephen A. Edwards
//
// Permission:
//
//   Terasic grants permission to use and modify this code for use in
//   synthesis for all Terasic Development Boards and Altera
//   Development Kits made by Terasic.  Other use of this code,
//   including the selling ,duplication, or modification of any
//   portion is strictly prohibited.
//
// Disclaimer:
//
//   This VHDL/Verilog or C/C++ source code is intended as a design
//   reference which illustrates how these types of functions can be
//   implemented.  It is the user's responsibility to verify their
//   design for consistency and functionality through the use of
//   formal verification methods.  Terasic provides no warranty
//   regarding the use or functionality of this code.
//
// ===============================================================
//           
//  Terasic Technologies Inc

//  9F., No.176, Sec.2, Gongdao 5th Rd, East Dist, Hsinchu City, 30070. Taiwan
//  
//  
//                     web: http://www.terasic.com/  
//                     email: support@terasic.com
module soc_system_top(
		      
 ///////// ADC /////////
 inout 	       ADC_CS_N,
 output        ADC_DIN,
 input 	       ADC_DOUT,
 output        ADC_SCLK,

 ///////// CLOCK2 /////////
 input 	       CLOCK2_50,

 ///////// CLOCK3 /////////
 input 	       CLOCK3_50,

 ///////// CLOCK4 /////////
 input 	       CLOCK4_50,

 ///////// CLOCK /////////
 input 	       CLOCK_50,

 ///////// DRAM /////////
 output [12:0] DRAM_ADDR,
 output [1:0]  DRAM_BA,
 output        DRAM_CAS_N,
 output        DRAM_CKE,
 output        DRAM_CLK,
 output        DRAM_CS_N,
 inout [15:0]  DRAM_DQ,
 output[1:0]   DRAM_DQM,
 output        DRAM_RAS_N,
 //output        DRAM_UDQM,
 output        DRAM_WE_N,

 ///////// FAN /////////
 output        FAN_CTRL,

 ///////// FPGA /////////
 output        FPGA_I2C_SCLK,
 inout 	       FPGA_I2C_SDAT,

 ///////// GPIO /////////
 inout [35:0]  GPIO_0,
 inout [35:0]  GPIO_1,

 ///////// HEX0 /////////
 output [6:0]  HEX0,

 ///////// HEX1 /////////
 output [6:0]  HEX1,

 ///////// HEX2 /////////
 output [6:0]  HEX2,

 ///////// HEX3 /////////
 output [6:0]  HEX3,

 ///////// HEX4 /////////
 output [6:0]  HEX4,

 ///////// HEX5 /////////
 output [6:0]  HEX5,

 ///////// HPS /////////
 inout 	       HPS_CONV_USB_N,
 output [14:0] HPS_DDR3_ADDR,
 output [2:0]  HPS_DDR3_BA,
 output        HPS_DDR3_CAS_N,
 output        HPS_DDR3_CKE,
 output        HPS_DDR3_CK_N,
 output        HPS_DDR3_CK_P,
 output        HPS_DDR3_CS_N,
 output [3:0]  HPS_DDR3_DM,
 inout [31:0]  HPS_DDR3_DQ,
 inout [3:0]   HPS_DDR3_DQS_N,
 inout [3:0]   HPS_DDR3_DQS_P,
 output        HPS_DDR3_ODT,
 output        HPS_DDR3_RAS_N,
 output        HPS_DDR3_RESET_N,
 input 	       HPS_DDR3_RZQ,
 output        HPS_DDR3_WE_N,
 output        HPS_ENET_GTX_CLK,
 inout 	       HPS_ENET_INT_N,
 output        HPS_ENET_MDC,
 inout 	       HPS_ENET_MDIO,
 input 	       HPS_ENET_RX_CLK,
 input [3:0]   HPS_ENET_RX_DATA,
 input 	       HPS_ENET_RX_DV,
 output [3:0]  HPS_ENET_TX_DATA,
 output        HPS_ENET_TX_EN,
 inout  [3:0]  HPS_FLASH_DATA,
 output        HPS_FLASH_DCLK,
 output        HPS_FLASH_NCSO,
 inout 	       HPS_GSENSOR_INT,
 inout 	       HPS_I2C1_SCLK,
 inout 	       HPS_I2C1_SDAT,
 inout 	       HPS_I2C2_SCLK,
 inout 	       HPS_I2C2_SDAT,
 inout 	       HPS_I2C_CONTROL,
 inout 	       HPS_KEY,
 inout 	       HPS_LED,
 inout         HPS_LTC_GPIO,
 output        HPS_SD_CLK,
 inout 	       HPS_SD_CMD,
 inout [3:0]   HPS_SD_DATA,
 output        HPS_SPIM_CLK,
 input 	       HPS_SPIM_MISO,
 output        HPS_SPIM_MOSI,
 inout 	       HPS_SPIM_SS,
 input 	       HPS_UART_RX,
 output        HPS_UART_TX,
 input 	       HPS_USB_CLKOUT,
 inout [7:0]   HPS_USB_DATA,
 input 	       HPS_USB_DIR,
 input 	       HPS_USB_NXT,
 output        HPS_USB_STP,

 ///////// IRDA /////////
 input 	       IRDA_RXD,
 output        IRDA_TXD,

 ///////// KEY /////////
 input [3:0]   KEY,

 ///////// LEDR /////////
 output [9:0]  LEDR,

 ///////// PS2 /////////
 inout 	       PS2_CLK,
 inout 	       PS2_CLK2,
 inout 	       PS2_DAT,
 inout 	       PS2_DAT2,

 ///////// SW /////////
 input [9:0]   SW,

 ///////// TD /////////
 input 	       TD_CLK27,
 input [7:0]   TD_DATA,
 input 	       TD_HS,
 output        TD_RESET_N,
 input 	       TD_VS

   ///////// SDRAM /////////
/*   output [13:0]   SDRAM_ADDR,
   output [1:0]    SDRAM_BA,
   output          SDRAM_CAS_N,
   output          SDRAM_CKE,
	output          SDRAM_CLK,
   output          SDRAM_CS_N,
   inout  [15:0]   SDRAM_DQ,
   output [1:0]    SDRAM_DQM,
   output          SDRAM_RAS_N,
   output          SDRAM_WE_N
*/
/*
   ///////// GIPO /////////
   input           HPS_HPS_IO_GPIO_INST_HLGPI0,
   input           HPS_HPS_IO_GPIO_INST_HLGPI1,
   input           HPS_HPS_IO_GPIO_INST_HLGPI2,
   input           HPS_HPS_IO_GPIO_INST_HLGPI3,
   input           HPS_HPS_IO_GPIO_INST_HLGPI4,
   input           HPS_HPS_IO_GPIO_INST_HLGPI5,
   input           HPS_HPS_IO_GPIO_INST_HLGPI6,
   input           HPS_HPS_IO_GPIO_INST_HLGPI7,
   input           HPS_HPS_IO_GPIO_INST_HLGPI8,
   input           HPS_HPS_IO_GPIO_INST_HLGPI9,
   input           HPS_HPS_IO_GPIO_INST_HLGPI10,
   input           HPS_HPS_IO_GPIO_INST_HLGPI11,
   input           HPS_HPS_IO_GPIO_INST_HLGPI12,
   input           HPS_HPS_IO_GPIO_INST_HLGPI13,
*//*
   ///////// ON_CHIP_FIFO /////////
   output [63:0]   FIFO_0_OUT_READDATA,
   input           FIFO_0_OUT_READ,
   output          FIFO_0_OUT_WAITREQUEST
*/
);

   soc_system soc_system0(
     .clk_clk                      ( CLOCK_50 ),
     .reset_reset_n                ( 1'b1 ),
	  .dram_clk_clk                 ( DRAM_CLK),
     //HPS ddr3             
     .hps_ddr3_mem_a               ( HPS_DDR3_ADDR    ),
     .hps_ddr3_mem_ba              ( HPS_DDR3_BA      ),
     .hps_ddr3_mem_ck              ( HPS_DDR3_CK_P    ),
     .hps_ddr3_mem_ck_n            ( HPS_DDR3_CK_N    ),
     .hps_ddr3_mem_cke             ( HPS_DDR3_CKE     ),
     .hps_ddr3_mem_cs_n            ( HPS_DDR3_CS_N    ),
     .hps_ddr3_mem_ras_n           ( HPS_DDR3_RAS_N   ),
     .hps_ddr3_mem_cas_n           ( HPS_DDR3_CAS_N   ),
     .hps_ddr3_mem_we_n            ( HPS_DDR3_WE_N    ),
     .hps_ddr3_mem_reset_n         ( HPS_DDR3_RESET_N ),
     .hps_ddr3_mem_dq              ( HPS_DDR3_DQ      ),
     .hps_ddr3_mem_dqs             ( HPS_DDR3_DQS_P   ),
     .hps_ddr3_mem_dqs_n           ( HPS_DDR3_DQS_N   ),
     .hps_ddr3_mem_odt             ( HPS_DDR3_ODT     ),
     .hps_ddr3_mem_dm              ( HPS_DDR3_DM      ),
     .hps_ddr3_oct_rzqin           ( HPS_DDR3_RZQ     ),
     //HPS ethernet
     .hps_hps_io_emac1_inst_TX_CLK ( HPS_ENET_GTX_CLK ),
     .hps_hps_io_emac1_inst_TXD0   ( HPS_ENET_TX_DATA[0] ),
     .hps_hps_io_emac1_inst_TXD1   ( HPS_ENET_TX_DATA[1] ),
     .hps_hps_io_emac1_inst_TXD2   ( HPS_ENET_TX_DATA[2] ),
     .hps_hps_io_emac1_inst_TXD3   ( HPS_ENET_TX_DATA[3] ),
     .hps_hps_io_emac1_inst_RXD0   ( HPS_ENET_RX_DATA[0] ),
     .hps_hps_io_emac1_inst_MDIO   ( HPS_ENET_MDIO  ),
     .hps_hps_io_emac1_inst_MDC    ( HPS_ENET_MDC   ),
     .hps_hps_io_emac1_inst_RX_CTL ( HPS_ENET_RX_DV ),
     .hps_hps_io_emac1_inst_TX_CTL ( HPS_ENET_TX_EN ),
     .hps_hps_io_emac1_inst_RX_CLK ( HPS_ENET_RX_CLK ),
     .hps_hps_io_emac1_inst_RXD1   ( HPS_ENET_RX_DATA[1]  ),
     .hps_hps_io_emac1_inst_RXD2   ( HPS_ENET_RX_DATA[2]  ),
     .hps_hps_io_emac1_inst_RXD3   ( HPS_ENET_RX_DATA[3]  ),
     //HPS QSPI
	  
	  .hps_hps_io_qspi_inst_IO0   ( HPS_FLASH_DATA[0]  ),    
     .hps_hps_io_qspi_inst_IO1   ( HPS_FLASH_DATA[1]  ),    
     .hps_hps_io_qspi_inst_IO2   ( HPS_FLASH_DATA[2]  ),    
     .hps_hps_io_qspi_inst_IO3   ( HPS_FLASH_DATA[3]  ),   
     .hps_hps_io_qspi_inst_SS0   ( HPS_FLASH_NCSO     ),       
     .hps_hps_io_qspi_inst_CLK   ( HPS_FLASH_DCLK     ),  
	  
     //HPS SD card        
     .hps_hps_io_sdio_inst_CMD     ( HPS_SD_CMD          ),
     .hps_hps_io_sdio_inst_D0      ( HPS_SD_DATA[0]      ),
     .hps_hps_io_sdio_inst_D1      ( HPS_SD_DATA[1]      ),
     .hps_hps_io_sdio_inst_CLK     ( HPS_SD_CLK          ),
     .hps_hps_io_sdio_inst_D2      ( HPS_SD_DATA[2]      ),
     .hps_hps_io_sdio_inst_D3      ( HPS_SD_DATA[3]      ),
     //HPS USB
     .hps_hps_io_usb1_inst_D0      ( HPS_USB_DATA[0]     ),
     .hps_hps_io_usb1_inst_D1      ( HPS_USB_DATA[1]     ),
     .hps_hps_io_usb1_inst_D2      ( HPS_USB_DATA[2]     ),
     .hps_hps_io_usb1_inst_D3      ( HPS_USB_DATA[3]     ),
     .hps_hps_io_usb1_inst_D4      ( HPS_USB_DATA[4]     ),
     .hps_hps_io_usb1_inst_D5      ( HPS_USB_DATA[5]     ),
     .hps_hps_io_usb1_inst_D6      ( HPS_USB_DATA[6]     ),
     .hps_hps_io_usb1_inst_D7      ( HPS_USB_DATA[7]     ),
     .hps_hps_io_usb1_inst_CLK     ( HPS_USB_CLKOUT      ),
     .hps_hps_io_usb1_inst_STP     ( HPS_USB_STP         ),
     .hps_hps_io_usb1_inst_DIR     ( HPS_USB_DIR         ),
     .hps_hps_io_usb1_inst_NXT     ( HPS_USB_NXT         ),
     //HPS SPI
     .hps_hps_io_spim1_inst_CLK    ( HPS_SPIM_CLK  ),
     .hps_hps_io_spim1_inst_MOSI   ( HPS_SPIM_MOSI ),
     .hps_hps_io_spim1_inst_MISO   ( HPS_SPIM_MISO ),
     .hps_hps_io_spim1_inst_SS0    ( HPS_SPIM_SS   ),
     //HPS UART
     .hps_hps_io_uart0_inst_RX     ( HPS_UART_RX     ),
     .hps_hps_io_uart0_inst_TX     ( HPS_UART_TX     ),
     //HPS I2C1
     .hps_hps_io_i2c0_inst_SDA     ( HPS_I2C1_SDAT     ),
     .hps_hps_io_i2c0_inst_SCL     ( HPS_I2C1_SCLK     ),
     //HPS I2C2
     .hps_hps_io_i2c1_inst_SDA     ( HPS_I2C2_SDAT     ),
     .hps_hps_io_i2c1_inst_SCL     ( HPS_I2C2_SCLK     ),
	  //FPGA SDRAM
	  .dram_addr                   (DRAM_ADDR), // sdram.addr
	  .dram_ba                     (DRAM_BA),//      .ba
	  .dram_cas_n                  (DRAM_CAS_N),//      .cas_n
	  .dram_cke                    (DRAM_CKE),//      .cke
	  .dram_cs_n                   (DRAM_CS_N),//      .cs_n
	  .dram_dq                     (DRAM_DQ),//      .dq
	  .dram_dqm                    (DRAM_DQM),//      .dqm
	  .dram_ras_n                  (DRAM_RAS_N),//      .ras_n
	  .dram_we_n                   (DRAM_WE_N),
     //HPS GPIO
     .hps_hps_io_gpio_inst_GPIO09  ( HPS_CONV_USB_N ),
     .hps_hps_io_gpio_inst_GPIO35  ( HPS_ENET_INT_N ),
     .hps_hps_io_gpio_inst_GPIO40  ( HPS_LTC_GPIO ),

     .hps_hps_io_gpio_inst_GPIO48  ( HPS_I2C_CONTROL ),
     .hps_hps_io_gpio_inst_GPIO53  ( HPS_LED ),
     .hps_hps_io_gpio_inst_GPIO54  ( HPS_KEY ),
     .hps_hps_io_gpio_inst_GPIO61  ( HPS_GSENSOR_INT )
    
  );

   // The following quiet the "no driver" warnings for output
   // pins and should be removed if you use any of these peripherals

   assign ADC_CS_N = SW[1] ? SW[0] : 1'bZ;
   assign ADC_DIN = SW[0];
   assign ADC_SCLK = SW[0];
   
   assign AUD_ADCLRCK = SW[1] ? SW[0] : 1'bZ;
   assign AUD_BCLK = SW[1] ? SW[0] : 1'bZ;
   assign AUD_DACDAT = SW[0];
   assign AUD_DACLRCK = SW[1] ? SW[0] : 1'bZ;
   assign AUD_XCK = SW[0];      
/*
   assign DRAM_ADDR = { 13{ SW[0] } };
   assign DRAM_BA = { 2{ SW[0] } };
   assign DRAM_DQ = SW[1] ? { 16{ SW[0] } } : 16'bZ;
   assign {DRAM_CAS_N, DRAM_CKE, DRAM_CLK, DRAM_CS_N, DRAM_LDQM, DRAM_RAS_N, DRAM_UDQM, DRAM_WE_N} = { 8{SW[0]} };
*/
   //assign DRAM_CLK = CLOCK_50;
	
   assign FAN_CTRL = SW[0];

   assign FPGA_I2C_SCLK = SW[0];
   assign FPGA_I2C_SDAT = SW[1] ? SW[0] : 1'bZ;

   assign GPIO_0 = SW[1] ? { 36{ SW[0] } } : 36'bZ;
   assign GPIO_1 = SW[1] ? { 36{ SW[0] } } : 36'bZ;

   assign HEX0 = 7'b1111001;
   assign HEX1 = 7'b0011001;
   assign HEX2 = 7'b1111001;
   assign HEX3 = 7'b0100100;
   assign HEX4 = 7'b1111111;
   assign HEX5 = 7'b1111111;

   assign IRDA_TXD = SW[0];

   assign LEDR = { 10{SW[7]} };

   assign PS2_CLK = SW[1] ? SW[0] : 1'bZ;
   assign PS2_CLK2 = SW[1] ? SW[0] : 1'bZ;
   assign PS2_DAT = SW[1] ? SW[0] : 1'bZ;
   assign PS2_DAT2 = SW[1] ? SW[0] : 1'bZ;

   assign TD_RESET_N = SW[0];
/*		
	logic	[15: 0]  hex3_hex0=0;
   //logic [15: 0]	hex5_hex4;	
   assign HEX4 = 7'b1111111;
   assign HEX5 = 7'b1111111;

   HexDigit Digit0(HEX0, hex3_hex0[3:0]);
   HexDigit Digit1(HEX1, hex3_hex0[7:4]);
   HexDigit Digit2(HEX2, hex3_hex0[11:8]);
   HexDigit Digit3(HEX3, hex3_hex0[15:12]);
	*/
endmodule
