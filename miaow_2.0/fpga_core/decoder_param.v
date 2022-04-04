`define FPGA_BUILD 1
`include "alu_definitions.v"
`include "decode_definitions.v"
`include "issue_definitions.v"
`include "lsu_definitions.v"
`include "global_definitions.v"
module decoder_param (out, in);

   parameter BITS = 2;
   parameter SIZE = 4;

   output [SIZE - 1:0] out;
   input [BITS - 1:0] in;

   reg [SIZE -1: 0] out;
   integer i;

   always @ (in)
      begin
         for(i = 0; i < SIZE; i = i + 1)
         begin
            out[i] = (in == i) ? 1'b1 : 1'b0;
         end
      end

endmodule
