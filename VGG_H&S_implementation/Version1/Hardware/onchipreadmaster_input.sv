module onchip_read_master_input(
	input logic clk,
	input logic rst_n,
	//avalon master interface
	output logic [10:0]addr_read,
	input  logic [15:0]data_read_input,
	output logic [1:0] byteenable,
	output logic 	   read,
	input  logic 	   wait_request,
	input  logic	   readdatavalid,
	output logic 	   chipselect,
	//interface to control
	input logic [10:0]addr_read_input,
	input logic read_en_input,
	//interface to pe array
	output logic [15:0]data_read,
	output logic data_valid
);
    enum {IDLE, READ,READ_RESULT} state;
    assign byteenable=2'b11;
    assign data_read = data_read_input;
    assign addr_read =addr_read_input;
    assign read_en_reg = read_en_input;
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
					read <= 1'b0;
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
