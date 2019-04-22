module pe_array_tb();
logic clk;
logic [143:0]weight;
logic [111:0]feature;
logic [1:0]weight_control;
logic [2:0]rst_n;
logic [2:0]output_en_line;
logic [111:0]feature_out;
pe_array pe_array_tb(.clk(clk),.weight(weight),.feature(feature),.weight_control(weight_control),.rst_n(rst_n),.output_en_line(output_en_line),.feature_out(feature_out));
initial
begin
clk=0;
weight=144'h010002000300040005000600070008000900;
feature=1112'h0100010001000100010001000100;
weight_control=0;
rst_n=0;
output_en_line=0;
end
initial
begin
forever #5 clk=~clk;
end
initial
begin
#50
rst_n=001;
#1000 
output_en_line=001;
end
endmodule

