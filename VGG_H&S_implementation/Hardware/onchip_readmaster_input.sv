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
	output logic data_valid,
	output logic readdatavalid_out
);
	enum {IDLE, READ, ENABLE_READ} state;
	assign byteenable = 2'b11;
	assign readdatavalid_out = readdatavalid;
	logic read_en_reg;
	always_ff @(posedge clk) 
	begin
		if (rst_n == 0) begin
			state <= IDLE;
			data_valid <= 1'b0;
			read <= 1'b0;
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
					state <= READ;
					chipselect <= 1;
				end
				else if(read_en_reg == 0) begin
					data_valid <= 1'b0;
					state <= IDLE;
					chipselect <= 0;
				end
			end
			
            READ:
            	begin 
					if (readdatavalid == 1 && wait_request == 0) begin
                        data_read <= data_read_input;
                        data_valid <= 1'b1;
                        
                        if (read_en_reg == 1) begin 
                            state <= READ;
                            chipselect <= 1;
                        end 
                        else begin 
                            state <= IDLE;
                            chipselect <= 0;
                        end
                        
                    end 
                    else begin 
                        data_valid <= 1'b0;
                        chipselect <= 0;
                    end
                end
			endcase
		end
	end
	
endmodule
	 
		
		
		
	 
	 
	 
    
	 
