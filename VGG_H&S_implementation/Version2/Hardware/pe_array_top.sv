module pe_array_top(input logic clk,
                    input logic rst_n,
                    input logic rst_n_pe,
                    input logic [3:0] conv_num,
                    input logic partial_en_control,
                    input logic [1023:0]data_readw_weight,
                    input logic [15:0]data_readi_input,
                    output logic [15:0]data_write_output,
                    output logic [16:0]addr_readi_input,
                    output logic [16:0]addr_readw_weight,
                    output logic [14:0]addr_write_output,
                    output logic [1:0]byteenable_readi_input,
                    output logic [127:0]byteenable_readw_weight,
		            output logic [1:0]byteenable_write_output,
                    input logic [16:0]addr_readi_control,
                    input logic [16:0]addr_readw_control,
                    input logic [14:0]addr_write_control,
                    input logic pool_en_control,
                    input logic relu_en_control,
                    input logic output_en_control,
		            input logic waitrequest_output,
		            input logic waitrequest_input,
		            input logic waitrequest_weight,
                    input logic en_write_control,
                    input logic en_readi_control,
                    input logic en_readw_control,
                    input logic readdatavalid_input,
                    input logic readdatavalid_weight,
                    output logic write_output,
                    output logic read_readw_weight,
                    output logic read_readi_input,
                    output logic chipselect_input,
                    output logic chipselect_weight,
                    output logic chipselect_output,
                    output logic readdatavalid_out_input,
                    output logic readdatavalid_out_weight
                    );

logic[1023:0]data_write_pe;
logic[1023:0]data_readw_pe;
logic[15:0]data_readi_pe;
logic data_valid_readi_pe;
logic data_valid_readw_pe;


p1024_to_s16 pts(.clk(clk),
			 	.rst_n(rst_n),
				.datain_pe(data_write_pe),
				.input_en_control(en_write_control),
				.addr_in_control(addr_write_control),
				.writedata_output(data_write_output),
				.address_output(addr_write_output),
				.write_output(write_output),
				.waitrequest_output(waitrequest_output),
				.byteenable_output(byteenable_write_output),
				.chipselect(chipselect_output),
				.pool_en(pool_en_control),
				.output_en(output_en_control)
                );


onchip_read_master_weight master_to_weight_bias(.clk(clk),
                                         .addr_read(addr_readw_weight),
                                         .data_read_input(data_readw_weight),
                                         .byteenable(byteenable_readw_weight),
                                         .read(read_readw_weight),
					                     .wait_request(waitrequest_weight),
                                         .addr_read_input(addr_readw_control),
                                         .read_en_input(en_readw_control),
                                         .rst_n(rst_n),
                                         .data_read(data_readw_pe),
                                         .readdatavalid(readdatavalid_weight),
                                         .data_valid(data_valid_readw_pe),
                                         .chipselect(chipselect_weight),
                                         .readdatavalid_out(readdatavalid_out_weight)
                                         );
onchip_read_master_input master_to_input(.clk(clk),
                                   .addr_read(addr_readi_input),
                                   .data_read_input(data_readi_input),
                                   .byteenable(byteenable_readi_input),
                                   .read(read_readi_input),
				   				   .wait_request(waitrequest_input),
                                   .addr_read_input(addr_readi_control),
                                   .read_en_input(en_readi_control),
                                   .rst_n(rst_n),
                                   .data_read(data_readi_pe),
                                   .readdatavalid(readdatavalid_input),
                                   .data_valid(data_valid_readi_pe),
                                   .chipselect(chipselect_input),
                                   .readdatavalid_out(readdatavalid_out_input)
                                   );
pe_array array(.clk(clk),
               .rst_n_pe(rst_n_pe),
               .feature_valid(data_valid_readi_pe),
	       	   .weight_valid(data_valid_readw_pe),
               .input_feature(data_readi_pe),
               .conv_num(conv_num),
               .output_feature(data_write_pe),
               .input_weight_bias(data_readw_pe),
               .relu_en(relu_en_control),
               .partial_en(partial_en_control));
               
endmodule 
