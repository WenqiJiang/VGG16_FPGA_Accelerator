`timescale 1ns/1ps
module mac_tb();
logic clk;
logic [15:0]inputa;
logic [15:0]inputb;
logic rst_n;
logic [15:0]mac_result;
logic output_en;
mac mac1(.clk(clk),.inputa(inputa),.inputb(inputb),.rst_n(rst_n),.mac_result(mac_result),.output_en(output_en));
initial
begin
clk=0;
rst_n=0;
inputa=16'h0;
inputb=16'h0;
output_en=0;
#35 rst_n=1;
end
initial
begin
forever #5
begin 
clk=~clk;
inputa=16'hffff;
inputb=16'hff12;
end
end
initial 
begin
#35
forever #30
output_en=~output_en;
end
endmodule
