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

uint32_t inst_mem[] = {
		//Load exec_lo with constant
		0xBEFE03FF, 0x0000000F,
		//Load exec_hi with constant
		0xBEFF03FF, 0x00000000,
		//end load
		0xC2000504,
		0xC2008518,
		0xC242090C,
		0xC2430910,
		0xC2068900,
		0xC2070904,
		0xC2040908,
		0xBF8C007F,
		0x8380FF00,
		0x0000FFFF,
		0x9300000C,
		0x80000100,
		0x4A000000,
		0x7E020C04,
		0x7E025501,
		0x100202FF,
		0x4F800000,
		0x7E020F01,
		0xD2D20002,
		0x00020204,
		0xD2D40003,
		0x00020204,
		0x4C080480,
		//edit
		0x7d0a0680,
		0xbe80046a,
		0x00040504,
		//0xD10A0000,
		//0x00020680,
		//0xD2000002,
		//0x00020504,
		0xD2D40002,
		0x00020302,
		0x4C060501,
		0x4A020501,
		//edit
		0xbe80046a,
		0x00020701,
		//0xD2000001,
		//0x00020701,
		0xD2D40001,
		0x00020101,
		0xD2D20002,
		0x00000901,
		0x4C060500,
		0xD18C0000,
		0x00020500,
		0x4A040281,
		0x4A0802C1,
		0x7D860604,
		0x87EA6A00,
		0x20501,
		//edit
		0xbe80046a,
		0x00020701,

		//0xD2000001,
		//0x00020304,
		0xD10A006A,
		0x00000880,
		0x000202C1,
		0xD2D60002,
		0x00000901,
		0x4C040500,
		0x800006C1,
		0x90008100,
		0x4E060400,
		0x7D860400,
		0x001A0680,
		0x4A080400,
		0x7D880804,
		0x800104C1,
		0x7E0A0201,
		0x00080905,
		0x7D86090D,
		0xBE8A246A,
		0xBF88004B,
		0x800107C1,
		0x90018101,
		0x280A0201,
		0x4C0C0305,
		0x4E0A0A01,
		0xD2D60006,
		0x00020C06,
		0x280E0400,
		0xD2D60005,
		0x00020A04,
		0x4A0C0F06,
		0x4A0A0B07,
		0x4A0E0201,
		0x800705C1,
		0x4C040506,
		0x4E0A0A00,
		0xD1880010,
		0x00020E05,
		0x7E0C0207,
		0x4E100201,
		0xD1860000,
		0x00020201,
		0x34020482,
		0x34040A82,
		0xC08A0368,
		0xC08C0370,
		//edit
		0xbeea0410,
		0x000a0f06,
		0xbeea0400,
		0x000c1080,
		//0xD2000005,
		//0x00420F06,
		//0xD2000006,
		//0x00021080,
		0x4A020208,
		0x4A06040E,
		0x8F008206,
		0x8F018204,
		0xBE84047E,
		0xBE86047E,
		0x7E040280,
		0x7D880B06,
		0xBE88246A,
		0x8AFE7E08,
		0xBF880016,
		0xBE8E047E,
		0x7E0E0306,
		0x7E100301,
		0x7E140303,
		0x4A161000,
		0x4A181401,
		0xBF8C007F,
		0xEBA01000,
		0x80050A0A,
		0xEBA01000,
		0x80060808,
		0xBF8C0F70,
		0xD2D60008,
		0x00021508,
		0x4A041102,
		0x4A0E0E81,
		0x7D880B07,
		0x8AFE6A7E,
		0xBF880003,
		0x7E10030B,
		0x7E14030C,
		0xBF82FFEE,
		0xBEFE0408,
		0x4A1A1A81,
		0xD188000E,
		0x0002090D,
		0x4A020284,
		0x4A060684,
		0x87FE0E08,
		0x8A867E06,
		0xBF840002,
		0x87FE0608,
		0xBF82FFDB,
		0xBEFE0404,
		0x8AFE7E0A,
		0x7E040280,
		0xBEFE040A,
		0xC0800360,
		0x34000082,
		0x4A00000D,
		0xBF8C007F,
		0xEBA41000,
		0x80000200,
		0xBF810000};

int32_t create_sop1(int32_t op, int32_t sdst, int32_t s0) {
	/* SOP1 instruction format:
	 * MSB -> LSB
	 * | ENC(9) = 9'b101111101 | SDST(7) | OP(8) | SSRC0(8) |
	 */
	int32_t inst = 0xBE800000;
	op = op << 8;
	sdst = sdst << 16;
	inst = inst | sdst | op | s0;
	return (inst);
}

int32_t create_smrd(int32_t op, int32_t sdst, int32_t sbase, int32_t imm,
		int32_t offset) {
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
	return (inst);
}

int32_t run_vop_program_neko(uint32_t insts[], int32_t num_insts,
		int32_t inst_scalar_data[], int32_t num_scalar_data,
		int32_t inst_vect_data[], int32_t num_vect_data, int32_t max_clocks) {
	/*
	 * Execution Flow:
	 * Resets Neko
	 * Populates NEKO's instruction buffer, the scalar registers and the vector registers (all 64 words of a register are initialized with the same value)
	 * Send "start execution" command and waits for program completion or until the timeout is reached
	 * If the program reaches the end of execution before the timeout the data in the registers is read and success(1) is returned
	 * Otherwise returns 0 (unsuccessful)
	 */
	//int count=0;
	//uint32_t pc, old_pc=0;
	int32_t index, address, data;		//,counter = 0;
	int32_t vgpr, vgpr_word;
	int32_t * vgpr_data_pointer = (int32_t*) VGPR_DATA;

	//NEKO's reset pulse
	XIo_Out32(NEKO_RESET, 0);
	XIo_Out32(NEKO_RESET, 1);
	XIo_Out32(NEKO_RESET, 0);

	XIo_Out32(NEKO_BASE_LDS, XPAR_MIG_7SERIES_0_BASEADDR);

	//Load scalar registers with data
	for (index = 0; index < num_scalar_data; index += 4) {
		XIo_Out32(NEKO_SGRP_ADDR, index);
		XIo_Out32(NEKO_SGRP_QUAD_0, inst_scalar_data[index]);
		XIo_Out32(NEKO_SGRP_QUAD_1, inst_scalar_data[index + 1]);
		XIo_Out32(NEKO_SGRP_QUAD_2, inst_scalar_data[index + 2]);
		XIo_Out32(NEKO_SGRP_QUAD_3, inst_scalar_data[index + 3]);
		XIo_Out32(NEKO_GPR_CMD, 1);
	}
	//Load vector registers with data (replicating the data for every word of the register)
	for (vgpr = 0; vgpr < 64; vgpr++) {
		XIo_Out32(VGPR_ADDR, vgpr);
		XIo_Out32(VGPR_WR_CLEAN, 1);
		XIo_Out32(VGPR_WR_CMD, 1);
	}

	for (vgpr = 0; vgpr < num_vect_data; vgpr++) {
		XIo_Out32(VGPR_ADDR, vgpr);
		for (vgpr_word = 0; vgpr_word < 64; vgpr_word++) {
			vgpr_data_pointer[vgpr_word] =
					inst_vect_data[vgpr * 64 + vgpr_word];
		}
		XIo_Out32(VGPR_WR_CMD, 1);

	}

	//Load the instruction buffer
	for (index = 0; index < num_insts; index++) {
		XIo_Out32(NEKO_INSTR_ADDR, index);
		XIo_Out32(NEKO_INSTR_VALUE, insts[index]);
	}

	//Start execution
	XIo_Out32(NEKO_CMD_ADDR, 1);

	//Wait for the end of execution
	while (XIo_In32(NEKO_CMD_ADDR) != 1) {
		//Verify the timeout

		data = XIo_In32(NEKO_MEM_OP);
		if (data != 0) {

			int nextValue = MEM_RD_RDY_WAIT;

			if (data == MEM_RD_ACK_WAIT) {
				nextValue = MEM_RD_RDY_WAIT;
			} else if (data == MEM_WR_ACK_WAIT) {
				nextValue = MEM_WR_RDY_WAIT;

			} else if (data == MEM_WR_LSU_WAIT || data == MEM_RD_LSU_WAIT)//last instruction is not finished yet
				continue;

			XIo_Out32(NEKO_MEM_ACK, 0);
			XIo_Out32(NEKO_MEM_ACK, 1);
			XIo_Out32(NEKO_MEM_ACK, 0);

			do {
				data = XIo_In32(NEKO_MEM_OP);
			} while (data != nextValue);

			address = XIo_In32(NEKO_MEM_ADDR);
			if (nextValue == MEM_RD_RDY_WAIT) {
				//xil_printf("addr %08x\n\r", address);
				//count++;
				data = XIo_In32(address);
				XIo_Out32(NEKO_MEM_WR_DATA, data);
				nextValue = MEM_RD_LSU_WAIT;
			} else {
				data = XIo_In32(NEKO_MEM_RD_DATA);
				XIo_Out32(address, data);
				nextValue = MEM_WR_LSU_WAIT;
			}

			XIo_Out32(NEKO_MEM_DONE, 0);
			XIo_Out32(NEKO_MEM_DONE, 1);
			XIo_Out32(NEKO_MEM_DONE, 0);
			do {
				data = XIo_In32(NEKO_MEM_OP);
			} while (data != 0 && data != nextValue && data != MEM_RD_ACK_WAIT
					&& data != MEM_WR_ACK_WAIT);
		}
	}
	int32_t num_cycles = XIo_In32(NEKO_CYCLE_COUNTER);
//xil_printf("count: %d\n\r",count);
	//NEKO's reset pulse
	XIo_Out32(NEKO_RESET, 0);
	XIo_Out32(NEKO_RESET, 1);
	XIo_Out32(NEKO_RESET, 0);

	return (num_cycles);
}
union ufloat {
	float f;
	uint32_t u;
};
int32_t create_vop2(int32_t op, int32_t vdst, int32_t vsrc1, int32_t src0) {
	/* VOP2 instruction format:
	 * MSB -> LSB
	 * | ENC(1) = 1'b0 | OP(6) | VDST(8) | VSRC1(8) | SRC0(9) |
	 */
	int32_t inst = 0x00000000;
	op = op << 25;
	vdst = vdst << 17;
	vsrc1 = vsrc1 << 9;
	inst = inst | op | vdst | vsrc1 | src0;
	return (inst);
}

void print_float(union ufloat num, int dec_cases) {

	int32_t aux, counter;
	if (num.f < 0) {
		xil_printf("-");
		num.f *= -1.0f;
	}

	aux = (int) num.f;
	xil_printf("%d.", aux);
	num.f -= (int) num.f;

	for (counter = 0; counter < dec_cases; counter++) {
		num.f *= 10.0f;
		aux = (int) num.f;
		xil_printf("%d", aux);
		num.f -= (int) num.f;
	}
}
int32_t create_vopc(int32_t op_base, int32_t op_offset, int32_t vsrc1,
		int32_t src0) {
	/* VOPC instruction format:
	 * MSB -> LSB
	 * | ENC(7) = 7'b0111110 | OP_Base + OP_Offset (8) | VSRC1(8) | SRC0(9) |
	 */
	int32_t inst = 0x7C000000;
	int32_t op = (op_base + op_offset) << 17;
	vsrc1 = vsrc1 << 9;
	inst = inst | op | vsrc1 | src0;
	return (inst);
}
int main() {

	init_platform();
	static XTmrCtr XPS_Timer;
	unsigned int volatile timer = 0;
	XTmrCtr_Initialize(&XPS_Timer, XPAR_TMRCTR_0_DEVICE_ID);
	XTmrCtr_SetResetValue(&XPS_Timer, 0, 0x00000000);
	int *dram = (int *) XPAR_MIG_7SERIES_0_BASEADDR;
	dram[0]=1;
	xil_printf("dram ok %d\n\r",dram[0]);



	uint32_t dram_flag=1;

  int total_run_count=0;
  for( total_run_count=0; total_run_count<1; total_run_count++){
	//	int *dram = (int *) XPAR_MIG_7SERIES_0_BASEADDR;

	XTmrCtr_Reset(&XPS_Timer, 0);

	uint32_t num_regs = 64;


	int32_t inst_num = sizeof(inst_mem) / sizeof(uint32_t);
	int32_t data[num_regs], vdata_numregs = 1, counter;
	int32_t vdata[vdata_numregs * 64];

	uint64_t num_cycles = 0;

	//uint32_t im_width = 66, im_height = 66, mask_width = 3, mask_height = 3,
	//uint32_t im_width = 34, im_height = 34, mask_width = 3, mask_height = 3,
	uint32_t mask_width = 5;
    uint32_t mask_height = mask_width , frame_size=mask_width-1;
	uint32_t im_width = 512+frame_size, im_height = im_width,
			num_pixels = (im_width) * (im_height);

	int32_t kernel_calls_num =  (im_width-frame_size) * (im_height-frame_size)/64, kernel_calls_counter = 0,
			num_threads = num_pixels;

	kernel_calls_num = kernel_calls_num < 1 ? 1 : kernel_calls_num;

	uint32_t *exec_lo = &inst_mem[1], *exec_hi = &inst_mem[3];

	uint32_t arg0 = dram_flag ? 0x80000000+0x00000260 : 0x00000260;

	uint32_t arg1 = arg0 + im_width * im_height * 4, arg2 =
			arg1 + im_width * im_height * 4, arg3_0 = im_width, arg3_1 =
			im_height, arg4_0 = mask_width, arg4_1 = mask_height, arg5 = 0,
			arg6 = 0, arg7 = 0;

	int32_t ram_init[] = { 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, dram_flag ? 0x80000000+0x00000084 : 0x00000084, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0x00000000, 0, 0, 0, 0, 0, 0, 0, 0x00000000, 0, 0, 0, 0, 0, 0, 0, 0, arg0, 0,
			0, 0, arg1, 0, 0, 0, arg2, 0, 0, 0, arg3_0, arg3_1, 0, 0, arg4_0,
			arg4_1, 0, 0, arg5, 0, 0, 0, arg6, 0, 0, 0, arg7, 0 };//0-150 (151 valores)

	XIo_Out32(NEKO_RESET, 0);
	XIo_Out32(NEKO_RESET, 1);

	XIo_Out32(NEKO_RESET, 0);
	XIo_Out32(NEKO_MEM_ACK, 0);
	XIo_Out32(NEKO_MEM_DONE, 0);

	int bram_counter;

	//Initialization values for the bram
	//int * mem_init=(int*)0x80000000;
	for (bram_counter = 0; bram_counter <= 150; bram_counter++) {
		if(dram_flag){
			dram[bram_counter]=ram_init[bram_counter];
			XIo_Out32(NEKO_MEM_WR_ADDR, bram_counter * 4);
			XIo_Out32(NEKO_MEM_WR_DATA,  ram_init[bram_counter]);
			XIo_Out32(NEKO_MEM_DONE, 0);
			XIo_Out32(NEKO_MEM_DONE, 1);
			XIo_Out32(NEKO_MEM_DONE, 0);
		}
		else{
		//mem_init[bram_counter]=ram_init[bram_counter];
		//dram[bram_counter] = ram_init[bram_counter];
			XIo_Out32(NEKO_MEM_WR_ADDR, bram_counter * 4);
			XIo_Out32(NEKO_MEM_WR_DATA, ram_init[bram_counter]);//0);
			XIo_Out32(NEKO_MEM_DONE, 0);
			XIo_Out32(NEKO_MEM_DONE, 1);
			XIo_Out32(NEKO_MEM_DONE, 0);
		}
	}

	dram=(int*)arg0;
	for(bram_counter=0; bram_counter<num_pixels; bram_counter++){
		if(dram_flag){
			dram[bram_counter]=0;
		}
		else{
			XIo_Out32(NEKO_MEM_WR_ADDR,arg0+bram_counter*4);
			XIo_Out32(NEKO_MEM_WR_DATA,0);
			XIo_Out32(NEKO_MEM_DONE,0);
			XIo_Out32(NEKO_MEM_DONE,1);
			XIo_Out32(NEKO_MEM_DONE,0);
		}
	}
	dram=(int*)arg1;
	for(bram_counter=0; bram_counter<num_pixels; bram_counter++){
		if(dram_flag){
			dram[bram_counter]=1;
		}
		else{
			XIo_Out32(NEKO_MEM_WR_ADDR,arg1+bram_counter*4);
			XIo_Out32(NEKO_MEM_WR_DATA, 1);
			XIo_Out32(NEKO_MEM_DONE,0);
			XIo_Out32(NEKO_MEM_DONE,1);
			XIo_Out32(NEKO_MEM_DONE,0);
		}
	}
	dram=(int*)arg2;
	for (bram_counter = 0; bram_counter < (mask_width * mask_height);
			bram_counter++) {
		if(dram_flag){
			dram[bram_counter]=1;
		}
		else{

			XIo_Out32(NEKO_MEM_WR_ADDR, arg2 + bram_counter * 4);
			XIo_Out32(NEKO_MEM_WR_DATA, 1);
			XIo_Out32(NEKO_MEM_DONE, 0);
			XIo_Out32(NEKO_MEM_DONE, 1);
			XIo_Out32(NEKO_MEM_DONE, 0);
		}
	}

	dram = (int *) 0x80000000;
	num_cycles = 0;

	kernel_calls_counter = 1;
	int offset=0, line_offset=0;
	for (kernel_calls_counter = 0; kernel_calls_counter <  kernel_calls_num;	kernel_calls_counter++) {

		*exec_lo = 0xFFFFFFFF;
		*exec_hi = 0xFFFFFFFF;

		for (counter = 0; counter < vdata_numregs; counter++) {
			vdata[counter] = 0;
		}
		for (counter = 0; counter < 64; counter++) {
			data[counter] = 0;
		}
		if(im_width==(32+frame_size)){
			for (counter = 0; counter < 32; counter++) {
				vdata[counter] = counter + kernel_calls_counter * (im_width*2) + frame_size/2 + im_width*(frame_size/2);
				vdata[counter+32] = vdata[counter] + im_width;
				//xil_printf("%d,",vdata[counter]);
			//	xil_printf("%d,",vdata[counter+32]);
			}
		}
		else{
			for (counter = 0; counter < 64; counter++) {
				vdata[counter] = counter + (frame_size/2+line_offset)*im_width+offset+frame_size/2;
			}
		}
		//xil_printf("\n\r\n\r");
		/*if(offset==0)
			offset=64;
		else if(offset==64){
			offset=0;
			line_offset++;
		}*/

		offset=(offset+64)%(im_width-frame_size);
		if(offset==0)
			line_offset++;

		data[4] = dram_flag ? 0x80000000 : 0x00000000;
		data[2] = dram_flag ? 0x80000000 : 0x00000000;
		data[8] = dram_flag ? 0x80000000 + 0x000001E4 : 0x000001E4;
		data[12] = 0;
		data[15] = 0;
		data[16] = 0;

		num_cycles = run_vop_program_neko(inst_mem, inst_num, data, num_regs,
				vdata, vdata_numregs, 2147483647);
		xil_printf("%d,",num_cycles);
		//	xil_printf("\n\rTotal number of cycles: %ld", num_cycles);
/*
		int32_t * vgpr_data_pointer = (int32_t*) VGPR_DATA;
		int vgpr_word;
		XIo_Out32(VGPR_ADDR, 0);
		for (vgpr_word = 0; vgpr_word < 5; vgpr_word++) {
			xil_printf("%08x\n\r",vgpr_data_pointer[vgpr_word]);
		}
		XIo_Out32(VGPR_ADDR, 1);
		for (vgpr_word = 0; vgpr_word < 5; vgpr_word++) {
			xil_printf("%08x\n\r",vgpr_data_pointer[vgpr_word]);
		}
		xil_printf("\n\r\n\r");
		XIo_Out32(VGPR_ADDR, 2);
		for (vgpr_word = 0; vgpr_word < 5; vgpr_word++) {
			xil_printf("%08x\n\r",vgpr_data_pointer[vgpr_word]);
		}
		XIo_Out32(NEKO_SGRP_ADDR, 0);
					xil_printf("s%d= %08x\n\r",0, XIo_In32(NEKO_SGRP_QUAD_0));
					xil_printf("s%d= %08x\n\r", 1, XIo_In32(NEKO_SGRP_QUAD_1));*/
	}

	//xil_printf("\n\r\n\r");
	//xil_printf("\n\rTotal number of cycles: %ld", num_cycles);



	/*int32_t * vgpr_data_pointer = (int32_t*) VGPR_DATA;
	int vgpr_word;
	XIo_Out32(VGPR_ADDR, 0);
			for (vgpr_word = 0; vgpr_word < 5; vgpr_word++) {
				xil_printf("%08x\n\r",vgpr_data_pointer[vgpr_word]);
			}
		XIo_Out32(VGPR_ADDR,1);
		for (vgpr_word = 0; vgpr_word < 5; vgpr_word++) {
			xil_printf("%08x\n\r",vgpr_data_pointer[vgpr_word]);
		}
		XIo_Out32(VGPR_ADDR, 2);
		for (vgpr_word = 0; vgpr_word < 5; vgpr_word++) {
			xil_printf("%08x\n\r",vgpr_data_pointer[vgpr_word]);
		}*/
		/*for (bram_counter = 0; bram_counter < 27; bram_counter += 4) {
			XIo_Out32(NEKO_SGRP_ADDR, bram_counter);
			xil_printf("s%d= %08x\n\r", bram_counter, XIo_In32(NEKO_SGRP_QUAD_0));
			xil_printf("s%d= %08x\n\r", bram_counter + 1, XIo_In32(NEKO_SGRP_QUAD_1));
			xil_printf("s%d= %08x\n\r", bram_counter + 2, XIo_In32(NEKO_SGRP_QUAD_2));
			xil_printf("s%d= %08x\n\r", bram_counter + 3, XIo_In32(NEKO_SGRP_QUAD_3));
		}*/
		/*XIo_Out32(VGPR_ADDR, 12);
				for (vgpr_word = 0; vgpr_word < 5; vgpr_word++) {
					xil_printf("%08x\n\r",vgpr_data_pointer[vgpr_word]);
				}*/



  }
/*  uint32_t mask_width = 3;
  uint32_t mask_height = mask_width , frame_size=mask_width-1;
  uint32_t im_width = 64+frame_size, im_height = im_width,
		  num_pixels = (im_width) * (im_height);
  int32_t counter;
  uint32_t arg0 = dram_flag ? 0x80000000+0x00000260 : 0x00000260;

  uint32_t arg1 = arg0 + im_width * im_height * 4, arg2 =
		  arg1 + im_width * im_height * 4, arg3_0 = im_width, arg3_1 =
				  im_height, arg4_0 = mask_width, arg4_1 = mask_height, arg5 = 0,
				  arg6 = 0, arg7 = 0;
  for(counter=(im_height-2-frame_size)*im_width;counter<(im_height-2-frame_size)*im_width+2*im_width;counter++){

	  XIo_Out32(NEKO_MEM_WR_ADDR,arg0+counter*4);
	  //
	  if(counter%(frame_size*im_width)==0)
		  xil_printf("\n\r");
	  xil_printf("%d, ",XIo_In32(NEKO_MEM_RD_DATA));
  }*/
  /*int num_errors=0,bram_counter,value,num_pixels=34*34;
  int * mem_init=(int*)0x80000260;
  	for(bram_counter=0; bram_counter<num_pixels; bram_counter++){
  	 //XIo_Out32(NEKO_MEM_WR_ADDR,arg0+bram_counter*4);
  	 //int value = XIo_In32(NEKO_MEM_RD_DATA);
  	 if(mem_init[bram_counter]!=9){//1+bram_counter%5){
  		 xil_printf("Error on pixel:%d, %d\n\r",bram_counter, value);
  	 	 num_errors++;
  	 }
  	 //dram[bram_counter] = res_matrix[bram_counter].u;
  	 }

  	xil_printf("total eerors %d\n\r",num_errors);*/
	cleanup_platform();
	return 0;
}

