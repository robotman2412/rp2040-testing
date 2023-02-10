
#include <stdlib.h>
#include <elfloader.h>

typedef enum {
	R_ARM_PC24					= 1,	// Deprecated, Arm
	R_ARM_ABS32					= 2,	// Static, Data
	R_ARM_REL32					= 3,	// Static, Data
	R_ARM_LDR_PC_G0				= 4,	// Static, Arm
	R_ARM_ABS16					= 5,	// Static, Data
	R_ARM_ABS12					= 6,	// Static, Arm
	R_ARM_THM_ABS5				= 7,	// Static, Thumb16
	R_ARM_ABS8					= 8,	// Static, Data
	R_ARM_SBREL32				= 9,	// Static, Data
	R_ARM_THM_CALL				= 10,	// Static, Thumb32
	R_ARM_THM_PC8				= 11,	// Static, Thumb16
	R_ARM_BREL_ADJ				= 12,	// Dynamic, Data
	R_ARM_TLS_DTPMOD32			= 17,	// Dynamic, Data
	R_ARM_TLS_DTPOFF32			= 18,	// Dynamic, Data
	R_ARM_TLS_TPOFF32			= 19,	// Dynamic, Data
	R_ARM_GLOB_DAT				= 21,	// Dynamic, Data
	R_ARM_JUMP_SLOT				= 22,	// Dynamic, Data
	R_ARM_RELATIVE				= 23,	// Dynamic, Data
	R_ARM_GOTOFF32				= 24,	// Static, Data
	R_ARM_BASE_PREL				= 25,	// Static, Data
	R_ARM_GOT_BREL				= 26,	// Static, Data
	R_ARM_PLT32					= 27,	// Deprecated, Arm
	R_ARM_CALL					= 28,	// Static, Arm
	R_ARM_JUMP24				= 29,	// Static, Arm
	R_ARM_THM_JUMP24			= 30,	// Static, Thumb32
	R_ARM_BASE_ABS				= 31,	// Static, Data
	R_ARM_LDR_SBREL_11_0_NC		= 35,	// Deprecated, Arm
	R_ARM_ALU_SBREL_19_12_NC	= 36,	// Deprecated, Arm
	R_ARM_ALU_SBREL_27_20_CK	= 37,	// Deprecated, Arm
	R_ARM_TARGET1				= 38,	// Static, Miscellaneous
	R_ARM_SBREL31				= 39,	// Deprecated, Data
	R_ARM_PREL31				= 42,	// Static, Data
	R_ARM_MOVW_ABS_NC			= 43,	// Static, Arm
	R_ARM_MOVT_ABS				= 44,	// Static, Arm
	R_ARM_MOVW_PREL_NC			= 45,	// Static, Arm
	R_ARM_MOVT_PREL				= 46,	// Static, Arm
	R_ARM_THM_MOVW_ABS_NC		= 47,	// Static, Thumb32
	R_ARM_THM_MOVT_ABS			= 48,	// Static, Thumb32
	R_ARM_THM_MOVW_PREL_NC		= 49,	// Static, Thumb32
	R_ARM_THM_MOVT_PREL			= 50,	// Static, Thumb32
	R_ARM_THM_JUMP19			= 51,	// Static, Thumb32
	R_ARM_THM_JUMP6				= 52,	// Static, Thumb16
	R_ARM_THM_ALU_PREL_11_0		= 53,	// Static, Thumb32
	R_ARM_THM_PC12				= 54,	// Static, Thumb32
	R_ARM_ABS32_NOI				= 55,	// Static, Data
	R_ARM_REL32_NOI				= 56,	// Static, Data
	R_ARM_ALU_PC_G0_NC			= 57,	// Static, Arm
	R_ARM_ALU_PC_G0				= 58,	// Static, Arm
	R_ARM_ALU_PC_G1_NC			= 59,	// Static, Arm
	R_ARM_ALU_PC_G1				= 60,	// Static, Arm
	R_ARM_ALU_PC_G2				= 61,	// Static, Arm
	R_ARM_LDR_PC_G1				= 62,	// Static, Arm
	R_ARM_LDR_PC_G2				= 63,	// Static, Arm
	R_ARM_LDRS_PC_G0			= 64,	// Static, Arm
	R_ARM_LDRS_PC_G1			= 65,	// Static, Arm
	R_ARM_LDRS_PC_G2			= 66,	// Static, Arm
	R_ARM_LDC_PC_G0				= 67,	// Static, Arm
	R_ARM_LDC_PC_G1				= 68,	// Static, Arm
	R_ARM_LDC_PC_G2				= 69,	// Static, Arm
	R_ARM_ALU_SB_G0_NC			= 70,	// Static, Arm
	R_ARM_ALU_SB_G0				= 71,	// Static, Arm
	R_ARM_ALU_SB_G1_NC			= 72,	// Static, Arm
	R_ARM_ALU_SB_G1				= 73,	// Static, Arm
	R_ARM_ALU_SB_G2				= 74,	// Static, Arm
	R_ARM_LDR_SB_G0				= 75,	// Static, Arm
	R_ARM_LDR_SB_G1				= 76,	// Static, Arm
	R_ARM_LDR_SB_G2				= 77,	// Static, Arm
	R_ARM_LDRS_SB_G0			= 78,	// Static, Arm
	R_ARM_LDRS_SB_G1			= 79,	// Static, Arm
	R_ARM_LDRS_SB_G2			= 80,	// Static, Arm
	R_ARM_LDC_SB_G0				= 81,	// Static, Arm
	R_ARM_LDC_SB_G1				= 82,	// Static, Arm
	R_ARM_LDC_SB_G2				= 83,	// Static, Arm
	R_ARM_MOVW_BREL_NC			= 84,	// Static, Arm
	R_ARM_MOVT_BREL				= 85,	// Static, Arm
	R_ARM_MOVW_BREL				= 86,	// Static, Arm
	R_ARM_THM_MOVW_BREL_NC		= 87,	// Static, Thumb32
	R_ARM_THM_MOVT_BREL			= 88,	// Static, Thumb32
	R_ARM_THM_MOVW_BREL			= 89,	// Static, Thumb32
	R_ARM_PLT32_ABS				= 94,	// Static, Data
	R_ARM_GOT_ABS				= 95,	// Static, Data
	R_ARM_GOT_PREL				= 96,	// Static, Data
	R_ARM_GOT_BREL12			= 97,	// Static, Arm
	R_ARM_GOTOFF12				= 98,	// Static, Arm
	R_ARM_THM_JUMP11			= 102,	// Static, Thumb16
	R_ARM_THM_JUMP8				= 103,	// Static, Thumb16
	R_ARM_TLS_GD32				= 104,	// Static, Data
	R_ARM_TLS_LDM32				= 105,	// Static, Data
	R_ARM_TLS_LDO32				= 106,	// Static, Data
	R_ARM_TLS_IE32				= 107,	// Static, Data
	R_ARM_TLS_LE32				= 108,	// Static, Data
	R_ARM_TLS_LDO12				= 109,	// Static, Arm
	R_ARM_TLS_LE12				= 110,	// Static, Arm
	R_ARM_TLS_IE12GP			= 111,	// Static, Arm
	R_ARM_THM_GOT_BREL12		= 131,	// Static, Thumb32
	R_ARM_THM_ALU_ABS_G0_NC		= 132,	// Static, Thumb16
	R_ARM_THM_ALU_ABS_G1_NC		= 133,	// Static, Thumb16
	R_ARM_THM_ALU_ABS_G2_NC		= 134,	// Static, Thumb16
	R_ARM_THM_ALU_ABS_G3		= 135,	// Static, Thumb16
	R_ARM_THM_BF16				= 136,	// Static, Arm
	R_ARM_THM_BF12				= 137,	// Static, Arm
	R_ARM_THM_BF18				= 138,	// Static, Arm
} arm_rel_t;

uint32_t sign_extend(uint bits, uint32_t raw) {
	if ((raw >> (bits-1)) & 1) {
		raw |= UINT32_MAX - (1 << bits) + 1;
	}
	return raw;
}

static uint32_t read32le(uint8_t *location) {
	return ((location)[0] | ((location)[1] << 8) | ((location)[2] << 16) | ((location)[3] << 24));
}

static uint32_t read16le(uint8_t *location) {
	return ((location)[0] | ((location)[1] << 8));
}

static uint32_t ror32() {
	
}

uint32_t elf_rel_get_addend(elf_reloc_t *reloc, uint8_t *location) {
	
	switch ((arm_rel_t) reloc->type) {
		case R_ARM_ABS32:
		case R_ARM_BASE_PREL:
		case R_ARM_GLOB_DAT:
		case R_ARM_GOTOFF32:
		case R_ARM_GOT_BREL:
		case R_ARM_GOT_PREL:
		case R_ARM_REL32:
		case R_ARM_RELATIVE:
		case R_ARM_SBREL32:
		case R_ARM_TARGET1:
		case R_ARM_TLS_DTPMOD32:
		case R_ARM_TLS_DTPOFF32:
		case R_ARM_TLS_GD32:
		case R_ARM_TLS_IE32:
		case R_ARM_TLS_LDM32:
		case R_ARM_TLS_LE32:
		case R_ARM_TLS_LDO32:
		case R_ARM_TLS_TPOFF32:
			return read32le(location);
		case R_ARM_PREL31:
			return sign_extend(31, read32le(location));
		case R_ARM_CALL:
		case R_ARM_JUMP24:
		case R_ARM_PC24:
		case R_ARM_PLT32:
			return sign_extend(26, read32le(location) << 2);
		case R_ARM_THM_JUMP8:
			return sign_extend(9,  read16le(location) << 1);
		case R_ARM_THM_JUMP11:
			return sign_extend(12, read16le(location) << 1);
		case R_ARM_THM_JUMP19: {
			uint16_t hi = read16le(location);
			uint16_t lo = read16le(location + 2);
			return sign_extend(
				20,
				((hi & 0x0400) << 10) | // S
				((lo & 0x0800) <<  8) | // J2
				((lo & 0x2000) <<  5) | // J1
				((hi & 0x003f) << 12) | // imm6
				((lo & 0x07ff) <<  1)   // imm11:0
			);
		}
		case R_ARM_THM_CALL:
		case R_ARM_THM_JUMP24: {
			uint16_t hi = read16le(location);
			uint16_t lo = read16le(location + 2);
			return sign_extend(
				24,
				((hi & 0x0400)  << 14) |                 // S
				(~((lo ^ (hi << 3)) << 10) & 0x800000) | // I1
				(~((lo ^ (hi << 1)) << 11) & 0x400000) | // I2
				((hi & 0x003ff) << 12) |                 // imm0
				((lo & 0x007ff) <<  1)                   // imm11:0
			);
		}
		case R_ARM_MOVW_ABS_NC:
		case R_ARM_MOVT_ABS:
		case R_ARM_MOVW_PREL_NC:
		case R_ARM_MOVT_PREL:
		case R_ARM_MOVW_BREL_NC:
		case R_ARM_MOVT_BREL: {
			uint32_t val = read32le(location);
			return sign_extend(16,
				((val & 0x000f0000) >> 4) |
				 (val & 0x00000fff)
			);
		}
		case R_ARM_THM_MOVW_ABS_NC:
		case R_ARM_THM_MOVT_ABS:
		case R_ARM_THM_MOVW_PREL_NC:
		case R_ARM_THM_MOVT_PREL:
		case R_ARM_THM_MOVW_BREL_NC:
		case R_ARM_THM_MOVT_BREL: {
			uint16_t hi = read16le(location);
			uint16_t lo = read16le(location + 2);
			return sign_extend(16,
				((hi & 0x000f) << 12) | // imm4
				((hi & 0x0400) <<  1) | // i
				((lo & 0x7000) >>  4) | // imm3
				 (lo & 0x00ff)          // imm8
			);
		}
		case R_ARM_ALU_PC_G0:
		case R_ARM_ALU_PC_G0_NC:
		case R_ARM_ALU_PC_G1:
		case R_ARM_ALU_PC_G1_NC:
		case R_ARM_ALU_PC_G2: {
			// TODO
			return 0;
			// uint32_t instr = read32le(location);
			// uint32_t val   = ror32   (instr & 0xff, ((instr & 0xf00) >> 8) * 2);
			// return (instr & 0x00400000) ? -val : val;
		}
		case R_ARM_LDR_PC_G0:
		case R_ARM_LDR_PC_G1:
		case R_ARM_LDR_PC_G2: {
			uint32_t raw = read32le(location);
			bool     u   = raw & 0x00800000;
			uint32_t imm = raw & 0x00000fff;
			return u ? imm : -imm;
		}
		case R_ARM_LDRS_PC_G0:
		case R_ARM_LDRS_PC_G1:
		case R_ARM_LDRS_PC_G2: {
			uint32_t opcode = read32le(location);
			bool     u      =  opcode & 0x00800000;
			uint32_t imm4l  =  opcode & 0xf;
			uint32_t imm4h  = (opcode & 0xf00) >> 4;
			return u ? (imm4h | imm4l) : -(imm4h | imm4l);
		}
		case R_ARM_THM_ALU_PREL_11_0: {
			uint16_t hi  = read16le(location);
			uint16_t lo  = read16le(location + 2);
			uint32_t imm = (hi & 0x0400) << 1 | // i
						   (lo & 0x7000) >> 4 | // imm3
						   (lo & 0x00ff);       // imm8
			return (hi & 0x00f0) ? -imm : imm;
		}
		case R_ARM_THM_PC8:
			// ADR and LDR (literal) encoding T1
			// From ELF for the ARM Architecture the initial signed addend is formed
			// from an unsigned field using expression (((imm8:00 + 4) & 0x3ff) – 4)
			// this trick permits the PC bias of -4 to be encoded using imm8 = 0xff
			return ((((read16le(location) & 0xff) << 2) + 4) & 0x3ff) - 4;
		case R_ARM_THM_PC12: {
			// LDR (literal) encoding T2, add = (U == '1') imm12
			bool     u   = read16le(location) & 0x0080;
			uint32_t imm = read16le(location + 2) & 0x0fff;
			return u ? imm : -imm;
		}
		
		default:
			return 0;
	}
}

static void write32le(uint8_t *location, uint32_t value) {
	location[0] = value;
	location[1] = value >>  8;
	location[2] = value >> 16;
	location[3] = value >> 24;
}

static void write16le(uint8_t *location, uint32_t value) {
	location[0] = value;
	location[1] = value >> 8;
}

static void merge32le(uint8_t *location, uint32_t value, uint32_t replace_bits) {
	location[0] = (location[0] & ~ replace_bits       ) | (value &  replace_bits);
	location[1] = (location[1] & ~(replace_bits >>  8)) | (value & (replace_bits >>  8));
	location[2] = (location[2] & ~(replace_bits >> 16)) | (value & (replace_bits >> 16));
	location[3] = (location[3] & ~(replace_bits >> 24)) | (value & (replace_bits >> 24));
}

static void merge16le(uint8_t *location, uint16_t value, uint16_t replace_bits) {
	location[0] = (location[0] & ~ replace_bits       ) | (value &  replace_bits);
	location[1] = (location[1] & ~(replace_bits >>  8)) | (value & (replace_bits >>  8));
}

void elf_rel_apply_value(elf_reloc_t *reloc, uint8_t *location, uint32_t value) {
	
	switch ((arm_rel_t) reloc->type) {
		case R_ARM_ABS32:
		case R_ARM_BASE_PREL:
		case R_ARM_GLOB_DAT:
		case R_ARM_GOTOFF32:
		case R_ARM_GOT_BREL:
		case R_ARM_GOT_PREL:
		case R_ARM_REL32:
		case R_ARM_RELATIVE:
		case R_ARM_SBREL32:
		case R_ARM_TARGET1:
		case R_ARM_TLS_DTPMOD32:
		case R_ARM_TLS_DTPOFF32:
		case R_ARM_TLS_GD32:
		case R_ARM_TLS_IE32:
		case R_ARM_TLS_LDM32:
		case R_ARM_TLS_LE32:
		case R_ARM_TLS_LDO32:
		case R_ARM_TLS_TPOFF32:
			write32le(location, value);
			break;
			
		case R_ARM_PREL31:
			merge32le(location, value, 0x7fffffff);
			break;
			
		case R_ARM_CALL:
		case R_ARM_JUMP24:
		case R_ARM_PC24:
		case R_ARM_PLT32:
			merge32le(location, value >> 2, 0x03ffffff);
			break;
			
		// TODO: Some stuff in between here.
		
		case R_ARM_THM_CALL:
		case R_ARM_THM_JUMP24: {
			uint16_t hi = 0, lo = 0;
			hi |= (value >> 14) & 0x0400;
			
			lo |= (~value & 0x800000) >> 10;
			// (~(lo << 10) & 0x800000); // I1
			// (~((lo ^ (hi << 3)) << 10) & 0x800000); // I1
			
			lo |= (~value & 0x400000) >> 11;
			// (~(lo << 11) & 0x400000); // I2
			// (~((lo ^ (hi << 1)) << 11) & 0x400000); // I2
			
			hi |= (value >> 12) & 0x003ff;
			lo |= (value >>  1) & 0x007ff;
			
			uint16_t mask_lo = 0x2fff;
			uint16_t mask_hi = 0x07ff;
			
			merge16le(location, hi, mask_hi);
			merge16le(location, lo, mask_lo);
			
		} break;
			
		default:
			printf("Error: Don't know how to apply relocation type %d\n", reloc->type);
	}
}

uint32_t elf_resolve_rel(elf_reloc_t *reloc, uint32_t got_address, uint32_t raw) {
	elf_load_sym_t *sym = &reloc->ctx->symbols[reloc->symbol];
	
	#define A		reloc->addend
	#define S		sym->vaddr
	#define P		(reloc->offset)
	#define Pa		(P & 0xfffffffc)
	#define T		((sym->parent->info & 0x0f == 0x02) && (sym->vaddr & 1))
	#define B_S		got_address
	#define GOT_ORG	got_address
	#define GOT_S	sym->got_addr
	
	switch ((arm_rel_t) reloc->type) {
		case R_ARM_PC24:				return ((S + A) | T) - P;
		case R_ARM_ABS32:				return (S + A) | T;
		case R_ARM_REL32:				return ((S + A) | T) - P;
		case R_ARM_LDR_PC_G0:			return S + A - P;
		case R_ARM_ABS16:				return S + A;
		case R_ARM_ABS12:				return S + A;
		case R_ARM_THM_ABS5:			return S + A;
		case R_ARM_ABS8:				return S + A;
//		case R_ARM_SBREL32:				return ((S + A) | T) - B_S;
		case R_ARM_THM_CALL:			return ((S + A) | T) - P;
		case R_ARM_THM_PC8:				return S + A - Pa;
//		case R_ARM_BREL_ADJ:			return ChangeIn[B_S] + A;
//		case R_ARM_TLS_DTPMOD32:		return Module[S];
//		case R_ARM_TLS_DTPOFF32:		return S + A - TLS;
//		case R_ARM_TLS_TPOFF32:			return S + A - tp;
		case R_ARM_GLOB_DAT:			return (S + A) | T;
		case R_ARM_JUMP_SLOT:			return (S + A) | T;
//		case R_ARM_RELATIVE:			return B_S + A; // Note: see Dynamic relocations
		case R_ARM_GOTOFF32:			return ((S + A) | T) - GOT_ORG;
		case R_ARM_BASE_PREL:			return B_S + A - P;
		case R_ARM_GOT_BREL:			return GOT_S + A - GOT_ORG;
		case R_ARM_PLT32:				return ((S + A) | T) - P;
		case R_ARM_CALL:				return ((S + A) | T) - P;
		case R_ARM_JUMP24:				return ((S + A) | T) - P;
		case R_ARM_THM_JUMP24:			return ((S + A) | T) - P;
//		case R_ARM_BASE_ABS:			return B_S + A;
//		case R_ARM_LDR_SBREL_11_0_NC:	return S + A - B_S;
//		case R_ARM_ALU_SBREL_19_12_NC:	return S + A - B_S;
//		case R_ARM_ALU_SBREL_27_20_CK:	return S + A - B_S;
//		case R_ARM_TARGET1:				return (S + A) | T or ((S + | A) | T) - P;
//		case R_ARM_SBREL31:				return ((S + A) | T) - B_S;
		case R_ARM_PREL31:				return ((S + A) | T) - P;
		case R_ARM_MOVW_ABS_NC:			return (S + A) | T;
		case R_ARM_MOVT_ABS:			return S + A;
		case R_ARM_MOVW_PREL_NC:		return ((S + A) | T) - P;
		case R_ARM_MOVT_PREL:			return S + A - P;
		case R_ARM_THM_MOVW_ABS_NC:		return (S + A) | T;
		case R_ARM_THM_MOVT_ABS:		return S + A;
		case R_ARM_THM_MOVW_PREL_NC:	return ((S + A) | T) - P;
		case R_ARM_THM_MOVT_PREL:		return S + A - P;
		case R_ARM_THM_JUMP19:			return ((S + A) | T) - P;
		case R_ARM_THM_JUMP6:			return S + A - P;
		case R_ARM_THM_ALU_PREL_11_0:	return ((S + A) | T) - Pa;
		case R_ARM_THM_PC12:			return S + A - Pa;
		case R_ARM_ABS32_NOI:			return S + A;
		case R_ARM_REL32_NOI:			return S + A - P;
		case R_ARM_ALU_PC_G0_NC:		return ((S + A) | T) - P;
		case R_ARM_ALU_PC_G0:			return ((S + A) | T) - P;
		case R_ARM_ALU_PC_G1_NC:		return ((S + A) | T) - P;
		case R_ARM_ALU_PC_G1:			return ((S + A) | T) - P;
		case R_ARM_ALU_PC_G2:			return ((S + A) | T) - P;
		case R_ARM_LDR_PC_G1:			return S + A - P;
		case R_ARM_LDR_PC_G2:			return S + A - P;
		case R_ARM_LDRS_PC_G0:			return S + A - P;
		case R_ARM_LDRS_PC_G1:			return S + A - P;
		case R_ARM_LDRS_PC_G2:			return S + A - P;
		case R_ARM_LDC_PC_G0:			return S + A - P;
		case R_ARM_LDC_PC_G1:			return S + A - P;
		case R_ARM_LDC_PC_G2:			return S + A - P;
//		case R_ARM_ALU_SB_G0_NC:		return ((S + A) | T) - B_S;
//		case R_ARM_ALU_SB_G0:			return ((S + A) | T) - B_S;
//		case R_ARM_ALU_SB_G1_NC:		return ((S + A) | T) - B_S;
//		case R_ARM_ALU_SB_G1:			return ((S + A) | T) - B_S;
//		case R_ARM_ALU_SB_G2:			return ((S + A) | T) - B_S;
//		case R_ARM_LDR_SB_G0:			return S + A - B_S;
//		case R_ARM_LDR_SB_G1:			return S + A - B_S;
//		case R_ARM_LDR_SB_G2:			return S + A - B_S;
//		case R_ARM_LDRS_SB_G0:			return S + A - B_S;
//		case R_ARM_LDRS_SB_G1:			return S + A - B_S;
//		case R_ARM_LDRS_SB_G2:			return S + A - B_S;
//		case R_ARM_LDC_SB_G0:			return S + A - B_S;
//		case R_ARM_LDC_SB_G1:			return S + A - B_S;
//		case R_ARM_LDC_SB_G2:			return S + A - B_S;
//		case R_ARM_MOVW_BREL_NC:		return ((S + A) | T) - B_S;
//		case R_ARM_MOVT_BREL:			return S + A - B_S;
//		case R_ARM_MOVW_BREL:			return ((S + A) | T) - B_S;
//		case R_ARM_THM_MOVW_BREL_NC:	return ((S + A) | T) - B_S;
//		case R_ARM_THM_MOVT_BREL:		return S + A - B_S;
//		case R_ARM_THM_MOVW_BREL:		return ((S + A) | T) - B_S;
//		case R_ARM_PLT32_ABS:			return PLT(S) + A;
		case R_ARM_GOT_ABS:				return GOT_S + A;
		case R_ARM_GOT_PREL:			return GOT_S + A - P;
		case R_ARM_GOT_BREL12:			return GOT_S + A - GOT_ORG;
		case R_ARM_GOTOFF12:			return S + A - GOT_ORG;
		case R_ARM_THM_JUMP11:			return S + A - P;
		case R_ARM_THM_JUMP8:			return S + A - P;
		case R_ARM_TLS_GD32:			return GOT_S + A - P;
		case R_ARM_TLS_LDM32:			return GOT_S + A - P;
//		case R_ARM_TLS_LDO32:			return S + A - TLS;
		case R_ARM_TLS_IE32:			return GOT_S + A - P;
//		case R_ARM_TLS_LE32:			return S + A - tp;
//		case R_ARM_TLS_LDO12:			return S + A - TLS;
//		case R_ARM_TLS_LE12:			return S + A - tp;
		case R_ARM_TLS_IE12GP:			return GOT_S + A - GOT_ORG;
		case R_ARM_THM_GOT_BREL12:		return GOT_S + A - GOT_ORG;
		case R_ARM_THM_ALU_ABS_G0_NC:	return (S + A) | T;
		case R_ARM_THM_ALU_ABS_G1_NC:	return S + A;
		case R_ARM_THM_ALU_ABS_G2_NC:	return S + A;
		case R_ARM_THM_ALU_ABS_G3:		return S + A;
		case R_ARM_THM_BF16:			return ((S + A) | T) - P;
		case R_ARM_THM_BF12:			return ((S + A) | T) - P;
		case R_ARM_THM_BF18:			return ((S + A) | T) - P;
	}
	return 0;
}

