`define FPGA_BUILD 1
`include "alu_definitions.v"
`include "decode_definitions.v"
`include "issue_definitions.v"
`include "lsu_definitions.v"
`include "global_definitions.v"
module vgpr_busy_table_mux
  (
   in,
   select,
   out
   );
   parameter MAX_NUMBER_WORDS = 4;
   
   input[`NUMBER_VGPR-1:0] in;
   wire[`NUMBER_VGPR+4-1:0] in_rotated;
   input [`VGPR_ADDR_LENGTH-1:0] select;

   output [MAX_NUMBER_WORDS-1:0] out;

   assign in_rotated = {in[3:0], in};
   assign out = in_rotated >> select;
endmodule
