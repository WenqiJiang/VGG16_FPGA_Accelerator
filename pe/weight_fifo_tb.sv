module weight_buffer_tb();
logic clk;
logic rst_n;
logic [47:0]parallel_in;
logic [15:0]series_out;
weight_fifo weight_fifo_t(.clk(clk),.rst_n(rst_n),.parallel_in(parallel_in),.series_out(series_out));
initial
begin
clk=0;
rst_n=0;
parallel_in='b0;
end
initial
begin
forever #5
begin 
clk=~clk;
parallel_in=parallel_in+100;
end
end
initial 
begin
#50
rst_n=1;
end
endmodule


