module onchip_read_master_input(
	input logic clk,
	input logic rst_n,
	//avalon master interface
	output logic [10:0]addr_read,
	input  logic [15:0]data_read_input,
	output logic [1:0] byteenable,
	output logic 	   read_en,
	input  logic	 	   wait_request,
	//interface to control
	input logic [10:0]addr_read_input,
	input logic read_en_input,
	//interface to pe array
	output logic [15:0]data_read,
	output logic data_valid
);
	enum {idle,read,read_wait,read_consec}state;
	assign byteenable=2'b11;
	always_ff @(posedge clk) begin

		if (rst_n == 0) begin
			state <= idle;
			data_valid <= 1'b0;
			read_en <= 1'b1;
		end

		else begin
			addr_read <= addr_read_input;
			case(state)
				idle:
				begin
					if (read_en_input == 1) begin 
						data_valid <= 1'b0;
						state <= read_wait;
					end
					else if(read_en_input == 0) begin
						data_valid <= 1'b0;
						state <= idle;
					end
				end
				read_wait:
				begin
					data_valid <= 0;
					if(read_en_input == 1) begin
						state <= read_consec;
					end
					else if(read_en_input == 0) begin
						state <= read;
					end
				end
				read_consec:
				begin
					data_read <= data_read_input;
					data_valid <= 1'b1;
					if(read_en_input == 1) begin
						state <= read_consec;
					end
					else if(read_en_input == 0) begin
						state <= read;
					end
				end
				read:
				begin
					data_read <= data_read_input;
					data_valid <= 1'b1;
					if(read_en_input == 1) begin
						state <= read_wait;
					end
					else if(read_en_input == 0) begin
						state <= idle;
					end
				end
			endcase
	end
	end
endmodule
	 
		
		
	 
	 
	 
    
	 
