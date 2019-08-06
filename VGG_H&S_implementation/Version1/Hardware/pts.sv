module p1024_to_s16(
	input logic clk,
    input logic rst_n,
	//interface to pe
	input logic [1023:0] datain_pe,
	input logic 		 input_en_control,
	//interface to controller
	input logic  [14:0] addr_in_control,
	input logic pool_en,
	input logic output_en,
	//avalon interface to output memory
	//input logic [15:0]writedata_input,
	output logic [15:0] writedata_output,	
	output logic [14:0] address_output,
	output logic        write_output,
	input  logic 		waitrequest_output,
	output logic [1:0]	byteenable_output,
	output logic chipselect
);
	logic [1023:0] buffer;
	logic [1023:0] buffer_out;
	logic [6:0]    i,q;
	logic [14:0]   addr_out_base;
	logic [14:0]   address_output_in;
	enum {IDLE,OUT,POOL}state;
	assign byteenable_output=2'b11;

	always_ff @(posedge clk)
	begin
		if(rst_n==0) begin
			buffer <= 1024'b0;
			state <= IDLE;
			write_output <= 1;
			chipselect <= 0;
			buffer_out <= 1024'b0;
		end
		else begin
			case(state)

				IDLE: begin
					if (input_en_control==1) begin
						buffer <= datain_pe;
						i	   <= 0;
						state  <= POOL;
						address_output_in <= addr_in_control;
						write_output <= 1;
					end
				end
				POOL:begin
					if(pool_en==1)begin
                        if (buffer[15:0] >buffer_out[15:0])
                        buffer_out[15:0] <= buffer[15:0];
                        if (buffer[31:16] >buffer_out[31:16])
                        buffer_out[31:16] <= buffer[31:16];
                        if (buffer[47:32] >buffer_out[47:32])
                        buffer_out[47:32] <= buffer[47:32];
                        if (buffer[63:48] >buffer_out[63:48])
                        buffer_out[63:48] <= buffer[63:48];
                        if (buffer[79:64] >buffer_out[79:64])
                        buffer_out[79:64] <= buffer[79:64];
                        if (buffer[95:80] >buffer_out[95:80])
                        buffer_out[95:80] <= buffer[95:80];
                        if (buffer[111:96] >buffer_out[111:96])
                        buffer_out[111:96] <= buffer[111:96];
                        if (buffer[127:112] >buffer_out[127:112])
                        buffer_out[127:112] <= buffer[127:112];
                        if (buffer[143:128] >buffer_out[143:128])
                        buffer_out[143:128] <= buffer[143:128];
                        if (buffer[159:144] >buffer_out[159:144])
                        buffer_out[159:144] <= buffer[159:144];
                        if (buffer[175:160] >buffer_out[175:160])
                        buffer_out[175:160] <= buffer[175:160];
                        if (buffer[191:176] >buffer_out[191:176])
                        buffer_out[191:176] <= buffer[191:176];
                        if (buffer[207:192] >buffer_out[207:192])
                        buffer_out[207:192] <= buffer[207:192];
                        if (buffer[223:208] >buffer_out[223:208])
                        buffer_out[223:208] <= buffer[223:208];
                        if (buffer[239:224] >buffer_out[239:224])
                        buffer_out[239:224] <= buffer[239:224];
                        if (buffer[255:240] >buffer_out[255:240])
                        buffer_out[255:240] <= buffer[255:240];
                        if (buffer[271:256] >buffer_out[271:256])
                        buffer_out[271:256] <= buffer[271:256];
                        if (buffer[287:272] >buffer_out[287:272])
                        buffer_out[287:272] <= buffer[287:272];
                        if (buffer[303:288] >buffer_out[303:288])
                        buffer_out[303:288] <= buffer[303:288];
                        if (buffer[319:304] >buffer_out[319:304])
                        buffer_out[319:304] <= buffer[319:304];
                        if (buffer[335:320] >buffer_out[335:320])
                        buffer_out[335:320] <= buffer[335:320];
                        if (buffer[351:336] >buffer_out[351:336])
                        buffer_out[351:336] <= buffer[351:336];
                        if (buffer[367:352] >buffer_out[367:352])
                        buffer_out[367:352] <= buffer[367:352];
                        if (buffer[383:368] >buffer_out[383:368])
                        buffer_out[383:368] <= buffer[383:368];
                        if (buffer[399:384] >buffer_out[399:384])
                        buffer_out[399:384] <= buffer[399:384];
                        if (buffer[415:400] >buffer_out[415:400])
                        buffer_out[415:400] <= buffer[415:400];
                        if (buffer[431:416] >buffer_out[431:416])
                        buffer_out[431:416] <= buffer[431:416];
                        if (buffer[447:432] >buffer_out[447:432])
                        buffer_out[447:432] <= buffer[447:432];
                        if (buffer[463:448] >buffer_out[463:448])
                        buffer_out[463:448] <= buffer[463:448];
                        if (buffer[479:464] >buffer_out[479:464])
                        buffer_out[479:464] <= buffer[479:464];
                        if (buffer[495:480] >buffer_out[495:480])
                        buffer_out[495:480] <= buffer[495:480];
                        if (buffer[511:496] >buffer_out[511:496])
                        buffer_out[511:496] <= buffer[511:496];
                        if (buffer[527:512] >buffer_out[527:512])
                        buffer_out[527:512] <= buffer[527:512];
                        if (buffer[543:528] >buffer_out[543:528])
                        buffer_out[543:528] <= buffer[543:528];
                        if (buffer[559:544] >buffer_out[559:544])
                        buffer_out[559:544] <= buffer[559:544];
                        if (buffer[575:560] >buffer_out[575:560])
                        buffer_out[575:560] <= buffer[575:560];
                        if (buffer[591:576] >buffer_out[591:576])
                        buffer_out[591:576] <= buffer[591:576];
                        if (buffer[607:592] >buffer_out[607:592])
                        buffer_out[607:592] <= buffer[607:592];
                        if (buffer[623:608] >buffer_out[623:608])
                        buffer_out[623:608] <= buffer[623:608];
                        if (buffer[639:624] >buffer_out[639:624])
                        buffer_out[639:624] <= buffer[639:624];
                        if (buffer[655:640] >buffer_out[655:640])
                        buffer_out[655:640] <= buffer[655:640];
                        if (buffer[671:656] >buffer_out[671:656])
                        buffer_out[671:656] <= buffer[671:656];
                        if (buffer[687:672] >buffer_out[687:672])
                        buffer_out[687:672] <= buffer[687:672];
                        if (buffer[703:688] >buffer_out[703:688])
                        buffer_out[703:688] <= buffer[703:688];
                        if (buffer[719:704] >buffer_out[719:704])
                        buffer_out[719:704] <= buffer[719:704];
                        if (buffer[735:720] >buffer_out[735:720])
                        buffer_out[735:720] <= buffer[735:720];
                        if (buffer[751:736] >buffer_out[751:736])
                        buffer_out[751:736] <= buffer[751:736];
                        if (buffer[767:752] >buffer_out[767:752])
                        buffer_out[767:752] <= buffer[767:752];
                        if (buffer[783:768] >buffer_out[783:768])
                        buffer_out[783:768] <= buffer[783:768];
                        if (buffer[799:784] >buffer_out[799:784])
                        buffer_out[799:784] <= buffer[799:784];
                        if (buffer[815:800] >buffer_out[815:800])
                        buffer_out[815:800] <= buffer[815:800];
                        if (buffer[831:816] >buffer_out[831:816])
                        buffer_out[831:816] <= buffer[831:816];
                        if (buffer[847:832] >buffer_out[847:832])
                        buffer_out[847:832] <= buffer[847:832];
                        if (buffer[863:848] >buffer_out[863:848])
                        buffer_out[863:848] <= buffer[863:848];
                        if (buffer[879:864] >buffer_out[879:864])
                        buffer_out[879:864] <= buffer[879:864];
                        if (buffer[895:880] >buffer_out[895:880])
                        buffer_out[895:880] <= buffer[895:880];
                        if (buffer[911:896] >buffer_out[911:896])
                        buffer_out[911:896] <= buffer[911:896];
                        if (buffer[927:912] >buffer_out[927:912])
                        buffer_out[927:912] <= buffer[927:912];
                        if (buffer[943:928] >buffer_out[943:928])
                        buffer_out[943:928] <= buffer[943:928];
                        if (buffer[959:944] >buffer_out[959:944])
                        buffer_out[959:944] <= buffer[959:944];
                        if (buffer[975:960] >buffer_out[975:960])
                        buffer_out[975:960] <= buffer[975:960];
                        if (buffer[991:976] >buffer_out[991:976])
                        buffer_out[991:976] <= buffer[991:976];
                        if (buffer[1007:992] >buffer_out[1007:992])
                        buffer_out[1007:992] <= buffer[1007:992];
                        if (buffer[1023:1008] >buffer_out[1023:1008])
                        buffer_out[1023:1008] <= buffer[1023:1008];
					end
					else buffer_out <= buffer;
					state <= OUT;
				end
				OUT: begin
					if (output_en==1)
					begin
						if (waitrequest_output == 0) begin
							address_output    <= address_output_in;
							address_output_in <= address_output_in+2;
							writedata_output  <= buffer_out[15:0];
							buffer_out            <= buffer_out >> 16;
							if (i >= 63) begin
								state <= IDLE;
								chipselect <= 0;
								buffer <= 1024'b0;
								buffer_out <= 1024'b0;
							end
							else begin
								chipselect <= 1;
							end
							i <= i + 1;
						end
					end
				end
			endcase
		end
	end
endmodule  
