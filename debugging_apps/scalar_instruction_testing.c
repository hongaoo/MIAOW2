#include <stdio.h>
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
 unsigned u;
};

uint32_t reverse_bit_order(register uint32_t x){
  //Inverts the bits in a 32bit word
  //Credits to: Sean Eron Anderson
  //http://graphics.stanford.edu/~seander/bithacks.html
  x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
  x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
  x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
  x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
  return((x >> 16) | (x << 16));
}
int32_t create_sop2(int32_t op, int32_t sdst, int32_t s1, int32_t s0){
  /* SOP2 instruction format:
   * MSB -> LSB
   * | ENC(2) = 2'b10 | OP(7) | SDST(7) | SSRC1(8) | SSRC0 |
   */
   int32_t inst = 0x80000000;
   op = op << 23;
   sdst = sdst << 16;
   s1 = s1 << 8;
   inst = inst | op | sdst | s1 | s0;
   return(inst);
 }
 int32_t create_sopk(int32_t op, int32_t sdst, int32_t simm){
  /* SOPK instruction format:
   * MSB -> LSB
   * | ENC(4) = 4'b1011 | OP(5) | SDST(7) | SIMM16(16) |
   */
   int32_t inst = 0xB0000000;
   op = op << 23;
   sdst = sdst << 16;
   inst = inst | sdst | op | simm;
   return(inst);
 }
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
 int32_t create_sopc(int32_t op, int32_t s1, int32_t s0){
  /* SOPC instruction format:
   * MSB -> LSB
   * | ENC(9) = 9'b101111110 | OP(7) | SSRC1(8) | SSRC0(8) |
   */
   int32_t inst = 0xBF000000;
   s1 = s1 << 8;
   op = op << 16;
   inst = inst | op | s1 | s0;
   return(inst);
 }
 int32_t create_sopp(int32_t op, int32_t simm){
  /* SOPP instruction format:
   * MSB -> LSB
   * | ENC(9) = 9'b101111111 | OP(7) | SIMM16(16) |
   */
   int32_t inst = 0xBF800000;
   op = op << 16;
   inst = inst | op | simm;
   return(inst);
 }
 int32_t create_smrd(int32_t op, int32_t sdst, int32_t sbase, int32_t imm, 
   int32_t offset){
  /* SMRD instruction format:
   * MSB -> LSB
   * | ENC(5) = 5'b11000 | OP(5) | SDST(7) | SBASE(6) | IMM(1) | OFFSET(8) |
   */
   int32_t inst = 0xC0000000;
   op = op << 22;
   sdst = sdst << 15;
   sbase = sbase << 9;
   imm = imm << 8;
   inst = inst | op | sdst | sbase | imm | offset;
   return(inst);
 }
 int32_t run_sop_program_neko(int32_t insts[],int32_t num_insts, 
   int32_t inst_data[], int32_t num_data,
   int32_t max_clocks){
  /*
   * Resets Neko
   * Populates NEKO's instruction buffer and the scalar registers
   * Send "start execution" command and waits for program completion or until
   * the timeout is reached
   * If the program reaches the end of execution before the timeout the data in 
   * the scalar regs is read and success(1) is returned
   * Otherwise returns 0 (unsuccessful)
   */

   int32_t index, address, data;
   int32_t cycle_counter = 0;
   int32_t succeeded = 1;

  //NEKO's reset pulse
   XIo_Out32(NEKO_RESET,0);
   XIo_Out32(NEKO_RESET,1);
   XIo_Out32(NEKO_RESET,0);


   XIo_Out32(NEKO_BASE_LDS, XPAR_MIG_7SERIES_0_BASEADDR);

  //Load scalar registers with data
   for(index = 0; index < num_data; index+=4)
   {
    XIo_Out32(NEKO_SGRP_ADDR, index);
    XIo_Out32(NEKO_SGRP_QUAD_0, inst_data[index]);
    XIo_Out32(NEKO_SGRP_QUAD_1, inst_data[index+1]);
    XIo_Out32(NEKO_SGRP_QUAD_2, inst_data[index+2]);
    XIo_Out32(NEKO_SGRP_QUAD_3, inst_data[index+3]);
    XIo_Out32(NEKO_GPR_CMD, 1);
  }

  //Load the instruction buffer
  for(index = 0; index < num_insts; index++)
  {
    XIo_Out32(NEKO_INSTR_ADDR, index);
    XIo_Out32(NEKO_INSTR_VALUE, insts[index]);
  }

  //Start execution
  XIo_Out32(NEKO_CMD_ADDR, 1);

  //Wait for the end of execution
  while(XIo_In32(NEKO_CMD_ADDR) != 1)
  {
  //Verify the timeout
    cycle_counter = XIo_In32(NEKO_CYCLE_COUNTER);
    if(cycle_counter>max_clocks){
      succeeded=0;
      break;
    }
    data = XIo_In32(NEKO_MEM_OP);
    if(data != 0)
    {
      int nextValue = MEM_RD_RDY_WAIT;
      if(data == MEM_RD_ACK_WAIT)
      {
        nextValue = MEM_RD_RDY_WAIT;
      }
      else if(data == MEM_WR_ACK_WAIT)
      {
        nextValue = MEM_WR_RDY_WAIT;
      }
      else if(data == MEM_WR_LSU_WAIT || data == MEM_RD_LSU_WAIT )
        continue;//last instruction is not finished yet

      XIo_Out32(NEKO_MEM_ACK, 0);
      XIo_Out32(NEKO_MEM_ACK, 1);

      do {
        data = XIo_In32(NEKO_MEM_OP);
      } while(data != nextValue);

      address = XIo_In32(NEKO_MEM_ADDR);

      if(nextValue == MEM_RD_RDY_WAIT)
      {
        data = XIo_In32(address);
        XIo_Out32(NEKO_MEM_WR_DATA, 0x012345678);
        nextValue = MEM_RD_LSU_WAIT;
      }
      else
      {
        data = XIo_In32(NEKO_MEM_RD_DATA);
        XIo_Out32(address, data);
        nextValue = MEM_WR_LSU_WAIT;
      }

      XIo_Out32(NEKO_MEM_DONE, 0);
      XIo_Out32(NEKO_MEM_DONE, 1);

      do {
        data = XIo_In32(NEKO_MEM_OP);

      } while(data != 0 && data != nextValue && data != MEM_RD_ACK_WAIT 
       && data != MEM_WR_ACK_WAIT);
    }
  }

  if(succeeded){
    //Retrieve the data in the scalar registers (results)
    for(index = 0; index < num_data; index+=4)
    {
      XIo_Out32(NEKO_SGRP_ADDR, index);
      inst_data[index]=XIo_In32(NEKO_SGRP_QUAD_0);
      inst_data[index+1]=XIo_In32(NEKO_SGRP_QUAD_1);
      inst_data[index+2]=XIo_In32(NEKO_SGRP_QUAD_2);
      inst_data[index+3]=XIo_In32(NEKO_SGRP_QUAD_3);
    }
  }
  //NEKO's reset pulse
  XIo_Out32(NEKO_RESET,0);
  XIo_Out32(NEKO_RESET,1);
  XIo_Out32(NEKO_RESET,0);

  return(succeeded);
}
int32_t analyze_sop2(int32_t opcode, int32_t s1, int32_t s0, 
 int32_t res,int32_t scc){
  //Checks the result of the operation (opcode) with operands s1 and s0 and 
  //results in res
  //Some instructions write to the scc register
  int32_t correct = 0;
  //Convertion of operands and result to unsigned int which is used by some 
  //of the operations
  uint32_t u_s1 = (uint32_t)s1;
  uint32_t u_s0 = (uint32_t)s0;
  uint32_t u_res = (uint32_t)res;

  //Result storage variables
  int32_t ver_res;
  uint32_t u_ver_res;

  switch(opcode){
  case 0://s_add_u32
  u_ver_res = u_s0 + u_s1;
  if(u_res==u_ver_res)
    correct = 1;
  break;
  case 1://s_sub_u32
  u_ver_res = u_s0 - u_s1;
  if(u_res==u_ver_res)
    correct = 1;
  break;
  case 2://s_add_i32
  ver_res = s0+s1;
  if(ver_res==res)
    correct = 1;
  break;
  case 3://s_sub_i32
  ver_res = s0-s1;
  if(ver_res==res)
    correct = 1;
  break;
  case 7://s_min_u32
  ver_res = s0 < s1 ? s0 : s1;
  if(ver_res==res)
    correct = 1;
  break;
  case 8://s_max_i32
  ver_res = s0 > s1 ? s0 : s1;
  if(ver_res==res)
    correct = 1;
  break;
  case 9://s_max_u32
  u_ver_res = u_s0 > u_s1 ? u_s0 : u_s1;
  if(u_ver_res==u_res)
    correct = 1;
  break;
  case 10:
  u_ver_res = (scc & 0x01) ? u_s0 : u_s1;
  correct = (u_ver_res == u_res);
  break;
  case 14://s_and_b32
  case 15://s_and_b64
  if((s0 & s1) == res){
    if(res!=0)
      correct = scc == 1 ? 1 : 0;
    else
      correct = scc == 1 ? 0 : 1;
  }
  break;
  case 16://s_or_b32
  case 17://s_or_b64
  if((s0 | s1) == res){
    if(res!=0)
      correct = scc == 1 ? 1 : 0;
    else
      correct = scc == 1 ? 0 : 1;
  }
  break;
  case 21://s_andn2_b64
  if((s0 & ~s1) == res){
    if(res!=0)
      correct = scc == 1 ? 1 : 0;
    else
      correct = scc == 1 ? 0 : 1;
  }
  break;
  case 30://s_lshl_b32
  u_ver_res = u_s0 << (u_s1 & 0x01f);
  if(u_ver_res == u_res){
    if(u_res!=0)
      correct = scc == 1 ? 1 : 0;
    else
      correct = scc == 1 ? 0 : 1;
  }
  break;
  case 32://s_lshr_b32
  u_ver_res = u_s0 >> (u_s1 & 0x01f);
  if(u_ver_res == u_res){
    if(u_res!=0)
      correct = scc == 1 ? 1 : 0;
    else
      correct = scc == 1 ? 0 : 1;
  }
  break;
  case 34://s_ashr_i32
  ver_res = s0 >> (s1 & 0x01f);
  if(ver_res == res){
    if(res!=0)
      correct = scc == 1 ? 1 : 0;
    else
      correct = scc == 1 ? 0 : 1;
  }
  break;
  case 38://s_mul_i32
  ver_res = s0 * s1 ;
  if(ver_res == res)
    correct = 1;
  break;
  default:
  correct = -1;
}
return correct;
}

int32_t analyze_sopk(int32_t opcode, int16_t simm, int32_t s0,
 int32_t res,int32_t scc){
  //Checks the result of the operation (opcode) with operands simm and 
  //s0(original register value) and results in res(final register value)
  //Some instructions write to the scc register

  int32_t correct = 0;
  int32_t i_simm = (int32_t)simm;

  switch(opcode){
  case 0://s_movk_i32
  if(res == i_simm)
    correct = 1;
  break;
  case 15://s_addk_i32
  if((s0 + i_simm) == res)
    correct =  1;
  break;
  case 16://s_mulk_i32
  if((s0 * i_simm) == res)
    correct =  1;
  break;
  default:
  correct = -1;
}
return correct;
}
int32_t analyze_sop1(int32_t opcode, int32_t s[], int32_t res[],int32_t scc){
  //Checks the result of the operation (opcode) with operands s[] and results 
  //in res[]
  //Some instructions write to the scc register

  int32_t correct = 0;
  uint32_t u_ver_res;

  switch(opcode){
  case 3://s_mov_b32
  case 4://s_mov_b64
  if(res[0] == s[2])
    correct = 1;
  break;
  case 5:
  u_ver_res = (scc & 0x01) ? (uint32_t)s[2] : (uint32_t)s[0];
  correct = (u_ver_res == (uint32_t)res[0]);
  break;
  case 7://s_not_b32
  case 8://s_not_b64
  if(~s[2] == res[0])
    correct =  1;
  break;
  case 11:
  correct = (reverse_bit_order(s[2]) == (uint32_t)res[0]);
  break;
  case 36://s_and_saveexec_b64
    //needs to read exec!
    /* exec_lo = s2 (==1)
     * exec_hi = s1 (==0)
     * s_and_saveexec_b64 s0, s2
     * s2 = exec_lo
     * s3 = exec_hi
     *  */
    if(res[0] == s[2] && res[1] == s[1]){// res[0] = (exec_lo = s2)
                       // res[1] = (exec_hi = s1)
      if(res[2] == (res[0] & s[2]) && res[3] == (res[1] & s[3])){
                       //res[2] = exec[31:0] & s2
                       //res[3] = exec[63:32]& s3
        if(res[2]!=0 || res[3]!=0)//exec!=0 -> scc = 1
          correct = scc == 1 ? 1 : 0;
        else
          correct = scc == 1 ? 0 : 1;
      }
    }
    break;
    default:
    correct = -1;
  }
  return correct;
}
int32_t analyze_sopc(int32_t opcode, int32_t s1, int32_t s0, int32_t scc){
  //Checks the result(scc) of the comparison (opcode) with operands s1 and s0

  int32_t correct = 0;

  //Conversion for the unsigned comparisons
  uint32_t u_s0 = (uint32_t)s0;
  uint32_t u_s1 = (uint32_t)s1;

  switch(opcode){
  case 0://s_cmp_eq_i32
  if((s0 == s1) == scc)
    correct = 1;
  break;
  case 1://s_cmp_lg_i32
  if((s0 != s1) == scc)
    correct = 1;
  break;
  case 2://s_cmp_gt_i32
  if((s0 > s1) == scc)
    correct = 1;
  break;
  case 3://s_cmp_ge_i32
  if((s0 >= s1) == scc)
    correct = 1;
  break;
  case 4://s_cmp_lt_i32
  if((s0 < s1) == scc)
    correct = 1;
  break;
  case 5://s_cmp_le_i32
  if((s0 <= s1) == scc)
    correct = 1;
  break;
  case 6://s_cmp_eq_u32
  if((u_s0 == u_s1) == scc)
    correct = 1;
  break;
  case 7://s_cmp_lg_u32
  if((u_s0 != u_s1) == scc)
    correct = 1;
  break;
  case 8://s_cmp_gt_u32
  if((u_s0 > u_s1) == scc)
    correct = 1;
  break;
  case 9://s_cmp_ge_u32
  if((u_s0 >= u_s1) == scc)
    correct = 1;
  break;
  case 10://s_cmp_lt_u32
  if((u_s0 < u_s1) == scc)
    correct = 1;
  break;
  case 11://s_cmp_le_u32
  if((u_s0 <= u_s1) == scc)
    correct = 1;
  break;
  default:
  correct = -1;
}
return correct;
}
int32_t analyze_sopp(int32_t opcode, int32_t res, int32_t exec, int32_t scc){
  //Test if the branches were taken (jumps over s0 = 16)

  int32_t correct = 0;

  switch(opcode){
  case 0://s_nop
  correct = (res == 17);
  break;
  case 1://s_endpgm
  correct = (res == 0);
  break;
  case 2://s_branch
  correct = (res == 1);
  break;
  case 4://s_branch_scc0
  if(scc == 0)
    correct = (res == 1);
  else
    correct = (res == 17);
  break;
  case 5://s_branch_scc1
  if(scc == 1)
    correct = (res == 1);
  else
    correct = (res == 17);
  break;
  case 6://s_branch_vccz
  case 7://s_branch_vccnz
  break;
  case 8://s_branch_execz
  if(exec == 0)
    correct = (res == 1);
  else
    correct = (res == 17);
  break;
  case 9://s_branch_execnz
  if(exec == 1)
    correct = (res == 1);
  else
    correct = (res == 17);
  break;
    //TODO: not sure how to test these 2
  case 10://s_barrier
  case 12://s_waitcnt
  correct = 1;
  break;
  default:
  correct = -1;
}
return correct;
}
int32_t analyze_smrd(int32_t opcode, int32_t s[]){
  //Tests if the right number of registers got the word "0x12345678"

  switch(opcode){
  case 0://S_LOAD_DWORD
  case 8://S_BUFFER_LOAD_DWORD
  return (s[0] == 0x12345678);
  break;
  case 1://S_LOAD_DWORDX2
  case 9://S_BUFFER_LOAD_DWORDX2
  return (s[0] == 0x12345678 && s[1] == 0x12345678);
  break;
  case 2://S_LOAD_DWORDX4
  case 10://S_BUFFER_LOAD_DWORDX4
  return (s[0] == 0x12345678 && s[1] == 0x12345678 && s[2] == 0x12345678 
   && s[3] == 0x12345678);
  break;
  case 3://S_LOAD_DWORDX8
  case 11://S_BUFFER_LOAD_DWORDX8
  return (s[0] == 0x12345678 && s[1] == 0x12345678 && s[2] == 0x12345678 
   && s[3] == 0x12345678 && s[4] == 0x12345678 
   && s[5] == 0x12345678 && s[6] == 0x12345678 
   && s[7] == 0x12345678);
  break;
  case 4://S_LOAD_DWORDX16
  case 12://S_BUFFER_LOAD_DWORDX16
  return (s[0] == 0x12345678 && s[1] == 0x12345678 && s[2] == 0x12345678
   && s[3] == 0x12345678 && s[4] == 0x12345678 && s[5] == 0x12345678
   && s[6] == 0x12345678 && s[7] == 0x12345678 && s[8] == 0x12345678
   && s[9] == 0x12345678 && s[10] == 0x12345678 && s[11] == 0x12345678
   && s[12] == 0x12345678 && s[13] == 0x12345678 && s[14] == 0x12345678
   && s[15] == 0x12345678);
  break;

  default:
  return 0;
}
}
int32_t test_sop2(){
  //Tests are done by issuing the following instructions and reading the results
  //s0 = s2 op s3 -> sop2 inst
  //s1 = scc -> sop1 inst

  int32_t insts[3];
  insts[1] = create_sop1(3, 1, 0xFD); //s1 = scc - sop1 inst
  insts[2] = END_PRGRM;

  int32_t inc_counter, data_counter, res, res_check, inst_data[12];
  int32_t running_insts_counter = 0;

  //Generates SOP2 instructions and verifies their state (running with/without 
  //correct result or not running)
  for(inc_counter = 0; inc_counter<45; inc_counter++){
    if(inc_counter==12 || inc_counter == 13)
      continue;

    //Data for the scalar registers
    for(data_counter = 0; data_counter<12; data_counter++)
      inst_data[data_counter] = data_counter;

    //Generate SOP2 Instruction
    insts[0] = create_sop2(inc_counter, 0, 2, 3);

    //Check running state
    res = run_sop_program_neko(insts,3,inst_data,12,500);

    if(res){//If the program finishes verifies the result
      res_check = analyze_sop2(inc_counter, 2, 3, inst_data[0],inst_data[1]);
      if(res_check == 1){
        xil_printf("SOP2: OPCODE %d OK\n\r",inc_counter);
        running_insts_counter++;
      }
      else if(res_check == 0)
        xil_printf("SOP2: OPCODE %d Wrong Result\n\r",inc_counter);
      else
        xil_printf("SOP2: OPCODE %d No test available\n\r",inc_counter);
    }
    else
      xil_printf("SOP2: OPCODE %d did not finish running\n\r",inc_counter);
  }
  return(running_insts_counter);
}
int32_t test_sopk(){
  //Tests are done by issuing the following instructions and reading the results
  //s0 = op imm(=2) -> sopk inst
  //s1 = scc -> sop1 inst

  int32_t insts[3];
  insts[1] = create_sop1(3, 1, 0xFD); //s1 = scc -> sop1 inst
  insts[2] = END_PRGRM;
  int32_t inc_counter,data_counter;
  int32_t res, res_check, inst_data[12];
  int32_t running_insts_counter = 0;

  //Generates SOPK instructions and verifies their state (running with/without
  // correct result or not running)
  for(inc_counter = 0; inc_counter<17; inc_counter++){//Last 4 (17-21) 
    if(inc_counter==1)                                //are not implemented
      continue;

    //Data for the scalar registers
    for(data_counter = 0; data_counter<12; data_counter++)
      inst_data[data_counter] = data_counter+1;

    //Generate SOPK Instruction
    insts[0] = create_sopk(inc_counter, 0, 2);

    //Check running state
    res = run_sop_program_neko(insts,3,inst_data,12,500);

    if(res){//If the program finishes verifies the result
      res_check = analyze_sopk(inc_counter, 2, 1, inst_data[0],inst_data[1]);
      if(res_check == 1){
        running_insts_counter++;
        xil_printf("SOPK: OPCODE %d OK\n\r",inc_counter);
      }
      else if(res_check == 0)
        xil_printf("SOPK: OPCODE %d Wrong Result\n\r",inc_counter);
      else
        xil_printf("SOPK: OPCODE %d No test available\n\r",inc_counter);
    }
    else
      xil_printf("SOPK: OPCODE %d did not finish running\n\r",inc_counter);
  }
  return(running_insts_counter);
}
int32_t test_sop1(){
  //Tests are done by issuing the following instructions and reading the results
  //exec_hi = s1 -> sop1 inst
  //exec_lo = s2 -> sop1 inst
  //s0 = op s2 -> sop1 inst
  //s2 = exec_lo -> sop1 inst
  //s3 = exec_hi -> sop1 inst
  //s4 = scc -> sop1 inst

  int32_t insts[7];
  insts[0] = create_sop1(3, 127,1);//exec_hi = s1 (==0)
  insts[1] = create_sop1(3, 126,2);//exec_lo = s2 (==1)
  insts[3] = create_sop1(3, 2,126);//s2 = exec_lo (==0)
  insts[4] = create_sop1(3, 3,127);//s3 = exec_hi (==0)
  insts[5] = create_sop1(3, 4, 0xFD); //s4 = scc -> sop1 inst
  insts[6] = END_PRGRM;

  int32_t inc_counter, data_counter, res, res_check, inst_data[12],ori_data[12];
  int32_t running_insts_counter = 0;

  //Generates SOP1 instructions and verifies their state (running with/without
  //correct result or not running)
  for(inc_counter = 3; inc_counter<54; inc_counter++){ 
    if(inc_counter == 35 || inc_counter == 51)
      continue;

    //Data for the scalar registers
    for(data_counter = 0; data_counter<12; data_counter++){
      inst_data[data_counter] = data_counter-1;
      ori_data[data_counter] = data_counter-1;
    }

    //Generate SOP1 Instruction
    insts[2] = create_sop1(inc_counter, 0, 2);

    //Check running state
    res = run_sop_program_neko(insts,7,inst_data,12,500);

    if(res){//If the program finishes verifies the result
      res_check = analyze_sop1(inc_counter, ori_data, inst_data,inst_data[4]);
      if(res_check == 1 ){
        running_insts_counter++;
        xil_printf("SOP1: OPCODE %d OK\n\r",inc_counter);
      }
      else if(res_check == 0)
        xil_printf("SOP1: OPCODE %d wrong result\n\r",inc_counter);
      else
        xil_printf("SOP1: OPCODE %d No test available\n\r",inc_counter);
    }
    else
      xil_printf("SOP1: OPCODE %d did not finish running\n\r",inc_counter);
  }
  return(running_insts_counter);
}
int32_t test_sopc(){
  //Tests are done by issuing the following instructions and reading the results
  //scc = s2 comp s3 -> sopc inst
  //s1 = scc -> sopc inst

  int32_t insts[3];
  insts[1] = create_sop1(3, 1, 0xFD); //s1 = scc -> sop1 inst
  insts[2] = END_PRGRM;

  int32_t inc_counter, res, res_check, inst_data[12];
  int32_t running_insts_counter = 0;

  //Data for the scalar registers
  for(inc_counter = 0; inc_counter<12; inc_counter++)
    inst_data[inc_counter] = inc_counter-1;

  //Generates SOPC instructions and verifies their state (running with/without 
  //correct result or not running)
  for(inc_counter = 0; inc_counter<17; inc_counter++){//Last 5 (12-16)
                                                      // are not implemented

    //Generate SOPC Instruction
    insts[0] = create_sopc(inc_counter, 2, 3);

    //Check running state
    res = run_sop_program_neko(insts,3,inst_data,12,500);

    if(res){//If the program finishes verifies the result
      res_check = analyze_sopc(inc_counter, 2, 3, inst_data[1]);
      if(res_check == 1){
        running_insts_counter++;
        xil_printf("SOPC: OPCODE %d OK\n\r",inc_counter);
      }
      else if(res_check == 0)
        xil_printf("SOPC: OPCODE %d wrong result\n\r",inc_counter);
      else
        xil_printf("SOPC: OPCODE %d No test available\n\r",inc_counter);
    }
    else
      xil_printf("SOPC: OPCODE %d did not finish running\n\r",inc_counter);
  }
  return(running_insts_counter);
}
int32_t test_sopp(){
  /*Tests are done by issuing the following instructions and reading the results
   * scc = s2 comp s3  == 1 -> sopc inst
   * exec = 0x00000000 0x00000001 -> sop1 instructions
   * vcc = ??? (not implemented yet)
   * sopp instruction with imm=1
   * s0 = 16 (may be jumped) -> sopk
   * s0 = s0 + 1 -> sopk
   */

   int32_t insts[7];
  insts[0] = create_sopc(1, 2, 3);//set scc as 1->(scc=(s2 != s3) which is true)

  //set exec as 1
  insts[1] = create_sop1(3, 127,0);//exec_hi = s0 (==0)
  insts[2] = create_sop1(3, 126,1);//exec_lo = s1 (==1)

  //branch testing instructions
  insts[4] = create_sopk(0, 0, 16);//s0 = 16
  insts[5] = create_sopk(15, 0, 1);//s0 = s0 +1

  insts[6] = END_PRGRM;

  int32_t inc_counter,data_counter, res, res_check, inst_data[12];
  int32_t running_insts_counter = 0;


  for(inc_counter = 0; inc_counter<13; inc_counter++){
    if(inc_counter == 3 || inc_counter == 11)
      continue;
    //Data for the scalar registers
    for(data_counter = 0; data_counter<12; data_counter++)
      inst_data[data_counter] = data_counter;

    //Generate sopp instruction
    insts[3] = create_sopp(inc_counter, 1);

    //Test if the instruction is running
    res = run_sop_program_neko(insts,7,inst_data,12,500);

    if(res){//If the instruction is running verifies the result
      res_check =  analyze_sopp(inc_counter, inst_data[0], 1, 1);
      if(res_check == 1){
        running_insts_counter++;
        xil_printf("SOPP: OPCODE %d OK\n\r",inc_counter);
      }
      else if(res_check == 0)
        xil_printf("SOPP: OPCODE %d wrong result\n\r",inc_counter);
      else
        xil_printf("SOPP: OPCODE %d No test available\n\r",inc_counter);
    }
    else
      xil_printf("SOPP: OPCODE %d did not finish running\n\r",inc_counter);
  }
  return(running_insts_counter);
}
int32_t test_smrd(){
  //Tests are done by issuing the following instructions and reading the results
  //exec_lo = 0xffffffff
  //exec_hi = 0xffffffff
  //s[0..] = mem_access (accesses can get 1,2,4,8,or 16 words from memory, which 
  //will be written contiguosly in the registers)
  //s0 = memory access sbase = s4, imm = 0;

  int32_t insts[4];

  insts[0] = create_sop1(3, 126, 0);//0xBEFE0302;//exec_lo = s0 = 0xFFFFFFFF
  insts[1] = create_sop1(3, 127, 0);//0xBEFF0302;//exec_hi = s0 = 0xFFFFFFFF


  insts[3] = END_PRGRM;
  int32_t inc_counter, data_counter;
  int32_t res;
  int32_t inst_data[16];
  int32_t running_insts_counter = 0;


  inc_counter = 0;
  for(inc_counter = 0; inc_counter<13; inc_counter++){
    if(inc_counter>4 && inc_counter<8)
      continue;

    for(data_counter = 0; data_counter<16; data_counter++){
      inst_data[data_counter] = data_counter-1;
      if(data_counter == 0)
        inst_data[data_counter] = 0xffffffff;
    }

    insts[2] = create_smrd(inc_counter, 0, 2, 1, 0);

    res = run_sop_program_neko(insts,4,inst_data,16,300000000);
    if(res){
      if(analyze_smrd(inc_counter, inst_data)){
        running_insts_counter++;
        xil_printf("SMRD: OPCODE %d OK\n\r",inc_counter);
      }
      else
        xil_printf("SMRD: OPCODE %d Wrong Result\n\r",inc_counter);
    }
    else
      xil_printf("SMRD: OPCODE %d did not finish running\n\r",inc_counter);
  }
  return(running_insts_counter);
}
int32_t test_scalar_inst_creation(){
  //Verifies the instruction creation by comparing against manually generated 
  //instructions

  xil_printf("Instruction generation\n\r");
  if(create_sop2(2, 1, 4, 1) != 0x81010401){//s_add_i32 s1, s1, s4 - 81010401
    xil_printf("SOP2: Failed\n\r");
    return 0;
  }
  else
    xil_printf("SOP2: Ok\n\r");

  if(create_sopk(15, 5, 2) != 0xB7850002){//s_addk_i32 s5, 0x0002 - B7850002
    xil_printf("SOPK: Failed\n\r");
    return 0;
  }
  else
   xil_printf("SOPK: Ok\n\r");

  if(create_sop1(36, 2, 6) != 0xBE822406){//s_and_saveexec_b64 s[2:3], s[6:7]
                                          // - BE822406
   xil_printf("SOP1: Failed\n\r");
   return 0;
 }
 else
  xil_printf("SOP1: Ok\n\r");

  if(create_sopc(11, 0, 4) != 0xBF0B0004){//s_cmp_le_u32 s4, s0 - BF0B0004
    xil_printf("SOPC: Failed\n\r");
    return 0;
  }
  else
    xil_printf("SOPC: Ok\n\r");

  if(create_sopp(12, 1) != 0xBF8C0001){//s_waitcnt vmcnt(1) & lgkmcnt(0) 
                                       //& expcnt(0) -BF8C0001
    xil_printf("SOPP: Failed\n\r");
    return 0;
  }
  else
    xil_printf("SOPP: Ok\n\r");

  if(create_smrd(2, 0,6,1,2) != 0xC0800D02){//s_load_dwordx4 s[0:3], s[12:13], 2
    xil_printf("SMRD: Failed\n\r");
    return 0;
  }
  else
    xil_printf("SMRD: Ok\n\r");

  return 1;
}

void test_scalar_instructions(){
  uint32_t total = 0, type_count = 0;
  type_count = test_sop2();
  total += type_count;
  xil_printf("SOP2 - %d instructions running\n\r",type_count);
  type_count = test_sopk();
  total += type_count;
  xil_printf("SOPK - %d instructions running\n\r",type_count);
  type_count = test_sop1();
  total += type_count;
  xil_printf("SOP1 - %d instructions running\n\r",type_count);
  type_count = test_sopc();
  total += type_count;
  xil_printf("SOPC - %d instructions running\n\r",type_count);
  type_count = test_sopp();
  total += type_count;
  xil_printf("SOPP - %d instructions running\n\r",type_count);
  type_count = test_smrd();
  total += type_count;
  xil_printf("SMRD - %d instructions running\n\r",type_count);
  xil_printf("Total scalar instructions running: %d\n\r",total);
}
int main()
{
  init_platform();

  XIo_Out32(NEKO_RESET,0);
  XIo_Out32(NEKO_RESET,1);
  XIo_Out32(NEKO_RESET,0);

  if(!test_scalar_inst_creation())
    return 1;

  test_scalar_instructions();

  cleanup_platform();
  return 0;
}

