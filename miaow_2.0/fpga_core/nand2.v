`define FPGA_BUILD 1
`include "alu_definitions.v"
`include "decode_definitions.v"
`include "issue_definitions.v"
`include "lsu_definitions.v"
`include "global_definitions.v"
module nand2 (in1,in2,out);
input in1,in2;
output out;
assign out = ~(in1 & in2);
endmodule
