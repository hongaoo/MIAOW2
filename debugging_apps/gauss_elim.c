#include <stdio.h>
#include <math.h>
#include "platform.h"
#include "xio.h"
#include "xparameters.h"
#include "time.h"
#include "xtmrctr.h"


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
#define NEKO_MEM_WR_ADDR (NEKO_CMD_ADDR + 188) // Address for writing data to MIAOW
#define NEKO_MEM_WR_DATA (NEKO_CMD_ADDR + 192) // Address for writing data to MIAOW
#define NEKO_MEM_WR_EN (NEKO_CMD_ADDR + 196)
#define NEKO_MEM_ACK (NEKO_CMD_ADDR + 200)

#define NEKO_MEM_DONE (NEKO_CMD_ADDR + 204)

#define NEKO_PROC_CYCLE_COUNTER (NEKO_CMD_ADDR + 164)

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
//k-means distance kernel

uint32_t inst_mem_fan1[] = {
		//Load exec_lo with constant
		0xBEFE03FF,
		0x0000000F,
		//Load exec_hi with constant
		0xBEFF03FF,
		0x00000000,
		//end load
		0xC2000504,0xC2008518,0xC2020900,0xC2028904,0xC203090C,0xC2038910,0xBF8C007F,0x8380FF00,0x0000FFFF,0x9300000C,0x80000100,0x4A000000,
		0x800006FF,0xFFFFFFFF,

		0x80800700,0x7D080000,0xBE80246A,0x4A000007,
		0xBF88002f,//0xBF880027,//0xBF880021,
		0x4A0000FF,0x00000001,
		0xD2D60000,0x00000D00,0x4A000007,0xC0840368,
		0x34000082,0x4A020005,0xBF8C007F,
		0xEBA01000,0x80020101,0xBF8C007F,0xBF8C007F,0xBF8C007F,0xBF8C007F,0xBF8C007F,0xBF8C007F,
		0xBF8C007F,0xBF8C007F,0xBF8C007F,0xBF8C007F,0xBF8C007F,0xBF8C007F,
		0xBF8C007F,0xBF8C007F,0xBF8C007F,0xBF8C007F,0xBF8C007F,0xBF8C007F,
		0xBF8C007F,0xBF8C007F,0xBF8C007F,0xBF8C007F,0xBF8C007F,0xBF8C007F,

		0x93060607,0x80060607,0x8F06FF06,0x00000002,0x80050605,
		//added instructions to compute offset prior to the memory access
		0x80100805,
		0xBE9103FF,
		0x00000000,
		0xBE9203FF,
		0x00000000,
		0xBE9303FF,
		0x00000000,
		//0xBE910380,
		//0xBE920380,
		//0xBE930380,
		0xC0029100,
		//0xC2028805,
		//end of added instructions

		0x7E0402FF,0x6F800000,0xBF8C007F,0xD008016A,0x00020405,0x7E0402FF,0x2F800000,
		//added register with value '1.0' to replace malfunctioning inline constant in the next instruction
		0x7E0802FF,0x3F800000,
		0x00040504,//0x000404F2,
		0x10060405,	0x7E065503,0xC0840360,0xBF8C0F70,0x10020701,0x10020302,0x4A000004,0xBF8C007F,0xEBA41000,0x80020100,/**/
		0xBF810000
};

uint32_t inst_mem_fan2[] = {
		//Load exec_lo with constant
		0xBEFE03FF,
		0x0000000F,
		//Load exec_hi with constant
		0xBEFF03FF,
		0x00000000,
		0xC2400504,0xC2420518,0xC203090C,0xC2038910,0xBF8C007F,0x8380FF00,0x0000FFFF,0x8381FF01,0x0000FFFF,0x9300000C,0x9301010D,0x80000400,0x80010501,0x800406C1,0x4A000000,0x4A020201,0x80800706,0x80810704,0xD1080004,0x00020200,0xD1080000,0x00020001,0xC2060900,0xC2068904,0xC2040908,0x87EA0004,0xBE80246A,
		0xBF880037,//0xBF880033,//branch - added +4
		0xC0880368,0xC08A0360,0x93040607,0x4A040207,0x4A060404,0x34060682,0xBF8C007F,0x4A06060D,0x4A000007,0x4A080081,0xD2D60004,0x00000D04,0x4A0A0807,0x340A0A82,0x4A0A0A0C,0x4A040902,0x34040482,0x4A08040D,0x7D040280,0xEBA01000,0x80040103,0xEBA01000,0x80050305,0xEBA01000,0x80040504,0xBF8C0F70,
//		0xD2820005,0x24160701,//removed mad instruction and replaced it by
		// v6 = v1*(-1); v6 = v6*v3; v5 = v6 + v5; which is equal to -> v5 = ( ( v1*(-1) ) * v3 ) + v5
		0x100C02FF,0xbf800000,0x100C0D03,
		//0x8F068207,//0x060A0D05,//testing instructions
		0x060A0B06,
		//end of added instructions
		0xEBA41000,0x80040504,0xBE84246A,
		0xBF880015,//0xBF880013,//branch - added +2
		0x8F068207,0xC0880370,0x80020608,0x34000082,0x7E020202,0x4A04040C,0x4A000008,0xBF8C007F,0xEBA01000,0x80040101,0xEBA01000,0x80050202,0xEBA01004,0x80040300,0xBF8C0F70,
//		0xD2820003,0x240E0501,//removed mad instruction and replaced it by
		0x100C02FF,0xbf800000,0x100C0D02,0x06060706,
		//end of added instructions
		0xEBA41004,0x80040300,0xBF810000};
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

int32_t create_smrd(int32_t op, int32_t sdst, int32_t sbase, int32_t imm, int32_t offset){
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

int32_t run_vop_program_neko(int32_t insts[],int32_t num_insts, int32_t inst_scalar_data[], int32_t num_scalar_data,int32_t inst_vect_data[], int32_t num_vect_data,int32_t max_clocks){
	/*
	 * Execution Flow:
	 * Resets Neko
	 * Populates NEKO's instruction buffer, the scalar registers and the vector registers (all 64 words of a register are initialized with the same value)
	 * Send "start execution" command and waits for program completion or until the timeout is reached
	 * If the program reaches the end of execution before the timeout the data in the registers is read and success(1) is returned
	 * Otherwise returns 0 (unsuccessful)
	 */
	//uint32_t pc, old_pc=0;
	int32_t index, address,data;//,counter = 0;
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
	for(vgpr=0;vgpr<64;vgpr++){
		XIo_Out32(VGPR_ADDR, vgpr);
		XIo_Out32(VGPR_WR_CLEAN, 1);
		XIo_Out32(VGPR_WR_CMD, 1);
	}

	for(vgpr=0;vgpr<num_vect_data;vgpr++){
		XIo_Out32(VGPR_ADDR, vgpr);
		for(vgpr_word=0;vgpr_word<64;vgpr_word++){
			vgpr_data_pointer[vgpr_word] = inst_vect_data[vgpr*64+vgpr_word];
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


			XIo_Out32(NEKO_MEM_ACK, 0);
			XIo_Out32(NEKO_MEM_ACK, 1);

			do {
				data = XIo_In32(NEKO_MEM_OP);
			} while(data != nextValue);

			address = XIo_In32(NEKO_MEM_ADDR);
			if(nextValue == MEM_RD_RDY_WAIT)
			{
				xil_printf("addr %08x\n\r",address);
				data = XIo_In32(address);
				XIo_Out32(NEKO_MEM_WR_DATA, data);
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
			}while(data != 0 && data != nextValue && data != MEM_RD_ACK_WAIT && data != MEM_WR_ACK_WAIT);
		}
	}
	int32_t num_cycles = XIo_In32(NEKO_CYCLE_COUNTER);

	//NEKO's reset pulse
	XIo_Out32(NEKO_RESET,0);
	XIo_Out32(NEKO_RESET,1);
	XIo_Out32(NEKO_RESET,0);

	return(num_cycles);
}
union ufloat{
	float f;
	uint32_t u;
};
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
void
create_matrix(union ufloat *m, int size){
  int i,j;
  float lamda = -0.01;
  uint32_t coe[2*size-1];
  union ufloat coe_i;
  coe_i.f=0.0f;

  for (i=0; i < size; i++)
    {
      coe_i.f = 10*exp(lamda*i);
      j=size-1+i;
      coe[j]=coe_i.u;
      j=size-1-i;
      coe[j]=coe_i.u;

    }


  for (i=0; i < size; i++) {
      for (j=0; j < size; j++) {
	m[i*size+j].u=coe[size-1-i+j];
      }
  }


}

void InitPerRun(int size,union ufloat *m)
{
	int i;
	for (i=0; i<size*size; i++)
			m[i].f = 0.0f;
}
void BackSub(union ufloat *a,union ufloat *b, union ufloat *finalVec, int size)
{
	// solve "bottom up"
	int i,j;
	for(i=0;i<size;i++){
		finalVec[size-i-1].f=b[size-i-1].f;
		for(j=0;j<i;j++)
		{
			finalVec[size-i-1].f-=a[size*(size-i-1)+(size-j-1)].f * finalVec[size-j-1].f;
		}
		finalVec[size-i-1].f=finalVec[size-i-1].f/ (a[size*(size-i-1)+(size-i-1)].f);
	}
}


/*
a = (float *) malloc(size * size * sizeof(float));
	create_matrix(a, size);

	b = (float *) malloc(size * sizeof(float));
	for (int i =0; i< size; i++)
	  b[i]=1.0;

      m = (float *) malloc(size * size * sizeof(float));

    // create a new vector to hold the final answer

    finalVec = (float *) malloc(size * sizeof(float));

    InitPerRun(size,m);
ForwardSub(context,a,b,m,size,timing);

        BackSub(a,b,finalVec,size);


*/
//forward sub
//init cenas
//for t=0; t<size-1; t++{
//fan1
//fan2 }


void print_float(union ufloat num, int dec_cases){

	int32_t aux,counter;
	if(num.f<0){
		xil_printf("-");
		num.f*=-1.0f;
	}

	aux = (int)num.f;
	xil_printf("%d.",aux);
	num.f-=(int)num.f;

	for(counter = 0; counter < dec_cases; counter++){
		num.f*=10.0f;
		aux = (int)num.f;
		xil_printf("%d",aux);
		num.f-=(int)num.f;
	}
}
int32_t create_vopc(int32_t op_base,int32_t op_offset, int32_t vsrc1, int32_t src0){
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
int main()
{

	//xil_printf("0x%08x,\n\r",create_sop1(4, 0, 126));
	init_platform();

	/*xil_printf("0x%08x,\n\r",create_sop1(3, 17, 255));
		xil_printf("0x%08x,\n\r",create_sop1(3, 18, 255));
		xil_printf("0x%08x,\n\r",create_sop1(3, 19, 255));*/
	/*union ufloat num;
	num.f = 1.5836f;*/


	/*Added instructions for FAN2
	xil_printf("0x%08x,\n\r",create_vop2(8, 6, 1, 255));
	xil_printf("0xbf800000,\n\r");
	xil_printf("0x%08x,\n\r",create_vop2(8, 6, 6, 0x103));
	xil_printf("0x%08x,\n\r",create_vop2(3, 5, 6, 0x105));

	xil_printf("0x%08x,\n\r",create_vop2(8, 6, 1, 255));
	xil_printf("0xbf800000,\n\r");
	xil_printf("0x%08x,\n\r",create_vop2(8, 6, 6, 0x102));
	xil_printf("0x%08x,\n\r",create_vop2(3, 3, 3, 0x106));*/


	/*Added instructions for FAN1
	 * xil_printf("0x%08x,\n\r",create_sop2(0, 16, 8,5));
	xil_printf("0x%08x,\n\r",create_sop1(3, 17, 255));
	xil_printf("0x%08x,\n\r",create_sop1(3, 18, 255));
	xil_printf("0x%08x,\n\r",create_sop1(3, 19, 255));
create_smrd(int32_t op, int32_t sdst, int32_t sbase, int32_t imm, int32_t offset)
	xil_printf("0x%08x,\n\r",create_smrd(0, 5, 8, 1,0));//8*2->s16 */

	//xil_printf("0x%08x,\n\r",create_vop2(3, 5, 6, 0x105));
	static XTmrCtr XPS_Timer;
	unsigned int volatile timer = 0;
	XTmrCtr_Initialize(&XPS_Timer, XPAR_TMRCTR_0_DEVICE_ID);
	XTmrCtr_SetResetValue(&XPS_Timer, 0, 0x00000000);
	XTmrCtr_Reset(&XPS_Timer, 0);


	//int32_t data_bram;

	uint32_t num_regs = 64;

	//int *dram = (int *)XPAR_MIG_7SERIES_0_BASEADDR;
	int32_t inst_num = sizeof(inst_mem_fan1) / sizeof(int32_t), inst_num_fan2 = sizeof(inst_mem_fan2) / sizeof(int32_t);
	int32_t data[num_regs], vdata_numregs = 3*64, counter;
	int32_t vdata[vdata_numregs];


	int64_t  num_cycles = 0;
	int32_t i;
	//int32_t threshold = 0, max_it = 500, iteration=0;

	int32_t kernel_calls_num = 64/64, kernel_calls_counter = 0;

	kernel_calls_num = kernel_calls_num < 1 ? 1 : kernel_calls_num;

	uint32_t *exec_lo = &inst_mem_fan1[1], *exec_hi = &inst_mem_fan1[3];

	int32_t size = 32;
	//int32_t res_matrix[size*size], initial_matrix[size*size];
	union ufloat res_matrix[size*size], initial_matrix[size*size],b[size];
	create_matrix(initial_matrix, size);
	InitPerRun(size,res_matrix);


	for (i=0; i< size; i++)
		b[i].f =1.0f;

	*exec_lo = 0x0f;
	*exec_hi = 0x0;

	exec_lo = &inst_mem_fan2[1], exec_hi = &inst_mem_fan2[3];
	//*exec_lo = 0x0fff;
	//*exec_hi = 0x0;
	//inst_mem_fan2[];
    /*if(n_points == 32){
    	*exec_lo = 0xFFFFFFFF;
    	*exec_hi = 0x00000000;
    }
    else{
    	*exec_lo = 0xFFFFFFFF;
    	*exec_hi = 0xFFFFFFFF;
    }*/



    //for(counter=0; counter < 512*4; counter++)
    	//dram[512*4*feat_offset+counter] = features[counter];

	uint32_t mat_m = 0x00000260, mat_a = mat_m + (size*size*4), vec_b = mat_a + (size*size*4);
	uint32_t t = 0;
    uint32_t arg0=0,arg1=0,arg2=0,arg3=0,arg4=0,arg5=0,arg6=0,arg7=0;
    /*uint32_t features_address = 0x00000240,
    		 clusters_address=features_address+(n_points*n_features*4),
    		 membership_address = clusters_address+(n_clusters*n_features*4);*/
    //int32_t membership[n_points];

    //int32_t n_membership,new_centers_len[n_clusters];
    //union ufloat  new_centers[n_clusters * n_features];



    int32_t ram_init[] = {1,1,1,1,1,1,1,0,1,1,1,
        		0,0,0,0,0,0,0,0,0,0,
        		0,0,0,0,0,0,0,0,0,0,
        		0,0x00000084,0,0,0,0,0,0,0,0,
        		0,0,0,0,0,0,0,0,0,0,
        		0,0,0,0,0,0,0,0,0,0,
        		0,0,0,0,0,0,0,0,0,0,
        		0,0,0,0,0,0,0,0,0,0,
        		0,0,0,0,0,0,0,0,0,0,
        		0,0,0,0,0,0,0,0,0,0,
        		0,0,0,0x00000000,0,0,0,0,0,0,
        		0,0,0,0,0,0,0,0,0,0,
        		mat_m,0,0,0,mat_a,0,0,0,vec_b,0,
        		0,0,size,0,0,0,t,0,0,0,
        		arg5,0,0,0,arg6,0,0,0,arg7,0
        };//133
    /*union ufloat num;
    num.f = 1.0f*0.5f;
    xil_printf("%08x\n\r",num.u);*/
    //int32_t global_launch_counter;

    //for(global_launch_counter = 0; global_launch_counter<10; global_launch_counter++){
    XIo_Out32(NEKO_RESET,0);
    XIo_Out32(NEKO_RESET,1);

    XIo_Out32(NEKO_RESET,0);
	XIo_Out32(NEKO_MEM_ACK, 0);
	XIo_Out32(NEKO_MEM_DONE,0);

    int bram_counter;



    /*for(bram_counter=0; bram_counter<400; bram_counter+=4){
    	XIo_Out32(NEKO_MEM_WR_ADDR,bram_counter);
    	XIo_Out32(NEKO_MEM_WR_DATA,0);
    	XIo_Out32(NEKO_MEM_DONE,0);
    	XIo_Out32(NEKO_MEM_DONE,1);
    	XIo_Out32(NEKO_MEM_DONE,0);

    }*/
    //Initialization values for the bram
    for(bram_counter=0; bram_counter<=150; bram_counter++){
    	XIo_Out32(NEKO_MEM_WR_ADDR,bram_counter*4);
    	XIo_Out32(NEKO_MEM_WR_DATA,ram_init[bram_counter]);
    	XIo_Out32(NEKO_MEM_DONE,0);
    	XIo_Out32(NEKO_MEM_DONE,1);
    	XIo_Out32(NEKO_MEM_DONE,0);
    	//dram[bram_counter] = ram_init[bram_counter];
    }
    for(bram_counter=0; bram_counter<size*size; bram_counter++){
    	XIo_Out32(NEKO_MEM_WR_ADDR,mat_m+bram_counter*4);
    	XIo_Out32(NEKO_MEM_WR_DATA,res_matrix[bram_counter].u);
    	XIo_Out32(NEKO_MEM_DONE,0);
    	XIo_Out32(NEKO_MEM_DONE,1);
    	XIo_Out32(NEKO_MEM_DONE,0);

    }
    for(bram_counter=0; bram_counter<size*size; bram_counter++){
    	XIo_Out32(NEKO_MEM_WR_ADDR,mat_a+bram_counter*4);
    	XIo_Out32(NEKO_MEM_WR_DATA,initial_matrix[bram_counter].u);
    	XIo_Out32(NEKO_MEM_DONE,0);
    	XIo_Out32(NEKO_MEM_DONE,1);
    	XIo_Out32(NEKO_MEM_DONE,0);

    }
    for(bram_counter=0; bram_counter<size; bram_counter++){
    	XIo_Out32(NEKO_MEM_WR_ADDR,vec_b+bram_counter*4);
    	XIo_Out32(NEKO_MEM_WR_DATA,b[bram_counter].u);
    	XIo_Out32(NEKO_MEM_DONE,0);
    	XIo_Out32(NEKO_MEM_DONE,1);
    	XIo_Out32(NEKO_MEM_DONE,0);

    }

    union ufloat res;
    /*xil_printf("\n\rMat M\n\r");
    for(bram_counter=0; bram_counter<size*size; bram_counter++){
    	XIo_Out32(NEKO_MEM_WR_ADDR,mat_m+bram_counter*4);
    	res.u = XIo_In32(NEKO_MEM_RD_DATA);
    	print_float(res,5);//xil_printf("%08x   ",res.u);
    	    		xil_printf("   ");
    	if((bram_counter+1)%size==0) xil_printf("\n\r");
    }*/
    //xil_printf("\n\r");
    /*xil_printf("\n\rInitial Mat A\n\r");
    for(bram_counter=0; bram_counter<size*size; bram_counter++){
    	XIo_Out32(NEKO_MEM_WR_ADDR,mat_a+bram_counter*4);
    	res.u = XIo_In32(NEKO_MEM_RD_DATA);
    	print_float(res,5);//xil_printf("%08x   ",res.u);
    	xil_printf("   ");
    	if((bram_counter+1)%size==0) xil_printf("\n\r");
    }
    xil_printf("\n\r");
    xil_printf("\n\Initial Vec B\n\r");
    for(bram_counter=0; bram_counter<size; bram_counter++){
    	XIo_Out32(NEKO_MEM_WR_ADDR,vec_b+bram_counter*4);
    	res.u = XIo_In32(NEKO_MEM_RD_DATA);
    	//xil_printf("%08x   ",res.u);
    	print_float(res,5);
    	xil_printf("   ");
    	xil_printf("\n\r");
    }
    xil_printf("\n\r");*/


    	num_cycles = 0;

    	//for(kernel_calls_counter = 0; kernel_calls_counter < kernel_calls_num; kernel_calls_counter++){
    	int32_t column_counter = 0;
    	int32_t num_threads_fan1 = size, num_threads_fan2 = size*(size-1);

    	for(column_counter = 0; column_counter < size-1; column_counter ++){

    		kernel_calls_num = (size - column_counter-1)/64;
    		kernel_calls_num = kernel_calls_num == 0 ? 1 : kernel_calls_num;

    		//xil_printf("kernel_num %d\n\r",kernel_calls_num);
    		for(kernel_calls_counter = 0; kernel_calls_counter < kernel_calls_num; kernel_calls_counter++){
    			ram_init[137] = column_counter; //update t
    			XIo_Out32(NEKO_MEM_WR_ADDR,137*4);
    			XIo_Out32(NEKO_MEM_WR_DATA,ram_init[137]);
    			XIo_Out32(NEKO_MEM_DONE,0);
    			XIo_Out32(NEKO_MEM_DONE,1);
    			XIo_Out32(NEKO_MEM_DONE,0);

    			if((size - column_counter-1-64*kernel_calls_counter)>=64)
    				num_threads_fan1 = 64;//%65;
    			else
    				num_threads_fan1 = (size - column_counter-1-64*kernel_calls_counter);

    			exec_lo = &inst_mem_fan1[1], exec_hi = &inst_mem_fan1[3];

    			*exec_lo = 0x01;
    			*exec_hi = 0x0;

    			for(counter=1; counter < num_threads_fan1; counter++){
    				if(counter<32){
    					*exec_lo = *exec_lo << 1;
    					*exec_lo = *exec_lo | 0x01;
    				}
    				else{
    					if(counter==32)
    						*exec_hi = 0x01;
    					else{
    						*exec_hi = *exec_hi << 1;
    						*exec_hi = *exec_hi | 0x01;
    					}
    				}
    			}

    			//xil_printf("%08x\n\r",*exec_lo);xil_printf("%08x\n\r",*exec_hi);


    			for(counter = 0; counter<vdata_numregs; counter++){
    				vdata[counter] = 0;
    			}
    			for(counter = 0; counter<64; counter++){
    				data[counter] = 0;
    			}
    			for(counter = 0; counter<64; counter++){
    				vdata[counter] = counter+kernel_calls_counter*64;
    				//vdata[counter+64] = 0;
    				//vdata[counter+128] = 0;
    			}

    			data[4] = 0x00000000;
    			data[2] = 0x00000000;
    			data[8] = 0x000001E4;

    			data[12] = 0;
    			data[13] = 0;

    			num_cycles += run_vop_program_neko(inst_mem_fan1,inst_num, data, num_regs,vdata,vdata_numregs,2147483647);

    		}

    		kernel_calls_num = ((size-column_counter)*(size-column_counter-1))/64;

    		if(((size-column_counter)*(size-column_counter-1))%64)
    			kernel_calls_num++;

    		//kernel_calls_num = kernel_calls_num == 0 ? 1 : kernel_calls_num;




    		//xil_printf("kernel_num %d\n\r",kernel_calls_num);
    		int32_t line_counter =-1, init_offset = 0;

    		for(kernel_calls_counter = 0; kernel_calls_counter < kernel_calls_num; kernel_calls_counter++){

    			exec_lo = &inst_mem_fan2[1], exec_hi = &inst_mem_fan2[3];
    			*exec_lo = 0x01;
    			*exec_hi = 0x0;

    			if(((size-column_counter)*(size-column_counter-1) -64*kernel_calls_counter)>=64)//%65;
    				num_threads_fan2 = 64;
    			else
    				num_threads_fan2 = ((size-column_counter)*(size-column_counter-1) -64*kernel_calls_counter);
    		//	xil_printf("num_threads_fan2 %d\n\r",num_threads_fan2);


    			//xil_printf("num_threads_fan2: %d\n\r",num_threads_fan2);
    			for(counter=1; counter < num_threads_fan2; counter++){
    				if(counter<32){
    					*exec_lo = *exec_lo << 1;
    					*exec_lo = *exec_lo | 0x01;
    				}
    				else{
    					if(counter==32)
    						*exec_hi = 0x01;
    					else{
    						*exec_hi = *exec_hi << 1;
    						*exec_hi = *exec_hi | 0x01;
    					}
    				}
    			}

    			//xil_printf("%08x\n\r",*exec_lo);xil_printf("%08x\n\r",*exec_hi);

    			for(counter = 0; counter<vdata_numregs; counter++){
    				vdata[counter] = 0;
    			}
    			for(counter = 0; counter<64; counter++){
    				data[counter] = 0;
    			}


    			//for(line_counter=0; line_counter < (size-column_counter); line_counter++){
    				//for(counter = 0; counter < (size-column_counter-1); counter ++){
    					//vdata[line_counter*(size-column_counter-1)+counter+64] = line_counter;
    					//vdata[line_counter*(size-column_counter-1)+counter] = counter;
    					//xil_printf("(%d,%d),",vdata[line_counter*(size-column_counter-1)+counter],
    						//	vdata[line_counter*(size-column_counter-1)+counter+64]);
    				//}
    			//}
    			//xil_printf("\n\r");
    			//line_counter =-1, init_offset = 0;



    			for(counter = 0; counter < (size-column_counter-1)*(size-column_counter) && counter<num_threads_fan2; counter ++){
    				if((counter+init_offset)%(size-column_counter-1) == 0)
    					++line_counter;

    				vdata[counter] = (counter+init_offset)%(size-column_counter-1);
    				vdata[counter+64] = line_counter;
    				//if((counter+init_offset)%(size-column_counter-1)==0)
    					//xil_printf("\n\r");
    				//xil_printf("(%d,%d),",vdata[counter],vdata[counter+64]);
    			}

    			init_offset = (counter+init_offset)%(size-column_counter-1);



    			data[4] = 0x00000000;
    			data[2] = 0x00000000;
    			data[8] = 0x000001E4;

    			data[12] = 0;
    			data[13] = 0;

    			num_cycles += run_vop_program_neko(inst_mem_fan2,inst_num_fan2, data, num_regs,vdata,vdata_numregs,2147483647);
    		}//*/
    	}
    	xil_printf("Total number of cycles: %ld\n\r", num_cycles);
    	//retrieve A
    	//retrieve b
    	//BackSub(union ufloat *a,union ufloat *b, union ufloat *finalVec,size)
    	/*int32_t sgpr;
    	for(sgpr=0;sgpr<=20;sgpr+=4){
    		XIo_Out32(NEKO_SGRP_ADDR,sgpr);
    		xil_printf("s%d %08x\n\r",sgpr,XIo_In32(NEKO_SGRP_QUAD_0));
    		xil_printf("s%d %08x\n\r",sgpr+1,XIo_In32(NEKO_SGRP_QUAD_1));
    		xil_printf("s%d %08x\n\r",sgpr+2,XIo_In32(NEKO_SGRP_QUAD_2));
    		xil_printf("s%d %08x\n\r",sgpr+3,XIo_In32(NEKO_SGRP_QUAD_3));
    	}
    	int32_t * vgpr_data_pointer = (int32_t*)VGPR_DATA;
    	XIo_Out32(VGPR_ADDR, 0);
    	    	xil_printf("v0 %08x\n\r",vgpr_data_pointer[0]);
    	    	xil_printf("v0 %08x\n\r",vgpr_data_pointer[1]);
    	    	xil_printf("v0 %08x\n\r",vgpr_data_pointer[2]);
    	    	xil_printf("v0 %08x\n\r",vgpr_data_pointer[3]);
    	    	xil_printf("v0 %08x\n\r",vgpr_data_pointer[4]);
    	XIo_Out32(VGPR_ADDR, 1);
    	xil_printf("v1 %08x\n\r",vgpr_data_pointer[0]);
    	xil_printf("v1 %08x\n\r",vgpr_data_pointer[1]);
    	xil_printf("v1 %08x\n\r",vgpr_data_pointer[2]);
    	xil_printf("v1 %08x\n\r",vgpr_data_pointer[3]);
    	xil_printf("v1 %08x\n\r",vgpr_data_pointer[4]);
    	XIo_Out32(VGPR_ADDR, 3);
    	xil_printf("v3 %08x\n\r",vgpr_data_pointer[0]);
    	XIo_Out32(VGPR_ADDR, 4);
    	xil_printf("v4 %08x\n\r",vgpr_data_pointer[0]);
    	XIo_Out32(VGPR_ADDR, 5);
    	xil_printf("v5 %08x\n\r",vgpr_data_pointer[0]);
    	XIo_Out32(VGPR_ADDR, 6);
    	xil_printf("v6 %08x\n\r",vgpr_data_pointer[0]);

    	//xil_printf("Number of cycles to completion: %d\n\r", num_cycles);*/

    	/*xil_printf("\n\rMat M\n\r");
    	//union ufloat res;
    	for(bram_counter=0; bram_counter<size*size; bram_counter++){
    		XIo_Out32(NEKO_MEM_WR_ADDR,mat_m+bram_counter*4);
    		res.u = XIo_In32(NEKO_MEM_RD_DATA);
    		print_float(res,5);
//    		xil_printf("%08x   ",res.u);
    		xil_printf("   ");
    		if((bram_counter+1)%size==0) xil_printf("\n\r");
    	}
    	//xil_printf("\n\r");
    	xil_printf("\n\rFinal Mat A\n\r");
    	for(bram_counter=0; bram_counter<size*size; bram_counter++){
    		XIo_Out32(NEKO_MEM_WR_ADDR,mat_a+bram_counter*4);
    		res.u = XIo_In32(NEKO_MEM_RD_DATA);
    		print_float(res,5);//xil_printf("%08x   ",res.u);
    		xil_printf("   ");
    		if((bram_counter+1)%size==0) xil_printf("\n\r");
    	}
    	xil_printf("\n\r");
    	xil_printf("\n\rFinal Vec B\n\r");
    	for(bram_counter=0; bram_counter<size; bram_counter++){
    		XIo_Out32(NEKO_MEM_WR_ADDR,vec_b+bram_counter*4);
    		res.u = XIo_In32(NEKO_MEM_RD_DATA);
    		//xil_printf("%08x   ",res.u);
    		print_float(res,5);
    		xil_printf("   ");
    		xil_printf("\n\r");
    	}*/
    	xil_printf("\n\r");
    	//membership_OCL -> bram
    	//membership -> local memory of the microblaze

    	for(bram_counter=0; bram_counter<size*size; bram_counter++){
    		XIo_Out32(NEKO_MEM_WR_ADDR,mat_a+bram_counter*4);
    		initial_matrix[bram_counter].u = XIo_In32(NEKO_MEM_RD_DATA);
    	}
    	for(bram_counter=0; bram_counter<size; bram_counter++){
    		XIo_Out32(NEKO_MEM_WR_ADDR,vec_b+bram_counter*4);
    		b[bram_counter].u = XIo_In32(NEKO_MEM_RD_DATA);
    	}
    	union ufloat final_result[size];

    	XTmrCtr_Reset(&XPS_Timer, 0);
    	XTmrCtr_Start(&XPS_Timer, 0);
    	timer = XTmrCtr_GetValue(&XPS_Timer, 0);

    	BackSub(initial_matrix,b, final_result ,size);

    	XTmrCtr_Stop(&XPS_Timer, 0);
    	timer = XTmrCtr_GetValue(&XPS_Timer, 0)-timer;

    	xil_printf("Back sub time: %d\n\r",timer);

    	xil_printf("\n\r\n\rFinal result\n\r");
    	for(bram_counter=0; bram_counter<size; bram_counter++){
    		print_float(final_result[bram_counter],5);
    		xil_printf("   ");
    		xil_printf("\n\r");
    	}







    /*xil_printf("Took %d iterations\n\rResults:\n\r",iteration);
    for(bram_counter=0; bram_counter<n_points; bram_counter++){
        	xil_printf("%d,",membership[bram_counter]);
        }*/
    	/*int32_t sgpr;
    for(sgpr=0;sgpr<=20;sgpr+=4){
    	XIo_Out32(NEKO_SGRP_ADDR,sgpr);
    	xil_printf("s%d %08x\n\r",sgpr,XIo_In32(NEKO_SGRP_QUAD_0));
    	xil_printf("s%d %08x\n\r",sgpr+1,XIo_In32(NEKO_SGRP_QUAD_1));
    	xil_printf("s%d %08x\n\r",sgpr+2,XIo_In32(NEKO_SGRP_QUAD_2));
    	xil_printf("s%d %08x\n\r",sgpr+3,XIo_In32(NEKO_SGRP_QUAD_3));
    }//*/
    //uint32_t clusters[]
    /*for(bram_counter=0; bram_counter<512*256; bram_counter++){
    	XIo_Out32(NEKO_MEM_WR_ADDR,0x021E8+4*bram_counter);
    	XIo_Out32(NEKO_MEM_WR_DATA,0);//(512*(bram_counter/256)+(bram_counter+1)%512)%1024);
    	XIo_Out32(NEKO_MEM_DONE,0);
    	XIo_Out32(NEKO_MEM_DONE,1);
    	XIo_Out32(NEKO_MEM_DONE,0);
    }

    for(bram_counter=0; bram_counter<512*4; bram_counter++){
    	XIo_Out32(NEKO_MEM_WR_ADDR,0x001E8+4*bram_counter);
    	XIo_Out32(NEKO_MEM_WR_DATA,0);
    	XIo_Out32(NEKO_MEM_DONE,0);
    	XIo_Out32(NEKO_MEM_DONE,1);
    	XIo_Out32(NEKO_MEM_DONE,0);
    }*/


    //v0

    //4x4 -> v0 = 0, v1 = 1
    //8x8 -> v0 = 0,0,1,1 v1 = 0,1,0,1
    //16x16 -> v0 = 0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3 v1 = 0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3
    //vdata[4] = 1;
    /*vdata[2] = 1;
    vdata[3] = 1;

    vdata[64+1] = 1;
    vdata[64+3] = 1;*/



    /*for(counter = 0; counter<1048576; counter++){
    	dram[counter] = 0;
    	dram[counter+1048576] = 0;
    	dram[counter+2097152] = 0;
    }*/
    //Thread info








    //global size
   /* dram[0] = 1;
    dram[1] = 1;
    dram[2] = 1;

    //work dim
    dram[3] = 1;

    //local size
    dram[4] = 1;
    dram[5] = 1;
    dram[6] = 1;

    //num groups
    dram[8] = 1;
    dram[9] = 1;
    dram[10] = 1;

    //group id
    dram[28] = 0;
    dram[29] = 0;
    dram[30] = 0;

    //global id
    dram[24] = 0;
    dram[25] = 0;
    dram[26] = 0;

    dram[32] = 0x80000084;
    dram[136] = 0x0;
    dram[137] = 0x0;
    dram[138] = 0x0;
    dram[139] = 0x0;
    dram[128] = 0x0;
    dram[129] = 0x0;
    dram[130] = 0x0;
    dram[131] = 0x0;*/

    /*for(bram_counter=128; bram_counter<192; bram_counter++){
    	XIo_Out32(NEKO_MEM_WR_ADDR,4*bram_counter);
    	    	XIo_Out32(NEKO_MEM_WR_DATA,0);
    	    	XIo_Out32(NEKO_MEM_DONE,0);
    	    	XIo_Out32(NEKO_MEM_DONE,1);
    	    	XIo_Out32(NEKO_MEM_DONE,0);
    }*/

    /*int32_t input_args[]={0x000001A4,0x000021A4,0x000021A4,mat_dim1,mat_dim2};
    for(bram_counter=0; bram_counter<5; bram_counter++){
    	XIo_Out32(NEKO_MEM_WR_ADDR,0x00002000+16*bram_counter);
    	XIo_Out32(NEKO_MEM_WR_DATA,input_args[bram_counter]);
    	XIo_Out32(NEKO_MEM_DONE,0);
    	XIo_Out32(NEKO_MEM_DONE,1);
    	XIo_Out32(NEKO_MEM_DONE,0);
    }*/


    //Input arguments
    /*dram[2048] = 0x00040000;//allows a matrix of 1024*1024 to be stored between arguments
    dram[2052] = 0x00080000;
    dram[2056] = 0x000c0000;
    dram[2060] = mat_dim;
    dram[2064] = mat_dim;*/

    //dram=0x80040000;

    /*int32_t run_line=0, run_column=0;

    union ufloat num;

    //for(run_column=0; run_column<2; run_column++){




    		for(counter = 0; counter < 256; counter++){//4*256
    			XIo_Out32(NEKO_MEM_WR_ADDR,0x01E8+4*counter);
    			dram[counter+run_line*2048+run_column*256] = XIo_In32(NEKO_MEM_RD_DATA);//4 lines of 512 -> 2048

    			XIo_Out32(NEKO_MEM_WR_ADDR,0x01E8+1024+4*counter);
    			dram[counter+512+run_line*2048+run_column*256] = XIo_In32(NEKO_MEM_RD_DATA);//4 lines of 512 -> 2048

    			XIo_Out32(NEKO_MEM_WR_ADDR,0x01E8+2048+4*counter);
    			dram[counter+1024+run_line*2048+run_column*256] = XIo_In32(NEKO_MEM_RD_DATA);//4 lines of 512 -> 2048

    			XIo_Out32(NEKO_MEM_WR_ADDR,0x01E8+3072+4*counter);
    			dram[counter+1536+run_line*2048+run_column*256] = XIo_In32(NEKO_MEM_RD_DATA);//4 lines of 512 -> 2048
    		}*/
    	//}
    //}

    /*for(counter=0; counter<16; counter++){
    	//xil_printf("%d - %d - %d - %d \n\r", dram[counter*256], dram[counter*256+1], dram[counter*256+2], dram[counter*256+3]);
    	xil_printf("%08x - %08x - %08x - %08x \n\r", dram[counter*256], dram[counter*256+1], dram[counter*256+2], dram[counter*256+3]);
    }*/
    //put results in ddr3

    //1024*1024 = 1048576
    //int32_t value_ofB = 0;
    /*for(counter = 0; counter<num_elems; counter++){
    	//value.f = 0.5f*((counter + 1)%1024);
    	//xil_printf("%08x\n\r",value.u);
    	dram[counter] =((counter + 1)%1024);// value.u;
    	dram[counter+1048576] = ((counter + 1)%1024);//value.u;
    }*/
//dram[5] = 2;



    /*uint32_t v0_offset = 0, first_time = 1, init_offset = 0;
    uint64_t  total_counter=0;
    counter = 0;
    for(kernel_launch_counter = 0; kernel_launch_counter < kernel_launches; kernel_launch_counter++){
    	if(counter==64)
    		init_offset += 64;
    	else
    		init_offset = 0;
    	for(counter = 0; counter<64 && counter<(num_submat*num_submat); counter++){
    		if(!first_time && (counter+init_offset)%num_submat==0)
    			v0_offset++;
    		first_time = 0;
    		vdata[counter] = v0_offset;//(counter+mat_dim*kernel_launch_counter)/num_submat;
    		vdata[counter+64] = (counter+init_offset)%num_submat;
    		vdata[counter+128] = 0;
    		//xil_printf("(%d,%d)",vdata[counter],vdata[counter+64]);
    	}
    	//xil_printf("\n\r");
    	if(init_offset == 192)
    		counter = 0;*/
    	//total_counter+=
    //}

    //xil_printf("%ld\n", total_counter);
    /*if(global_launch_counter==9){
    	dram=0x80c00000;
    	for(counter = 0; counter<(mat_dim*mat_dim); counter++){
    		/*if(counter%((mat_dim*mat_dim)/16) == 0){
    			xil_printf("\n\r\n\r\n\r\n\r\n\r\n\r\n\r");
    		}
    		xil_printf("%d",dram[counter]);
    		if((counter+1)%mat_dim == 0)
    			xil_printf("\n\r");
    		else
    			xil_printf(",");
    	}
    }*/
//}
    /*xil_printf("s0 %08x\n\r",XIo_In32(NEKO_SGRP_QUAD_0));
    xil_printf("s1 %08x\n\r",XIo_In32(NEKO_SGRP_QUAD_1));
    XIo_Out32(NEKO_SGRP_ADDR,16);

    XIo_Out32(NEKO_SGRP_ADDR,20);
    xil_printf("s20 %08x\n\r",XIo_In32(NEKO_SGRP_QUAD_0));
    xil_printf("s21 %08x\n\r",XIo_In32(NEKO_SGRP_QUAD_1));
    xil_printf("s22 %08x\n\r",XIo_In32(NEKO_SGRP_QUAD_2));
    xil_printf("s23 %08x\n\r",XIo_In32(NEKO_SGRP_QUAD_3));
	XIo_Out32(VGPR_ADDR, 1);
	XIo_Out32(VGPR_ADDR, 2);
	XIo_Out32(VGPR_ADDR, 3);
	XIo_Out32(VGPR_ADDR, 4);

    for(counter = 22; counter<=26; counter++)
    	XIo_Out32(VGPR_ADDR, counter);
*/
   // }
    //int32_t vgpr;
    //
    /*for(vgpr=13;vgpr<=24;vgpr++){
    	XIo_Out32(VGPR_ADDR, vgpr);
    	xil_printf("%08x\n\r",vgpr_data_pointer[0]);
    }*/
    /*for(vgpr=22;vgpr<=25;vgpr++){
    	XIo_Out32(VGPR_ADDR, vgpr);
    	xil_printf("%08x\n\r",vgpr_data_pointer[0]);
    }*/
    /*for(vgpr=59;vgpr<=62;vgpr++){
    	XIo_Out32(VGPR_ADDR, vgpr);
    	xil_printf("%08x\n\r",vgpr_data_pointer[0]);
    }*/

    /*int32_t vgpr,sgpr;
    for(sgpr=0;sgpr<=20;sgpr+=4){
    XIo_Out32(NEKO_SGRP_ADDR,sgpr);
        xil_printf("s%d %08x\n\r",sgpr,XIo_In32(NEKO_SGRP_QUAD_0));
        xil_printf("s%d %08x\n\r",sgpr+1,XIo_In32(NEKO_SGRP_QUAD_1));
        xil_printf("s%d %08x\n\r",sgpr+2,XIo_In32(NEKO_SGRP_QUAD_2));
        xil_printf("s%d %08x\n\r",sgpr+3,XIo_In32(NEKO_SGRP_QUAD_3));
    }

    int32_t * vgpr_data_pointer = (int32_t*)VGPR_DATA;
    for(vgpr=10;vgpr<=17;vgpr++){
        	XIo_Out32(VGPR_ADDR, vgpr);
        	xil_printf("%d\n\r",vgpr_data_pointer[0]);
        }
        for(vgpr=22;vgpr<=25;vgpr++){
        	XIo_Out32(VGPR_ADDR, vgpr);
        	xil_printf("%d\n\r",vgpr_data_pointer[0]);
        }
        for(vgpr=75;vgpr<=78;vgpr++){
        	XIo_Out32(VGPR_ADDR, vgpr);
        	xil_printf("%08x\n\r",vgpr_data_pointer[0]);
        }*/

    /*for(bram_counter=0; bram_counter<4; bram_counter++){
            	XIo_Out32(NEKO_MEM_WR_ADDR,0x1E8+4*bram_counter);
            	data_bram = XIo_In32(NEKO_MEM_RD_DATA);
            	xil_printf("%d - ",data_bram);

            }
    xil_printf("\n\r");
    for(bram_counter=256; bram_counter<256+16; bram_counter++){
                    	XIo_Out32(NEKO_MEM_WR_ADDR,0x1E8+4*bram_counter);
                    	data_bram = XIo_In32(NEKO_MEM_RD_DATA);
                    	xil_printf("%d - ",data_bram);

                    }
    xil_printf("\n\r");
    for(bram_counter=512; bram_counter<512+16; bram_counter++){
                	XIo_Out32(NEKO_MEM_WR_ADDR,0x1E8+4*bram_counter);
                	data_bram = XIo_In32(NEKO_MEM_RD_DATA);
                	xil_printf("%d - ",data_bram);

                }
    xil_printf("\n\r");
    xil_printf("\n\r");


    for(bram_counter=768; bram_counter<768+4; bram_counter++){
                    	XIo_Out32(NEKO_MEM_WR_ADDR,0x1E8+4*bram_counter);
                    	data_bram = XIo_In32(NEKO_MEM_RD_DATA);
                    	xil_printf("%d - ",data_bram);

                    }*/
    cleanup_platform();
    return 0;
}
