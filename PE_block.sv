
module PE1(	input 	logic clk,
		   	input 	logic pe_reset,
		   	input 	logic [143:0] input_fm,
		   	input 	logic [47:0]  filter,
		   	output 	logic [111:0] output_fm,
			output	logic output_en,
		   );
	/*	one PE contains 7 MACs, c.r.t. 7 output feature maps 
	 *	9 input FM = 16 * 9 = 144 
   	 *	3 input filter = 16 * 3 = 48 
   	 *	7 output FM = 16 * 7 = 112 */

	logic part_sum[111:0]; /* 7 output FM = 16 * 7 = 112 */
	/*	need at max 3 x 3 x 512 times of MAC per output, 
	 *	+ 3 x 512 times of reset MAC,
	 *	6144 < 2 ^ 13 = 8296 */
	logic count_output[12:0]; 
	logic count_mac[1:0]; /* control reset signal of macs */

	assign output_fm = part_sum;

	/* whether reset MAC result */
	always_ff @ (posedge clk) begin
		/* when count = 3 -> overflow to 0 */
		count_mac <= count_mac + 1'b1; 
		if (count_mac == 3) begin
			/*	when MAC finish a 3 * 1 convolution,
			 *	add partial sum to output */
			output_fm[111:96]	<= output_fm[111:96]	+ part_sum[111:96];
			output_fm[95:80] 	<= output_fm[95:80]		+ part_sum[95:80];
			output_fm[79:64]	<= output_fm[79:64]		+ part_sum[79:64];
			output_fm[63:48]	<= output_fm[63:48]		+ part_sum[63:48];
			output_fm[47:32]	<= output_fm[47:32]		+ part_sum[47:32];
			output_fm[31:16]	<= output_fm[31:16]		+ part_sum[31:16];
			output_fm[15:0]		<= output_fm[15:0]		+ part_sum[15:0];
		end
	end
	mac1(clk, count_mac[1:0], input_fm[143:96], flt[47:0], part_sum[111:96]);
	mac2(clk, count_mac[1:0], input_fm[127:80], flt[47:0], part_sum[95:80]);
	mac3(clk, count_mac[1:0], input_fm[111:64], flt[47:0], part_sum[79:64]);
	mac4(clk, count_mac[1:0], input_fm[95:48],  flt[47:0], part_sum[63:48]);
	mac5(clk, count_mac[1:0], input_fm[79:32],  flt[47:0], part_sum[47:32]);
	mac6(clk, count_mac[1:0], input_fm[63:16],  flt[47:0], part_sum[31:16]);
	mac7(clk, count_mac[1:0], input_fm[47:0],	flt[47:0], part_sum[15:0]);

	/* whether output result or continue computing partial sum */
	always_ff @ (posedge clk) begin
		count_output <= count_output + 1'b1;
		/* enable output & reset register every 3 x 4 x 512 = 6144 clock cycles */
		if (count_output == 6143)
			output_en <= 1;
		else if (count_output == 6144 || pe_reset) begin /* reset */
			output_en <= 1'b0;
			part_sum <= 112'b0;
			count_output <= 13'b0;
			count_mac <= 2'b0; end
	end

endmodule

module mac1(input 	logic	clk,
			input 	logic 	[1:0]	count_mac,
			input 	logic 	[47:0] 	in_fm,
			input 	logic 	[47:0] 	flt,
			output 	logic 	[15:0] 	result);
	/*	in_fm:	input feature map (3,1)
	 *	flt:		filter	(3,1)
	 *	result:  (1,1), output this value after 
	 *  			3 times of MAC operations */
				 
	logic [31:0]	mul_result;
	logic [15:0]	result; /* MAC result after 3 clock cycles */
	
	
	/*	count_mac = 0 -> in_fm[47:32] * flt[47:32]
	 *	count_mac = 1 -> in_fm[31:16] * flt[31:16]
	 *	count_mac = 2 -> in_fm[15:0]  * flt[15:0] 
	 *	count_mac = 3 -> reset */

	always_ff @(posedge clk or negedge mac_reset) begin
			if (count_mac == 2'b0) begin
				mul_result = in_fm[47:32] * flt[47:32];
				result <= result + mul_result[23:8]; end
			else if (count_mac == 2'b1) begin
				mul_result = in_fm[31:16] * flt[31:16];
				result <= result + mul_result[23:8]; end
			else if (count_mac == 2'b2) begin
				mul_result = in_fm[15:0] * flt[15:0];
				result <= result + mul_result[23:8]; end
			else begin /* count_mac == 3: reset */
				result <= 16'b0; end
	end
		
endmodule

module mac2(input 	logic	clk,
			input 	logic 	[1:0]	count_mac,
			input 	logic 	[47:0] 	in_fm,
			input 	logic 	[47:0] 	flt,
			output 	logic 	[15:0] 	result);
	/*	in_fm:	input feature map (3,1)
	 *	flt:		filter	(3,1)
	 *	result:  (1,1), output this value after 
	 *  			3 times of MAC operations */
				 
	logic [31:0]	mul_result;
	logic [15:0]	result; /* MAC result after 3 clock cycles */
	
	
	/*	count_mac = 0 -> in_fm[47:32] * flt[47:32]
	 *	count_mac = 1 -> in_fm[31:16] * flt[31:16]
	 *	count_mac = 2 -> in_fm[15:0]  * flt[15:0] 
	 *	count_mac = 3 -> reset */

	always_ff @(posedge clk or negedge mac_reset) begin
			if (count_mac == 2'b0) begin
				mul_result = in_fm[47:32] * flt[47:32];
				result <= result + mul_result[23:8]; end
			else if (count_mac == 2'b1) begin
				mul_result = in_fm[31:16] * flt[31:16];
				result <= result + mul_result[23:8]; end
			else if (count_mac == 2'b2) begin
				mul_result = in_fm[15:0] * flt[15:0];
				result <= result + mul_result[23:8]; end
			else begin /* count_mac == 3: reset */
				result <= 16'b0; end
	end
		
endmodule

module mac3(input 	logic	clk,
			input 	logic 	[1:0]	count_mac,
			input 	logic 	[47:0] 	in_fm,
			input 	logic 	[47:0] 	flt,
			output 	logic 	[15:0] 	result);
	/*	in_fm:	input feature map (3,1)
	 *	flt:		filter	(3,1)
	 *	result:  (1,1), output this value after 
	 *  			3 times of MAC operations */
				 
	logic [31:0]	mul_result;
	logic [15:0]	result; /* MAC result after 3 clock cycles */
	
	
	/*	count_mac = 0 -> in_fm[47:32] * flt[47:32]
	 *	count_mac = 1 -> in_fm[31:16] * flt[31:16]
	 *	count_mac = 2 -> in_fm[15:0]  * flt[15:0] 
	 *	count_mac = 3 -> reset */

	always_ff @(posedge clk or negedge mac_reset) begin
			if (count_mac == 2'b0) begin
				mul_result = in_fm[47:32] * flt[47:32];
				result <= result + mul_result[23:8]; end
			else if (count_mac == 2'b1) begin
				mul_result = in_fm[31:16] * flt[31:16];
				result <= result + mul_result[23:8]; end
			else if (count_mac == 2'b2) begin
				mul_result = in_fm[15:0] * flt[15:0];
				result <= result + mul_result[23:8]; end
			else begin /* count_mac == 3: reset */
				result <= 16'b0; end
	end
		
endmodule

module mac4(input 	logic	clk,
			input 	logic 	[1:0]	count_mac,
			input 	logic 	[47:0] 	in_fm,
			input 	logic 	[47:0] 	flt,
			output 	logic 	[15:0] 	result);
	/*	in_fm:	input feature map (3,1)
	 *	flt:		filter	(3,1)
	 *	result:  (1,1), output this value after 
	 *  			3 times of MAC operations */
				 
	logic [31:0]	mul_result;
	logic [15:0]	result; /* MAC result after 3 clock cycles */
	
	
	/*	count_mac = 0 -> in_fm[47:32] * flt[47:32]
	 *	count_mac = 1 -> in_fm[31:16] * flt[31:16]
	 *	count_mac = 2 -> in_fm[15:0]  * flt[15:0] 
	 *	count_mac = 3 -> reset */

	always_ff @(posedge clk or negedge mac_reset) begin
			if (count_mac == 2'b0) begin
				mul_result = in_fm[47:32] * flt[47:32];
				result <= result + mul_result[23:8]; end
			else if (count_mac == 2'b1) begin
				mul_result = in_fm[31:16] * flt[31:16];
				result <= result + mul_result[23:8]; end
			else if (count_mac == 2'b2) begin
				mul_result = in_fm[15:0] * flt[15:0];
				result <= result + mul_result[23:8]; end
			else begin /* count_mac == 3: reset */
				result <= 16'b0; end
	end
		
endmodule

module mac5(input 	logic	clk,
			input 	logic 	[1:0]	count_mac,
			input 	logic 	[47:0] 	in_fm,
			input 	logic 	[47:0] 	flt,
			output 	logic 	[15:0] 	result);
	/*	in_fm:	input feature map (3,1)
	 *	flt:		filter	(3,1)
	 *	result:  (1,1), output this value after 
	 *  			3 times of MAC operations */
				 
	logic [31:0]	mul_result;
	logic [15:0]	result; /* MAC result after 3 clock cycles */
	
	
	/*	count_mac = 0 -> in_fm[47:32] * flt[47:32]
	 *	count_mac = 1 -> in_fm[31:16] * flt[31:16]
	 *	count_mac = 2 -> in_fm[15:0]  * flt[15:0] 
	 *	count_mac = 3 -> reset */

	always_ff @(posedge clk or negedge mac_reset) begin
			if (count_mac == 2'b0) begin
				mul_result = in_fm[47:32] * flt[47:32];
				result <= result + mul_result[23:8]; end
			else if (count_mac == 2'b1) begin
				mul_result = in_fm[31:16] * flt[31:16];
				result <= result + mul_result[23:8]; end
			else if (count_mac == 2'b2) begin
				mul_result = in_fm[15:0] * flt[15:0];
				result <= result + mul_result[23:8]; end
			else begin /* count_mac == 3: reset */
				result <= 16'b0; end
	end
		
endmodule

module mac6(input 	logic	clk,
			input 	logic 	[1:0]	count_mac,
			input 	logic 	[47:0] 	in_fm,
			input 	logic 	[47:0] 	flt,
			output 	logic 	[15:0] 	result);
	/*	in_fm:	input feature map (3,1)
	 *	flt:		filter	(3,1)
	 *	result:  (1,1), output this value after 
	 *  			3 times of MAC operations */
				 
	logic [31:0]	mul_result;
	logic [15:0]	result; /* MAC result after 3 clock cycles */
	
	
	/*	count_mac = 0 -> in_fm[47:32] * flt[47:32]
	 *	count_mac = 1 -> in_fm[31:16] * flt[31:16]
	 *	count_mac = 2 -> in_fm[15:0]  * flt[15:0] 
	 *	count_mac = 3 -> reset */

	always_ff @(posedge clk or negedge mac_reset) begin
			if (count_mac == 2'b0) begin
				mul_result = in_fm[47:32] * flt[47:32];
				result <= result + mul_result[23:8]; end
			else if (count_mac == 2'b1) begin
				mul_result = in_fm[31:16] * flt[31:16];
				result <= result + mul_result[23:8]; end
			else if (count_mac == 2'b2) begin
				mul_result = in_fm[15:0] * flt[15:0];
				result <= result + mul_result[23:8]; end
			else begin /* count_mac == 3: reset */
				result <= 16'b0; end
	end
		
endmodule

module mac7(input 	logic	clk,
			input 	logic 	[1:0]	count_mac,
			input 	logic 	[47:0] 	in_fm,
			input 	logic 	[47:0] 	flt,
			output 	logic 	[15:0] 	result);
	/*	in_fm:	input feature map (3,1)
	 *	flt:		filter	(3,1)
	 *	result:  (1,1), output this value after 
	 *  			3 times of MAC operations */
				 
	logic [31:0]	mul_result;
	logic [15:0]	result; /* MAC result after 3 clock cycles */
	
	
	/*	count_mac = 0 -> in_fm[47:32] * flt[47:32]
	 *	count_mac = 1 -> in_fm[31:16] * flt[31:16]
	 *	count_mac = 2 -> in_fm[15:0]  * flt[15:0] 
	 *	count_mac = 3 -> reset */

	always_ff @(posedge clk or negedge mac_reset) begin
			if (count_mac == 2'b0) begin
				mul_result = in_fm[47:32] * flt[47:32];
				result <= result + mul_result[23:8]; end
			else if (count_mac == 2'b1) begin
				mul_result = in_fm[31:16] * flt[31:16];
				result <= result + mul_result[23:8]; end
			else if (count_mac == 2'b2) begin
				mul_result = in_fm[15:0] * flt[15:0];
				result <= result + mul_result[23:8]; end
			else begin /* count_mac == 3: reset */
				result <= 16'b0; end
	end
		
endmodule
