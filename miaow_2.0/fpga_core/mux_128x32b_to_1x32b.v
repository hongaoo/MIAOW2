`define FPGA_BUILD 1
`include "alu_definitions.v"
`include "decode_definitions.v"
`include "issue_definitions.v"
`include "lsu_definitions.v"
`include "global_definitions.v"
module mux_128x32b_to_1x32b (out, in, select);

  output [31:0] out;
  input [4095:0] in;
  input [6:0] select;

  reg [31:0] out;

  always @ (in or select) begin
    casex(select)
// %%start_veriperl
// my $i;
// my $low_index;
// my $high_index;
// for($i=0; $i<128; $i=$i+1)
// {
//   $low_index = 32*$i;
//   $high_index = 32*$i+31;
//   print "      7'd$i : out <= in [$high_index:$low_index];\n";
// }
// %%stop_veriperl
      7'd0 : out <= in [31:0];
      7'd1 : out <= in [63:32];
      7'd2 : out <= in [95:64];
      7'd3 : out <= in [127:96];
      7'd4 : out <= in [159:128];
      7'd5 : out <= in [191:160];
      7'd6 : out <= in [223:192];
      7'd7 : out <= in [255:224];
      7'd8 : out <= in [287:256];
      7'd9 : out <= in [319:288];
      7'd10 : out <= in [351:320];
      7'd11 : out <= in [383:352];
      7'd12 : out <= in [415:384];
      7'd13 : out <= in [447:416];
      7'd14 : out <= in [479:448];
      7'd15 : out <= in [511:480];
      7'd16 : out <= in [543:512];
      7'd17 : out <= in [575:544];
      7'd18 : out <= in [607:576];
      7'd19 : out <= in [639:608];
      7'd20 : out <= in [671:640];
      7'd21 : out <= in [703:672];
      7'd22 : out <= in [735:704];
      7'd23 : out <= in [767:736];
      7'd24 : out <= in [799:768];
      7'd25 : out <= in [831:800];
      7'd26 : out <= in [863:832];
      7'd27 : out <= in [895:864];
      7'd28 : out <= in [927:896];
      7'd29 : out <= in [959:928];
      7'd30 : out <= in [991:960];
      7'd31 : out <= in [1023:992];
      7'd32 : out <= in [1055:1024];
      7'd33 : out <= in [1087:1056];
      7'd34 : out <= in [1119:1088];
      7'd35 : out <= in [1151:1120];
      7'd36 : out <= in [1183:1152];
      7'd37 : out <= in [1215:1184];
      7'd38 : out <= in [1247:1216];
      7'd39 : out <= in [1279:1248];
      7'd40 : out <= in [1311:1280];
      7'd41 : out <= in [1343:1312];
      7'd42 : out <= in [1375:1344];
      7'd43 : out <= in [1407:1376];
      7'd44 : out <= in [1439:1408];
      7'd45 : out <= in [1471:1440];
      7'd46 : out <= in [1503:1472];
      7'd47 : out <= in [1535:1504];
      7'd48 : out <= in [1567:1536];
      7'd49 : out <= in [1599:1568];
      7'd50 : out <= in [1631:1600];
      7'd51 : out <= in [1663:1632];
      7'd52 : out <= in [1695:1664];
      7'd53 : out <= in [1727:1696];
      7'd54 : out <= in [1759:1728];
      7'd55 : out <= in [1791:1760];
      7'd56 : out <= in [1823:1792];
      7'd57 : out <= in [1855:1824];
      7'd58 : out <= in [1887:1856];
      7'd59 : out <= in [1919:1888];
      7'd60 : out <= in [1951:1920];
      7'd61 : out <= in [1983:1952];
      7'd62 : out <= in [2015:1984];
      7'd63 : out <= in [2047:2016];
      7'd64 : out <= in [2079:2048];
      7'd65 : out <= in [2111:2080];
      7'd66 : out <= in [2143:2112];
      7'd67 : out <= in [2175:2144];
      7'd68 : out <= in [2207:2176];
      7'd69 : out <= in [2239:2208];
      7'd70 : out <= in [2271:2240];
      7'd71 : out <= in [2303:2272];
      7'd72 : out <= in [2335:2304];
      7'd73 : out <= in [2367:2336];
      7'd74 : out <= in [2399:2368];
      7'd75 : out <= in [2431:2400];
      7'd76 : out <= in [2463:2432];
      7'd77 : out <= in [2495:2464];
      7'd78 : out <= in [2527:2496];
      7'd79 : out <= in [2559:2528];
      7'd80 : out <= in [2591:2560];
      7'd81 : out <= in [2623:2592];
      7'd82 : out <= in [2655:2624];
      7'd83 : out <= in [2687:2656];
      7'd84 : out <= in [2719:2688];
      7'd85 : out <= in [2751:2720];
      7'd86 : out <= in [2783:2752];
      7'd87 : out <= in [2815:2784];
      7'd88 : out <= in [2847:2816];
      7'd89 : out <= in [2879:2848];
      7'd90 : out <= in [2911:2880];
      7'd91 : out <= in [2943:2912];
      7'd92 : out <= in [2975:2944];
      7'd93 : out <= in [3007:2976];
      7'd94 : out <= in [3039:3008];
      7'd95 : out <= in [3071:3040];
      7'd96 : out <= in [3103:3072];
      7'd97 : out <= in [3135:3104];
      7'd98 : out <= in [3167:3136];
      7'd99 : out <= in [3199:3168];
      7'd100 : out <= in [3231:3200];
      7'd101 : out <= in [3263:3232];
      7'd102 : out <= in [3295:3264];
      7'd103 : out <= in [3327:3296];
      7'd104 : out <= in [3359:3328];
      7'd105 : out <= in [3391:3360];
      7'd106 : out <= in [3423:3392];
      7'd107 : out <= in [3455:3424];
      7'd108 : out <= in [3487:3456];
      7'd109 : out <= in [3519:3488];
      7'd110 : out <= in [3551:3520];
      7'd111 : out <= in [3583:3552];
      7'd112 : out <= in [3615:3584];
      7'd113 : out <= in [3647:3616];
      7'd114 : out <= in [3679:3648];
      7'd115 : out <= in [3711:3680];
      7'd116 : out <= in [3743:3712];
      7'd117 : out <= in [3775:3744];
      7'd118 : out <= in [3807:3776];
      7'd119 : out <= in [3839:3808];
      7'd120 : out <= in [3871:3840];
      7'd121 : out <= in [3903:3872];
      7'd122 : out <= in [3935:3904];
      7'd123 : out <= in [3967:3936];
      7'd124 : out <= in [3999:3968];
      7'd125 : out <= in [4031:4000];
      7'd126 : out <= in [4063:4032];
      7'd127 : out <= in [4095:4064];
      default: out <= {32{1'bx}};
    endcase
  end
endmodule
