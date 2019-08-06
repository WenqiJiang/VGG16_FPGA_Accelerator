module mac_unit(input logic clk,
                input logic rst_n_pe,
				input shortint input_feature,
				input shortint input_weight_bias,
				input logic [3:0]conv_num,
				input logic feature_valid,
				input logic weight_valid,
				input logic relu_en,
				input logic partial_en,
				output shortint output_feature
				);

	longint mac_result;
	int ac_result;
	enum {idle,work,relu} state;

	always_ff @(posedge clk) begin

  		if (rst_n_pe==0) begin
	 		mac_result     = 0;
         	ac_result      = 0; 
	 		output_feature = 0;
	 		state = idle;
  		end
  		else
  		begin
  		case (state)
  		idle:begin
  			if(feature_valid && weight_valid) begin
				ac_result  = input_feature * input_weight_bias;
    			mac_result = mac_result + ac_result;         
  			end
  			else if(weight_valid) begin
  				if(partial_en == 1)
  				begin
  					case(conv_num)
						4'd0: ac_result = input_weight_bias * 65536;//should be 512
						4'd1: ac_result =  input_weight_bias * 65536;
						4'd2: ac_result = input_weight_bias * 32768;
						4'd3: ac_result = input_weight_bias *32768;
						4'd4: ac_result = input_weight_bias * 16384;
						4'd5: ac_result = input_weight_bias * 16384;
						4'd6: ac_result = input_weight_bias * 32768;
						4'd7: ac_result = input_weight_bias * 8192;
						4'd8: ac_result = input_weight_bias * 8192;
						4'd9: ac_result = input_weight_bias * 8192;
						4'd10: ac_result = input_weight_bias * 8192;
						4'd11: ac_result = input_weight_bias * 8192;
						4'd12: ac_result = input_weight_bias * 16384;
  					endcase
  				end
  				else
  				begin
  					case (conv_num)
						4'd0: ac_result = input_weight_bias*512;//should be 512
						4'd1: ac_result =  input_weight_bias*64;
						4'd2: ac_result =  input_weight_bias*8;
						4'd3: ac_result =  input_weight_bias*4;
						4'd4: ac_result =  input_weight_bias*2;
						4'd5: ac_result =  input_weight_bias*2;
						4'd6: ac_result =  input_weight_bias*2;
						4'd7: ac_result =  input_weight_bias*0;
						4'd8: ac_result =  input_weight_bias*2;
						4'd9: ac_result =  input_weight_bias*4;
						4'd10: ac_result =  input_weight_bias*8;
						4'd11: ac_result =  input_weight_bias*16;
						4'd12: ac_result =  input_weight_bias*512;
						4'd13: ac_result =  input_weight_bias;
						4'd14: ac_result =  input_weight_bias;
						4'd15: ac_result =  input_weight_bias;
					endcase
				end
				mac_result = mac_result + ac_result;
    			state = relu;
    		end
    	end
    	relu:begin
    		if (relu_en == 1)
    		begin
    			if (mac_result [31] == 1)
    			begin	
    				mac_result = 0;
    			end
    		end
    		state = work;
    	end
		work:begin
      		//need to be changed according to conv layer.
				case (conv_num)
				4'd0: output_feature = mac_result[31:16];
				4'd1: output_feature = mac_result[31:16];
				4'd2: output_feature = mac_result[30:15];
				4'd3: output_feature = mac_result[30:15];
				4'd4: output_feature = mac_result[29:14];
				4'd5: output_feature = mac_result[29:14];
				4'd6: output_feature = mac_result[30:15];
				4'd7: output_feature = mac_result[28:13];
				4'd8: output_feature = mac_result[28:13];
				4'd9: output_feature = mac_result[28:13];
				4'd10: output_feature = mac_result[28:13];
				4'd11: output_feature = mac_result[28:13];
				4'd12: output_feature = mac_result[29:14];
				4'd13: output_feature = mac_result[25:10];
				4'd14: output_feature = mac_result[27:12];
				4'd15: output_feature = mac_result[30:15];
				endcase
  		end
  		endcase
		end
	end
endmodule
