module fc_pe(input logic clk,
            input logic [143:0]weight,
            input logic [111:0]feature,
            input logic [1:0]weight_control,
            input logic [2:0]rst_n,
            input logic [2:0]output_en_line,
            output logic [111:0]feature_out);
/* one Processing Element (PE) for Fully Connected (FC) layer, 
 * the size of which is 1 x 16
 * in this case, an input FM can be reused by 16 times */


endmodule