#include <stdio.h>
//#include <math.h>
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
//integer mat mult kernel
uint32_t inst_mem[]={
		//Load exec_lo with constant
				0xBEFE03FF,
				0x00000001,
				//Load exec_hi with constant
				0xBEFF03FF,
				0x00000000,
		0xC2400504,
		0xC2420518,
		0xC2030900,
		0xC2038904,
		0xC2070908,
		0xC207890C,
		0xC2040910,
		0xBF8C007F,
		0x8380FF00,
		0x0000FFFF,
		0x8381FF01,
		0x0000FFFF,
		0x9300000C,
		0x9301010D,
		0x80000400,
		0x4A000000,
		0x80000501,
		0x4A020200,
		0x90008208,
		0x34020282,
		0xBF00800F,
		0xBF850118,
		0x9001820F,
		0x4A040283,
		0xD2D60003,
		0x00000302,
		0x4A8E0282,
		0xD2D60005,
		0x00000347,
		0x4A080281,
		0xD2D60007,
		0x00000304,
		0xD2D60008,
		0x00000301,
		0x34120084,
		0x9301B000,
		0x8F048500,
		0x8F058400,
		0x8F088600,
		0xC0880368,
		0xC08A0360,
		0x7E140280,
		0x7E160280,
		0x7E180280,
		0x7E1A0280,
		0xB0090000,
		0x7E1C0280,
		0x7E1E0280,
		0x7E200280,
		0x7E220280,
		0x7E960280,
		0x7E980280,
		0x7E9A0280,
		0x7E9C0280,
		0x7E2C0280,
		0x7E2E0280,
		0x7E300280,
		0x7E320280,
		0x910A9F09,
		0x900A9E0A,
		0x800A0A09,
		0x910A820A,
		0x4A34060A,
		0x4A360A0A,
		0x4A380E0A,
		0x4A3A100A,
		0x34343484,
		0x34363684,
		0x34383884,
		0x343A3A84,
		0x800A0705,
		0x4A343406,
		0x4A3C1207,
		0x4A363606,
		0x4A383806,
		0x4A3A3A06,
		0x4A3E120A,
		0x800A0704,
		0x4A40120A,
		0x800A0701,
		0x4A42120A,
		0xBF8C007F,
		0xEBF31000,
		0x8005221A,
		0xEBF31000,
		0x8004261E,
		0xEBF31000,
		0x80052A1B,
		0xEBF31000,
		0x80052E1C,
		0xEBF31000,
		0x80051A1D,
		0xEBF31000,
		0x8004321F,
		0xEBF31000,
		0x80043620,
		0xEBF31000,
		0x80041E21,
		0xBF8C0F76,
		0xD2D6003A,
		0x00024529,
		0xD2D6003B,
		0x00024526,
		0xD2D6003C,
		0x00024528,
		0xD2D60022,
		0x00024527,
		0xBF8C0F75,
		0xD2D6003D,
		0x00025529,
		0xD2D6003E,
		0x00025526,
		0xD2D6003F,
		0x00025528,
		0xD2D6002A,
		0x00025527,
		0xBF8C0F74,
		0xD2D60040,
		0x00025D29,
		0xD2D60041,
		0x00025D26,
		0xD2D60042,
		0x00025D28,
		0xD2D6002E,
		0x00025D27,
		0xBF8C0F73,
		0xD2D60029,
		0x00023529,
		0xD2D60026,
		0x00023526,
		0xD2D60028,
		0x00023528,
		0xD2D6001A,
		0x00023527,
		0xBF8C0F72,
		0xD2D60027,
		0x00024735,
		0x4A1A750D,
		0xD2D6003A,
		0x00024732,
		0x4A14770A,
		0xD2D6003B,
		0x00024734,
		0x4A18790C,
		0xD2D60023,
		0x00024733,
		0x4A16450B,
		0xD2D60022,
		0x00025735,
		0x4A227B11,
		0xD2D6003C,
		0x00025732,
		0x4A1C7D0E,
		0xD2D6003D,
		0x00025734,
		0x4A207F10,
		0xD2D6002B,
		0x00025733,
		0x4A1E550F,
		0xD2D6002A,
		0x00025F35,
		0x4A2A814E,
		0xD2D6003E,
		0x00025F32,
		0x4A24834B,
		0xD2D6003F,
		0x00025F34,
		0x4A28854D,
		0xD2D6002F,
		0x00025F33,
		0x4A265D4C,
		0xD2D6002E,
		0x00023735,
		0x4A325319,
		0xD2D60029,
		0x00023732,
		0x4A2C4D16,
		0xD2D60026,
		0x00023734,
		0x4A305118,
		0xD2D6001B,
		0x00023733,
		0x4A2E3517,
		0x4A1A1B27,
		0xBF8C0F71,
		0xD2D6001A,
		0x00024939,
		0x4A14153A,
		0xD2D60027,
		0x00024936,
		0x4A18193B,
		0xD2D60028,
		0x00024938,
		0x4A161723,
		0xD2D60023,
		0x00024937,
		0x4A222322,
		0xD2D60022,
		0x00025939,
		0x4A1C1D3C,
		0xD2D60024,
		0x00025936,
		0x4A20213D,
		0xD2D60032,
		0x00025938,
		0x4A1E1F2B,
		0xD2D6002B,
		0x00025937,
		0x4A2A2B2A,
		0xD2D6002A,
		0x00026139,
		0x4A24253E,
		0xD2D6002C,
		0x00026136,
		0x4A28293F,
		0xD2D60033,
		0x00026138,
		0x4A26272F,
		0xD2D6002F,
		0x00026137,
		0x4A32332E,
		0xD2D6002E,
		0x00023939,
		0x4A2C2D29,
		0xD2D60029,
		0x00023936,
		0x4A303126,
		0xD2D60026,
		0x00023938,
		0x4A2E2F1B,
		0xD2D6001B,
		0x00023937,
		0x4A1A350D,
		0xBF8C0F70,
		0xD2D6001A,
		0x00024B21,
		0x4A144F0A,
		0xD2D6001C,
		0x00024B1E,
		0x4A18510C,
		0xD2D60027,
		0x00024B20,
		0x4A16470B,
		0xD2D60023,
		0x00024B1F,
		0x4A224511,
		0xD2D60022,
		0x00025B21,
		0x4A1C490E,
		0xD2D60024,
		0x00025B1E,
		0x4A206510,
		0xD2D60025,
		0x00025B20,
		0x4A1E570F,
		0xD2D60028,
		0x00025B1F,
		0x4A2A5515,
		0xD2D6002A,
		0x00026321,
		0x4A245912,
		0xD2D6002B,
		0x0002631E,
		0x4A286714,
		0xD2D6002C,
		0x00026320,
		0x4A265F13,
		0xD2D6002D,
		0x0002631F,
		0x4A325D19,
		0xD2D60021,
		0x00023B21,
		0x4A2C5316,
		0xD2D6001E,
		0x00023B1E,
		0x4A304D18,
		0xD2D60020,
		0x00023B20,
		0x4A2E3717,
		0xD2D6001B,
		0x00023B1F,
		0x4A1A350D,
		0x4A14390A,
		0x4A184F0C,
		0x4A16470B,
		0x4A224511,
		0x4A1C490E,
		0x4A204B10,
		0x4A1E510F,
		0x4A9C5515,
		0x4A965712,
		0x4A9A5914,
		0x4A985B13,
		0x4A324319,
		0x4A2C3D16,
		0x4A304118,
		0x4A2E3717,
		0x80070708,
		0x80098409,
		0xBF0B090F,
		0xBF850014,
		0xBF82FF0C,
		0x4A040283,
		0x4A8E0282,
		0x4A080281,
		0x7E140280,
		0x7E160280,
		0x7E180280,
		0x7E1A0280,
		0x7E1C0280,
		0x7E1E0280,
		0x7E200280,
		0x7E220280,
		0x7E960280,
		0x7E980280,
		0x7E9A0280,
		0x7E9C0280,
		0x7E2C0280,
		0x7E2E0280,
		0x7E300280,
		0x7E320280,
		0xD2D60001,
		0x00000101,
		0x4A020300,
		0x34020284,
		0x4A02020E,
		0xD2D60008,
		0x00000104,
		0x4A101100,
		0x34101084,
		0x4A10100E,
		0xD2D60007,
		0x00000147,
		0x4A0E0F00,
		0x340E0E84,
		0x4A0E0E0E,
		0xD2D60002,
		0x00000102,
		0x4A000500,
		0x34000084,
		0x4A00000E,
		0xC0800370,
		0xBF8C007F,
		0xEBF71000,
		0x80001601,
		0xEBF71000,
		0x80004B08,
		0xEBF71000,
		0x80000E07,
		0xEBF71000,
		0x80000A00,
		0xBF810000};
//F.P. matrix multiplication kernel
/*uint32_t inst_mem[] = {
		//Load exec_lo with constant
		0xBEFE03FF,
		0x0000000F,
		//Load exec_hi with constant
		0xBEFF03FF,
		0x00000000,
		//end load
		0xC2400504,
		0xC2420518,
		0xC2030900,
		0xC2038904,
		0xC2070908,
		0xC207890C,
		0xC2040910,
		0xBF8C007F,
		0x8380FF00,
		0x0000FFFF,
		0x8381FF01,
		0x0000FFFF,
		0x9300000C,
		0x9301010D,
		0x80000400,
		0x4A000000,
		0x80000501,
		0x4A020200,
		0x90008208,
		0x34020282,
		0xBF00800F,

		//0xBF8500A7,//pduarte: branch had to be modified because of offset changes
		0xBF8500D7,

		0x9001820F,
		0x4A040283,
		0xD2D60003,
		0x00000302,
		0x4A7E0282,
		0xD2D60005,
		0x0000033F,
		0x4A080281,
		0xD2D60007,
		0x00000304,
		0xD2D60008,
		0x00000301,
		0x34120084,
		0x9301B000,
		0x8F048500,
		0x8F058400,
		0x8F088600,
		0xC0880368,
		0xC08A0360,
		0x7E760280,
		0x7E780280,
		0x7E7A0280,
		0x7E7C0280,
		0xB0090000,
		0x7E2A0280,
		0x7E2C0280,
		0x7E2E0280,
		0x7E300280,
		0x7E1A0280,
		0x7E1C0280,
		0x7E1E0280,
		0x7E200280,
		0x7E220280,
		0x7E240280,
		0x7E260280,
		0x7E280280,
		0x800A0705,
		0x4A34120A,
		0x910A9F09,
		0x900A9E0A,
		0x800A0A09,
		0x910A820A,
		0x4A36060A,
		0x34363684,
		0x4A363606,
		0x4A381207,
		0x800B0704,
		0x4A3A120B,
		0x800B0701,
		0x4A3C120B,
		0x4A3E0A0A,
		0x343E3E84,
		0x4A3E3E06,
		0x4A400E0A,
		0x34404084,
		0x4A404006,
		0x4A42100A,
		0x34424284,
		0x4A424206,
		0xBF8C007F,
		0xEBF31000,
		0x8004221A,
		0xEBF31000,
		0x8005261B,
		0xEBF31000,
		0x80042A1C,
		0xEBF31000,
		0x80041A1D,
		0xEBF31000,
		0x80042E1E,
		0xEBF31000,
		0x8005321F,
		0xEBF31000,
		0x80053620,
		0xEBF31000,
		0x80051E21,
		0xBF8C0F75,
		0x10745B26,




		//start of mac's
		//0x3E744F25,
		0x10804F25,
		0x0674813A,
		//
		0xBF8C0F74,
		//0x3E743B28,
		0x10803B28,
		0x0674813A,
		//
		0xBF8C0F73,
		//0x3E746329,
		0x10806329,
		0x0674813A,
		//
		0x067C753E,
		0x10745526,
		//0x3E744F22,
		0x10804F22,
		0x0674813A,
		//
		//0x3E743528,
		0x10803528,
		0x0674813A,
		//
		//0x3E745D29,
		0x10805D29,
		0x0674813A,
		//
		0x0676753B,
		//--------

		0x10745926,
		//0x3E744F24,
		0x10804F24,
		0x0674813A,
		//
		//0x3E743928,
		0x10803928,
		0x0674813A,
		//
		//0x3E746129,
		0x10806129,
		0x0674813A,
		//
		0x067A753D,
		0x104C5726,
		//0x3E4C4F23,
		0x10804F23,
		0x064C8126,
		//
		//0x3E4C3728,
		0x10803728,
		0x064C8126,
		//
		//0x3E4C5F29,
		0x10805F29,
		0x064C8126,
		//
		0x06784D3C,
		0xBF8C0F72,
		//---------

		0x104C652D,
		//0x3E4C6725,
		0x10806725,
		0x064C8126,
		//0x3E4C691D,
		0x1080691D,
		0x064C8126,
		//0x3E4C6B31,
		0x10806B31,
		0x064C8126,
		//
		0x06304D18,
		0x104C652A,
		//0x3E4C6722,
		0x10806722,
		0x064C8126,
		//0x3E4C691A,
		0x1080691A,
		0x064C8126,
		//0x3E4C6B2E,
		0x10806B2E,
		0x064C8126,
		//
		0x062A4D15,
		0x104C652C,
		//---------

		//0x3E4C6724,
		0x10806724,
		0x064C8126,
		//0x3E4C691C,
		0x1080691C,
		0x064C8126,
		//0x3E4C6B30,
		0x10806B30,
		0x064C8126,
		//
		0x062E4D17,
		0x104C652B,
		//0x3E4C6723,
		0x10806723,
		0x064C8126,
		//0x3E4C691B,
		0x1080691B,
		0x064C8126,
		//0x3E4C6B2F,
		0x10806B2F,
		0x064C8126,
		//
		0x062C4D16,
		0xBF8C0F71,
		0x104C6D2D,
		//---------

		//0x3E4C6F25,
		0x10806F25,
		0x064C8126,
		//0x3E4C711D,
		0x1080711D,
		0x064C8126,
		//0x3E4C7331,
		0x10807331,
		0x064C8126,
		//
		0x06204D10,
		0x104C6D2A,
		//0x3E4C6F22,
		0x10806F22,
		0x064C8126,
		//0x3E4C711A,
		0x1080711A,
		0x064C8126,
		//0x3E4C732E,
		0x1080732E,
		0x064C8126,
		//
		0x061A4D0D,
		0x104C6D2C,
		//---------

		//0x3E4C6F24,
		0x10806F24,
		0x064C8126,
		//0x3E4C711C,
		0x1080711C,
		0x064C8126,
		//0x3E4C7330,
		0x10807330,
		0x064C8126,
		//
		0x061E4D0F,
		0x104C6D2B,
		//0x3E4C6F23,
		0x10806F23,
		0x064C8126,
		//0x3E4C711B,
		0x1080711B,
		0x064C8126,
		//0x3E4C732F,
		0x1080732F,
		0x064C8126,
		//
		0x061C4D0E,
		0xBF8C0F70,
		0x104C3D2D,
		//---------

		//0x3E4C3F25,
		0x10803F25,
		0x064C8126,
		//0x3E4C411D,
		0x1080411D,
		0x064C8126,
		//0x3E4C4331,
		0x10804331,
		0x064C8126,
		//
		0x06284D14,
		0x103A3D2A,

		//0x3E3A3F22,
		0x10803F22,
		0x063A811D,
		//0x3E3A411A,
		0x1080411A,
		0x063A811D,
		//0x3E3A432E,
		0x1080432E,
		0x063A811D,
		//
		0x06223B11,
		0x10343D2C,
		//---------

		//0x3E343F24,
		0x10803F24,
		0x0634811A,
		//0x3E34411C,
		0x1080411C,
		0x0634811A,
		//0x3E344330,
		0x10804330,
		0x0634811A,
		0x06263513,
		0x10343D2B,
		//0x3E343F23,
		0x10803F23,
		0x0634811A,
		//0x3E34411B,
		0x1080411B,
		0x0634811A,
		//0x3E34432F,
		0x1080432F,
		0x0634811A,
		//--------


		0x06243512,
		0x80070708,
		0x80098409,
		0xBF0B090F,
		0xBF850014,

		//0xBF82FF7D,//had to modify branch offset
		0xBF82FF4D,

		0x4A040283,
		0x4A7E0282,
		0x4A080281,
		0x7E760280,
		0x7E780280,
		0x7E7A0280,
		0x7E7C0280,
		0x7E2A0280,
		0x7E2C0280,
		0x7E2E0280,
		0x7E300280,
		0x7E1A0280,
		0x7E1C0280,
		0x7E1E0280,
		0x7E200280,
		0x7E220280,
		0x7E240280,
		0x7E260280,
		0x7E280280,
		0xD2D60001,
		0x00000101,
		0x4A020300,
		0x34020284,
		0x4A02020E,
		0xD2D60008,
		0x00000104,
		0x4A101100,
		0x34101084,
		0x4A10100E,
		0xD2D60007,
		0x0000013F,
		0x4A0E0F00,
		0x340E0E84,
		0x4A0E0E0E,
		0xD2D60002,
		0x00000102,
		0x4A000500,
		0x34000084,
		0x4A00000E,
		0xC0800370,
		0xBF8C007F,
		0xEBF71000,
		0x80001101,
		0xEBF71000,
		0x80000D08,
		0xEBF71000,
		0x80001507,
		0xEBF71000,
		0x80003B00,
		0xBF810000};*/

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
	int32_t index, succeeded = 1,address,data;//,counter = 0;
	int32_t vgpr, vgpr_word,pc,old_pc=0;
	int32_t * vgpr_data_pointer = (int32_t*)VGPR_DATA;
	uint32_t min_addr = 0x80000000;

	//NEKO's reset pulse
	XIo_Out32(NEKO_RESET,0);
	XIo_Out32(NEKO_RESET,1);
	XIo_Out32(NEKO_RESET,0);

	XIo_Out32(NEKO_BASE_LDS, XPAR_MIG_7SERIES_0_BASEADDR);

	//Load scalar registers with data
	/*int32_t sgpr[]={1,1,0,0,0,0,0x80400000,0x80800000,512,0,0,0,0,0,0x80c00000,512,0,0,0,0,0,0,0,0,0};

	for(index = 0; index < 24; index+=4){
		XIo_Out32(NEKO_SGRP_ADDR, index);
		XIo_Out32(NEKO_SGRP_QUAD_0, sgpr[index]);
		XIo_Out32(NEKO_SGRP_QUAD_1, sgpr[index+1]);
		XIo_Out32(NEKO_SGRP_QUAD_2, sgpr[index+2]);
		XIo_Out32(NEKO_SGRP_QUAD_3, sgpr[index+3]);
		XIo_Out32(NEKO_GPR_CMD, 1);
	}*/
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

	//int32_t tab[]={1,2,3,4,13,14,15,16,13,14,15,16,1,2,3,4,9,10,11,12,5,6,7,8,5,6,7,8,9,10,11,12};
	/*int32_t tab[]={0x3f800000,0x40000000,0x40400000,0x40800000,
			       0x41500000,0x41600000,0x41700000,0x41800000,
			       0x41500000,0x41600000,0x41700000,0x41800000,
			       0x3f800000,0x40000000,0x40400000,0x40800000,
			       0x41100000,0x41200000,0x41300000,0x41400000,
			       0x40a00000,0x40c00000,0x40e00000,0x41000000,
			       0x40a00000,0x40c00000,0x40e00000,0x41000000,
			       0x41100000,0x41200000,0x41300000,0x41400000};

	for(vgpr=26;vgpr<=57;vgpr++){
		XIo_Out32(VGPR_ADDR, vgpr);
		vgpr_data_pointer[0] = tab[vgpr-26];
		XIo_Out32(VGPR_WR_CMD, 1);
	}*/

	for(vgpr=0;vgpr<num_vect_data;vgpr++){
		XIo_Out32(VGPR_ADDR, vgpr);
		for(vgpr_word=0;vgpr_word<64;vgpr_word++){
			vgpr_data_pointer[vgpr_word] = inst_vect_data[vgpr*64+vgpr_word];
		}
		XIo_Out32(VGPR_WR_CMD, 1);

	}
	/*XIo_Out32(VGPR_ADDR, 0);
	vgpr_data_pointer[0] = 0;
	vgpr_data_pointer[1] = 0;
	vgpr_data_pointer[2] = 0;
	vgpr_data_pointer[3] = 0;
	vgpr_data_pointer[4] = 1;
	vgpr_data_pointer[5] = 1;
	vgpr_data_pointer[6] = 1;
	vgpr_data_pointer[7] = 1;
	vgpr_data_pointer[8] = 2;
	vgpr_data_pointer[9] = 2;
	vgpr_data_pointer[10] = 2;
	vgpr_data_pointer[11] = 2;
	vgpr_data_pointer[12] = 3;
	vgpr_data_pointer[13] = 3;
	vgpr_data_pointer[14] = 3;
	vgpr_data_pointer[15] = 3;

	XIo_Out32(VGPR_WR_CMD, 1);
	XIo_Out32(VGPR_ADDR, 1);
	vgpr_data_pointer[0] = 0;
	vgpr_data_pointer[1] = 1;
	vgpr_data_pointer[2] = 2;
	vgpr_data_pointer[3] = 3;
	vgpr_data_pointer[4] = 0;
	vgpr_data_pointer[5] = 1;
	vgpr_data_pointer[6] = 2;
	vgpr_data_pointer[7] = 3;
	vgpr_data_pointer[8] = 0;
	vgpr_data_pointer[9] = 1;
	vgpr_data_pointer[10] = 2;
	vgpr_data_pointer[11] = 3;
	vgpr_data_pointer[12] = 0;
	vgpr_data_pointer[13] = 1;
	vgpr_data_pointer[14] = 2;
	vgpr_data_pointer[15] = 3;
	XIo_Out32(VGPR_WR_CMD, 1);*/



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
		//cycle_counter = XIo_In32(NEKO_CYCLE_COUNTER);
		/*if(cycle_counter>max_clocks){
			succeeded=0;
			break;
		}*/

		/*pc = XIo_In32(NEKO_PC);

		if(pc!=old_pc){
			old_pc=pc;
			xil_printf("pc: %08x\n\r",pc);
		}*/


		data = XIo_In32(NEKO_MEM_OP);
		if(data != 0)
		{
			//data = XIo_In32(NEKO_CYCLE_COUNTER);
			//xil_printf("Mem OP: %d\r\n", data);
			int nextValue = MEM_RD_RDY_WAIT;

//xil_printf("mem op %d\n\r", data);
			if(data == MEM_RD_ACK_WAIT)
			{
				nextValue = MEM_RD_RDY_WAIT;
				//xil_printf("Mem read\r\n", data);


			}
			else if(data == MEM_WR_ACK_WAIT)
			{
				nextValue = MEM_WR_RDY_WAIT;
				//xil_printf("Mem write\r\n", data);


			}
			else if(data == MEM_WR_LSU_WAIT || data == MEM_RD_LSU_WAIT )//last instruction is not finished yet
				continue;

			//pc = XIo_In32(NEKO_PC);
			//xil_printf("Mem OP: %08x,%d, %d\n\r",pc,counter,data);
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
			XIo_Out32(NEKO_MEM_ACK, 0);

			do {
				data = XIo_In32(NEKO_MEM_OP);
				// xil_printf("Mem OP: %d\r\n", data);

			} while(data != nextValue);

			address = XIo_In32(NEKO_MEM_ADDR);
			//xil_printf("Mem addr: %08x\r\n", address);
			if(nextValue == MEM_RD_RDY_WAIT)
			{
				//xil_printf("read addr: %08x \n\r", address);

				data = 0x12121212;//XIo_In32(address);
				//data = tab[counter];
				//counter++;
				XIo_Out32(NEKO_MEM_WR_DATA, data);
				//xil_printf("data %08x\n\r",data);
				/*XIo_Out32(NEKO_MEM_WR_EN, 0);
		      			XIo_Out32(NEKO_MEM_WR_EN, 1);
		      			XIo_Out32(NEKO_MEM_WR_EN, 0);*/
				nextValue = MEM_RD_LSU_WAIT;
			}
			else
			{
//				xil_printf("write addr: %08x\r\n", address);

				data = XIo_In32(NEKO_MEM_RD_DATA);
				//if(data==4)
				//xil_printf("%08x  <- %d\n\r",address,data);
				//counter++;
				if(address >= min_addr)
					XIo_Out32(address, data);
				//xil_printf("data %08x\n\r",data);
				nextValue = MEM_WR_LSU_WAIT;
			}

			XIo_Out32(NEKO_MEM_DONE, 0);
			XIo_Out32(NEKO_MEM_DONE, 1);

			do {
				data = XIo_In32(NEKO_MEM_OP);
				// xil_printf("Mem OP: %d\r\n", data);

			} while(data != 0 && data != nextValue && data != MEM_RD_ACK_WAIT && data != MEM_WR_ACK_WAIT);
			//xil_printf("Mem OP: %d\r\n", data);

		}
	}
	//xil_printf("write counter %d\n\r", counter);
	//xil_printf("Mem OP: %08x\n\r",pc);
	int32_t num_cycles = XIo_In32(NEKO_CYCLE_COUNTER);
	xil_printf("Number of cycles to completion: %d\n\r", num_cycles);
	//xil_printf("Number of proc cycles to completion: %d\n\r", XIo_In32(NEKO_PROC_CYCLE_COUNTER));

	//NEKO's reset pulse
	XIo_Out32(NEKO_RESET,0);
	XIo_Out32(NEKO_RESET,1);
	XIo_Out32(NEKO_RESET,0);

	/*if(succeeded){
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
	}*/
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
int main()
{
    init_platform();

    /*union ufloat num;
    num.f = 1.0f*0.5f;
    xil_printf("%08x\n\r",num.u);*/
    int32_t global_launch_counter;

    //for(global_launch_counter = 0; global_launch_counter<10; global_launch_counter++){
    XIo_Out32(NEKO_RESET,0);
    XIo_Out32(NEKO_RESET,1);

    XIo_Out32(NEKO_RESET,0);
	XIo_Out32(NEKO_MEM_ACK, 0);
	XIo_Out32(NEKO_MEM_DONE,0);

    int bram_counter;
    for(bram_counter=0; bram_counter<400; bram_counter+=4){
    	XIo_Out32(NEKO_MEM_WR_ADDR,bram_counter);
    	XIo_Out32(NEKO_MEM_WR_DATA,bram_counter+1);
    	XIo_Out32(NEKO_MEM_DONE,0);
    	XIo_Out32(NEKO_MEM_DONE,1);
    	XIo_Out32(NEKO_MEM_DONE,0);

    }

    int32_t data_bram;
    /*for(bram_counter=0; bram_counter<400; bram_counter+=4){
        	XIo_Out32(NEKO_MEM_WR_ADDR,bram_counter);
        	data_bram = XIo_In32(NEKO_MEM_RD_DATA);
        	xil_printf("data at %d: %d\n\r",bram_counter,data_bram);

        }*/

/*#define NEKO_MEM_OP (NEKO_CMD_ADDR + 128)
#define NEKO_MEM_RD_DATA (NEKO_CMD_ADDR + 132) // Address for data to be read from MIAOW and written to memory
#define NEKO_MEM_ADDR (NEKO_CMD_ADDR + 136)
#define NEKO_MEM_WR_ADDR (NEKO_CMD_ADDR + 188) // Address for writing data to MIAOW
#define NEKO_MEM_WR_DATA (NEKO_CMD_ADDR + 192) // Address for writing data to MIAOW
#define NEKO_MEM_WR_EN (NEKO_CMD_ADDR + 196)
#define NEKO_MEM_ACK (NEKO_CMD_ADDR + 200)
#define NEKO_MEM_DONE (NEKO_CMD_ADDR + 204)*/
//xil_printf("%08x\n\r",create_sop1(3, 126, 255));
//xil_printf("%08x\n\r",create_sop1(3, 127, 255));
    uint32_t num_regs = 64,mat_dim1 = 8,mat_dim2 = 8;
    uint32_t num_elems = mat_dim1*mat_dim2, num_submat = mat_dim1 / 4, kernel_launch_counter = 0, kernel_launches = (mat_dim1/32)*(mat_dim2/32);//each launch performs a mult with max matrix size 32x32
    union ufloat value;
    if(kernel_launches==0)
    	kernel_launches = 1;
    uint32_t *exec_lo = &inst_mem[1], *exec_hi = &inst_mem[3];
    if(mat_dim1>=32){
    	*exec_lo = 0xFFFFFFFF;
    	*exec_hi = 0xFFFFFFFF;
    }
    else{
    	*exec_lo = 0x0FFFF;//FFFF;//(2^(num_elems/16))-1;//16 because we're dividing in 4x4 matrices
    	*exec_hi = 0;
    }
    int *dram = (int *)XPAR_MIG_7SERIES_0_BASEADDR;
    int32_t inst_num = sizeof(inst_mem) / sizeof(int32_t);
    int32_t data[num_regs], vdata_numregs = 3*64, counter;
    int32_t vdata[vdata_numregs];


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
    		                0,0,0,0,0x000001E8,0,0,0,0x000401E8,0,
    		                0,0,0x000801E8,0,0,0,mat_dim1,0,0,0,
    		                mat_dim2
    };






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

    //int32_t run_line=0, run_column=0;


    /*for(run_column=0; run_column<2; run_column++){
    	//for(bram_counter=0; bram_counter<512*256; bram_counter++){
    		XIo_Out32(NEKO_MEM_WR_ADDR,0x021E8+4*bram_counter);
    		XIo_Out32(NEKO_MEM_WR_DATA,(bram_counter+1)/256);//(512*(bram_counter/256)+(bram_counter+1)%512)%1024);
    		XIo_Out32(NEKO_MEM_DONE,0);
    		XIo_Out32(NEKO_MEM_DONE,1);
    		XIo_Out32(NEKO_MEM_DONE,0);

    		//xil_printf("%08x\n\r",0x80000+4*bram_counter);
    	}
    	for(run_line=0; run_line<128; run_line++){



    		data[4] = 0x00000000;
    		data[2] = 0x00000000;
    		data[8] = 0x000001A4;

    		data[12] = 0;
    		data[13] = 0;


    		for(bram_counter=0; bram_counter<512*4; bram_counter++){
    			XIo_Out32(NEKO_MEM_WR_ADDR,0x001E8+4*bram_counter);
    			XIo_Out32(NEKO_MEM_WR_DATA,(bram_counter+1)%1024);
    			XIo_Out32(NEKO_MEM_DONE,0);
    			XIo_Out32(NEKO_MEM_DONE,1);
    			XIo_Out32(NEKO_MEM_DONE,0);
    		}





    		for(counter = 0; counter < 256; counter++){//4*256
    			XIo_Out32(NEKO_MEM_WR_ADDR,0x01E8+4*counter);
    			dram[counter+run_line*2048+run_column*256] = XIo_In32(NEKO_MEM_RD_DATA);//4 lines of 512 -> 2048

    			XIo_Out32(NEKO_MEM_WR_ADDR,0x01E8+1024+4*counter);
    			dram[counter+512+run_line*2048+run_column*256] = XIo_In32(NEKO_MEM_RD_DATA);//4 lines of 512 -> 2048

    			XIo_Out32(NEKO_MEM_WR_ADDR,0x01E8+2048+4*counter);
    			dram[counter+1024+run_line*2048+run_column*256] = XIo_In32(NEKO_MEM_RD_DATA);//4 lines of 512 -> 2048

    			XIo_Out32(NEKO_MEM_WR_ADDR,0x01E8+3072+4*counter);
    			dram[counter+1536+run_line*2048+run_column*256] = XIo_In32(NEKO_MEM_RD_DATA);//4 lines of 512 -> 2048
    		}
    	}
    }

    for(counter=0; counter<16; counter++){
    	xil_printf("%d - %d - %d - %d \n\r", dram[counter*256], dram[counter*256+1], dram[counter*256+2], dram[counter*256+3]);
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

		for(counter = 0; counter<vdata_numregs; counter++){
			vdata[counter] = 0;
		}


		for(counter = 0; counter<64; counter++){
			data[counter] = 0;
		}
		data[4] = 0x00000000;
		data[2] = 0x00000000;
		data[8] = 0x000001A4;

		data[12] = 0;
		data[13] = 0;
		/*for(counter = 0; counter<64; counter++){
			vdata[counter] = counter;
			vdata[counter+64] = 0;
			vdata[counter+128] = 0;
		}*/
		union ufloat mat_value;
		for(bram_counter=0; bram_counter<mat_dim1*mat_dim2; bram_counter++){
			//mat_value.f = 0.5f*((bram_counter+1)%256);
			//Mat A
			XIo_Out32(NEKO_MEM_WR_ADDR,0x000001E8+4*bram_counter);
			XIo_Out32(NEKO_MEM_WR_DATA,(bram_counter+1)%256);//(512*(bram_counter/256)+(bram_counter+1)%512)%1024);
			XIo_Out32(NEKO_MEM_DONE,0);
			XIo_Out32(NEKO_MEM_DONE,1);
			XIo_Out32(NEKO_MEM_DONE,0);

			//Mat B
			XIo_Out32(NEKO_MEM_WR_ADDR,0x000401E8+4*bram_counter);
			XIo_Out32(NEKO_MEM_WR_DATA,(bram_counter+1)%256);//(512*(bram_counter/256)+(bram_counter+1)%512)%1024);
			XIo_Out32(NEKO_MEM_DONE,0);
			XIo_Out32(NEKO_MEM_DONE,1);
			XIo_Out32(NEKO_MEM_DONE,0);

			//xil_printf("%08x\n\r",0x80000+4*bram_counter);
		}
		for(bram_counter=0; bram_counter<=121; bram_counter++){
			XIo_Out32(NEKO_MEM_WR_ADDR,bram_counter*4);
			XIo_Out32(NEKO_MEM_WR_DATA,ram_init[bram_counter]);
			XIo_Out32(NEKO_MEM_DONE,0);
			XIo_Out32(NEKO_MEM_DONE,1);
			XIo_Out32(NEKO_MEM_DONE,0);

		}

    uint32_t v0_offset = 0, first_time = 1, init_offset = 0;
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
    		counter = 0;
    	//total_counter+=
    	//}
    	run_vop_program_neko(inst_mem,inst_num, data, num_regs,vdata,vdata_numregs,2147483647);
    }
    for(bram_counter=0; bram_counter<64; bram_counter++){
    	if(bram_counter%4==0 && bram_counter!=0){
    		xil_printf("\n\r");
    		//bram_counter+=12;
    	}
    	XIo_Out32(NEKO_MEM_WR_ADDR,0x0801E8+4*bram_counter);
    	data_bram = XIo_In32(NEKO_MEM_RD_DATA);
    	xil_printf("%d - ",data_bram);

    }
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
