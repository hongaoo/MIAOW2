`define FPGA_BUILD 1
`include "alu_definitions.v"
`include "decode_definitions.v"
`include "issue_definitions.v"
`include "lsu_definitions.v"
`include "global_definitions.v"
module reg_256x32b_3r_2w
(
  rd0_data, rd1_data, rd2_data,
  clk,
  rd0_addr, rd1_addr, rd2_addr,
  wr0_addr, wr1_addr,
  wr0_en, wr1_en, wr0_data, wr1_data
);
input clk;

output [31:0] rd0_data;
output [31:0] rd1_data;
output [31:0] rd2_data;

input [7:0] 	  rd0_addr;
input [7:0] 	  rd1_addr;
input [7:0] 	  rd2_addr;

input [7:0] 	  wr0_addr;
input [7:0] 	  wr1_addr;

input 	  wr0_en;
input 	  wr1_en;

input [31:0]   wr0_data;
input [31:0]   wr1_data;

wire [8191:0] word_out;
wire [8191:0] word_in;
wire [255:0] wr_en_word;

wire [255:0] wr0_word_select;
wire [255:0] wr1_word_select;
wire [255:0] wr0_word_enable;
wire [255:0] wr1_word_enable;

//Register file
flop_32b word[255:0](.out(word_out), .in(word_in), .wr_en(wr_en_word), .clk(clk));

//Muxes for read ports
mux_256x32b_to_1x32b mux_rd_port_0 (.out(rd0_data), .in(word_out), .select(rd0_addr));
mux_256x32b_to_1x32b mux_rd_port_1 (.out(rd1_data), .in(word_out), .select(rd1_addr));
mux_256x32b_to_1x32b mux_rd_port_2 (.out(rd2_data), .in(word_out), .select(rd2_addr));

//Write port logic
decoder_param #(8,256) decoder_wr_port_0 (.out(wr0_word_select), .in(wr0_addr));
decoder_param #(8,256) decoder_wr_port_1 (.out(wr1_word_select), .in(wr1_addr));
assign wr0_word_enable = {256{wr0_en}} & wr0_word_select;
assign wr1_word_enable = {256{wr1_en}} & wr1_word_select;
assign wr_en_word = wr0_word_enable | wr1_word_enable;

// %%start_veriperl
// my $i;
// my $low_index;
// my $high_index;
// for($i=0; $i<256; $i=$i+1)
// {
//   $low_index = 32*$i;
//   $high_index = 32*$i+31;
//   print "assign word_in[$high_index:$low_index] = ({wr1_word_enable[$i],wr0_word_enable[$i]} == 2'b01) ? wr0_data : (({wr1_word_enable[$i],wr0_word_enable[$i]} == 2'b10) ? wr1_data : {32{1'bx}});\n";
// }
// %%stop_veriperl
assign word_in[31:0] = ({wr1_word_enable[0],wr0_word_enable[0]} == 2'b01) ? wr0_data : (({wr1_word_enable[0],wr0_word_enable[0]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[63:32] = ({wr1_word_enable[1],wr0_word_enable[1]} == 2'b01) ? wr0_data : (({wr1_word_enable[1],wr0_word_enable[1]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[95:64] = ({wr1_word_enable[2],wr0_word_enable[2]} == 2'b01) ? wr0_data : (({wr1_word_enable[2],wr0_word_enable[2]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[127:96] = ({wr1_word_enable[3],wr0_word_enable[3]} == 2'b01) ? wr0_data : (({wr1_word_enable[3],wr0_word_enable[3]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[159:128] = ({wr1_word_enable[4],wr0_word_enable[4]} == 2'b01) ? wr0_data : (({wr1_word_enable[4],wr0_word_enable[4]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[191:160] = ({wr1_word_enable[5],wr0_word_enable[5]} == 2'b01) ? wr0_data : (({wr1_word_enable[5],wr0_word_enable[5]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[223:192] = ({wr1_word_enable[6],wr0_word_enable[6]} == 2'b01) ? wr0_data : (({wr1_word_enable[6],wr0_word_enable[6]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[255:224] = ({wr1_word_enable[7],wr0_word_enable[7]} == 2'b01) ? wr0_data : (({wr1_word_enable[7],wr0_word_enable[7]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[287:256] = ({wr1_word_enable[8],wr0_word_enable[8]} == 2'b01) ? wr0_data : (({wr1_word_enable[8],wr0_word_enable[8]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[319:288] = ({wr1_word_enable[9],wr0_word_enable[9]} == 2'b01) ? wr0_data : (({wr1_word_enable[9],wr0_word_enable[9]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[351:320] = ({wr1_word_enable[10],wr0_word_enable[10]} == 2'b01) ? wr0_data : (({wr1_word_enable[10],wr0_word_enable[10]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[383:352] = ({wr1_word_enable[11],wr0_word_enable[11]} == 2'b01) ? wr0_data : (({wr1_word_enable[11],wr0_word_enable[11]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[415:384] = ({wr1_word_enable[12],wr0_word_enable[12]} == 2'b01) ? wr0_data : (({wr1_word_enable[12],wr0_word_enable[12]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[447:416] = ({wr1_word_enable[13],wr0_word_enable[13]} == 2'b01) ? wr0_data : (({wr1_word_enable[13],wr0_word_enable[13]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[479:448] = ({wr1_word_enable[14],wr0_word_enable[14]} == 2'b01) ? wr0_data : (({wr1_word_enable[14],wr0_word_enable[14]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[511:480] = ({wr1_word_enable[15],wr0_word_enable[15]} == 2'b01) ? wr0_data : (({wr1_word_enable[15],wr0_word_enable[15]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[543:512] = ({wr1_word_enable[16],wr0_word_enable[16]} == 2'b01) ? wr0_data : (({wr1_word_enable[16],wr0_word_enable[16]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[575:544] = ({wr1_word_enable[17],wr0_word_enable[17]} == 2'b01) ? wr0_data : (({wr1_word_enable[17],wr0_word_enable[17]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[607:576] = ({wr1_word_enable[18],wr0_word_enable[18]} == 2'b01) ? wr0_data : (({wr1_word_enable[18],wr0_word_enable[18]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[639:608] = ({wr1_word_enable[19],wr0_word_enable[19]} == 2'b01) ? wr0_data : (({wr1_word_enable[19],wr0_word_enable[19]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[671:640] = ({wr1_word_enable[20],wr0_word_enable[20]} == 2'b01) ? wr0_data : (({wr1_word_enable[20],wr0_word_enable[20]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[703:672] = ({wr1_word_enable[21],wr0_word_enable[21]} == 2'b01) ? wr0_data : (({wr1_word_enable[21],wr0_word_enable[21]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[735:704] = ({wr1_word_enable[22],wr0_word_enable[22]} == 2'b01) ? wr0_data : (({wr1_word_enable[22],wr0_word_enable[22]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[767:736] = ({wr1_word_enable[23],wr0_word_enable[23]} == 2'b01) ? wr0_data : (({wr1_word_enable[23],wr0_word_enable[23]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[799:768] = ({wr1_word_enable[24],wr0_word_enable[24]} == 2'b01) ? wr0_data : (({wr1_word_enable[24],wr0_word_enable[24]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[831:800] = ({wr1_word_enable[25],wr0_word_enable[25]} == 2'b01) ? wr0_data : (({wr1_word_enable[25],wr0_word_enable[25]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[863:832] = ({wr1_word_enable[26],wr0_word_enable[26]} == 2'b01) ? wr0_data : (({wr1_word_enable[26],wr0_word_enable[26]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[895:864] = ({wr1_word_enable[27],wr0_word_enable[27]} == 2'b01) ? wr0_data : (({wr1_word_enable[27],wr0_word_enable[27]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[927:896] = ({wr1_word_enable[28],wr0_word_enable[28]} == 2'b01) ? wr0_data : (({wr1_word_enable[28],wr0_word_enable[28]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[959:928] = ({wr1_word_enable[29],wr0_word_enable[29]} == 2'b01) ? wr0_data : (({wr1_word_enable[29],wr0_word_enable[29]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[991:960] = ({wr1_word_enable[30],wr0_word_enable[30]} == 2'b01) ? wr0_data : (({wr1_word_enable[30],wr0_word_enable[30]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1023:992] = ({wr1_word_enable[31],wr0_word_enable[31]} == 2'b01) ? wr0_data : (({wr1_word_enable[31],wr0_word_enable[31]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1055:1024] = ({wr1_word_enable[32],wr0_word_enable[32]} == 2'b01) ? wr0_data : (({wr1_word_enable[32],wr0_word_enable[32]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1087:1056] = ({wr1_word_enable[33],wr0_word_enable[33]} == 2'b01) ? wr0_data : (({wr1_word_enable[33],wr0_word_enable[33]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1119:1088] = ({wr1_word_enable[34],wr0_word_enable[34]} == 2'b01) ? wr0_data : (({wr1_word_enable[34],wr0_word_enable[34]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1151:1120] = ({wr1_word_enable[35],wr0_word_enable[35]} == 2'b01) ? wr0_data : (({wr1_word_enable[35],wr0_word_enable[35]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1183:1152] = ({wr1_word_enable[36],wr0_word_enable[36]} == 2'b01) ? wr0_data : (({wr1_word_enable[36],wr0_word_enable[36]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1215:1184] = ({wr1_word_enable[37],wr0_word_enable[37]} == 2'b01) ? wr0_data : (({wr1_word_enable[37],wr0_word_enable[37]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1247:1216] = ({wr1_word_enable[38],wr0_word_enable[38]} == 2'b01) ? wr0_data : (({wr1_word_enable[38],wr0_word_enable[38]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1279:1248] = ({wr1_word_enable[39],wr0_word_enable[39]} == 2'b01) ? wr0_data : (({wr1_word_enable[39],wr0_word_enable[39]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1311:1280] = ({wr1_word_enable[40],wr0_word_enable[40]} == 2'b01) ? wr0_data : (({wr1_word_enable[40],wr0_word_enable[40]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1343:1312] = ({wr1_word_enable[41],wr0_word_enable[41]} == 2'b01) ? wr0_data : (({wr1_word_enable[41],wr0_word_enable[41]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1375:1344] = ({wr1_word_enable[42],wr0_word_enable[42]} == 2'b01) ? wr0_data : (({wr1_word_enable[42],wr0_word_enable[42]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1407:1376] = ({wr1_word_enable[43],wr0_word_enable[43]} == 2'b01) ? wr0_data : (({wr1_word_enable[43],wr0_word_enable[43]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1439:1408] = ({wr1_word_enable[44],wr0_word_enable[44]} == 2'b01) ? wr0_data : (({wr1_word_enable[44],wr0_word_enable[44]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1471:1440] = ({wr1_word_enable[45],wr0_word_enable[45]} == 2'b01) ? wr0_data : (({wr1_word_enable[45],wr0_word_enable[45]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1503:1472] = ({wr1_word_enable[46],wr0_word_enable[46]} == 2'b01) ? wr0_data : (({wr1_word_enable[46],wr0_word_enable[46]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1535:1504] = ({wr1_word_enable[47],wr0_word_enable[47]} == 2'b01) ? wr0_data : (({wr1_word_enable[47],wr0_word_enable[47]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1567:1536] = ({wr1_word_enable[48],wr0_word_enable[48]} == 2'b01) ? wr0_data : (({wr1_word_enable[48],wr0_word_enable[48]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1599:1568] = ({wr1_word_enable[49],wr0_word_enable[49]} == 2'b01) ? wr0_data : (({wr1_word_enable[49],wr0_word_enable[49]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1631:1600] = ({wr1_word_enable[50],wr0_word_enable[50]} == 2'b01) ? wr0_data : (({wr1_word_enable[50],wr0_word_enable[50]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1663:1632] = ({wr1_word_enable[51],wr0_word_enable[51]} == 2'b01) ? wr0_data : (({wr1_word_enable[51],wr0_word_enable[51]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1695:1664] = ({wr1_word_enable[52],wr0_word_enable[52]} == 2'b01) ? wr0_data : (({wr1_word_enable[52],wr0_word_enable[52]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1727:1696] = ({wr1_word_enable[53],wr0_word_enable[53]} == 2'b01) ? wr0_data : (({wr1_word_enable[53],wr0_word_enable[53]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1759:1728] = ({wr1_word_enable[54],wr0_word_enable[54]} == 2'b01) ? wr0_data : (({wr1_word_enable[54],wr0_word_enable[54]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1791:1760] = ({wr1_word_enable[55],wr0_word_enable[55]} == 2'b01) ? wr0_data : (({wr1_word_enable[55],wr0_word_enable[55]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1823:1792] = ({wr1_word_enable[56],wr0_word_enable[56]} == 2'b01) ? wr0_data : (({wr1_word_enable[56],wr0_word_enable[56]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1855:1824] = ({wr1_word_enable[57],wr0_word_enable[57]} == 2'b01) ? wr0_data : (({wr1_word_enable[57],wr0_word_enable[57]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1887:1856] = ({wr1_word_enable[58],wr0_word_enable[58]} == 2'b01) ? wr0_data : (({wr1_word_enable[58],wr0_word_enable[58]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1919:1888] = ({wr1_word_enable[59],wr0_word_enable[59]} == 2'b01) ? wr0_data : (({wr1_word_enable[59],wr0_word_enable[59]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1951:1920] = ({wr1_word_enable[60],wr0_word_enable[60]} == 2'b01) ? wr0_data : (({wr1_word_enable[60],wr0_word_enable[60]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[1983:1952] = ({wr1_word_enable[61],wr0_word_enable[61]} == 2'b01) ? wr0_data : (({wr1_word_enable[61],wr0_word_enable[61]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2015:1984] = ({wr1_word_enable[62],wr0_word_enable[62]} == 2'b01) ? wr0_data : (({wr1_word_enable[62],wr0_word_enable[62]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2047:2016] = ({wr1_word_enable[63],wr0_word_enable[63]} == 2'b01) ? wr0_data : (({wr1_word_enable[63],wr0_word_enable[63]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2079:2048] = ({wr1_word_enable[64],wr0_word_enable[64]} == 2'b01) ? wr0_data : (({wr1_word_enable[64],wr0_word_enable[64]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2111:2080] = ({wr1_word_enable[65],wr0_word_enable[65]} == 2'b01) ? wr0_data : (({wr1_word_enable[65],wr0_word_enable[65]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2143:2112] = ({wr1_word_enable[66],wr0_word_enable[66]} == 2'b01) ? wr0_data : (({wr1_word_enable[66],wr0_word_enable[66]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2175:2144] = ({wr1_word_enable[67],wr0_word_enable[67]} == 2'b01) ? wr0_data : (({wr1_word_enable[67],wr0_word_enable[67]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2207:2176] = ({wr1_word_enable[68],wr0_word_enable[68]} == 2'b01) ? wr0_data : (({wr1_word_enable[68],wr0_word_enable[68]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2239:2208] = ({wr1_word_enable[69],wr0_word_enable[69]} == 2'b01) ? wr0_data : (({wr1_word_enable[69],wr0_word_enable[69]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2271:2240] = ({wr1_word_enable[70],wr0_word_enable[70]} == 2'b01) ? wr0_data : (({wr1_word_enable[70],wr0_word_enable[70]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2303:2272] = ({wr1_word_enable[71],wr0_word_enable[71]} == 2'b01) ? wr0_data : (({wr1_word_enable[71],wr0_word_enable[71]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2335:2304] = ({wr1_word_enable[72],wr0_word_enable[72]} == 2'b01) ? wr0_data : (({wr1_word_enable[72],wr0_word_enable[72]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2367:2336] = ({wr1_word_enable[73],wr0_word_enable[73]} == 2'b01) ? wr0_data : (({wr1_word_enable[73],wr0_word_enable[73]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2399:2368] = ({wr1_word_enable[74],wr0_word_enable[74]} == 2'b01) ? wr0_data : (({wr1_word_enable[74],wr0_word_enable[74]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2431:2400] = ({wr1_word_enable[75],wr0_word_enable[75]} == 2'b01) ? wr0_data : (({wr1_word_enable[75],wr0_word_enable[75]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2463:2432] = ({wr1_word_enable[76],wr0_word_enable[76]} == 2'b01) ? wr0_data : (({wr1_word_enable[76],wr0_word_enable[76]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2495:2464] = ({wr1_word_enable[77],wr0_word_enable[77]} == 2'b01) ? wr0_data : (({wr1_word_enable[77],wr0_word_enable[77]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2527:2496] = ({wr1_word_enable[78],wr0_word_enable[78]} == 2'b01) ? wr0_data : (({wr1_word_enable[78],wr0_word_enable[78]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2559:2528] = ({wr1_word_enable[79],wr0_word_enable[79]} == 2'b01) ? wr0_data : (({wr1_word_enable[79],wr0_word_enable[79]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2591:2560] = ({wr1_word_enable[80],wr0_word_enable[80]} == 2'b01) ? wr0_data : (({wr1_word_enable[80],wr0_word_enable[80]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2623:2592] = ({wr1_word_enable[81],wr0_word_enable[81]} == 2'b01) ? wr0_data : (({wr1_word_enable[81],wr0_word_enable[81]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2655:2624] = ({wr1_word_enable[82],wr0_word_enable[82]} == 2'b01) ? wr0_data : (({wr1_word_enable[82],wr0_word_enable[82]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2687:2656] = ({wr1_word_enable[83],wr0_word_enable[83]} == 2'b01) ? wr0_data : (({wr1_word_enable[83],wr0_word_enable[83]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2719:2688] = ({wr1_word_enable[84],wr0_word_enable[84]} == 2'b01) ? wr0_data : (({wr1_word_enable[84],wr0_word_enable[84]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2751:2720] = ({wr1_word_enable[85],wr0_word_enable[85]} == 2'b01) ? wr0_data : (({wr1_word_enable[85],wr0_word_enable[85]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2783:2752] = ({wr1_word_enable[86],wr0_word_enable[86]} == 2'b01) ? wr0_data : (({wr1_word_enable[86],wr0_word_enable[86]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2815:2784] = ({wr1_word_enable[87],wr0_word_enable[87]} == 2'b01) ? wr0_data : (({wr1_word_enable[87],wr0_word_enable[87]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2847:2816] = ({wr1_word_enable[88],wr0_word_enable[88]} == 2'b01) ? wr0_data : (({wr1_word_enable[88],wr0_word_enable[88]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2879:2848] = ({wr1_word_enable[89],wr0_word_enable[89]} == 2'b01) ? wr0_data : (({wr1_word_enable[89],wr0_word_enable[89]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2911:2880] = ({wr1_word_enable[90],wr0_word_enable[90]} == 2'b01) ? wr0_data : (({wr1_word_enable[90],wr0_word_enable[90]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2943:2912] = ({wr1_word_enable[91],wr0_word_enable[91]} == 2'b01) ? wr0_data : (({wr1_word_enable[91],wr0_word_enable[91]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[2975:2944] = ({wr1_word_enable[92],wr0_word_enable[92]} == 2'b01) ? wr0_data : (({wr1_word_enable[92],wr0_word_enable[92]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3007:2976] = ({wr1_word_enable[93],wr0_word_enable[93]} == 2'b01) ? wr0_data : (({wr1_word_enable[93],wr0_word_enable[93]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3039:3008] = ({wr1_word_enable[94],wr0_word_enable[94]} == 2'b01) ? wr0_data : (({wr1_word_enable[94],wr0_word_enable[94]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3071:3040] = ({wr1_word_enable[95],wr0_word_enable[95]} == 2'b01) ? wr0_data : (({wr1_word_enable[95],wr0_word_enable[95]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3103:3072] = ({wr1_word_enable[96],wr0_word_enable[96]} == 2'b01) ? wr0_data : (({wr1_word_enable[96],wr0_word_enable[96]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3135:3104] = ({wr1_word_enable[97],wr0_word_enable[97]} == 2'b01) ? wr0_data : (({wr1_word_enable[97],wr0_word_enable[97]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3167:3136] = ({wr1_word_enable[98],wr0_word_enable[98]} == 2'b01) ? wr0_data : (({wr1_word_enable[98],wr0_word_enable[98]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3199:3168] = ({wr1_word_enable[99],wr0_word_enable[99]} == 2'b01) ? wr0_data : (({wr1_word_enable[99],wr0_word_enable[99]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3231:3200] = ({wr1_word_enable[100],wr0_word_enable[100]} == 2'b01) ? wr0_data : (({wr1_word_enable[100],wr0_word_enable[100]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3263:3232] = ({wr1_word_enable[101],wr0_word_enable[101]} == 2'b01) ? wr0_data : (({wr1_word_enable[101],wr0_word_enable[101]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3295:3264] = ({wr1_word_enable[102],wr0_word_enable[102]} == 2'b01) ? wr0_data : (({wr1_word_enable[102],wr0_word_enable[102]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3327:3296] = ({wr1_word_enable[103],wr0_word_enable[103]} == 2'b01) ? wr0_data : (({wr1_word_enable[103],wr0_word_enable[103]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3359:3328] = ({wr1_word_enable[104],wr0_word_enable[104]} == 2'b01) ? wr0_data : (({wr1_word_enable[104],wr0_word_enable[104]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3391:3360] = ({wr1_word_enable[105],wr0_word_enable[105]} == 2'b01) ? wr0_data : (({wr1_word_enable[105],wr0_word_enable[105]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3423:3392] = ({wr1_word_enable[106],wr0_word_enable[106]} == 2'b01) ? wr0_data : (({wr1_word_enable[106],wr0_word_enable[106]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3455:3424] = ({wr1_word_enable[107],wr0_word_enable[107]} == 2'b01) ? wr0_data : (({wr1_word_enable[107],wr0_word_enable[107]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3487:3456] = ({wr1_word_enable[108],wr0_word_enable[108]} == 2'b01) ? wr0_data : (({wr1_word_enable[108],wr0_word_enable[108]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3519:3488] = ({wr1_word_enable[109],wr0_word_enable[109]} == 2'b01) ? wr0_data : (({wr1_word_enable[109],wr0_word_enable[109]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3551:3520] = ({wr1_word_enable[110],wr0_word_enable[110]} == 2'b01) ? wr0_data : (({wr1_word_enable[110],wr0_word_enable[110]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3583:3552] = ({wr1_word_enable[111],wr0_word_enable[111]} == 2'b01) ? wr0_data : (({wr1_word_enable[111],wr0_word_enable[111]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3615:3584] = ({wr1_word_enable[112],wr0_word_enable[112]} == 2'b01) ? wr0_data : (({wr1_word_enable[112],wr0_word_enable[112]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3647:3616] = ({wr1_word_enable[113],wr0_word_enable[113]} == 2'b01) ? wr0_data : (({wr1_word_enable[113],wr0_word_enable[113]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3679:3648] = ({wr1_word_enable[114],wr0_word_enable[114]} == 2'b01) ? wr0_data : (({wr1_word_enable[114],wr0_word_enable[114]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3711:3680] = ({wr1_word_enable[115],wr0_word_enable[115]} == 2'b01) ? wr0_data : (({wr1_word_enable[115],wr0_word_enable[115]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3743:3712] = ({wr1_word_enable[116],wr0_word_enable[116]} == 2'b01) ? wr0_data : (({wr1_word_enable[116],wr0_word_enable[116]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3775:3744] = ({wr1_word_enable[117],wr0_word_enable[117]} == 2'b01) ? wr0_data : (({wr1_word_enable[117],wr0_word_enable[117]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3807:3776] = ({wr1_word_enable[118],wr0_word_enable[118]} == 2'b01) ? wr0_data : (({wr1_word_enable[118],wr0_word_enable[118]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3839:3808] = ({wr1_word_enable[119],wr0_word_enable[119]} == 2'b01) ? wr0_data : (({wr1_word_enable[119],wr0_word_enable[119]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3871:3840] = ({wr1_word_enable[120],wr0_word_enable[120]} == 2'b01) ? wr0_data : (({wr1_word_enable[120],wr0_word_enable[120]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3903:3872] = ({wr1_word_enable[121],wr0_word_enable[121]} == 2'b01) ? wr0_data : (({wr1_word_enable[121],wr0_word_enable[121]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3935:3904] = ({wr1_word_enable[122],wr0_word_enable[122]} == 2'b01) ? wr0_data : (({wr1_word_enable[122],wr0_word_enable[122]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3967:3936] = ({wr1_word_enable[123],wr0_word_enable[123]} == 2'b01) ? wr0_data : (({wr1_word_enable[123],wr0_word_enable[123]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[3999:3968] = ({wr1_word_enable[124],wr0_word_enable[124]} == 2'b01) ? wr0_data : (({wr1_word_enable[124],wr0_word_enable[124]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4031:4000] = ({wr1_word_enable[125],wr0_word_enable[125]} == 2'b01) ? wr0_data : (({wr1_word_enable[125],wr0_word_enable[125]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4063:4032] = ({wr1_word_enable[126],wr0_word_enable[126]} == 2'b01) ? wr0_data : (({wr1_word_enable[126],wr0_word_enable[126]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4095:4064] = ({wr1_word_enable[127],wr0_word_enable[127]} == 2'b01) ? wr0_data : (({wr1_word_enable[127],wr0_word_enable[127]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4127:4096] = ({wr1_word_enable[128],wr0_word_enable[128]} == 2'b01) ? wr0_data : (({wr1_word_enable[128],wr0_word_enable[128]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4159:4128] = ({wr1_word_enable[129],wr0_word_enable[129]} == 2'b01) ? wr0_data : (({wr1_word_enable[129],wr0_word_enable[129]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4191:4160] = ({wr1_word_enable[130],wr0_word_enable[130]} == 2'b01) ? wr0_data : (({wr1_word_enable[130],wr0_word_enable[130]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4223:4192] = ({wr1_word_enable[131],wr0_word_enable[131]} == 2'b01) ? wr0_data : (({wr1_word_enable[131],wr0_word_enable[131]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4255:4224] = ({wr1_word_enable[132],wr0_word_enable[132]} == 2'b01) ? wr0_data : (({wr1_word_enable[132],wr0_word_enable[132]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4287:4256] = ({wr1_word_enable[133],wr0_word_enable[133]} == 2'b01) ? wr0_data : (({wr1_word_enable[133],wr0_word_enable[133]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4319:4288] = ({wr1_word_enable[134],wr0_word_enable[134]} == 2'b01) ? wr0_data : (({wr1_word_enable[134],wr0_word_enable[134]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4351:4320] = ({wr1_word_enable[135],wr0_word_enable[135]} == 2'b01) ? wr0_data : (({wr1_word_enable[135],wr0_word_enable[135]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4383:4352] = ({wr1_word_enable[136],wr0_word_enable[136]} == 2'b01) ? wr0_data : (({wr1_word_enable[136],wr0_word_enable[136]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4415:4384] = ({wr1_word_enable[137],wr0_word_enable[137]} == 2'b01) ? wr0_data : (({wr1_word_enable[137],wr0_word_enable[137]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4447:4416] = ({wr1_word_enable[138],wr0_word_enable[138]} == 2'b01) ? wr0_data : (({wr1_word_enable[138],wr0_word_enable[138]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4479:4448] = ({wr1_word_enable[139],wr0_word_enable[139]} == 2'b01) ? wr0_data : (({wr1_word_enable[139],wr0_word_enable[139]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4511:4480] = ({wr1_word_enable[140],wr0_word_enable[140]} == 2'b01) ? wr0_data : (({wr1_word_enable[140],wr0_word_enable[140]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4543:4512] = ({wr1_word_enable[141],wr0_word_enable[141]} == 2'b01) ? wr0_data : (({wr1_word_enable[141],wr0_word_enable[141]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4575:4544] = ({wr1_word_enable[142],wr0_word_enable[142]} == 2'b01) ? wr0_data : (({wr1_word_enable[142],wr0_word_enable[142]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4607:4576] = ({wr1_word_enable[143],wr0_word_enable[143]} == 2'b01) ? wr0_data : (({wr1_word_enable[143],wr0_word_enable[143]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4639:4608] = ({wr1_word_enable[144],wr0_word_enable[144]} == 2'b01) ? wr0_data : (({wr1_word_enable[144],wr0_word_enable[144]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4671:4640] = ({wr1_word_enable[145],wr0_word_enable[145]} == 2'b01) ? wr0_data : (({wr1_word_enable[145],wr0_word_enable[145]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4703:4672] = ({wr1_word_enable[146],wr0_word_enable[146]} == 2'b01) ? wr0_data : (({wr1_word_enable[146],wr0_word_enable[146]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4735:4704] = ({wr1_word_enable[147],wr0_word_enable[147]} == 2'b01) ? wr0_data : (({wr1_word_enable[147],wr0_word_enable[147]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4767:4736] = ({wr1_word_enable[148],wr0_word_enable[148]} == 2'b01) ? wr0_data : (({wr1_word_enable[148],wr0_word_enable[148]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4799:4768] = ({wr1_word_enable[149],wr0_word_enable[149]} == 2'b01) ? wr0_data : (({wr1_word_enable[149],wr0_word_enable[149]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4831:4800] = ({wr1_word_enable[150],wr0_word_enable[150]} == 2'b01) ? wr0_data : (({wr1_word_enable[150],wr0_word_enable[150]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4863:4832] = ({wr1_word_enable[151],wr0_word_enable[151]} == 2'b01) ? wr0_data : (({wr1_word_enable[151],wr0_word_enable[151]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4895:4864] = ({wr1_word_enable[152],wr0_word_enable[152]} == 2'b01) ? wr0_data : (({wr1_word_enable[152],wr0_word_enable[152]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4927:4896] = ({wr1_word_enable[153],wr0_word_enable[153]} == 2'b01) ? wr0_data : (({wr1_word_enable[153],wr0_word_enable[153]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4959:4928] = ({wr1_word_enable[154],wr0_word_enable[154]} == 2'b01) ? wr0_data : (({wr1_word_enable[154],wr0_word_enable[154]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[4991:4960] = ({wr1_word_enable[155],wr0_word_enable[155]} == 2'b01) ? wr0_data : (({wr1_word_enable[155],wr0_word_enable[155]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5023:4992] = ({wr1_word_enable[156],wr0_word_enable[156]} == 2'b01) ? wr0_data : (({wr1_word_enable[156],wr0_word_enable[156]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5055:5024] = ({wr1_word_enable[157],wr0_word_enable[157]} == 2'b01) ? wr0_data : (({wr1_word_enable[157],wr0_word_enable[157]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5087:5056] = ({wr1_word_enable[158],wr0_word_enable[158]} == 2'b01) ? wr0_data : (({wr1_word_enable[158],wr0_word_enable[158]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5119:5088] = ({wr1_word_enable[159],wr0_word_enable[159]} == 2'b01) ? wr0_data : (({wr1_word_enable[159],wr0_word_enable[159]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5151:5120] = ({wr1_word_enable[160],wr0_word_enable[160]} == 2'b01) ? wr0_data : (({wr1_word_enable[160],wr0_word_enable[160]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5183:5152] = ({wr1_word_enable[161],wr0_word_enable[161]} == 2'b01) ? wr0_data : (({wr1_word_enable[161],wr0_word_enable[161]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5215:5184] = ({wr1_word_enable[162],wr0_word_enable[162]} == 2'b01) ? wr0_data : (({wr1_word_enable[162],wr0_word_enable[162]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5247:5216] = ({wr1_word_enable[163],wr0_word_enable[163]} == 2'b01) ? wr0_data : (({wr1_word_enable[163],wr0_word_enable[163]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5279:5248] = ({wr1_word_enable[164],wr0_word_enable[164]} == 2'b01) ? wr0_data : (({wr1_word_enable[164],wr0_word_enable[164]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5311:5280] = ({wr1_word_enable[165],wr0_word_enable[165]} == 2'b01) ? wr0_data : (({wr1_word_enable[165],wr0_word_enable[165]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5343:5312] = ({wr1_word_enable[166],wr0_word_enable[166]} == 2'b01) ? wr0_data : (({wr1_word_enable[166],wr0_word_enable[166]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5375:5344] = ({wr1_word_enable[167],wr0_word_enable[167]} == 2'b01) ? wr0_data : (({wr1_word_enable[167],wr0_word_enable[167]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5407:5376] = ({wr1_word_enable[168],wr0_word_enable[168]} == 2'b01) ? wr0_data : (({wr1_word_enable[168],wr0_word_enable[168]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5439:5408] = ({wr1_word_enable[169],wr0_word_enable[169]} == 2'b01) ? wr0_data : (({wr1_word_enable[169],wr0_word_enable[169]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5471:5440] = ({wr1_word_enable[170],wr0_word_enable[170]} == 2'b01) ? wr0_data : (({wr1_word_enable[170],wr0_word_enable[170]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5503:5472] = ({wr1_word_enable[171],wr0_word_enable[171]} == 2'b01) ? wr0_data : (({wr1_word_enable[171],wr0_word_enable[171]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5535:5504] = ({wr1_word_enable[172],wr0_word_enable[172]} == 2'b01) ? wr0_data : (({wr1_word_enable[172],wr0_word_enable[172]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5567:5536] = ({wr1_word_enable[173],wr0_word_enable[173]} == 2'b01) ? wr0_data : (({wr1_word_enable[173],wr0_word_enable[173]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5599:5568] = ({wr1_word_enable[174],wr0_word_enable[174]} == 2'b01) ? wr0_data : (({wr1_word_enable[174],wr0_word_enable[174]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5631:5600] = ({wr1_word_enable[175],wr0_word_enable[175]} == 2'b01) ? wr0_data : (({wr1_word_enable[175],wr0_word_enable[175]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5663:5632] = ({wr1_word_enable[176],wr0_word_enable[176]} == 2'b01) ? wr0_data : (({wr1_word_enable[176],wr0_word_enable[176]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5695:5664] = ({wr1_word_enable[177],wr0_word_enable[177]} == 2'b01) ? wr0_data : (({wr1_word_enable[177],wr0_word_enable[177]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5727:5696] = ({wr1_word_enable[178],wr0_word_enable[178]} == 2'b01) ? wr0_data : (({wr1_word_enable[178],wr0_word_enable[178]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5759:5728] = ({wr1_word_enable[179],wr0_word_enable[179]} == 2'b01) ? wr0_data : (({wr1_word_enable[179],wr0_word_enable[179]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5791:5760] = ({wr1_word_enable[180],wr0_word_enable[180]} == 2'b01) ? wr0_data : (({wr1_word_enable[180],wr0_word_enable[180]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5823:5792] = ({wr1_word_enable[181],wr0_word_enable[181]} == 2'b01) ? wr0_data : (({wr1_word_enable[181],wr0_word_enable[181]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5855:5824] = ({wr1_word_enable[182],wr0_word_enable[182]} == 2'b01) ? wr0_data : (({wr1_word_enable[182],wr0_word_enable[182]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5887:5856] = ({wr1_word_enable[183],wr0_word_enable[183]} == 2'b01) ? wr0_data : (({wr1_word_enable[183],wr0_word_enable[183]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5919:5888] = ({wr1_word_enable[184],wr0_word_enable[184]} == 2'b01) ? wr0_data : (({wr1_word_enable[184],wr0_word_enable[184]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5951:5920] = ({wr1_word_enable[185],wr0_word_enable[185]} == 2'b01) ? wr0_data : (({wr1_word_enable[185],wr0_word_enable[185]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[5983:5952] = ({wr1_word_enable[186],wr0_word_enable[186]} == 2'b01) ? wr0_data : (({wr1_word_enable[186],wr0_word_enable[186]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6015:5984] = ({wr1_word_enable[187],wr0_word_enable[187]} == 2'b01) ? wr0_data : (({wr1_word_enable[187],wr0_word_enable[187]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6047:6016] = ({wr1_word_enable[188],wr0_word_enable[188]} == 2'b01) ? wr0_data : (({wr1_word_enable[188],wr0_word_enable[188]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6079:6048] = ({wr1_word_enable[189],wr0_word_enable[189]} == 2'b01) ? wr0_data : (({wr1_word_enable[189],wr0_word_enable[189]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6111:6080] = ({wr1_word_enable[190],wr0_word_enable[190]} == 2'b01) ? wr0_data : (({wr1_word_enable[190],wr0_word_enable[190]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6143:6112] = ({wr1_word_enable[191],wr0_word_enable[191]} == 2'b01) ? wr0_data : (({wr1_word_enable[191],wr0_word_enable[191]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6175:6144] = ({wr1_word_enable[192],wr0_word_enable[192]} == 2'b01) ? wr0_data : (({wr1_word_enable[192],wr0_word_enable[192]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6207:6176] = ({wr1_word_enable[193],wr0_word_enable[193]} == 2'b01) ? wr0_data : (({wr1_word_enable[193],wr0_word_enable[193]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6239:6208] = ({wr1_word_enable[194],wr0_word_enable[194]} == 2'b01) ? wr0_data : (({wr1_word_enable[194],wr0_word_enable[194]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6271:6240] = ({wr1_word_enable[195],wr0_word_enable[195]} == 2'b01) ? wr0_data : (({wr1_word_enable[195],wr0_word_enable[195]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6303:6272] = ({wr1_word_enable[196],wr0_word_enable[196]} == 2'b01) ? wr0_data : (({wr1_word_enable[196],wr0_word_enable[196]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6335:6304] = ({wr1_word_enable[197],wr0_word_enable[197]} == 2'b01) ? wr0_data : (({wr1_word_enable[197],wr0_word_enable[197]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6367:6336] = ({wr1_word_enable[198],wr0_word_enable[198]} == 2'b01) ? wr0_data : (({wr1_word_enable[198],wr0_word_enable[198]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6399:6368] = ({wr1_word_enable[199],wr0_word_enable[199]} == 2'b01) ? wr0_data : (({wr1_word_enable[199],wr0_word_enable[199]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6431:6400] = ({wr1_word_enable[200],wr0_word_enable[200]} == 2'b01) ? wr0_data : (({wr1_word_enable[200],wr0_word_enable[200]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6463:6432] = ({wr1_word_enable[201],wr0_word_enable[201]} == 2'b01) ? wr0_data : (({wr1_word_enable[201],wr0_word_enable[201]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6495:6464] = ({wr1_word_enable[202],wr0_word_enable[202]} == 2'b01) ? wr0_data : (({wr1_word_enable[202],wr0_word_enable[202]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6527:6496] = ({wr1_word_enable[203],wr0_word_enable[203]} == 2'b01) ? wr0_data : (({wr1_word_enable[203],wr0_word_enable[203]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6559:6528] = ({wr1_word_enable[204],wr0_word_enable[204]} == 2'b01) ? wr0_data : (({wr1_word_enable[204],wr0_word_enable[204]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6591:6560] = ({wr1_word_enable[205],wr0_word_enable[205]} == 2'b01) ? wr0_data : (({wr1_word_enable[205],wr0_word_enable[205]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6623:6592] = ({wr1_word_enable[206],wr0_word_enable[206]} == 2'b01) ? wr0_data : (({wr1_word_enable[206],wr0_word_enable[206]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6655:6624] = ({wr1_word_enable[207],wr0_word_enable[207]} == 2'b01) ? wr0_data : (({wr1_word_enable[207],wr0_word_enable[207]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6687:6656] = ({wr1_word_enable[208],wr0_word_enable[208]} == 2'b01) ? wr0_data : (({wr1_word_enable[208],wr0_word_enable[208]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6719:6688] = ({wr1_word_enable[209],wr0_word_enable[209]} == 2'b01) ? wr0_data : (({wr1_word_enable[209],wr0_word_enable[209]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6751:6720] = ({wr1_word_enable[210],wr0_word_enable[210]} == 2'b01) ? wr0_data : (({wr1_word_enable[210],wr0_word_enable[210]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6783:6752] = ({wr1_word_enable[211],wr0_word_enable[211]} == 2'b01) ? wr0_data : (({wr1_word_enable[211],wr0_word_enable[211]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6815:6784] = ({wr1_word_enable[212],wr0_word_enable[212]} == 2'b01) ? wr0_data : (({wr1_word_enable[212],wr0_word_enable[212]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6847:6816] = ({wr1_word_enable[213],wr0_word_enable[213]} == 2'b01) ? wr0_data : (({wr1_word_enable[213],wr0_word_enable[213]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6879:6848] = ({wr1_word_enable[214],wr0_word_enable[214]} == 2'b01) ? wr0_data : (({wr1_word_enable[214],wr0_word_enable[214]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6911:6880] = ({wr1_word_enable[215],wr0_word_enable[215]} == 2'b01) ? wr0_data : (({wr1_word_enable[215],wr0_word_enable[215]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6943:6912] = ({wr1_word_enable[216],wr0_word_enable[216]} == 2'b01) ? wr0_data : (({wr1_word_enable[216],wr0_word_enable[216]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[6975:6944] = ({wr1_word_enable[217],wr0_word_enable[217]} == 2'b01) ? wr0_data : (({wr1_word_enable[217],wr0_word_enable[217]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7007:6976] = ({wr1_word_enable[218],wr0_word_enable[218]} == 2'b01) ? wr0_data : (({wr1_word_enable[218],wr0_word_enable[218]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7039:7008] = ({wr1_word_enable[219],wr0_word_enable[219]} == 2'b01) ? wr0_data : (({wr1_word_enable[219],wr0_word_enable[219]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7071:7040] = ({wr1_word_enable[220],wr0_word_enable[220]} == 2'b01) ? wr0_data : (({wr1_word_enable[220],wr0_word_enable[220]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7103:7072] = ({wr1_word_enable[221],wr0_word_enable[221]} == 2'b01) ? wr0_data : (({wr1_word_enable[221],wr0_word_enable[221]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7135:7104] = ({wr1_word_enable[222],wr0_word_enable[222]} == 2'b01) ? wr0_data : (({wr1_word_enable[222],wr0_word_enable[222]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7167:7136] = ({wr1_word_enable[223],wr0_word_enable[223]} == 2'b01) ? wr0_data : (({wr1_word_enable[223],wr0_word_enable[223]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7199:7168] = ({wr1_word_enable[224],wr0_word_enable[224]} == 2'b01) ? wr0_data : (({wr1_word_enable[224],wr0_word_enable[224]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7231:7200] = ({wr1_word_enable[225],wr0_word_enable[225]} == 2'b01) ? wr0_data : (({wr1_word_enable[225],wr0_word_enable[225]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7263:7232] = ({wr1_word_enable[226],wr0_word_enable[226]} == 2'b01) ? wr0_data : (({wr1_word_enable[226],wr0_word_enable[226]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7295:7264] = ({wr1_word_enable[227],wr0_word_enable[227]} == 2'b01) ? wr0_data : (({wr1_word_enable[227],wr0_word_enable[227]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7327:7296] = ({wr1_word_enable[228],wr0_word_enable[228]} == 2'b01) ? wr0_data : (({wr1_word_enable[228],wr0_word_enable[228]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7359:7328] = ({wr1_word_enable[229],wr0_word_enable[229]} == 2'b01) ? wr0_data : (({wr1_word_enable[229],wr0_word_enable[229]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7391:7360] = ({wr1_word_enable[230],wr0_word_enable[230]} == 2'b01) ? wr0_data : (({wr1_word_enable[230],wr0_word_enable[230]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7423:7392] = ({wr1_word_enable[231],wr0_word_enable[231]} == 2'b01) ? wr0_data : (({wr1_word_enable[231],wr0_word_enable[231]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7455:7424] = ({wr1_word_enable[232],wr0_word_enable[232]} == 2'b01) ? wr0_data : (({wr1_word_enable[232],wr0_word_enable[232]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7487:7456] = ({wr1_word_enable[233],wr0_word_enable[233]} == 2'b01) ? wr0_data : (({wr1_word_enable[233],wr0_word_enable[233]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7519:7488] = ({wr1_word_enable[234],wr0_word_enable[234]} == 2'b01) ? wr0_data : (({wr1_word_enable[234],wr0_word_enable[234]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7551:7520] = ({wr1_word_enable[235],wr0_word_enable[235]} == 2'b01) ? wr0_data : (({wr1_word_enable[235],wr0_word_enable[235]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7583:7552] = ({wr1_word_enable[236],wr0_word_enable[236]} == 2'b01) ? wr0_data : (({wr1_word_enable[236],wr0_word_enable[236]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7615:7584] = ({wr1_word_enable[237],wr0_word_enable[237]} == 2'b01) ? wr0_data : (({wr1_word_enable[237],wr0_word_enable[237]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7647:7616] = ({wr1_word_enable[238],wr0_word_enable[238]} == 2'b01) ? wr0_data : (({wr1_word_enable[238],wr0_word_enable[238]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7679:7648] = ({wr1_word_enable[239],wr0_word_enable[239]} == 2'b01) ? wr0_data : (({wr1_word_enable[239],wr0_word_enable[239]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7711:7680] = ({wr1_word_enable[240],wr0_word_enable[240]} == 2'b01) ? wr0_data : (({wr1_word_enable[240],wr0_word_enable[240]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7743:7712] = ({wr1_word_enable[241],wr0_word_enable[241]} == 2'b01) ? wr0_data : (({wr1_word_enable[241],wr0_word_enable[241]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7775:7744] = ({wr1_word_enable[242],wr0_word_enable[242]} == 2'b01) ? wr0_data : (({wr1_word_enable[242],wr0_word_enable[242]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7807:7776] = ({wr1_word_enable[243],wr0_word_enable[243]} == 2'b01) ? wr0_data : (({wr1_word_enable[243],wr0_word_enable[243]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7839:7808] = ({wr1_word_enable[244],wr0_word_enable[244]} == 2'b01) ? wr0_data : (({wr1_word_enable[244],wr0_word_enable[244]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7871:7840] = ({wr1_word_enable[245],wr0_word_enable[245]} == 2'b01) ? wr0_data : (({wr1_word_enable[245],wr0_word_enable[245]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7903:7872] = ({wr1_word_enable[246],wr0_word_enable[246]} == 2'b01) ? wr0_data : (({wr1_word_enable[246],wr0_word_enable[246]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7935:7904] = ({wr1_word_enable[247],wr0_word_enable[247]} == 2'b01) ? wr0_data : (({wr1_word_enable[247],wr0_word_enable[247]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7967:7936] = ({wr1_word_enable[248],wr0_word_enable[248]} == 2'b01) ? wr0_data : (({wr1_word_enable[248],wr0_word_enable[248]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[7999:7968] = ({wr1_word_enable[249],wr0_word_enable[249]} == 2'b01) ? wr0_data : (({wr1_word_enable[249],wr0_word_enable[249]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[8031:8000] = ({wr1_word_enable[250],wr0_word_enable[250]} == 2'b01) ? wr0_data : (({wr1_word_enable[250],wr0_word_enable[250]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[8063:8032] = ({wr1_word_enable[251],wr0_word_enable[251]} == 2'b01) ? wr0_data : (({wr1_word_enable[251],wr0_word_enable[251]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[8095:8064] = ({wr1_word_enable[252],wr0_word_enable[252]} == 2'b01) ? wr0_data : (({wr1_word_enable[252],wr0_word_enable[252]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[8127:8096] = ({wr1_word_enable[253],wr0_word_enable[253]} == 2'b01) ? wr0_data : (({wr1_word_enable[253],wr0_word_enable[253]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[8159:8128] = ({wr1_word_enable[254],wr0_word_enable[254]} == 2'b01) ? wr0_data : (({wr1_word_enable[254],wr0_word_enable[254]} == 2'b10) ? wr1_data : {32{1'bx}});
assign word_in[8191:8160] = ({wr1_word_enable[255],wr0_word_enable[255]} == 2'b01) ? wr0_data : (({wr1_word_enable[255],wr0_word_enable[255]} == 2'b10) ? wr1_data : {32{1'bx}});

endmodule
