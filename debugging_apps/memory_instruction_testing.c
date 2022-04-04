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
#define NEKO_MEM_RD_DATA (NEKO_CMD_ADDR + 132) // Address for data to be read from MIAOW and written to memory
#define NEKO_MEM_ADDR (NEKO_CMD_ADDR + 136)
#define NEKO_MEM_WR_DATA (NEKO_CMD_ADDR + 192) // Address for writing data to MIAOW
#define NEKO_MEM_WR_EN (NEKO_CMD_ADDR + 196)
#define NEKO_MEM_ACK (NEKO_CMD_ADDR + 200)
#define NEKO_MEM_DONE (NEKO_CMD_ADDR + 204)

#define NEKO_CYCLE_COUNTER (NEKO_CMD_ADDR + 192)
#define NEKO_PC (NEKO_CMD_ADDR + 196)

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
} x;
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

void create_mtbuf(int32_t op, int32_t offset,int32_t offen,int32_t idxen,int32_t glc,int32_t addr64,int32_t dfmt, int32_t nfmt, int32_t vaddr,int32_t vdata,int32_t srsrc,int32_t slc, int32_t tfe, int32_t soffset,int32_t *inst){
    inst[0] = 0xE8000000;
    inst[1] = 0x00000000;
    offen = offen << 12;
    idxen = idxen << 13;
    glc = glc << 14;
    addr64 = addr64 << 15;
    op = op << 16;
    dfmt = dfmt << 19;
    nfmt = nfmt << 23;
    inst[0] = inst[0] | nfmt | dfmt | op | addr64 | glc | idxen | offen | offset;

    vdata = vdata << 8;
    srsrc = srsrc << 16;
    slc = slc << 22;
    tfe = tfe << 23;
    soffset = soffset << 24;
    inst[1] = inst[1] | soffset | tfe | slc | srsrc | vdata | vaddr;
}


int32_t run_vop_program_neko(int32_t insts[],int32_t num_insts, int32_t inst_scalar_data[], int32_t num_scalar_data,int32_t inst_vect_data[], int32_t num_vect_data,int32_t max_clocks){
/*
 * Execution Flow:
 * Resets Neko
 * Populates NEKO's instruction buffer, the scalar registers and the vector registers (all 64 words of a register are initialized with the same value)
 * Send "start execution" command and waits for program completion or until the timeout is reached
 * If the program reaches the end of execution before the timeout the data in the registers is read and success(1) is returned
 * Otherwise returns 0 (unsuccessful)
 */
 uint32_t pc;
 int32_t index, cycle_counter = 0, succeeded = 1,address,data,counter = 0;
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

//Load vector registers with data (replicating the data for every word of the register)
for(vgpr=0;vgpr<num_vect_data;vgpr++){
  XIo_Out32(VGPR_ADDR, vgpr);
  XIo_Out32(VGPR_WR_CLEAN, 1);
  XIo_Out32(VGPR_WR_CMD, 1);
  for(vgpr_word=0;vgpr_word<64;vgpr_word++){
   vgpr_data_pointer[vgpr_word] = inst_vect_data[vgpr];
}
vgpr_data_pointer[60] = 1024;
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
		else if(data == MEM_WR_LSU_WAIT || data == MEM_RD_LSU_WAIT )//last instruction is not finished yet
			continue;

		pc = XIo_In32(NEKO_PC);
		/*
		 * MEM_WR_ACK_WAIT 1
	     * MEM_WR_RDY_WAIT 2
	     * MEM_WR_LSU_WAIT 3
	     * MEM_RD_ACK_WAIT 4
	     * MEM_RD_RDY_WAIT 5
	     * MEM_RD_LSU_WAIT 6
		 * */

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
        xil_printf("Mem OP: %08x,%08x\n\r",address,data);
        nextValue = MEM_WR_LSU_WAIT;
    }

    XIo_Out32(NEKO_MEM_DONE, 0);
    XIo_Out32(NEKO_MEM_DONE, 1);

    do {
        data = XIo_In32(NEKO_MEM_OP);
    } while(data != 0 && data != nextValue && data != MEM_RD_ACK_WAIT && data != MEM_WR_ACK_WAIT);
    counter++;
}
}

        //NEKO's reset pulse
XIo_Out32(NEKO_RESET,0);
XIo_Out32(NEKO_RESET,1);
XIo_Out32(NEKO_RESET,0);

if(succeeded){
	      // Retrieve the data in the scalar registers (results)
    for(index = 0; index < num_scalar_data; index+=4){
        XIo_Out32(NEKO_SGRP_ADDR, index);
        inst_scalar_data[index]=XIo_In32(NEKO_SGRP_QUAD_0);
        inst_scalar_data[index+1]=XIo_In32(NEKO_SGRP_QUAD_1);
        inst_scalar_data[index+2]=XIo_In32(NEKO_SGRP_QUAD_2);
        inst_scalar_data[index+3]=XIo_In32(NEKO_SGRP_QUAD_3);
    }

	     // Retrieve the data in the vector registers (results)
    for(vgpr=0;vgpr<num_vect_data;vgpr++){
        XIo_Out32(VGPR_ADDR, vgpr);
        inst_vect_data[vgpr] = vgpr_data_pointer[0];
    }
}
return(succeeded);
}
int32_t test_mtbuf(){
  //Tests are done by issuing the following instructions and reading the results
    int32_t insts[6];
    insts[0] = create_sop1(3, 126, 2);
    insts[1] = create_sop1(3, 127, 2);
    insts[4] = END_PRGRM;
    int32_t inc_counter;
    int32_t data_counter;
    int32_t res;
    int32_t inst_vect_data[10];
    int32_t running_insts_counter = 0;
    int32_t dmft;
    for(inc_counter = 0; inc_counter<1; inc_counter++){
        int32_t inst_data[] = {0,0,0xfff0ff0f,0,4,5,0x019,9,8,9,10,11};

        for(data_counter = 0; data_counter<10; data_counter++)
         inst_vect_data[data_counter] = (int32_t)data_counter+3;
     switch(inc_counter){
        case '0':
        case '4':
        dmft = 4;
        break;
        case '1':
        case '5':
        dmft = 11;
        break;
        case '2':
        case '6':
        dmft = 13;
        break;
        case '3':
        case '7':
        dmft = 14;
        break;
        default:
        dmft = 4;
    }
    insts[2] = 0xeba01000;
    insts[3] = 0x80030201;
    res = run_vop_program_neko(insts,6, inst_data, 12,inst_vect_data, 10,2000000000);

    if(res){
        running_insts_counter++;
        xil_printf("MTBUF: OPCODE %d OK\n\r",inc_counter);
        xil_printf("v0_0 = 0x%x       ",inst_vect_data[0]);
        xil_printf("s0 = 0x%x\n\r",inst_data[0]);
    }
    else
        xil_printf("MTBUF: OPCODE %d did not finish running\n\r",inc_counter);
}
return(running_insts_counter);
}

int main(){
    int32_t vgpr, count;
    int32_t * vgpr_data_pointer = (int32_t*)VGPR_DATA;
    test_mtbuf();
    for(vgpr=0;vgpr<3;vgpr++){
     XIo_Out32(VGPR_ADDR, vgpr);
     for(count = 0; count<64;count++)
      xil_printf("%08x | ",vgpr_data_pointer[count]);
  xil_printf("\n\r");
}

return 0;
}