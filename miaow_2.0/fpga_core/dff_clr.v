`define FPGA_BUILD 1
`include "alu_definitions.v"
`include "decode_definitions.v"
`include "issue_definitions.v"
`include "lsu_definitions.v"
`include "global_definitions.v"
module dff_clr (q, d, clk, rst);

   output         q;
   input          d;
   input          clk;
   input          rst;

   reg            state;

   assign q = state;

   always @(posedge clk or posedge rst)
   begin
      state <= rst? 1 : d;
   end

`ifdef dump_flops
   always @(posedge gpu_tb.rst)
   begin
      if ($test$plusargs("dump_flops"))
      begin
         $display("%m.state");
      end
   end
`endif
endmodule

