module weight_controller(
                         input logic [143:0] weight_in,
                         input logic [1:0]cal_state,
                         output logic [47:0]weight_line1,weight_line2,weight_line3
                         );
always_comb
begin
if (cal_state==0)
begin
weight_line1=weight_in[47:0];
weight_line2=weight_in[95:48];
weight_line3=weight_in[143:96];
end
else if (cal_state==1)
begin
weight_line2=weight_in[47:0];
weight_line3=weight_in[95:48];
weight_line1=weight_in[143:96];
end
else if (cal_state==2)
begin
weight_line3=weight_in[47:0];
weight_line1=weight_in[95:48];
weight_line2=weight_in[143:96];
end
end
endmodule