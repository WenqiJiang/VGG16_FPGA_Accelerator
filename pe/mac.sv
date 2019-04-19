module mac(input logic clk,
           input logic [15:0]inputa,
           input logic [15:0]inputb,
           input logic rst_n,
           input logic output_en,
           output logic [15:0]mac_result
           );
    logic [31:0] multi_result;
    logic [31:0] ac_result;
    always_ff@(posedge clk or negedge rst_n)
    begin
      if(rst_n!=1)
      begin
        multi_result='b0;
        ac_result='b0;
        mac_result='b0;
      end
      else
      begin
        if(output_en==1)
        begin
          mac_result=ac_result[31:16];
          ac_result='b0;
        end
        else 
        begin
          multi_result=inputa*inputb;
          ac_result=ac_result+multi_result;
        end
      end
    end
endmodule
