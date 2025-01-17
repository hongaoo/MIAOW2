`define FPGA_BUILD 1
`include "alu_definitions.v"
`include "decode_definitions.v"
`include "issue_definitions.v"
`include "lsu_definitions.v"
`include "global_definitions.v"
module regfile(
	d_in, 
	wr_en, 
	d_out, 
	clk, 
	rst
);  
  
parameter BITWIDTH = 11;

input [BITWIDTH-1:0] d_in;
input wr_en, clk, rst;

output [BITWIDTH-1:0] d_out;

wire [BITWIDTH-1:0] out;

dff d1[BITWIDTH-1:0](.q(d_out), .d(out), .clk(clk), .rst(rst));

mux2_1 m1[BITWIDTH-1:0](.InA(d_out), .InB(d_in), .out(out), .S(wr_en));
  
endmodule
