module pe_state_machine(
	input logic clk,
	input logic rst_n,
	//interface with HPS
	input logic write,
	input logic chipselect,
	input logic [31:0] writedata,
	output logic [31:0] readdata,
	input logic [3:0] address,
	//interface with pe array
	output logic [16:0]addr_input_pe,
	output logic [16:0]addr_weight_pe,
	output logic [14:0]addr_write_pe,
	output logic input_master_en,
	output logic weight_master_en,
	output logic output_master_en,
	output logic rst_n_pe,
	output logic [3:0]conv_num,
	output logic relu_en,
	output logic pool_en,
	output logic output_en,
	output logic partial_en,
	//interface with s2p buffer
	output logic [16:0]addr_s2p_buffer,
	input logic readdatavalid_input,
	input logic readdatavalid_weight
);

enum {idle,work,keep,input_end,keep1,keep2,keep3,keep4,work_init}state;
logic pe_work_en;
logic [10:0] conv_timer;
logic [10:0] dma_input_channel;
logic status_reg;
logic [16:0] addr_input_pe_in;
logic [16:0] addr_weight_pe_in;
logic [14:0] addr_write_pe_in;
logic [10:0] length_in;
logic [3:0] filter_size_in;
logic [16:0] addr_input_begin;
logic rst_n_pe_2;
assign rst_n_pe_2=rst_n_pe;
always_ff @(posedge clk)
begin
	if(rst_n == 0)
	begin
		pe_work_en <= 0;
		state <= idle;
		status_reg <= 1;
		conv_timer <= 0;
		conv_num <= 15;
		relu_en <= 0;
		output_en <= 0;
		pool_en <= 0;
		partial_en <= 0; 
	end
	else if(chipselect && write)
	begin
		case(address)
		4'd0: conv_num <= writedata[3:0]; 
		4'd1: pe_work_en <= writedata[0];
		4'd2: dma_input_channel <= writedata[10:0];
		4'd3: addr_write_pe_in <= writedata[14:0];
		4'd4: addr_s2p_buffer <= writedata[16:0];
		4'd5: relu_en <= writedata[0];
		4'd6: pool_en <= writedata[0];
		4'd7: output_en <= writedata[0];
		4'd8: partial_en <= writedata[0];
		4'd9: length_in <= writedata[10:0];
		4'd10: filter_size_in <= writedata[3:0];
		4'd11: addr_input_begin <= writedata[16:0];
		endcase
	end
	else if(chipselect && ~write)
	begin
		case(address)
		4'd0: readdata <= conv_timer;
		4'd1: readdata <= status_reg;
		4'd2: readdata <= dma_input_channel;
		4'd3: readdata <= addr_write_pe_in;
		4'd4: readdata <= state;
		4'd5: readdata <= pe_work_en;
		4'd6: readdata <= rst_n_pe_2;
		4'd7: readdata <= output_en;
		4'd8: readdata <= partial_en;
		4'd9: readdata <= length_in;
		4'd10: readdata <= filter_size_in;
		4'd11: readdata <= addr_input_begin;
		endcase
	end
	
	
	case (state)
		idle:
		begin
			if(pe_work_en==1)
			begin
				state <= work_init;
				status_reg <= 1;
				pe_work_en <= 0;
				conv_timer <= 0;
				addr_input_pe_in <= addr_input_begin;
				addr_weight_pe_in <=0;
			end
			else 
			begin
				state <= idle;
				rst_n_pe <= 0;
				conv_timer <= 0;
			end		
		end
		work_init:
		begin
			addr_input_pe <= addr_input_pe_in;
			addr_weight_pe <= addr_weight_pe_in;
			input_master_en <= 1;
			weight_master_en <= 1;
			if (readdatavalid_input == 1 && readdatavalid_weight == 1)
			begin
				state <= work;
			end
		end
		work:
		begin 
			if(conv_timer == 0)//all timing order need to be adjusted;
			begin
        		input_master_en <= 1;
				addr_input_pe <= addr_input_pe_in;
				addr_input_pe_in <= addr_input_pe_in+2;
				weight_master_en <= 1;
				addr_weight_pe <= addr_weight_pe_in;
				addr_weight_pe_in <= addr_weight_pe_in+128;
				rst_n_pe <= 0;
				state <= keep;
      		end   
			else if(conv_timer == 1)//all timing order need to be adjusted;
			begin
        		input_master_en <= 1;
				addr_input_pe <= addr_input_pe_in;
				addr_input_pe_in <= addr_input_pe_in+2;
				weight_master_en <= 1;
				addr_weight_pe <= addr_weight_pe_in;
				addr_weight_pe_in <= addr_weight_pe_in+128;
				rst_n_pe <= 0;
				state <= keep;
      		end
			else if(((conv_timer % (filter_size_in * dma_input_channel)) == (filter_size_in * dma_input_channel - 1)) && (conv_timer <(filter_size_in * filter_size_in *dma_input_channel - 1)))//all timing order need to be adjusted;
			begin
        		input_master_en <= 1;
				addr_input_pe <= addr_input_pe_in;
				addr_input_pe_in <= addr_input_pe_in + 2 + 2*(length_in * dma_input_channel - filter_size_in * dma_input_channel) ;
				weight_master_en <= 1;
				addr_weight_pe <= addr_weight_pe_in;
				addr_weight_pe_in <= addr_weight_pe_in+128;
				rst_n_pe <= 1;
				state <= keep;
      		end
			else if(conv_timer < 9* dma_input_channel )//all timing order need to be adjusted;
			begin
        		input_master_en <= 1;
				addr_input_pe <= addr_input_pe_in;
				addr_input_pe_in <= addr_input_pe_in+2;
				weight_master_en <= 1;
				addr_weight_pe <= addr_weight_pe_in;
				addr_weight_pe_in <= addr_weight_pe_in+128;
				rst_n_pe <= 1;
				state <= keep;
      		end
      		else if(conv_timer == (9* dma_input_channel))
      		begin
      			addr_weight_pe <=addr_weight_pe_in;
      			state <= keep;
      		end
      		else if(conv_timer == (9 * dma_input_channel + 1))
			begin
				input_master_en <= 0;
				addr_weight_pe <= addr_weight_pe_in ;
				state <= keep;
			end
			else if(conv_timer == (9 * dma_input_channel + 2))
			begin
				weight_master_en <= 0;
				state <= keep;
			end
		
			else if(conv_timer == (9 * dma_input_channel + 4))
			begin
				output_master_en <= 1;
				addr_write_pe <= addr_write_pe_in;
				state <= keep;
			end
		
			else if(conv_timer == (9 * dma_input_channel + 5))
			begin
				output_master_en <= 0;
				rst_n_pe <= 0;
				state <= keep;
			end	
		
			else if(conv_timer >= (9 * dma_input_channel + 68)) begin
				status_reg <= 0;
				state <= idle;
			end	
			else begin
				state <= keep;
			end
			conv_timer <= conv_timer + 1;
		end
		keep:
		begin
			state <= work;
			output_master_en <= 0;
		end
	endcase
	
end
endmodule
