# VGG16_FPGA_Accelerator
A VGG accelerator by SystemVerilog with 64 computation array used 16bits DSP on DE1-SoC FPGA. This accelerator contains two parts: Software control, as well as HPS, and Hardware convolution compututation. 

In Order to maximize the efficiency and due to the limited resource on DE1-SoC, only 13 convolution layers are placed on Board. Rest of computations, fully connected layer, are finished by software since it's cannot be done faster than hardware. By the way, the biggest problem is loading a huge weight file which is larger than 1GB in only 1GB DDR3 on software part. However, it can be solved in another way.
## Files Folder Introduction
This project contains several folders and all of them played the important roles during this project. <br> 

[VGG_C_implementation](https://github.com/WenqiJiang/VGG16_FPGA_Accelerator/tree/master/VGG_C_implementation) contains the files can run all computation on software. <br> 

[fc_C_implementation](https://github.com/WenqiJiang/VGG16_FPGA_Accelerator/tree/master/fc_C_implementation) is a temporary mode which is similar to final one. Only convolution computation is posted on FPGA. <br>

[VGG_H&S_implementation](https://github.com/WenqiJiang/VGG16_FPGA_Accelerator/tree/master/VGG_H%26S_implementation) is the `FINAL` project which has three version folders which all puts convolution parts on FPGA and FC layer on HPS part. In addition, functions for FC layer are used for reference in these folders.

As for these three versions, the only different between [Version 1](https://github.com/WenqiJiang/VGG16_FPGA_Accelerator/tree/master/VGG_H%26S_implementation/Version1) and [Version 2](https://github.com/WenqiJiang/VGG16_FPGA_Accelerator/tree/master/VGG_H%26S_implementation/Version2) is the numbers of data loaded each time. And Version 3 is following with Version 2 while it loads all weight, bias and input data into SDRAM and all FC layer weight data on DDR3 at first. In this way, this board could focus on computation instead of wasting a huge amount of time loading data.
