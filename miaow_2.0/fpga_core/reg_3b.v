`define FPGA_BUILD 1
`include "alu_definitions.v"
`include "decode_definitions.v"
`include "issue_definitions.v"
`include "lsu_definitions.v"
`include "global_definitions.v"
module reg_3b (out, in, wr_en, clk, rst);

  output [2:0] out;
  input [2:0] in;
  input wr_en;
  input clk;
  input rst;

  dff_en bits[2:0] (.q(out), .d(in), .en(wr_en), .clk(clk), .rst(rst));

endmodule
