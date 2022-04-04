#Initialize all input signals to avoid 'X' or 'Z' signals
add_force {/compute_unit_fpga/waveID_out}                   -radix bin {0 0ns}
add_force {/compute_unit_fpga/baseVGPR_out}                 -radix bin {0 0ns}
add_force {/compute_unit_fpga/baseSGPR_out}                 -radix bin {0 0ns}
add_force {/compute_unit_fpga/baseLDS_out}                  -radix bin {0 0ns}
add_force {/compute_unit_fpga/waveCount_out}                -radix bin {0 0ns}
add_force {/compute_unit_fpga/pcStart_out}                  -radix bin {0 0ns}
add_force {/compute_unit_fpga/instrAddrReg_out}             -radix bin {0 0ns}
add_force {/compute_unit_fpga/lsu2sgpr_dest_wr_en_out}      -radix bin {0 0ns}
add_force {/compute_unit_fpga/quadBaseAddress_out}          -radix bin {0 0ns}
add_force {/compute_unit_fpga/quadData0_out}                -radix bin {0 0ns}
add_force {/compute_unit_fpga/quadData1_out}                -radix bin {0 0ns}
add_force {/compute_unit_fpga/quadData2_out}                -radix hex {0 0ns}
add_force {/compute_unit_fpga/quadData3_out}                -radix bin {0 0ns}
add_force {/compute_unit_fpga/execute_out}                  -radix bin {0 0ns}
add_force {/compute_unit_fpga/executeStart_out}             -radix bin {0 0ns}
add_force {/compute_unit_fpga/instrBuffWrEn_out}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/axi_data_out}                 -radix bin {0 0ns}
add_force {/compute_unit_fpga/mb2fpgamem_data_in}           -radix bin {0 0ns}
add_force {/compute_unit_fpga/mb2fpgamem_data_we}           -radix bin {0 0ns}
add_force {/compute_unit_fpga/mb2fpgamem_ack}               -radix bin {0 0ns}
add_force {/compute_unit_fpga/mb2fpgamem_done}              -radix bin {0 0ns}
add_force {/compute_unit_fpga/clk_50}                       -radix bin {1 0ns} {0 10000ps} -repeat_every 20000ps
add_force {/compute_unit_fpga/simd1_2vgpr_source1_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd1_2vgpr_source2_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd1_2vgpr_source3_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd1_2vgpr_wr_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd2_2vgpr_source1_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd2_2vgpr_source2_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd2_2vgpr_source3_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd2_2vgpr_wr_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd3_2vgpr_source1_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd3_2vgpr_source2_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd3_2vgpr_source3_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd3_2vgpr_wr_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf1_2vgpr_source1_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf1_2vgpr_source2_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf1_2vgpr_source3_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf1_2vgpr_wr_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf2_2vgpr_source1_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf2_2vgpr_source2_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf2_2vgpr_source3_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf2_2vgpr_wr_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf3_2vgpr_source1_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf3_2vgpr_source2_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf3_2vgpr_source3_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf3_2vgpr_wr_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd1_2exec_rd_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd1_2exec_wr_vcc_en}        -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd1_2issue_alu_ready}       -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd1_2rfa_queue_entry_valid} -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd1_2sgpr_rd_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd1_2sgpr_wr_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd1_2vgpr_instr_done}       -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd2_2exec_rd_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd2_2exec_wr_vcc_en}        -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd2_2issue_alu_ready}       -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd2_2rfa_queue_entry_valid} -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd2_2sgpr_rd_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd2_2vgpr_instr_done}       -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd2_2sgpr_wr_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd3_2exec_rd_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd3_2exec_wr_vcc_en}        -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd3_2issue_alu_ready}       -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd3_2rfa_queue_entry_valid} -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd3_2sgpr_rd_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd3_2sgpr_wr_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simd3_2vgpr_instr_done}       -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf1_2exec_rd_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf1_2exec_wr_vcc_en}        -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf1_2issue_alu_ready}       -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf1_2rfa_queue_entry_valid} -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf1_2sgpr_rd_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf1_2sgpr_wr_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf1_2vgpr_instr_done}       -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf2_2exec_rd_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf2_2exec_wr_vcc_en}        -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf2_2issue_alu_ready}       -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf2_2rfa_queue_entry_valid} -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf2_2sgpr_rd_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf2_2sgpr_wr_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf2_2vgpr_instr_done}       -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf3_2exec_rd_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf3_2exec_wr_vcc_en}        -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf3_2issue_alu_ready}       -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf3_2rfa_queue_entry_valid} -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf3_2sgpr_rd_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf3_2sgpr_wr_en}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf3_2vgpr_instr_done}       -radix bin {0 0ns}
add_force {/compute_unit_fpga/simf3_2vgpr_source1_rd_en}    -radix bin {0 0ns}
add_force {/compute_unit_fpga/reset_out}                    -radix bin {0 0ns}
add_force {/compute_unit_fpga/reset_out}                    -radix bin {0 0ns}
add_force {/compute_unit_fpga/exec0/exec_file/rd0_data}     -radix hex {FFFFFFFFFFFFFFFF 0ns}
add_force {/compute_unit_fpga/exec0/exec_file/rd1_data}     -radix hex {FFFFFFFFFFFFFFFF 0ns}
run 20 ns

#De-assert the reset signal and put some initial data in scalar registers
add_force {/compute_unit_fpga/reset_out}               -radix bin {1 0ns}
add_force {/compute_unit_fpga/quadData0_out}           -radix hex {0 0ns}
add_force {/compute_unit_fpga/quadData1_out}           -radix hex {1 0ns}
add_force {/compute_unit_fpga/quadData2_out}           -radix hex {FFFFFFFF 0ns}
add_force {/compute_unit_fpga/quadData3_out}           -radix hex {4 0ns}
add_force {/compute_unit_fpga/lsu2sgpr_dest_wr_en_out} -radix bin {1111 0ns}
run 20 ns

#Populate the instruction buffer with some instructions
add_force {/compute_unit_fpga/lsu2sgpr_dest_wr_en_out} -radix bin {0 0ns}
add_force {/compute_unit_fpga/instrAddrReg_out}        -radix hex {00000000 0ns}
# v0 = RCP(v1)   == v0 = 1/v1
add_force {/compute_unit_fpga/axi_data_out}      -radix hex {7e005501 0ns}
add_force {/compute_unit_fpga/instrBuffWrEn_out} -radix bin {1111 0ns}
run 20 ns
add_force {/compute_unit_fpga/instrAddrReg_out}  -radix hex {00000001 0ns}
#end of program instruction - placeholder
add_force {/compute_unit_fpga/axi_data_out}      -radix hex {bf810000 0ns}
add_force {/compute_unit_fpga/instrBuffWrEn_out} -radix bin {1111 0ns}
run 20 ns
add_force {/compute_unit_fpga/instrAddrReg_out}  -radix hex {00000002 0ns}
#end of program instruction - placeholder
add_force {/compute_unit_fpga/axi_data_out}      -radix hex {bf810000 0ns}
add_force {/compute_unit_fpga/instrBuffWrEn_out} -radix bin {1111 0ns}
run 20 ns
#Populate more scalar registers with some data
add_force {/compute_unit_fpga/quadBaseAddress_out}     -radix hex {4 0ns}
add_force {/compute_unit_fpga/lsu2sgpr_dest_wr_en_out} -radix bin {1111 0ns}
add_force {/compute_unit_fpga/instrBuffWrEn_out}       -radix bin {0 0ns}
run 20 ns
#Place some initial value at the memory interface data entry
add_force {/compute_unit_fpga/lsu2sgpr_dest_wr_en_out} -radix bin {0 0ns}
add_force {/compute_unit_fpga/mb2fpgamem_data_in}      -radix hex {12121212 0ns}
add_force {/compute_unit_fpga/instrAddrReg_out}        -radix hex {00000003 0ns}
#end of program instruction - placeholder
add_force {/compute_unit_fpga/axi_data_out}      -radix hex {bf810000 0ns}
add_force {/compute_unit_fpga/instrBuffWrEn_out} -radix bin {1111 0ns}
run 20 ns
add_force {/compute_unit_fpga/instrBuffWrEn_out} -radix bin {0 0ns}
add_force {/compute_unit_fpga/baseLDS_out}       -radix hex {80000000 0ns}
run 20 ns
add_force {/compute_unit_fpga/instrAddrReg_out}  -radix hex {00000004 0ns}
#end of program instruction - placeholder
add_force {/compute_unit_fpga/axi_data_out}      -radix hex {bf810000 0ns}
add_force {/compute_unit_fpga/instrBuffWrEn_out} -radix bin {1111 0ns}
run 20 ns
add_force {/compute_unit_fpga/instrAddrReg_out}  -radix hex {00000005 0ns}
#end of program instruction - placeholder
add_force {/compute_unit_fpga/axi_data_out}      -radix hex {bf810000 0ns}
add_force {/compute_unit_fpga/instrBuffWrEn_out} -radix bin {1111 0ns}
run 20 ns
add_force {/compute_unit_fpga/instrAddrReg_out}  -radix hex {00000006 0ns}
#end of program instruction - placeholder
add_force {/compute_unit_fpga/axi_data_out}      -radix hex {bf810000 0ns}
add_force {/compute_unit_fpga/instrBuffWrEn_out} -radix bin {1111 0ns}
run 20 ns
add_force {/compute_unit_fpga/instrAddrReg_out}  -radix hex {00000007 0ns}
#end of program instruction
add_force {/compute_unit_fpga/axi_data_out}      -radix hex {bf810000 0ns}
add_force {/compute_unit_fpga/instrBuffWrEn_out} -radix bin {1111 0ns}
run 20 ns
#Initialize vector register with zeros
add_force {/compute_unit_fpga/instrBuffWrEn_out}           -radix bin {0 0ns}
add_force {/compute_unit_fpga/singleVectorBaseAddress_out} -radix bin {0 0ns}
add_force {/compute_unit_fpga/singleVectorWrData_out}      -radix hex {00000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001000000010000000100000001 0ns}
add_force {/compute_unit_fpga/singleVectorWrDataMask_out}  -radix hex {ffffffffffffffff 0ns}
add_force {/compute_unit_fpga/singleVectorWrEn_out}        -radix hex {f 0ns}
run 20 ns
add_force {/compute_unit_fpga/singleVectorBaseAddress_out} -radix bin {1 0ns}
add_force {/compute_unit_fpga/singleVectorWrData_out}      -radix hex {40000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000400000004000000040000000 0ns}
run 20 ns
add_force {/compute_unit_fpga/singleVectorBaseAddress_out} -radix hex {2 0ns}
add_force {/compute_unit_fpga/singleVectorWrData_out}      -radix bin {0 0ns}
run 20 ns
add_force {/compute_unit_fpga/singleVectorBaseAddress_out} -radix hex {3 0ns}
run 20 ns
add_force {/compute_unit_fpga/singleVectorWrEn_out}        -radix hex {0 0ns}
#Start execution
add_force {/compute_unit_fpga/executeStart_out}            -radix bin {1 0ns}
run 20 ns
add_force {/compute_unit_fpga/executeStart_out}            -radix bin {0 0ns}
add_force {/compute_unit_fpga/execute_out}                 -radix bin {1 0ns}
run 20 ns


