/**
 * global_definitions.v
 * File with al global definitions of the miaow project.
 * This file does not need to be included within the modules but it needs to be
 * listed for compilation/syntesys.
 */
`ifndef _global_definitions_v_
`define _global_definitions_v_
// Maximum number of wavefronts per compute unit
`define WF_PER_CU 40
// Maximum number of workgroups per compute unit
`define WG_PER_WF 40
// Maximum number of wavefronts per wavegroup
`define WF_PER_WG 16
// Length of WF_ID in bits
`define WF_ID_LENGTH 6

// Length of operand field on instructions
`define OPERAND_LENGTH_2WORD 13
`define OPERAND_LENGTH_4WORD 14

`define VGPR_ADDR_LENGTH 10
`define SGPR_ADDR_LENGTH 9
// Single word length
`define WORD_LENGTH 8
`define WORD_MSB (`WORD_LENGTH-1)
`define VECTOR_LENGTH 64*`WORD_LENGTH
`define VECTOR_MSB (`VECTOR_LENGTH-1)
`define WORD_DIF (24'b0) //32b - WORD_LENGTH
// Number of VGPRS
`define NUMBER_VGPR 1024
// Number of SGPRs
`define NUMBER_SGPR 512

`endif  //_global_definitions_v_
