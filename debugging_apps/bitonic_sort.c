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
		0xBEFE03FF,
		0x0000000F,
		//Load exec_hi with constant
		0xBEFF03FF,
		0x00000000,
		//end load
		0xC2000904,0xC2008918,0xC2010D04,0xC2018D08,0xBF8C007F,
		0x8380FF00,0x0000FFFF,0x93000010,0x80000100,0x4A000000,0x80800302,0x2C020000,0x8F010081,0xC2018D00,0xD2D60001,0x00020201,0x34020281,0xD2900002,0x00010100,0x4A020501,0x4A040201,0x34020282,0x34040482,0xBF8C007F,0x4A020203,0x4A040403,0xEBA01000,0x80010301,0xEBA01000,0x80010402,0xC2000D10,0x8F010281,0xBF8C007F,0x80820081,0x36000001,0x7D040080,0x7E000200,0x7E0A0202,0x00000105,0xBF8C0F70,0x260A0903,0x28060903,0x7D040080,0x00000705,0x00060B03,0xEBA41000,0x80010001,0xEBA41000,0x80010302,
		/**/0xBF810000
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

int32_t run_vop_program_neko(uint32_t insts[],int32_t num_insts, int32_t inst_scalar_data[], int32_t num_scalar_data,int32_t inst_vect_data[], int32_t num_vect_data,int32_t max_clocks){
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
				//xil_printf("addr %08x\n\r",address);
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


	init_platform();


	static XTmrCtr XPS_Timer;
	unsigned int volatile timer = 0;
	XTmrCtr_Initialize(&XPS_Timer, XPAR_TMRCTR_0_DEVICE_ID);
	XTmrCtr_SetResetValue(&XPS_Timer, 0, 0x00000000);
	XTmrCtr_Reset(&XPS_Timer, 0);



	//int32_t data_bram;

	uint32_t num_regs = 64;

	int *dram = (int *)XPAR_MIG_7SERIES_0_BASEADDR;
	int32_t inst_num = sizeof(inst_mem) / sizeof(int32_t);
	int32_t data[num_regs], vdata_numregs = 3*64, counter;
	int32_t vdata[vdata_numregs];


	int64_t  num_cycles = 0;
	//int32_t i;
	//int32_t threshold = 0, max_it = 500, iteration=0;

	int32_t kernel_calls_num = 64/64, kernel_calls_counter = 0, num_threads;

	kernel_calls_num = kernel_calls_num < 1 ? 1 : kernel_calls_num;

	uint32_t *exec_lo = &inst_mem[1], *exec_hi = &inst_mem[3];


	//*exec_lo = 0x0f;
	//*exec_hi = 0x0;


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




//	uint32_t mat_m = 0x80000260, mat_a = mat_m + (size*size*4), vec_b = mat_a + (size*size*4);
	//uint32_t t = 0;
	//union ufloat res;

	 //generate array
	int32_t length = 64, direction = 0, temp, numStages, stage, passOfStage;
	numStages=6;
	//for(temp = length; temp > 1; temp >>= 1)
	  //          ++numStages;
	int32_t array[] = {510,974,817,1228,1639,215,1682,1723,726,881,1172,1435,1521,1552,797,879,1959,1173,1740,566,1275,1205,1973,176,1025,1068,185,1853,1811,899,1601,304,1269,534,913,1729,402,622,990,692,1635,2022,326,485,1438,769,1994,1991,1318,1761,823,1294,2018,1146,1912,1475,991,1309,1818,407,810,2032,824,1349,1846,2039,1338,222,74,1266,1162,1970,1528,1357,1072,532,1970,1106,62,1426,1064,121,1823,676,470,233,637,468,1335,135,564,577,1802,910,1548,1236,1604,233,2004,1738,104,955,667,1291,472,1188,1235,1229,918,73,1052,835,221,942,923,1129,1649,1435,1786,107,450,941,1963,1618,925,683,121,1517,1038,409,875,345,1539,754,1929,35,1698,1283,1103,1332,1488,194,1797,29,603,368,1897,140,1190,1305,1334,1771,115,1673,1083,1422,435,1113,1439,1959,910,175,117,1289,1631,1416,707,1939,1065,1953,151,424,1587,1872,1603,605,311,1737,1607,555,467,657,1699,1684,1169,1171,586,1432,1631,904,914,954,571,1383,1851,1861,1530,534,1412,270,253,391,298,1198,150,1684,1481,1896,1009,1341,1823,1103,578,1999,75,668,1993,748,633,248,1875,277,680,1838,1023,1260,1194,1430,60,1081,66,1694,696,1734,504,1191,1920,98,111,42,1396,1226,234,1631,1265,144,142,279,1616,189,487,499,215,1758,1430,1503,1332,1057,668,1355,241,303,40,1975,1987,254,957,1345,594,1545,1143,876,547,1544,1840,1492,833,1922,523,1092,1955,548,512,1900,140,613,1212,416,1302,1635,1027,1333,1630,478,1231,230,1056,1716,1886,1020,569,1336,1879,1044,1995,404,228,609,812,862,638,1421,188,823,605,628,216,1216,579,318,1,581,1128,1784,87,1853,268,1707,1639,1880,281,1034,829,355,1178,1242,439,1065,2026,1003,1423,843,71,600,1641,710,171,1047,751,1514,1075,1648,1673,388,253,1681,1306,33,1835,1055,1115,1242,1557,1752,784,173,1503,680,1720,761,1696,362,265,1802,90,1406,1503,895,778,2006,817,902,321,668,643,1832,506,636,837,1450,294,1784,170,946,62,1543,1434,439,1392,1141,1742,1144,1847,859,733,1001,524,1903,956,520,883,1439,824,372,1754,1196,765,454,448,1070,888,1518,144,1735,1392,280,1758,409,1244,1112,332,12,1580,1566,862,116,1200,357,1492,1094,518,1878,1553,1817,141,376,1510,1427,1591,1028,871,1252,1753,1374,1072,612,1442,782,1163,1818,1726,1841,1923,1670,3,6,179,534,47,869,699,1109,1897,611,692,1760,697,283,1040,1754,787,1425,1286,922,970,1945,171,573,915,1203,1797,961,896,1528,958,1763,955,1020,998,470,175,138,1819,478,1765,1458,1788,1921,286,807,2008,1321,1836,988,29,1276,473,1080,1485,1244,1205,888,500,879,21,1247,1962,195,73,1815,506,18,1669,288,1802,195,722,1215,1198,1367,1327,888,286,1540,495,1332,1756,173,1991,64,1711,1712,102,1118,1932,658,1652,1232,1617,1637,102,580,1338,1003,1992,1533,1163,612,525,1816,915,1671,201,1760,57,1841,1843,1073,246,364,1446,1703,71,1552,1960,702,1307,702,443,1610,1481,571,1193,862,189,49,1006,570,696,588,350,818,1429,417,1365,907,887,359,396,1262,551,1146,1935,1463,1391,1965,1588,1245,1942,122,550,2021,1581,974,1394,854,779,437,784,61,967,683,1999,1138,1733,836,946,1692,2030,1073,1895,1514,1162,1984,1689,1965,1324,777,976,1868,30,321,966,1112,122,1348,1822,225,897,574,2018,1247,520,272,1116,1695,1714,1707,417,1115,1792,248,1754,1843,446,158,971,1710,961,847,1030,257,271,1783,1235,543,1771,119,937,1479,694,822,1079,1831,1594,142,571,777,1771,860,491,1224,982,1840,1914,1675,1452,1522,1843,134,688,9,1696,1039,750,464,1095,593,140,174,140,839,253,907,1841,724,246,1166,1792,714,86,292,157,1517,935,1369,1432,1170,1288,1798,1356,1793,957,289,140,1463,631,1375,1336,1088,1465,1034,999,1020,1917,797,240,492,1403,1719,1987,441,1557,1196,825,1045,1015,1334,1523,618,184,1692,798,1588,367,224,1854,1795,2048,1770,76,1116,2043,1047,1789,144,2022,1890,1156,884,692,1476,28,766,1890,1119,971,1017,633,1947,2011,1052,2033,934,873,437,396,1705,1488,1085,1698,1048,1130,437,1204,292,107,1399,1246,450,832,1290,1137,261,347,2,856,1000,327,1366,37,245,1950,1999,63,1011,1767,497,1709,1666,1288,5,778,1852,1393,776,1294,498,1170,2011,1740,580,1398,733,2021,172,513,1662,173,1088,1640,1513,290,897,718,980,1203,299,1854,1311,334,1159,1908,1604,1404,955,533,1166,509,654,1865,1813,1627,1896,366,1060,1284,1870,1360,797,1516,1675,1230,174,1889,110,1079,243,779,1665,500,1811,1460,774,510,518,1571,102,1403,1270,1529,2001,786,533,1797,1651,944,186,1156,384,1089,727,645,1488,1056,1619,419,1389,107,1641,1390,1937,188,1860,1044,1259,647,159,1742,296,759,1275,2043,1060,2029,464,815,1427,132,1531,861,1662,777,653,2019,1471,846,202,1504,1305,151,247,2010,1017,46,110,289,1830,954,1149,1013,139,1838,591,551,1217,975,754,1343,1921,1271,579,420,899,56,1794,1249,417,1065,110,1766,907,1122,1161,1393,761,160,935,98,1512,78,1954,1520,1920,1051,493,532,1554,2034,731,1542,225,1223,882,1497,535,194,924,1311,270,927,1336,1694,631,824,1811,1435,495,1556,596,568,13,767,895,623,596,497,1918,1762,814,982,1157,1003,553,2027,376,1765,67,680,1533,1320,347,1950,1113,515,1185,1875,1834,988,907,638,113,1544,270,729,811,1814,43,1729,590,513,1000,1493,415,443,2000,1215,623,1982,1835,389,4,1458,1777,242,80,1225,1238,1058,15,1411,1937,1789,232,726,495,1148,1255,616,1635,1629,1600,719,111,1451,2034,333,233,1870,986,1744,1659,382,506,111,1247,1592,1047,57,2028,1026,680,356,1281,1178,1538,314,731,295,1742,692,564,12,1642,1019,1102,1784,1480,1368,366,1127,1966,1221,1656,2016,1814,438,71,924,28,970,1948,510,791,884,1702,1689,928,779,1896,1517,1511,1939,1045,1622,926,1739,800,1512,2000,1072,880,424,662,227,768,676,701,1673,1089,1067,1586,246,1281,710,685,1177,1769,407,1377,1847,408,611,1017,1823,1027,567,1094,1176,845,30,1439,1038,781,133,734,480,417,1667,806,110,768,1587,339,1868,654,675,418,1571,143,1946,324,587,1407,289,1049,1477,1902,1499,1536,834,490,1067,449,1725,1358,1672,1626,961,634,1408,2021,1577,1699,1446,1219,1542,1017,1772,139,1984,202,1120,825,219,1483,1257,1604,1160,1662,1181,1933,1785,1040,1616,969,1697,660,1999,570,149,1539,1702,1889,670,1647,1102,949,1681,1950,156,1451,481,817,549,1705,2039,1331,1442,1909,1408,1164,780,1300,744,835,755,959,1031,1865,423,693,1176,997,537,1187,1799,125,903,173,1154,1105,1573,477,1203,940,1763,1353,725,711,520,1951,611,324,740,1519,1446,1435,13,767,1846,652,1223,610,256,795,1675,2009,1765,172,692,484,651,2016,1123,1534,1724,342,1850,215,1526,1494,1469,273,913,1042,1086,1761,1388,1650,1088,1958,137,1109,577,985,1403,427,1246,668,1804,273,210,1964,313,312,319,183,931,1370,1703,1618,1460,968,1451,1962,1036,625,1618,484,480,952,1269,1260,251,254,583,1507,842,1698,1915,817,107,1170,1531,656,1010,454,1924,988,1106,453,196,123,1678,1580,401,1833,1401,1345,2028,69,869,1003,1195,171,1352,107,1140,1458,999,1265,438,1322,780,212,773,538,494,1276,1071,846,446,1758,1763,582,1260,1596,1956,1883,788,333,1632,233,325,729,1736,1194,1200,1896,1178,20,1658,1247,983,550,529,985,466,100,347,529,405,1240,1687,1660,1643,1450,1760,1600,417,2034,192,1332,441,499,696,405,1038,1947,808,1197,1242,1464,822,1759,1885,1538,585,1632,292,1033,1251,1441,785,1492,1817,114,283,1768,864,842,1964,1537,2009,478,197,788,1025,1168,2000,1009,821,2038,535,1363,1975,1375,613,1088,3,1810,828,617,1947,943,589,173,1192,313,150,1189,588,741,1484,1758,713,1970,1953,422,1573,1261,1882,1234,1438,1523,789,515,75,967,1321,571,1061,503,609,1332,1826,1764,430,817,1818,525,1980,1268,339,1692,1343,1119,515,82,478,740,1297,2020,424,1551,1815,967,325,1661,976,238,1793,1301,199,1860,72,81,2025,1405,771,1033,1564,100,1487,1436,940,1193,694,349,818,1884,463,739,665,171,1050,1706,1853,1482,784,610,1417,1803,1893,166,989,263,518,1810,402,249,1113,644,782,1621,1719,1393,854,1317,438,1264,1383,1231,709,746,351,1629,1009,726,1587,485,1730,1672,1733,758,785,1764,950,1168,1424,1968,1119,1304,1169,1899,1769,348,366,499,1540,408,2013,1453,359,1758,1862,1969,1169,1153,362,1052,1123,338,1011,1096,407,1276,54,653,1092,669,1233,741,276,1871,1312,1349,1383,1525,1725,1058,311,780,1681,351,676,1979,1651,455,2048,131,871,828,820,229,869,1257,2024,450,725,545,597,386,47,920,499,1779,1083,1872,1995,1199,244,1898,1216,1810,869,1244,145,1894,1315,214,1434,811,174,439,510,464,1440,1545,1121,1134,1291,2018,1299,1230,1862,1169,687,1960,901,1232,1475,1390,436,167,562,1777,1146,952,881,1585,1339,1347,330,886,1034,769,984,701,1592,786,1457,985,1493,1920,1059,1849,447,1788,169,953,45,1655,367,339,372,1416,438,611,1574,1026,1863,118,894,1172,1157,1687,258,615,4,1948,1569,1539,284,715,310,1017,1656,1296,1410,1310,1494,1761,1284,370,1174,335,1856,158,693,1189,973,1649,1087,466,1453,304,1348,1298,470,373,341,306,415,1956,33,1961,53,1989,609,1075,1766,1836,387,1353,1928,1998,221,366,1529,101,146,1002,1741,2042,9,1111,1764,1862,1731,1800,1528,241,1042,346,1702,1901,347,1810,794,784,556,1777,1519,917,1453,1934,357,501,1313,1656,1748,815,237,164,738,1698,440,1620,1341,54,1609,1889,1008,1708,269,1556,1896,1705,531,436,1070,814,981,2036,1238,1935,1004,897,1582,1524,907,109,180,1634,1343,66,1141,1474,226,444,1661,284,1806,1891,26,772,344,1106,208,80,1911,1990,739,1319,139,426,81,961,307,2030,875,1957,1483,1190,1106,1445,10,1603,1898,17,1689,1572,2042,466,1883,1315,216,549,1564,1650,214,962,449,1890,656,1756,532,1798,386,1555,65,1316,1161,771,435,1622,298,1002,26,382,994,1717,289,1500,1415,71,1001,1989,230,1522,1308,1217,1021,1163,873,156,595,1150,1297,1906,2002,192,1355,1234,970,730,974,1374,1965,182,1634,1210,1868,207,601,106,1032,1574,580,462,678,928,1510,1044,783,1854,1977,1287,270,1266,784,2030,587,1446,1096,396};
	//64 elems
	//int32_t array[] = {81,91,13,91,63,10,28,55,96,96,16,97,96,49,80,14,42,92,79,96,66, 4,85,93,68,76,74,39,66,17,71, 3,28, 5,10,82,69,32,95, 3,44,38,77,80,19,49,45,65,71,75,28,68,66,16,12,50,96,34,59,22,75,26,51,70};

	//set arg0 -> pointer to array
	//set arg3 -> length
	//set arg4 -> increasing=1; decreasing=0
    uint32_t arg0=0x00000260,
    		 arg1=0,
    		 arg2=0,
    		 arg3=length,
    		 arg4=direction,
    		 arg5=0,arg6=0,arg7=0;

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
        		arg0,0,0,0,arg1,0,0,0,arg2,0,
        		0,0,arg3,0,0,0,arg4,0,0,0,
        		arg5,0,0,0,arg6,0,0,0,arg7,0
        };//0-150 (151 valores)

    XIo_Out32(NEKO_RESET,0);
    XIo_Out32(NEKO_RESET,1);

    XIo_Out32(NEKO_RESET,0);
	XIo_Out32(NEKO_MEM_ACK, 0);
	XIo_Out32(NEKO_MEM_DONE,0);

    int bram_counter;

    //Initialization values for the bram
    for(bram_counter=0; bram_counter<=150; bram_counter++){
    	//dram[bram_counter] = ram_init[bram_counter];
    	XIo_Out32(NEKO_MEM_WR_ADDR,bram_counter*4);
    	XIo_Out32(NEKO_MEM_WR_DATA,ram_init[bram_counter]);
    	XIo_Out32(NEKO_MEM_DONE,0);
    	XIo_Out32(NEKO_MEM_DONE,1);
    	XIo_Out32(NEKO_MEM_DONE,0);
    }

    for(bram_counter=0; bram_counter<length; bram_counter++){
    	XIo_Out32(NEKO_MEM_WR_ADDR,arg0+bram_counter*4);
    	XIo_Out32(NEKO_MEM_WR_DATA,array[bram_counter]);
    	XIo_Out32(NEKO_MEM_DONE,0);
    	XIo_Out32(NEKO_MEM_DONE,1);
    	XIo_Out32(NEKO_MEM_DONE,0);
    	//dram[bram_counter] = res_matrix[bram_counter].u;
    }
    xil_printf("Initial array\n\r");
    for(bram_counter=0; bram_counter<length; bram_counter++){
    	XIo_Out32(NEKO_MEM_WR_ADDR,arg0+bram_counter*4);
    	xil_printf("%d,",XIo_In32(NEKO_MEM_RD_DATA));
    	//dram[bram_counter] = res_matrix[bram_counter].u;
    }
    xil_printf("\n\r\n\r");

    dram = (int *)XPAR_MIG_7SERIES_0_BASEADDR;
    num_cycles = 0;


//copy array to mem


    //numStages = 6;
    //stage = 1;
    kernel_calls_num = (length/2)/64;
    if((length/2)%64)
    	kernel_calls_num++;

      for(stage = 0; stage < numStages; ++stage) {
    	  arg1 = stage;
    	  ram_init[125] = arg1;
    	  XIo_Out32(NEKO_MEM_WR_ADDR,125*4);
    	  XIo_Out32(NEKO_MEM_WR_DATA,ram_init[125]);
    	  XIo_Out32(NEKO_MEM_DONE,0);
    	  XIo_Out32(NEKO_MEM_DONE,1);
    	  XIo_Out32(NEKO_MEM_DONE,0);


           //set arg1 -> stage
    	  //passOfStage = 1;
            for(passOfStage = 0; passOfStage < stage + 1; ++passOfStage) {
            	arg2 = passOfStage;
            	ram_init[129] = arg2;
            	XIo_Out32(NEKO_MEM_WR_ADDR,129*4);
            	XIo_Out32(NEKO_MEM_WR_DATA,ram_init[129]);
            	XIo_Out32(NEKO_MEM_DONE,0);
            	XIo_Out32(NEKO_MEM_DONE,1);
            	XIo_Out32(NEKO_MEM_DONE,0);
                   //set arg2 -> passOfStage


    				//num_threads = length/2;
    				//if num_threads > 64 -> run multiple times

                 //run kernel

            	//int32_t kernel_calls_num = 64/64, kernel_calls_counter = 0, num_threads;

            		//kernel_calls_num = kernel_calls_num < 1 ? 1 : kernel_calls_num;

            	for(kernel_calls_counter=0; kernel_calls_counter<kernel_calls_num; kernel_calls_counter++){
            		num_threads = length/2 - 64*kernel_calls_counter;
            		if(num_threads>64)
            			num_threads = 64;

            		//exec_lo = &inst_mem[1], exec_hi = &inst_mem[3];

            		*exec_lo = 0x01;
            		*exec_hi = 0x0;

            		for(counter=1; counter < num_threads; counter++){
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
            		data[12] = 0x000001E4;

            		data[15] = 0;
            		data[16] = 0;

            		num_cycles += run_vop_program_neko(inst_mem,inst_num, data, num_regs,vdata,vdata_numregs,2147483647);
            	}
            }
       }
    	xil_printf("\n\r\n\rTotal number of cycles: %ld\n\r", num_cycles);

        xil_printf("\n\r\n\rOrdered array\n\r");

    	for(bram_counter=0; bram_counter<length; bram_counter++){
    		XIo_Out32(NEKO_MEM_WR_ADDR,arg0+bram_counter*4);
    		xil_printf("%d,",XIo_In32(NEKO_MEM_RD_DATA));
    		//dram[bram_counter] = res_matrix[bram_counter].u;
    	}

    	xil_printf("\n\r");



    cleanup_platform();
    return 0;
}


