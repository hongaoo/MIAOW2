`define FPGA_BUILD 1
`include "alu_definitions.v"
`include "decode_definitions.v"
`include "issue_definitions.v"
`include "lsu_definitions.v"
`include "global_definitions.v"
module dff_set_en (q, d, en, clk, set);

  output q;
  input d;
  input en;
  input clk;
  input set;

  wire d_int, q1;

  assign d_int = en ? d : q1;
  assign q = q1;

  dff_set dff_0(.q(q1), .d(d_int), .clk(clk), .set(set));
  
endmodule
