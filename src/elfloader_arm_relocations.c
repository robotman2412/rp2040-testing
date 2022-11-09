
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

uint32_t elf_resolve_rel(elf_reloc_t *reloc, uint32_t got_address, uint32_t raw) {
	elf_load_sym_t *sym;
	
	#define A		reloc->addend
	#define S		sym->vaddr
	#define P		(reloc->target->vaddr + reloc->offset)
	#define Pa		(P & 0xfffffffc)
	#define T		((sym->parent->info & 0x0f == 0x02) && (sym->vaddr & 1))
	// TODO B_S		???
	#define GOT_ORG	got_address
	#define GOT_S	sym->got_addr
	
	switch (reloc->type) {
		case R_ARM_PC24:				return ((S + A) | T) - P;
		case R_ARM_ABS32:				return (S + A) | T;
		case R_ARM_REL32:				return ((S + A) | T) - P; // ((S + A) | T) | - P
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
//		case R_ARM_BASE_PREL:			return B_S + A - P;
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

