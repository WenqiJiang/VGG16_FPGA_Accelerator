module s16_to_p1024(
	input logic clk,
	input logic rst_n,
	//dma to s16_to_1024 avalon slave interface 
	input logic [15:0]  write_writedata_dma,
	input logic 		chipselect_dma,
	input logic [8:0]   write_address_dma,
	input logic 		write_write_n_dma,
	output logic 		write_waitrequest_dma,
	//s16_to_p1024 to weight memory master interface
	output logic [1023:0]  writedata_weight,
	output logic [16:0]	   address_weight,
	//output logic clken_weight,
	output logic 		   write_weight,
	output logic [127:0]   byteenable_weight,
	input logic 		   waitrequest_weight,
	//input address from controller to tell module the address to write data
	input logic [16:0]     address_in_control//controller send address to tell buffer the address to send data
);

	logic [1023:0]buffer;
	logic [5:0]i;
	enum {IDLE, OUT} state;
	assign  write_waitrequest_dma=0;//tell dma we will never stop working
	//assign clken_weight=1;//clock of slave will always be enabled
	assign byteenable_weight=128'hffffffffffffffffffffffffffffffff;//we always send 1024 bits to on chip memory
	//assign write_weight=1;//we always use write function
	always_ff @(posedge clk) begin
  		if(rst_n == 0) begin
  			buffer = 1024'b0;
			i=0;
			state=IDLE;
  		end
  		else begin
  			case(state)
				//Collective the data
				IDLE: begin
					if(chipselect_dma==1 && write_write_n_dma==0)
					begin
						buffer[1023:1008]=write_writedata_dma;
						if(i<63) begin
							i=i+1;
							buffer=buffer>>16;
						end
						else if (i==63) begin
							i=0;
							state=OUT;
						end
					end
				end
				//Send data colleciton into onchip memory
				OUT: begin
					write_weight=1;
					writedata_weight=buffer;
					buffer=1024'b0;
					address_weight=address_in_control;
					state=IDLE;
				end
  			endcase
  		end
	end
endmodule
