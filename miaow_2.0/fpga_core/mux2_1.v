`define FPGA_BUILD 1
`include "alu_definitions.v"
`include "decode_definitions.v"
`include "issue_definitions.v"
`include "lsu_definitions.v"
`include "global_definitions.v"
module mux2_1(InA,InB,S,out);
input InA,InB,S;
output out;
wire a1,a2,n1;

not1 n5(S,n1);
nand2 n4(InA,n1,a1);
nand2 n2(InB,S,a2);
nand2 n3(a1,a2,out);

endmodule
