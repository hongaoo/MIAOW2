`define FPGA_BUILD 1
`include "alu_definitions.v"
`include "decode_definitions.v"
`include "issue_definitions.v"
`include "lsu_definitions.v"
`include "global_definitions.v"
module incr_wraparound_at40 (
	out_value,
	in_value
);

output[5:0] out_value;
input[5:0] in_value;

wire[5:0] inter_sum;
wire ign_cout;

adder6bit a6b(inter_sum, ign_cout, in_value, 6'b000001);

assign out_value = (inter_sum == 6'b101000) ? 6'b000000 : inter_sum;

endmodule
