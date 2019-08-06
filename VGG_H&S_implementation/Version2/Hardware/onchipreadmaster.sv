module onchip_read_master_weight(//avalon interface
	input logic clk,
	input logic rst_n,
	output logic [16:0]  addr_read,
	input logic  [1023:0]data_read_input,
	output logic [127:0] byteenable,
	output logic         read,
	input  logic         wait_request,
	input  logic   	     readdatavalid,
	output logic chipselect,
	//interface to control
	input logic[16:0]addr_read_input,
	input logic read_en_input,
	//interface to pe array
	output logic [1023:0]data_read,
	output logic data_valid
);

    enum {IDLE, READ,READ_RESULT} state;
    assign byteenable=128'hffffffffffffffffffffffffffffffff;
    assign data_read = data_read_input;
	logic read_en_reg;
	always_ff @(posedge clk) 
	begin
		if (rst_n == 0) begin
			state <= IDLE;
			data_valid <= 1'b0;
			read <= 1'b0;
			chipselect <= 0;
		end
		else begin	
			addr_read <= addr_read_input;
			read_en_reg <=	read_en_input;
			
			case(state)
			
			IDLE:
			begin
				if (read_en_reg == 1) begin 
					data_valid <= 1'b0;
					read  <= 1'b1;
					state <= READ_RESULT;
					chipselect <= 1;
				end
				else if(read_en_reg == 0) begin
					data_valid <= 1'b0;
					state <= IDLE;
					chipselect <= 0;
				end
			end
			
            READ_RESULT:
            begin 
                data_valid <= 1'b1;
                if (read_en_reg == 1) begin 
                     state <= READ;
                     chipselect <= 0;
                end 
                else begin 
                    state <= IDLE;
                    chipselect <= 0;
                end
            end
            READ:
            begin
            	data_valid <= 0;
            	chipselect <= 1;
            	state <= READ_RESULT;
            end
            endcase
		end
	end
	
endmodule
