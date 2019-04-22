module weight_buffer(input logic clk,
                   input logic rst_n,
                   input logic [47:0]parallel_in,
                   output logic [15:0]series_out);
logic [15:0]mem[2:0];
logic [1:0]counter;
always_ff@(posedge clk or negedge rst_n)
begin
if (rst_n!=1)
begin
mem[1]='b0;
mem[2]='b0;
mem[0]='b0;
counter='b0;
series_out='b0;
end
else 
begin
if (counter==0)
begin
mem[0]=parallel_in[15:0];
mem[1]=parallel_in[31:16];
mem[2]=parallel_in[47:32];
series_out=mem[0];
counter=counter+1;
end
else if (counter==1)
begin
series_out=mem[1];
counter=counter+1;
end
else if(counter==2)
begin
series_out=mem[2];
counter=0;
end
end
end
endmodule
