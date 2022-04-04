#include <stdio.h>
#include <math.h>
#include "platform.h"
#include "xio.h"
#include "xparameters.h"

#define NEKO_CMD_ADDR XPAR_AXI_SLAVE_0_S00_AXI_BASEADDR
#define NEKO_BASE_LDS (NEKO_CMD_ADDR + 16)
#define NEKO_INSTR_ADDR (NEKO_CMD_ADDR + 28)
#define NEKO_INSTR_VALUE (NEKO_CMD_ADDR + 32)
#define NEKO_GPR_CMD (NEKO_CMD_ADDR + 40)
#define NEKO_SGRP_ADDR (NEKO_CMD_ADDR + 44)
#define NEKO_SGRP_QUAD_0 (NEKO_CMD_ADDR + 48)
#define NEKO_SGRP_QUAD_1 (NEKO_CMD_ADDR + 52)
#define NEKO_SGRP_QUAD_2 (NEKO_CMD_ADDR + 56)
#define NEKO_SGRP_QUAD_3 (NEKO_CMD_ADDR + 60)

#define NEKO_MEM_OP (NEKO_CMD_ADDR + 128)
#define NEKO_MEM_RD_DATA (NEKO_CMD_ADDR + 132) // Address for data to be read 
//from MIAOW and written to memory
#define NEKO_MEM_ADDR (NEKO_CMD_ADDR + 136)
#define NEKO_MEM_WR_DATA (NEKO_CMD_ADDR + 192)//Addr. for writing data to MIAOW
#define NEKO_MEM_WR_EN (NEKO_CMD_ADDR + 196)
#define NEKO_MEM_ACK (NEKO_CMD_ADDR + 200)
#define NEKO_MEM_DONE (NEKO_CMD_ADDR + 204)

#define NEKO_CYCLE_COUNTER (NEKO_CMD_ADDR + 192)

#define NEKO_RESET (NEKO_CMD_ADDR + 36)

#define MEM_WR_ACK_WAIT 1
#define MEM_WR_RDY_WAIT 2
#define MEM_WR_LSU_WAIT 3
#define MEM_RD_ACK_WAIT 4
#define MEM_RD_RDY_WAIT 5
#define MEM_RD_LSU_WAIT 6
#define VGPR_DATA (NEKO_CMD_ADDR + 0x0D4)
#define VGPR_ADDR (NEKO_CMD_ADDR + 0x0D0)
#define VGPR_WR_CMD (NEKO_CMD_ADDR + 0x01D4)
#define VGPR_WR_CLEAN (NEKO_CMD_ADDR + 0x01D8)
#define VGPR_WR_MASK_LO (NEKO_CMD_ADDR + 0x01DC)
#define VGPR_WR_MASK_HI (NEKO_CMD_ADDR + 0x01E0)

#define END_PRGRM 0xBF810000
union ufloat{
  float f;
  uint32_t u;
};
union ufloat64{
    double f;
    uint64_t u;
};

int32_t create_sop1(int32_t op, int32_t sdst, int32_t s0){
  /* SOP1 instruction format:
   * MSB -> LSB
   * | ENC(9) = 9'b101111101 | SDST(7) | OP(8) | SSRC0(8) |
   */
  int32_t inst = 0xBE800000;
  op = op << 8;
  sdst = sdst << 16;
  inst = inst | sdst | op | s0;
  return(inst);
}
int32_t create_vop2(int32_t op, int32_t vdst, int32_t vsrc1, int32_t src0){
  /* VOP2 instruction format:
   * MSB -> LSB
   * | ENC(1) = 1'b0 | OP(6) | VDST(8) | VSRC1(8) | SRC0(9) |
   */
  int32_t inst = 0x00000000;
  op = op << 25;
  vdst = vdst << 17;
  vsrc1 = vsrc1 << 9;
  inst = inst | op | vdst | vsrc1 | src0;
  return(inst);
}
int32_t create_vop1(int32_t op, int32_t vdst, int32_t src0){
  /* VOP1 instruction format:
   * MSB -> LSB
   * | ENC(7) = 7'b0111111 | VDST(8) | OP(8) | SRC0(9) |
   */
  int32_t inst = 0x7E000000;
  op = op << 9;
  vdst = vdst << 17;
  inst = inst | op | vdst | src0;
  return(inst);
}
int32_t create_vopc(int32_t op_base,int32_t op_offset, 
                    int32_t vsrc1, int32_t src0){
  /* VOPC instruction format:
   * MSB -> LSB
   * | ENC(7) = 7'b0111110 | OP_Base + OP_Offset (8) | VSRC1(8) | SRC0(9) |
   */
  int32_t inst = 0x7C000000;
  int32_t op = (op_base + op_offset) << 17;
  vsrc1 = vsrc1 << 9;
  inst = inst | op | vsrc1 | src0;
  return(inst);
}
void create_vop3a(int32_t op, int32_t op_offset, int32_t vdst,int32_t src2, 
                  int32_t src1, int32_t src0,int32_t abs, int32_t clamp, 
                  int32_t omod, int32_t neg, int32_t *inst){
  /* VOP3a instruction format:
   * MSB -> LSB
   * | NEG(3) | OMOD(2) | SRC2(9) | SRC1(9) | SRC0(9) |
   * | ENC(6) = 6'b110100 | OP(9) | RESERVED(1) | CLAMP(1) | ABS(3) | VDST(8) |
   */
  inst[0] = 0xD0000000;
  op = (op + op_offset) << 17;
  clamp = clamp << 11;
  abs = abs << 8;
  inst[0] = inst[0] | vdst | abs | clamp | op;

  inst[1] = 0x00000000;
  src1 = src1 << 9;
  src2 = src2 << 18;
  omod = omod << 27;
  neg = neg << 29;
  inst[1] = inst[1] | neg | omod | src2 | src1 | src0;
}
void create_vop3b(int32_t op, int32_t sdst, int32_t vdst,int32_t src2, 
                  int32_t src1, int32_t src0, int32_t omod, int32_t neg, 
                  int32_t *inst){
  /* VOP3b instruction format:
   * MSB -> LSB
   * | NEG(3) | OMOD(2) | SRC2(9) | SRC1(9) | SRC0(9) |
   * | ENC(6) = 6'b110100 | OP(9) | RESERVED(2) | SDST(7) | VDST(8) |
   */
  inst[0] = 0xD0000000;
  op = op  << 17;
    sdst = sdst << 8;
  inst[0] = inst[0] | sdst | vdst | op;

  inst[1] = 0x00000000;
  src1 = src1 << 9;
  src2 = src2 << 18;
  omod = omod << 27;
  neg = neg << 29;
  inst[1] = inst[1] | neg | omod | src2 | src1 | src0;
}

int32_t run_vop_program_neko(int32_t insts[],int32_t num_insts, 
                             int32_t inst_scalar_data[], int32_t num_scalar_data
                            ,int32_t inst_vect_data[], int32_t num_vect_data,
                             int32_t max_clocks){
  /*
   * Execution Flow:
   * Resets Neko
   * Populates NEKO's instruction buffer, the scalar registers and the vector 
   * registers (all 64 words of a register are initialized with the same value)
   * Send "start execution" command and waits for program completion or until 
   * the timeout is reached
   * If the program reaches the end of execution before the timeout the data in 
   * the registers is read and success(1) is returned
   * Otherwise returns 0 (unsuccessful)
   */
  int32_t index, cycle_counter = 0, succeeded = 1;
  int32_t vgpr, vgpr_word;
  int32_t * vgpr_data_pointer = (int32_t*)VGPR_DATA;

  //NEKO's reset pulse
  XIo_Out32(NEKO_RESET,0);
  XIo_Out32(NEKO_RESET,1);
  XIo_Out32(NEKO_RESET,0);

  XIo_Out32(NEKO_BASE_LDS, XPAR_MIG_7SERIES_0_BASEADDR);

  //Load scalar registers with data
  for(index = 0; index < num_scalar_data; index+=4){
    XIo_Out32(NEKO_SGRP_ADDR, index);
    XIo_Out32(NEKO_SGRP_QUAD_0, inst_scalar_data[index]);
    XIo_Out32(NEKO_SGRP_QUAD_1, inst_scalar_data[index+1]);
    XIo_Out32(NEKO_SGRP_QUAD_2, inst_scalar_data[index+2]);
    XIo_Out32(NEKO_SGRP_QUAD_3, inst_scalar_data[index+3]);
    XIo_Out32(NEKO_GPR_CMD, 1);
  }

  //Load vector registers with data (replicating the data for every word of 
  //the register)
  for(vgpr=0;vgpr<num_vect_data;vgpr++){
    XIo_Out32(VGPR_ADDR, vgpr);
    XIo_Out32(VGPR_WR_CLEAN, 1);
    XIo_Out32(VGPR_WR_CMD, 1);
    for(vgpr_word=0;vgpr_word<64;vgpr_word++){
      vgpr_data_pointer[vgpr_word] = inst_vect_data[vgpr];
    }
    XIo_Out32(VGPR_WR_CMD, 1);
  }

  //Load the instruction buffer
  for(index = 0; index < num_insts; index++){
    XIo_Out32(NEKO_INSTR_ADDR, index);
    XIo_Out32(NEKO_INSTR_VALUE, insts[index]);
  }
  //Start execution
  XIo_Out32(NEKO_CMD_ADDR, 1);

  //Wait for the end of execution
  while(XIo_In32(NEKO_CMD_ADDR) != 1){
    //Verify the timeout
    cycle_counter = XIo_In32(NEKO_CYCLE_COUNTER);
    if(cycle_counter>max_clocks){
      succeeded=0;
      break;
    }
  }

  //NEKO's reset pulse
  XIo_Out32(NEKO_RESET,0);
  XIo_Out32(NEKO_RESET,1);
  XIo_Out32(NEKO_RESET,0);

  if(succeeded){
    //Retrieve the data in the scalar registers (results)
    for(index = 0; index < num_scalar_data; index+=4){
      XIo_Out32(NEKO_SGRP_ADDR, index);
      inst_scalar_data[index]=XIo_In32(NEKO_SGRP_QUAD_0);
      inst_scalar_data[index+1]=XIo_In32(NEKO_SGRP_QUAD_1);
      inst_scalar_data[index+2]=XIo_In32(NEKO_SGRP_QUAD_2);
      inst_scalar_data[index+3]=XIo_In32(NEKO_SGRP_QUAD_3);
    }

    //Retrieve the data in the vector registers (results)
    for(vgpr=0;vgpr<num_vect_data;vgpr++){
      XIo_Out32(VGPR_ADDR, vgpr);
      inst_vect_data[vgpr] = vgpr_data_pointer[0];
    }
  }
  return(succeeded);
}

int32_t test_vector_inst_creation(){
  //Verifies the instruction creation by comparing against manually generated 
//instructions

  xil_printf("Instruction generation\n\r");
  if(create_vop2(37, 3, 2, 5) != 0x4A060405){//v_add_i32 v3, vcc, s5,v2-4A060405
    xil_printf("VOP2: Failed\n\r");
    return 0;
  }
  else
    xil_printf("VOP2: Ok\n\r");

  if(create_vop1(1, 5, 0x106) != 0x7E0A0306){//v_mov_b32 v5, v6 - 7E0A0306
    xil_printf("VOP1: Failed\n\r");
    return 0;
  }
  else
    xil_printf("VOP1: Ok\n\r");

  if(create_vopc(0x80,4, 2, 0x102) != 0x7D080502){//v_cmp_gt_i32  vcc, v2, v2
    xil_printf("VOPC: Failed\n\r");
    return 0;
  }
  else
    xil_printf("VOPC: Ok\n\r");

  int32_t inst_3a[2];
  create_vop3a(362, 0, 5,0x108, 3,0x102,0, 0, 0, 0, inst_3a);
  //v_mul_hi_u32  v5, v2, s3 - D2D40005 04200702
  if(inst_3a[0] != 0xD2D40005 || inst_3a[1] != 0x04200702){
    xil_printf("VOP3a: Failed\n\r");
    return 0;
  }
  else
    xil_printf("VOP3a: Ok\n\r");

  int32_t inst_3b[2];
  create_vop3b(293, 1, 2,0, 0x100, 0x101, 0, 0, inst_3b);
  //v_add v2, v0, v1 ; s1 = carry_out
  if(inst_3b[0] != 0xD24A0102 || inst_3b[1] != 0x00020101){
    xil_printf("VOP3b: Failed\n\r");
    return 0;
  }
  else
    xil_printf("VOP3b: Ok\n\r");

  return 1;
}

int32_t analyze_vop2(int32_t opcode, int32_t s1, int32_t s0, int32_t d, 
                     int32_t res,int32_t vcc_lo, int32_t vcc_hi){
  /* Checks the first result of the operation (opcode) with vector operands s1 
   * and s0 and results in res
   *
   * Some instructions write to the vcc register, and some use the extra value 
   * 'd' (which can be the initial value of the register or some constant)
   *
   * TODO: IMPLEMENT COMPLETE VECTOR VERIFICATION (for all 64 words)
   *
   * IMPORTANT: This function assumes that the exec mask is all ones 
   * (EXEC_hi = EXEC_lo = 0xFFFFFFFF)
   *
   */

  int32_t correct = 0;
  //Convertion of operands and result to unsigned int and float formats which 
//are used by some of the operations
  uint32_t u_s1=(uint32_t)s1,
       u_s0=(uint32_t)s0,
       u_res=(uint32_t)res;


  union ufloat f_s1, f_s0, f_res, f_d, f_aux;
  f_s1.u=s1, f_s0.u=s0, f_res.u=res, f_d.u = d;
xil_printf("s1=%08x\n\rs0=%08x\n\rres=%08x\n\r",f_s1.u,f_s0.u,f_res.u);
  switch(opcode){
  case 0://v_cndmask_b32
    correct = (vcc_lo & 0x01) ? (u_res == u_s1) : (u_res == u_s0);
    break;
  case 3://v_add_f32
    f_aux.f = f_s1.f+f_s0.f;
    correct = (f_res.u==f_aux.u);
    break;
  case 4://v_sub_f32
    correct = (f_res.f==(f_s0.f-f_s1.f));
    break;
  case 5://v_subrev_f32
    correct = (f_res.f==(f_s1.f-f_s0.f));
    break;
  case 8://v_mul_f32
    correct = (f_res.f==(f_s0.f*f_s1.f));
    break;
  case 9://v_mul_i32_i24
    //only the 24 lsb's are multiplied
    correct = (res==(s0&0x00FFFFFF)*(s1&0x00FFFFFF));
    break;
  case 15://v_min_f32
    correct = (f_res.f == (f_s0.f < f_s1.f ? f_s0.f : f_s1.f));
    break;
  case 16://v_max_f32
    correct = (f_res.f == (f_s0.f > f_s1.f ? f_s0.f : f_s1.f));
    break;
  case 18://v_max_i32
    correct = (res == (s0 > s1 ? s0 : s1));
    break;
  case 19://v_min_u32
    correct = (u_res == (u_s0 < u_s1 ? u_s0 : u_s1));
    break;
  case 20://v_max_u32
    correct = (u_res == (u_s0 > u_s1 ? u_s0 : u_s1));
    break;
  case 22://s1.u >> s0[4:0] v_lshrrev_b32
    correct = (u_res == (u_s1 >> (u_s0 & 0x0F)));
    break;
  case 24://s1.i >> s0[4:0] v_ashrrev_i32
    correct = (res == (s1 >> (s0 & 0x0F)));
    break;
  case 26://s1.i << s0[4:0] v_lshlrev_b32
    correct = (u_res == (u_s1 << (u_s0 & 0x0F)));
    break;
  case 27://v_and_b32
    correct = (u_res == (u_s1 & u_s0 ));
    break;
  case 28://v_or_b32
    correct = (u_res == (u_s1 | u_s0 ));
    break;
  case 31://v_mac_f32 D = S0*S1+D
    correct = (f_res.f == (f_s0.f*f_s1.f+f_d.f));
    break;
  case 32://v_madmk_f32 D = S0*constant+S1
    correct = (f_res.f == (f_s0.f*f_d.f+f_s1.f));
    break;
  case 37://v_add_i32
    correct = (u_res == (u_s1 + u_s0 ));
    break;
  case 38://v_sub_i32
    correct = (u_res == (u_s0 - u_s1 ));
    break;
  case 39://v_subrev_i32
    correct = (u_res == (u_s1 - u_s0 ));
    break;
  case 40://v_addc_u32 d = s0 + s1 + vcc
    correct = (u_res == (u_s1 + u_s0 + vcc_lo));
    break;
  default:
    correct = -1;
  }
  return correct;
}
int32_t analyze_vop1(int32_t opcode, int32_t s0, int32_t res,int32_t vcc_lo, 
                     int32_t vcc_hi){
  /* Checks the first result of the operation (opcode) with vector operand s0 
   * and results in res
   *
   * Some instructions write to the vcc register
   *
   * IMPORTANT: This function assumes that the exec mask is all ones
   * (EXEC_hi = EXEC_lo = 0xFFFFFFFF)
   *
   */
  int32_t correct = 0;

  //Convertion of operands and result to unsigned int and to float formats which
// are used by some of the operations
  uint32_t u_s0=(uint32_t)s0,
       u_res=(uint32_t)res;

  union ufloat  f_s0, f_res;
  f_s0.u=s0, f_res.u=res;

  switch(opcode){
  case 1://v_mov_b32
    correct = (u_res == u_s0);
    break;
  case 5://v_cvt_f32_i32
    correct = (f_res.f == (float)s0);
  case 6://v_cvt_f32_u32
    correct = (f_res.f == (float)u_s0);
  case 7://v_cvt_u32_f32
    correct = (u_res == (uint32_t)f_s0.f);
  case 8://v_cvt_i32_f32
      correct = (res == (int32_t)f_s0.f);

  case 32://v_fract_f32
    correct = (f_res.f == (f_s0.f-floor(f_s0.f)));
    break;
  case 33://v_trunc_f32
    correct = (f_res.f == floor(f_s0.f));
    break;
  case 34://v_ceil_f32
    correct = (f_res.f == ceil(f_s0.f));
    break;
  case 35://v_rndne_f32
    correct = (f_res.f == 2.0f);
    break;
  case 36://v_floor_f32
    correct = (f_res.f == floor(f_s0.f));
    break;
  case 37://v_exp_f32
    correct = (f_res.u == 0x403504f3);//pow(2.0f,f_s0.f))
    break;
  case 38://v_log_clamp_f32
  case 39://v_log_f32
    correct = (f_res.f == log2(f_s0.f));
    break;
  case 40://v_rcp_clamp_f32
  case 42://v_rcp_f32
    correct = (f_res.f == (1.0f/f_s0.f));
    break;
  case 44://v_rsq_clamp_f32
  case 46://v_rsq_f32
    correct = (f_res.f == (1.0f/sqrt(f_s0.f)));
    break;
  case 51://v_sqrt_f32
    correct = (f_res.f == sqrt(f_s0.f));
    break;
  case 53://v_sin_f32
    correct = (f_res.f == 0x3f7f5bd5);//sin(f_s0.f))
    break;
  case 54://v_cos_f32
    correct = (f_res.f == 0x3d90deab);//cos(f_s0.f))
    break;
  default:
    correct = -1;
  }
  return correct;
}
int32_t analyze_vopc(int32_t opcode, int32_t s0, int32_t s1,int32_t vcc_lo, 
                     int32_t vcc_hi){
  /* Checks the first result of the comparison with vector operands s1 and s0 
   * and results in the vcc register
   * returns 1 if the result is correct; 0 if incorrect and -1 if there isn't a 
   * test available
   *
   * IMPORTANT: This function assumes that the exec mask is all ones
   * (EXEC_hi = EXEC_lo = 0xFFFFFFFF)
   */

  int32_t correct = 0;
  //Convertion of operands and result to unsigned int and to float formats which
// are used by some of the operations
  uint32_t u_s1=(uint32_t)s1,
       u_s0=(uint32_t)s0;
  union ufloat f_s1, f_s0;
    f_s1.u=s1, f_s0.u=s0;

  switch(opcode){
  case 0://v_cmp_F_f32
  case 0x10://v_cmpx_F_f32
  case 0x40://v_cmps_F_f32
  case 0x50://v_cmpsx_F_f32
    correct = (vcc_lo == 0 && vcc_hi == 0);
    break;
  case 1://v_cmp_LT_f32
  case 0x11://v_cmpx_LT_f32
  case 0x41://v_cmps_LT_f32
  case 0x51://v_cmpsx_LT_f32
    correct = ((vcc_lo & 0x01) == (f_s0.f < f_s1.f));
    break;
  case 2://v_cmp_EQ_f32
  case 0x12://v_cmpx_EQ_f32
  case 0x42://v_cmps_EQ_f32
  case 0x52://v_cmpsx_EQ_f32
    correct = ((vcc_lo & 0x01) == (f_s0.f == f_s1.f));
    break;
  case 3://v_cmp_LE_f32
  case 0x13://v_cmpx_LE_f32
  case 0x43://v_cmps_LE_f32
  case 0x53://v_cmpsx_LE_f32
    correct = ((vcc_lo & 0x01) == (f_s0.f <= f_s1.f));
    break;
  case 4://v_cmp_GT_f32
  case 0x14://v_cmpx_GT_f32
  case 0x44://v_cmps_GT_f32
  case 0x54://v_cmpsx_GT_f32
    correct = ((vcc_lo & 0x01) == (f_s0.f > f_s1.f));
    break;
  case 5://v_cmp_LG_f32
  case 0x15://v_cmpx_LG_f32
  case 0x45://v_cmps_LG_f32
  case 0x55://v_cmpsx_LG_f32
    correct = ((vcc_lo & 0x01) == (f_s0.f != f_s1.f));
    break;
  case 6://v_cmp_GE_f32
  case 0x16://v_cmpx_GE_f32
  case 0x46://v_cmps_GE_f32
  case 0x56://v_cmpsx_GE_f32
    correct = ((vcc_lo & 0x01) == (f_s0.f >= f_s1.f));
    break;
  case 7://v_cmp_O_f32
  case 0x17://v_cmpx_O_f32
  case 0x47://v_cmps_O_f32
  case 0x57://v_cmpsx_O_f32
    //float32 NaN -> Exponent = all ones (8bits) (infinity)
    //         Mantissa = at least one bit different than 0
    if(!(((f_s0.u & 0x7f800000) == 0x7f800000) && ((f_s0.u & 0x007fffff)!=0))
       //!(s0 is nan)
        &&
       !(((f_s1.u & 0x7f800000) == 0x7f800000) && ((f_s1.u & 0x007fffff)!=0))
       //!(s1 is nan)
    ){
      correct = ((vcc_lo & 0x01) == 1) ? 1 : 0;
    }
    else
      correct = ((vcc_lo & 0x01) == 0) ? 1 : 0;
    break;
  case 8://v_cmp_U_f32
  case 0x18://v_cmpx_U_f32
  case 0x48://v_cmps_U_f32
  case 0x58://v_cmpsx_U_f32
    if(!(( (f_s0.u & 0x7f800000) == 0x7f800000) && (f_s0.u & ~0x007fffff))
        //!(s0 is nan)
        ||
        !(((f_s1.u & 0x7f800000) == 0x7f800000) && (f_s1.u & ~0x007fffff))
        //!(s1 is nan)
    )
      correct = ((vcc_lo & 0x01) == 1) ? 1 : 0;
    else
      correct = ((vcc_lo & 0x01) == 0) ? 1 : 0;
    break;
  case 9://v_cmp_NGE_f32
  case 0x19://v_cmpx_NGE_f32
  case 0x49://v_cmps_NGE_f32
  case 0x59://v_cmpsx_NGE_f32
    correct = ((vcc_lo & 0x01) == !(f_s0.f >= f_s1.f));
    break;
  case 10://v_cmp_NLG_f32
  case 0x1A://v_cmpx_NLG_f32
  case 0x4A://v_cmps_NLG_f32
  case 0x5A://v_cmpsx_NLG_f32
    correct = ((vcc_lo & 0x01) == !(f_s0.f != f_s1.f));
    break;
  case 11://v_cmp_NGT_f32
  case 0x1B://v_cmpx_NGT_f32
  case 0x4B://v_cmps_NGT_f32
  case 0x5B://v_cmpsx_NGT_f32
    correct = ((vcc_lo & 0x01) == !(f_s0.f > f_s1.f));
    break;
  case 12://v_cmp_NLE_f32
  case 0x1C://v_cmpx_NLE_f32
  case 0x4C://v_cmps_NLE_f32
  case 0x5C://v_cmpsx_NLE_f32
    correct = ((vcc_lo & 0x01) == !(f_s0.f <= f_s1.f));
    break;
  case 13://v_cmp_NEQ_f32
  case 0x1D://v_cmpx_NEQ_f32
  case 0x4D://v_cmps_NEQ_f32
  case 0x5D://v_cmpsx_NEQ_f32
    correct = ((vcc_lo & 0x01) == !(f_s0.f == f_s1.f));
    break;
  case 14://v_cmp_NLT_f32
  case 0x1E://v_cmpx_NLT_f32
  case 0x4E://v_cmps_NLT_f32
  case 0x5E://v_cmpsx_NLT_f32
    correct = ((vcc_lo & 0x01) == !(f_s0.f < f_s1.f));
    break;
  case 15://v_cmp_TRU_f32
  case 0x1F://v_cmpx_TRU_f32
  case 0x4F://v_cmps_TRU_f32
  case 0x5F://v_cmpsx_TRU_f32
    correct = (vcc_lo == 0xFFFFFFFF && vcc_hi == 0xFFFFFFFF);
    break;

  case 0x80://v_cmp_F_i32
    correct = (vcc_lo == 0 && vcc_hi == 0);
    break;
  case 0x81://v_cmp_LT_i32
    correct = ((vcc_lo & 0x01) == (s0 < s1));
    break;
  case 0x82://v_cmp_EQ_i32
    correct = ((vcc_lo & 0x01) == (s0 == s1));
    break;
  case 0x83://v_cmp_LE_i32
    correct = ((vcc_lo & 0x01) == (s0 <= s1));
    break;
  case 0x84://v_cmp_GT_i32
    correct = ((vcc_lo & 0x01) == (s0 > s1));
    break;
  case 0x85://v_cmp_LG_i32
    correct = ((vcc_lo & 0x01) == (s0 != s1));
    break;
  case 0x86://v_cmp_GE_i32
    correct = ((vcc_lo & 0x01) == (s0 >= s1));
    break;
  case 0x87://v_cmp_TRU_i32
    correct = (vcc_lo == 0xFFFFFFFF && vcc_hi == 0xFFFFFFFF);
    break;

  case 0xC0://v_cmp_F_u32
    correct = (vcc_lo == 0 && vcc_hi == 0);
    break;
  case 0xC1://v_cmp_LT_u32
    correct = ((vcc_lo & 0x01) == (u_s0 < u_s1));
    break;
  case 0xC2://v_cmp_EQ_u32
    correct = ((vcc_lo & 0x01) == (u_s0 == u_s1));
    break;
  case 0xC3://v_cmp_LE_u32
    correct = ((vcc_lo & 0x01) == (u_s0 <= u_s1));
    break;
  case 0xC4://v_cmp_GT_u32
    correct = ((vcc_lo & 0x01) == (u_s0 > u_s1));
    break;
  case 0xC5://v_cmp_LG_u32
    correct = ((vcc_lo & 0x01) == (u_s0 != u_s1));
    break;
  case 0xC6://v_cmp_GE_u32
    correct = ((vcc_lo & 0x01) == (u_s0 >= u_s1));
    break;
  case 0xC7://v_cmp_TRU_u32
    correct = (vcc_lo == 0xFFFFFFFF && vcc_hi == 0xFFFFFFFF);
    break;
  default:
    correct = -1;
  }
  return correct;
}
int32_t analyze_vop3a(int32_t opcode, int32_t s2, int32_t s1, int32_t s0, 
                      int32_t d, int32_t res,int32_t vcc_lo, int32_t vcc_hi){
  /* Checks the first result of the operation with vector operands s1 and s0 
   * (and, in some cases s2) and results in res
     *
     * IMPORTANT: This function assumes that the exec mask is all ones
     * (EXEC_hi = EXEC_lo = 0xFFFFFFFF)
     */

  int32_t correct = 0;

  //Convertion of operands and result to unsigned int, float and unsigned int 
//64bit formats which are used by some of the operations
  uint32_t u_s2=(uint32_t)s2,
      u_s1=(uint32_t)s1,
       u_s0=(uint32_t)s0,
      u_res=(uint32_t)res;

  union ufloat f_s2,f_s1, f_s0, f_res, f_aux;
  f_s2.u=s2,f_s1.u=s1, f_s0.u=s0, f_res.u=res;

  uint64_t u64_s1 = (uint64_t)s1,
       u64_s0 = (uint64_t)s0;

  switch(opcode){
  case 321://v_mad_f32 s0*s1+s2
    correct = (f_res.f == (f_s0.f*f_s1.f+f_s2.f));
    break;
  case 328://v_bfe_u32 (s0>>s1[4:0]) & ( (1<<s2[4:0]) -1 )
    correct = (u_res == ((u_s0 >> (u_s1 & 0x0f)) & ((1<<(u_s2 & 0x0f))-1)));
    break;
  case 329://v_bfe_i32
    correct = (res == ((s0 >> (s1 & 0x0f)) & ((1<<(s2 & 0x0f))-1)));
    break;
  case 330://v_bfi_b32 (s0 & s1) | (~s0 & s2)
    correct = (u_res == ((u_s0 & u_s1) | (~u_s0 & u_s2)));
    break;
  case 331://v_fma_f32 s0*s1+s2
    correct = (f_res.f == (f_s0.f*f_s1.f+f_s2.f));
    break;
  case 337://v_min3_f32
      f_aux.f = f_s0.f < f_s1.f ? f_s0.f : f_s1.f;
      correct = (f_res.f == (f_aux.f < f_s2.f ? f_aux.f : f_s2.f));
      break;
  case 340://v_max3_f32
    f_aux.f = f_s0.f > f_s1.f ? f_s0.f : f_s1.f;
    correct = (f_res.f == (f_aux.f > f_s2.f ? f_aux.f : f_s2.f));
    break;
  case 343://v_med3_f32
    //med -> a, b, c -- b>=a && b=<c
    if(f_s0.f >= f_s1.f){
      if(f_s1.f >= f_s2.f)
        correct = (f_res.f == f_s1.f);
      else if(f_s0.f >= f_s2.f)
        correct = (f_res.f == f_s2.f);
      else
        correct = (f_res.f == f_s0.f);
    }
    else{
      if(f_s0.f >= f_s2.f)
        correct = (f_res.f == f_s0.f);
      else if(f_s1.f >= f_s2.f)
        correct = (f_res.f == f_s2.f);
      else
        correct = (f_res.f == f_s1.f);
    }
    break;
  case 361://v_mul_lo_u32
    correct = (u_res == u_s0 * u_s1);
    break;
  case 362://v_mul_hi_u32
    correct = (u_res == (uint32_t)((u64_s0 * u64_s1)>>32));
    break;
  case 363://v_mul_lo_i32
    correct = (res == s0 * s1);
    break;
  default:
    correct = -1;
  }
  return correct;
}

int32_t test_vop2(){
  /*Tests are done by issuing the following instructions and reading the results
   * exec_lo = s2
   * exec_hi = s2
   * s0 = vcc_lo
   * s1 = vcc_hi
   * v0 = v2 op v3
   * s3 = vcc_hi (this instruction can be replaced by constant when needed)
   * s2 = vcc_lo
   * s3 = vcc_hi
   */
  int32_t insts[9];

  insts[0] = 0xBEFE0302;//exec_lo = s2 = 0xFFFFFFFF
  insts[1] = 0xBEFF0302;//exec_hi = s2 = 0xFFFFFFFF
  insts[2] = create_sop1(3, 0, 106); //s0 = vcc_lo -> sop1 inst
  insts[3] = create_sop1(3, 1, 107); //s1 = vcc_hi -> sop1 inst

  insts[5] = create_sop1(3, 3, 107); //Repeated instruction is a place holder 
//for a constant
  insts[6] = create_sop1(3, 2, 106); //s2 = vcc_lo -> sop1 inst
  insts[7] = create_sop1(3, 3, 107); //s3 = vcc_hi -> sop1 inst
  insts[8] = END_PRGRM;

  int32_t inc_counter, data_counter, vect_data_counter;
  int32_t res, check_res;
  int32_t inst_data[12],inst_vect_data[10];//,ori_vect_data[10][64];
  int32_t running_insts_counter = 0;

  for(inc_counter = 0; inc_counter<49; inc_counter++){
    //Generate data for the scalar registers
    for(data_counter = 0; data_counter<12; data_counter++){
      inst_data[data_counter] = data_counter;
      if(data_counter == 2)
        inst_data[data_counter] = 0xFFFFFFFF;
    }
    //Generate data for the vector registers (which will be replicated for all 
//64 words of the register)
    for(vect_data_counter = 0; vect_data_counter<10; vect_data_counter++){
      inst_vect_data[vect_data_counter] = vect_data_counter;
    }

    //Generate the VOP2 instruction to be tested
    insts[4] =  create_vop2(inc_counter, 0, 3, 0x102);

    //Set the constant for the op's that need it (v_madmk_f32 and v_madak_f32)
    if(inc_counter == 32 || inc_counter == 33){
      insts[5] = 0x0;
    }
    else
      insts[5] = create_sop1(3, 3, 107);

    //Test the instruction
    res = run_vop_program_neko(insts,9, inst_data, 12,inst_vect_data, 10,15000);

    if(res){
      //If the program completed then the result is tested
      check_res = analyze_vop2(inc_counter, 3,2,0, inst_vect_data[0],
                               inst_data[0], inst_data[1]);
      if(check_res == 1){
        xil_printf("VOP2: OPCODE %d OK\n\r",inc_counter);
        running_insts_counter++;
      }
      else if(check_res == 0)
        xil_printf("VOP2: OPCODE %d wrong result\n\r",inc_counter);
      else
        xil_printf("VOP2: OPCODE %d No test available\n\r",inc_counter);
    }
    else
      xil_printf("VOP2: OPCODE %d did not finish running\n\r",inc_counter);
  }
  return(running_insts_counter);
}
int32_t test_vop1(){
  /*Tests are done by issuing the following instructions and reading the results
   * exec_lo = s2
   * exec_hi = s2
   * v0 = op v4
   * s0 = vcc_lo
   * s1 = vcc_hi
   */

  int32_t insts[6];

  insts[0] = 0xBEFE0302;//exec_lo = s2 = 0xFFFFFFFF
  insts[1] = 0xBEFF0302;//exec_hi = s2 = 0xFFFFFFFF

  insts[3] = create_sop1(3, 0, 106); //s0 = vcc_lo -> sop1 inst
  insts[4] = create_sop1(3, 1, 107); //s1 = vcc_hi -> sop1 inst
  insts[5] = END_PRGRM;

  int32_t inc_counter, data_counter, running_insts_counter = 0;
  int32_t res, check_res;
  int32_t inst_data[12],inst_vect_data[10];

  for(inc_counter = 0; inc_counter<68; inc_counter++){
    if(inc_counter >= 23 && inc_counter <= 31)//reserved values
      continue;

    //Generate data for the scalar registers
    for(data_counter = 0; data_counter<12; data_counter++){
      inst_data[data_counter] = data_counter;
      if(data_counter == 2)
        inst_data[data_counter] = 0xFFFFFFFF;
    }
    //Generate data for the vector registers (which will be replicated for all 
//64 words of the register)
    for(data_counter = 0; data_counter<10; data_counter++){
      inst_vect_data[data_counter] = data_counter;
      if(data_counter == 4)
        inst_vect_data[data_counter] = 0x3fc00000;//v[4] = 1.5f
    }

    //Generate the VOP1 instruction to be tested
    insts[2] =  create_vop1(inc_counter, 0, 0x104);

    //Test the instruction
    res = run_vop_program_neko(insts,6, inst_data, 12,inst_vect_data, 10,15000);

    if(res){
      //If the program completed then the result is verified
      check_res = analyze_vop1(inc_counter, 0x3fc00000, inst_vect_data[0],
                               inst_data[0], inst_data[1]);
      if(check_res == 1){
        running_insts_counter++;
        xil_printf("VOP1: OPCODE %d OK\n\r",inc_counter);
      }
      else if(check_res == 0)
        xil_printf("VOP1: OPCODE %d wrong result\n\r",inc_counter);
      else
        xil_printf("VOP1: OPCODE %d No test available\n\r",inc_counter);

    }
    else
      xil_printf("VOP1: OPCODE %d did not finish running\n\r",inc_counter);
  }
  return(running_insts_counter);
}
int32_t test_vopc(){
  /*Tests are done by issuing the following instructions and reading the results
   * exec_lo = s2
   * exec_hi = s2
   * vcc = v2 comp v3
   * s0 = vcc_lo
   * s1 = vcc_hi
   *
   * The instruction are only considered correct if the whole family
   * (opcode base) is running correctly
   * This rule has an exception when the base opcode corresponds to an OP16 and
   *  the offset is either 7 or 8 (checking if there is a NaN).
   */

  int32_t insts[6];

  insts[0] = 0xBEFE0302;//exec_lo = s2 = 0xFFFFFFFF
  insts[1] = 0xBEFF0302;//exec_hi = s2 = 0xFFFFFFFF

  insts[3] = create_sop1(3, 0, 106); //s0 = vcc_lo -> sop1 inst
  insts[4] = create_sop1(3, 1, 107); //s1 = vcc_hi -> sop1 inst
  insts[5] = END_PRGRM;

  int32_t inc_counter, data_counter, running_insts_counter = 0,
          group_inst_counter;
  int32_t res, check_res;
  int32_t inst_data[12],inst_vect_data[10];
  int32_t max_offset, offset, opcode;

  for(inc_counter = 0; inc_counter<16; inc_counter++){
    //Set if it's a cmp_{op16} or a cmp_{op8}
    if(inc_counter<8)
      max_offset = 15;
    else
      max_offset = 7;

    //Set the OPcode Base
    opcode = inc_counter << 4;

    //Run through every OPcode offset
    group_inst_counter = 0;
    for(offset = 0; offset <= max_offset; offset++){

      //Generate data for the scalar registers
      for(data_counter = 0; data_counter<12; data_counter++){
        inst_data[data_counter] = data_counter;
        if(data_counter == 2)
          inst_data[data_counter] = 0xFFFFFFFF;
      }

      //Generate data for the vector registers (which will be replicated for all
// 64 words of the register)
      for(data_counter = 0; data_counter<10; data_counter++)
        inst_vect_data[data_counter] = data_counter;

      //Generate the VOPC operation to be tested
      insts[2] =  create_vopc(opcode,offset, 3, 0x102);

      //Test the OP
      res = run_vop_program_neko(insts,6, inst_data, 12,inst_vect_data, 10,15000);

      if(res){
        //If the program finished running then the result is verified
        check_res = analyze_vopc(opcode+offset, 2,3,inst_data[0],inst_data[1]);
        if(check_res == 1){
          group_inst_counter++;
          xil_printf("VOPC: OPCODE %02x OK\n\r",opcode+offset);
        }
        else if(check_res == 0){
          xil_printf("VOPC: OPCODE %02x wrong result\n\r",opcode+offset);
          if(!((opcode==0x0 || opcode==0x10 ||opcode==0x40||opcode==0x50) && 
               (offset == 7 || offset == 8))){
            group_inst_counter=0;
            break;
          }
        }
        else
          xil_printf("VOPC: OPCODE %02x No test available\n\r",opcode+offset);
      }
      else{
        xil_printf("VOPC: OPCODE %02x did not finish running\n\r",opcode+offset);
        if(!((opcode==0x0 || opcode==0x10 ||opcode==0x40||opcode==0x50) &&
             (offset == 7 || offset == 8))){
          group_inst_counter=0;
          break;
        }
      }
    }
    running_insts_counter+=group_inst_counter;
  }
  return(running_insts_counter);
}
int32_t test_vop3a(){
  /*Tests are done by issuing the following instructions and reading the results
   * exec_lo = s0
   * exec_hi = s0
   * In the comparison (VOPC equivalent) instructions:
   *   s0 = v3 comp v2
   * In the VOP2 equivalent:
   *  v0 = v2 op v1
   *some of the vop2 have an extra constant which is added after the instruction
   * In the VOP3a only:
   *  v0 = v2 op v1 op v3
   * In the VOP1 equivalent:
   *  v0 = op v4
   * s2 = vcc_lo
   * s3 = vcc_hi
   */

  int32_t insts[8];

  insts[0] = create_sop1(3, 126, 0);
  insts[1] = create_sop1(3, 127, 0);

  insts[4] = create_sop1(3, 2, 106);
  insts[5] = create_sop1(3, 3, 107);
  insts[6] = END_PRGRM;

  int32_t inc_counter, data_counter,running_insts_counter = 0;
  int32_t max_offset, offset, opcode, group_inst_counter;
  int32_t res, check_res;
  int32_t inst_data[12], inst_vect_data[10];
  int32_t d_constant = 0;

  /*Separate the instructions between the comparison ones, the vop2 ones, the
   * vop3 and the vop1 ones*/

  //Testing the VOPC equivalent
  for(inc_counter = 0; inc_counter<16; inc_counter++){
    //Set if it's a cmp_{op16} or a cmp_{op8}
    if(inc_counter<8)
      max_offset = 15;
    else
      max_offset = 7;

    group_inst_counter = 0;

    //Set the OPcode Base
    opcode = inc_counter << 4;

    //Run through every opcode offset
    for(offset = 0; offset <= max_offset; offset++){
      //Generate data for the scalar registers
      for(data_counter = 0; data_counter<12; data_counter++){
        inst_data[data_counter] = data_counter;
        if(data_counter == 0)
          inst_data[data_counter] = 0xFFFFFFFF;
      }
      //Generate data for the vector registers (which will be replicated for all
// 64 words of the register)
      for(data_counter = 0; data_counter<10; data_counter++)
        inst_vect_data[data_counter] = data_counter;

      //Generate the VOP3a operation to be tested
      create_vop3a(opcode,offset, 0,0x101, 0x102, 0x103,0,0,0,0, &insts[2]);

      //Test the program
      res = run_vop_program_neko(insts,7, inst_data, 12,
                                          inst_vect_data, 10,15000);

      if(res){//If the program finishes test the result (tests if the whole 
//family is running correctly)
       //These first instructions can be directly analyzed in the "analyze_vopc"
        check_res = analyze_vopc(opcode+offset, 3,2,inst_data[0],inst_data[1]);
        if(check_res == 1){
          group_inst_counter++;
        }
        else if(check_res == 0){
          xil_printf("VOP3a: OPCODE %02x wrong result\n\r",opcode+offset);
          if(!((opcode==0x0 || opcode==0x10 ||opcode==0x40||opcode==0x50) 
            && (offset == 7 || offset == 8))){
            group_inst_counter=0;
            break;
          }
        }
        else
          xil_printf("VOP3a: OPCODE %02x No test available\n\r",opcode+offset);
      }
      else{
        xil_printf("VOP3a: OPCODE %02x did not finish running\n\r",
                   opcode+offset);
        if(!((opcode==0x0 || opcode==0x10 ||opcode==0x40||opcode==0x50)
          && (offset == 7 || offset == 8))){
          group_inst_counter=0;
          break;
        }
      }
    }
    if(group_inst_counter!=0)//If the whole family ran that opcode base is OK!
      xil_printf("VOP3a: OPCODE %02x OK - %d instructions running\n\r",opcode,
                                                            group_inst_counter);
    else
      xil_printf("VOP3a: OPCODE %02x NOT OK\n\r",opcode);
    running_insts_counter+=group_inst_counter;
  }
  //Testing the VOP2 instructions
  for(inc_counter = 256; inc_counter<306; inc_counter++){
    if(inc_counter>=293 && inc_counter<=298)//vop3b instructions
      continue;

    //Generate data for the scalar registers
    for(data_counter = 0; data_counter<12; data_counter++){
      inst_data[data_counter] = data_counter+1;
      if(data_counter == 0)
        inst_data[data_counter] = 0xFFFFFFFF;
    }
    //Generate data for the vector registers (which will be replicated for all
//64 words of the register)
    for(data_counter = 0; data_counter<10; data_counter++)
      inst_vect_data[data_counter] = (int32_t)data_counter;

    d_constant = 0;

    //Sets v1 and v2 to more computation "friendly" f32 values except for the
//v_mul_i32_i24 instruction
    if(inc_counter != 265){
      inst_vect_data[1] = 0x3fc00000;//v[1] = 1.5f
      inst_vect_data[2] = 0x40000000;//v[2] = 2.0f
    }

    //Adds a constant for the instructions that need it
    //(v_madmk_f32 and v_madak_f32)
    if(inc_counter==288 || inc_counter==289){

      d_constant = 0x3f800000;//1.0
      //Sets the constant after the instruction
      insts[4] = d_constant; //inline constant == 1.0f
      //Moves the remaining instructions one slot down
      insts[5] = create_sop1(3, 2, 106); //s2 = vcc_lo -> sop1 inst
      insts[6] = create_sop1(3, 3, 107); //s3 = vcc_hi -> sop1 inst
      insts[7] = END_PRGRM;
    }

    //Generates the instruction to be tested
    create_vop3a(inc_counter, 0, 0,0x103, 0x101, 0x102,0,0,0,0, &insts[2]);

    //Tests the program
    res = run_vop_program_neko(insts,8, inst_data, 12,inst_vect_data, 10,15000);

    if(res){
      //If the program has finished tests the result with the VOP2 verification
      check_res = analyze_vop2(inc_counter-256, inst_vect_data[1], 
                               inst_vect_data[2], d_constant, inst_vect_data[0],
                               inst_data[2],inst_data[3]);
      if(check_res == 1){
        running_insts_counter++;
        xil_printf("VOP3a: OPCODE %d OK\n\r",inc_counter);
      }
      else if(check_res == 0)
        xil_printf("VOP3a: OPCODE %d wrong result\n\r",inc_counter);
      else
        xil_printf("VOP3a: OPCODE %d No test available\n\r",inc_counter);
    }
    else
      xil_printf("VOP3a: OPCODE %d did not finish running\n\r",inc_counter);

    if(inc_counter==288 || inc_counter==289){
      //Restores the instruction to their rightful place
      insts[4] = create_sop1(3, 2, 106); //s2 = vcc_lo -> sop1 inst
      insts[5] = create_sop1(3, 3, 107); //s3 = vcc_hi -> sop1 inst
      insts[6] = END_PRGRM;
    }
  }
  //Testing the VOP3 instructions
  for(inc_counter = 320; inc_counter<373; inc_counter++){
    if(inc_counter== 365|| inc_counter == 366)//vop3b
      continue;
    //Generate data for the scalar registers
    for(data_counter = 0; data_counter<12; data_counter++){
      inst_data[data_counter] = data_counter+1;
      if(data_counter == 0)
        inst_data[data_counter] = 0xFFFFFFFF;
    }
    //Generate data for the vector registers (which will be replicated for all 
//64 words of the register)
    for(data_counter = 0; data_counter<10; data_counter++)
      inst_vect_data[data_counter] = (int32_t)data_counter;

    //Generates the instruction to be tested
    create_vop3a(inc_counter, 0, 0,0x103, 0x101, 0x102,0,0,0,0, &insts[2]);

    //Tests the program
    res = run_vop_program_neko(insts,7, inst_data, 12,inst_vect_data, 10,15000);

    if(res){
      //If the program has finished tests the result with the VOP3a verification
      check_res = analyze_vop3a(inc_counter, inst_vect_data[3], 
                                inst_vect_data[1],inst_vect_data[2], d_constant,
                                inst_vect_data[0],inst_data[2],inst_data[3]);
      if(check_res == 1){
        running_insts_counter++;
        xil_printf("VOP3a: OPCODE %d OK\n\r",inc_counter);
      }
      else if(check_res == 0)
        xil_printf("VOP3a: OPCODE %d wrong result\n\r",inc_counter);
      else
        xil_printf("VOP3a: OPCODE %d No test available\n\r",inc_counter);

    }
    else
      xil_printf("VOP3a: OPCODE %d did not finish running\n\r",inc_counter);
  }

  //Testing the VOP1 equivalent instructions
  for(inc_counter = 384; inc_counter<453; inc_counter++){
    if(inc_counter>=407 && inc_counter<=415)//Reserved
      continue;
    //Generate data for the scalar registers
    for(data_counter = 0; data_counter<12; data_counter++){
      inst_data[data_counter] = data_counter+1;
      if(data_counter == 0)
        inst_data[data_counter] = 0xFFFFFFFF;
    }
    //Generate data for the vector registers (which will be replicated for all 
//64 words of the register)
    for(data_counter = 0; data_counter<10; data_counter++){
      inst_vect_data[data_counter] = (int32_t)data_counter;
      if(data_counter == 4)
        inst_vect_data[data_counter] = 0x3fc00000;//1.5f
    }
    //Generates the instruction to be tested
    create_vop3a(inc_counter, 0, 0,0x100, 0x100, 0x104,0,0,0,0, &insts[2]);

    //Runs the program
    res = run_vop_program_neko(insts,7, inst_data, 12,inst_vect_data, 10,15000);
    if(res){
      //If the program has finished tests the result with the VOP1 verification
      check_res = analyze_vop1(inc_counter-384, 0x3fc00000, inst_vect_data[0],
                               inst_data[2], inst_data[3]);
      if(check_res == 1){
        running_insts_counter++;
        xil_printf("VOP3a: OPCODE %d OK\n\r",inc_counter);
      }
      else if(check_res == 0)
        xil_printf("VOP3a: OPCODE %d wrong result\n\r",inc_counter);
      else
        xil_printf("VOP3a: OPCODE %d No test available\n\r",inc_counter);
    }
    else
      xil_printf("VOP3a: OPCODE %d did not finish running\n\r",inc_counter);
  }
  return(running_insts_counter);
}
int32_t test_vop3b(){
  /*Tests are done by issuing the following instructions and reading the results
   * exec_lo = s0
   * exec_hi = s0
   * v0 = v3 op v2 ; s0 = carry_out
   */

  int32_t insts[5];

  insts[0] = create_sop1(3, 126, 0);//0xBEFE0302;//exec_lo = s0 = 0xFFFFFFFF
  insts[1] = create_sop1(3, 127, 0);//0xBEFF0302;//exec_hi = s0 = 0xFFFFFFFF

  insts[4] = END_PRGRM;
  int32_t inc_counter, data_counter, running_insts_counter = 0;
  int32_t res;
  int32_t inst_data[12], inst_vect_data[10];

  for(inc_counter = 293; inc_counter<299; inc_counter++){//missing 365 and 366
    //Generate data for the scalar registers
      for(data_counter = 0; data_counter<12; data_counter++){
        inst_data[data_counter] = data_counter+1;
        if(data_counter == 0)
          inst_data[data_counter] = 0xFFFFFFFF;
      }
      //Generate data for the vector registers
      for(data_counter = 0; data_counter<10; data_counter++)
        inst_vect_data[data_counter] = (int32_t)data_counter;

      //Generate the VOP3b operation to be tested
      create_vop3b(inc_counter, 0, 0,0x101, 0x102, 0x103, 0, 0, &insts[2]);

      //Run the program
      res = run_vop_program_neko(insts,5, inst_data, 12,
                                          inst_vect_data, 10,15000);

      if(res){//Check if the program finishes
        running_insts_counter++;
        xil_printf("VOP3b: OPCODE %d is running\n\r",inc_counter);
        }
      else
        xil_printf("VOP3b: OPCODE %d did not finish running\n\r",inc_counter);
  }
  return(running_insts_counter);
}

void test_vector_instructions(){
  uint32_t total = 0, type_count = 0;
  type_count = test_vop2();
  total += type_count;
  xil_printf("VOP2 - %d instructions running\n\r",type_count);

  type_count = test_vop1();
  total += type_count;
  xil_printf("VOP1 - %d instructions running\n\r",type_count);

  type_count = test_vopc();
  total += type_count;
  xil_printf("VOPC - %d instructions running\n\r",type_count);

  type_count = test_vop3a();
  total += type_count;
  xil_printf("VOP3a - %d instructions running\n\r",type_count);

  type_count = test_vop3b();
  total += type_count;
  xil_printf("VOP3b - %d instructions running\n\r",type_count);

  xil_printf("Total number of vector instructions running correctly: %d\n\r",
                                                                         total);
}

int main()
{
    init_platform();

    XIo_Out32(NEKO_RESET,0);
    XIo_Out32(NEKO_RESET,1);
    XIo_Out32(NEKO_RESET,0);

    if(!test_vector_inst_creation())
          return 1;

    test_vector_instructions();

    XIo_Out32(VGPR_ADDR, 0);
    cleanup_platform();
    return 0;
}
