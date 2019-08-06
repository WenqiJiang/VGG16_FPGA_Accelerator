module pe_array(
	input logic clk,
	input logic rst_n_pe,
	input logic weight_valid,
	input logic feature_valid,
	input logic[15:0]input_feature,
	input logic[1023:0]input_weight_bias,
	input logic [3:0]conv_num,
	output logic[1023:0]output_feature,
	input logic relu_en,
	input logic partial_en
);

	genvar i;
	generate
		for (i=0; i<64; i=i+1)
			begin:mac
			mac_unit mac(
				.clk(clk),
				.rst_n_pe(rst_n_pe),
				.input_feature(input_feature),
				.input_weight_bias(input_weight_bias[(i+1)*16-1:(i)*16]),
				.output_feature(   output_feature   [(i+1)*16-1:(i)*16]),
				.conv_num(conv_num),                  
				.weight_valid(weight_valid),
				.feature_valid(feature_valid),
				.relu_en(relu_en),
				.partial_en(partial_en)
			);
		end
	endgenerate
endmodule


