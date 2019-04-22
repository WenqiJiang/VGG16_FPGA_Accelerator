module pe_array(input logic clk,
                input logic [143:0]weight,
                input logic [111:0]feature,
                input logic [1:0]weight_control,
                input logic [2:0]rst_n,
                input logic [2:0]output_en_line,
                output logic [111:0]feature_out);
logic [47:0]weight_1;
logic [47:0]weight_2;
logic [47:0]weight_3;
logic [15:0]mac_weight_in1;
logic [15:0]mac_weight_in2;
logic [15:0]mac_weight_in3;
mac_array mac_array_1(.clk(clk),.rst_n(rst_n),.output_en_line(output_en_line),.feature_out(feature_out),.feature_in(feature),.weight_in1(mac_weight_in1),.weight_in2(mac_weight_in2),.weight_in3(mac_weight_in3));

weight_controller weight_controller1(.cal_state(weight_control),.weight_in(weight),.weight_line1(weight_1),.weight_line2(weight_2),.weight_line3(weight_3));

weight_buffer weight_buffer1(.clk(clk),.rst_n(rst_n[0]),.parallel_in(weight_1),.series_out(mac_weight_in1));

weight_buffer weight_buffer2(.clk(clk),.rst_n(rst_n[1]),.parallel_in(weight_2),.series_out(mac_weight_in2));

weight_buffer weight_buffer3(.clk(clk),.rst_n(rst_n[2]),.parallel_in(weight_3),.series_out(mac_weight_in3));

endmodule