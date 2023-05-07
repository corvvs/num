#include "nm.h"

// 「引数が指定したマクロのいずれかに一致する場合、一致したマクロの名前の文字列を返す関数」を生成するワンライナー
// echo "VALUE_A, VALUE_B, VALUE_C" | ruby -e 'NS = gets.chomp; puts "const char* xxx(int value) {"; puts "\tstatic const char* names[] = (const char* []){#{NS.split(/\s*,\s*/).map{ |s| "\"#{s}\"" }.join(", ")}};"; puts "\tstatic const int values[] = (int []){ #{NS} };"; puts "\tfor (size_t i = 0; i < sizeof(names) / sizeof(char *); ++i) { if (value == values[i]) { return names[i]; } }"; puts "\treturn \"UNKNOWN\";"; puts "}"; ' | pbcopy 

const char*	objtype_to_name(int value) {
	static const char* names[] = (const char* []){"ET_NONE", "ET_REL", "ET_EXEC", "ET_DYN", "ET_CORE", "ET_LOPROC", "ET_HIPROC"};
	static const int values[] = (int []){ ET_NONE, ET_REL, ET_EXEC, ET_DYN, ET_CORE, ET_LOPROC, ET_HIPROC };
	for (size_t i = 0; i < sizeof(names) / sizeof(char *); ++i) { if (value == values[i]) { return names[i]; } }
	return "UNKNOWN";
}

const char*	machinetype_to_name(int value) {
	static const char* names[] = (const char* []){"EM_NONE", "EM_M32", "EM_SPARC", "EM_386", "EM_68K", "EM_88K", "EM_IAMCU", "EM_860", "EM_MIPS", "EM_S370", "EM_MIPS_RS3_LE", "EM_PARISC", "EM_VPP500", "EM_SPARC32PLUS", "EM_960", "EM_PPC", "EM_PPC64", "EM_S390", "EM_SPU", "EM_V800", "EM_FR20", "EM_RH32", "EM_RCE", "EM_ARM", "EM_FAKE_ALPHA", "EM_SH", "EM_SPARCV9", "EM_TRICORE", "EM_ARC", "EM_H8_300", "EM_H8_300H", "EM_H8S", "EM_H8_500", "EM_IA_64", "EM_MIPS_X", "EM_COLDFIRE", "EM_68HC12", "EM_MMA", "EM_PCP", "EM_NCPU", "EM_NDR1", "EM_STARCORE", "EM_ME16", "EM_ST100", "EM_TINYJ", "EM_X86_64", "EM_PDSP", "EM_PDP10", "EM_PDP11", "EM_FX66", "EM_ST9PLUS", "EM_ST7", "EM_68HC16", "EM_68HC11", "EM_68HC08", "EM_68HC05", "EM_SVX", "EM_ST19", "EM_VAX", "EM_CRIS", "EM_JAVELIN", "EM_FIREPATH", "EM_ZSP", "EM_MMIX", "EM_HUANY", "EM_PRISM", "EM_AVR", "EM_FR30", "EM_D10V", "EM_D30V", "EM_V850", "EM_M32R", "EM_MN10300", "EM_MN10200", "EM_PJ", "EM_OPENRISC", "EM_ARC_COMPACT", "EM_XTENSA", "EM_VIDEOCORE", "EM_TMM_GPP", "EM_NS32K", "EM_TPC", "EM_SNP1K", "EM_ST200", "EM_IP2K", "EM_MAX", "EM_CR", "EM_F2MC16", "EM_MSP430", "EM_BLACKFIN", "EM_SE_C33", "EM_SEP", "EM_ARCA", "EM_UNICORE", "EM_EXCESS", "EM_DXP", "EM_ALTERA_NIOS2", "EM_CRX", "EM_XGATE", "EM_C166", "EM_M16C", "EM_DSPIC30F", "EM_CE", "EM_M32C", "EM_TSK3000", "EM_RS08", "EM_SHARC", "EM_ECOG2", "EM_SCORE7", "EM_DSP24", "EM_VIDEOCORE3", "EM_LATTICEMICO32", "EM_SE_C17", "EM_TI_C6000", "EM_TI_C2000", "EM_TI_C5500", "EM_TI_ARP32", "EM_TI_PRU", "EM_MMDSP_PLUS", "EM_CYPRESS_M8C", "EM_R32C", "EM_TRIMEDIA", "EM_QDSP6", "EM_8051", "EM_STXP7X", "EM_NDS32", "EM_ECOG1X", "EM_MAXQ30", "EM_XIMO16", "EM_MANIK", "EM_CRAYNV2", "EM_RX", "EM_METAG", "EM_MCST_ELBRUS", "EM_ECOG16", "EM_CR16", "EM_ETPU", "EM_SLE9X", "EM_L10M", "EM_K10M", "EM_AARCH64", "EM_AVR32", "EM_STM8", "EM_TILE64", "EM_TILEPRO", "EM_MICROBLAZE", "EM_CUDA", "EM_TILEGX", "EM_CLOUDSHIELD", "EM_COREA_1ST", "EM_COREA_2ND", "EM_ARCV2", "EM_OPEN8", "EM_RL78", "EM_VIDEOCORE5", "EM_78KOR", "EM_56800EX", "EM_BA1", "EM_BA2", "EM_XCORE", "EM_MCHP_PIC", "EM_INTELGT", "EM_KM32", "EM_KMX32", "EM_EMX16", "EM_EMX8", "EM_KVARC", "EM_CDP", "EM_COGE", "EM_COOL", "EM_NORC", "EM_CSR_KALIMBA", "EM_Z80", "EM_VISIUM", "EM_FT32", "EM_MOXIE", "EM_AMDGPU", "EM_RISCV", "EM_BPF", "EM_CSKY"};
	static const int values[] = (int []){ EM_NONE, EM_M32, EM_SPARC, EM_386, EM_68K, EM_88K, EM_IAMCU, EM_860, EM_MIPS, EM_S370, EM_MIPS_RS3_LE, EM_PARISC, EM_VPP500, EM_SPARC32PLUS, EM_960, EM_PPC, EM_PPC64, EM_S390, EM_SPU, EM_V800, EM_FR20, EM_RH32, EM_RCE, EM_ARM, EM_FAKE_ALPHA, EM_SH, EM_SPARCV9, EM_TRICORE, EM_ARC, EM_H8_300, EM_H8_300H, EM_H8S, EM_H8_500, EM_IA_64, EM_MIPS_X, EM_COLDFIRE, EM_68HC12, EM_MMA, EM_PCP, EM_NCPU, EM_NDR1, EM_STARCORE, EM_ME16, EM_ST100, EM_TINYJ, EM_X86_64, EM_PDSP, EM_PDP10, EM_PDP11, EM_FX66, EM_ST9PLUS, EM_ST7, EM_68HC16, EM_68HC11, EM_68HC08, EM_68HC05, EM_SVX, EM_ST19, EM_VAX, EM_CRIS, EM_JAVELIN, EM_FIREPATH, EM_ZSP, EM_MMIX, EM_HUANY, EM_PRISM, EM_AVR, EM_FR30, EM_D10V, EM_D30V, EM_V850, EM_M32R, EM_MN10300, EM_MN10200, EM_PJ, EM_OPENRISC, EM_ARC_COMPACT, EM_XTENSA, EM_VIDEOCORE, EM_TMM_GPP, EM_NS32K, EM_TPC, EM_SNP1K, EM_ST200, EM_IP2K, EM_MAX, EM_CR, EM_F2MC16, EM_MSP430, EM_BLACKFIN, EM_SE_C33, EM_SEP, EM_ARCA, EM_UNICORE, EM_EXCESS, EM_DXP, EM_ALTERA_NIOS2, EM_CRX, EM_XGATE, EM_C166, EM_M16C, EM_DSPIC30F, EM_CE, EM_M32C, EM_TSK3000, EM_RS08, EM_SHARC, EM_ECOG2, EM_SCORE7, EM_DSP24, EM_VIDEOCORE3, EM_LATTICEMICO32, EM_SE_C17, EM_TI_C6000, EM_TI_C2000, EM_TI_C5500, EM_TI_ARP32, EM_TI_PRU, EM_MMDSP_PLUS, EM_CYPRESS_M8C, EM_R32C, EM_TRIMEDIA, EM_QDSP6, EM_8051, EM_STXP7X, EM_NDS32, EM_ECOG1X, EM_MAXQ30, EM_XIMO16, EM_MANIK, EM_CRAYNV2, EM_RX, EM_METAG, EM_MCST_ELBRUS, EM_ECOG16, EM_CR16, EM_ETPU, EM_SLE9X, EM_L10M, EM_K10M, EM_AARCH64, EM_AVR32, EM_STM8, EM_TILE64, EM_TILEPRO, EM_MICROBLAZE, EM_CUDA, EM_TILEGX, EM_CLOUDSHIELD, EM_COREA_1ST, EM_COREA_2ND, EM_ARCV2, EM_OPEN8, EM_RL78, EM_VIDEOCORE5, EM_78KOR, EM_56800EX, EM_BA1, EM_BA2, EM_XCORE, EM_MCHP_PIC, EM_INTELGT, EM_KM32, EM_KMX32, EM_EMX16, EM_EMX8, EM_KVARC, EM_CDP, EM_COGE, EM_COOL, EM_NORC, EM_CSR_KALIMBA, EM_Z80, EM_VISIUM, EM_FT32, EM_MOXIE, EM_AMDGPU, EM_RISCV, EM_BPF, EM_CSKY };
	for (size_t i = 0; i < sizeof(names) / sizeof(char *); ++i) { if (value == values[i]) { return names[i]; } }
	return "UNKNOWN";
}

const char* elfclass_to_name(int value) {
	static const char* names[] = (const char* []){"ELFCLASSNONE", "ELFCLASS32", "ELFCLASS64"};
	static const int values[] = (int []){ ELFCLASSNONE, ELFCLASS32, ELFCLASS64 };
	for (size_t i = 0; i < sizeof(names) / sizeof(char *); ++i) { if (value == values[i]) { return names[i]; } }
	return "UNKNOWN";
}

const char* sectiontype_to_name(int value) {
	static const char* names[] = (const char* []){"SHT_DYNAMIC", "SHT_DYNSYM", "SHT_FINI_ARRAY", "SHT_HASH", "SHT_HIPROC", "SHT_HIUSER", "SHT_INIT_ARRAY", "SHT_LOPROC", "SHT_LOUSER", "SHT_NOBITS", "SHT_NOTE", "SHT_NULL", "SHT_PREINIT_ARRAY", "SHT_PROGBITS", "SHT_REL", "SHT_RELA", "SHT_SHLIB", "SHT_STRTAB", "SHT_SYMTAB", "SHT_GNU_verdef", "SHT_GNU_verneed", "SHT_GNU_versym", "SHT_GNU_HASH"};
	static const int values[] = (int []){ SHT_DYNAMIC, SHT_DYNSYM, SHT_FINI_ARRAY, SHT_HASH, SHT_HIPROC, SHT_HIUSER, SHT_INIT_ARRAY, SHT_LOPROC, SHT_LOUSER, SHT_NOBITS, SHT_NOTE, SHT_NULL, SHT_PREINIT_ARRAY, SHT_PROGBITS, SHT_REL, SHT_RELA, SHT_SHLIB, SHT_STRTAB, SHT_SYMTAB, SHT_GNU_verdef, SHT_GNU_verneed, SHT_GNU_versym, SHT_GNU_HASH };
	for (size_t i = 0; i < sizeof(names) / sizeof(char *); ++i) { if (value == values[i]) { return names[i]; } }
	return "UNKNOWN";
}

const char* symbinding_to_name(int value) {
	static const char* names[] = (const char* []){"STB_LOCAL", "STB_GLOBAL", "STB_WEAK", "STB_LOOS", "STB_HIOS", "STB_LOPROC", "STB_HIPROC"};
	static const int values[] = (int []){ STB_LOCAL, STB_GLOBAL, STB_WEAK, STB_LOOS, STB_HIOS, STB_LOPROC, STB_HIPROC };
	for (size_t i = 0; i < sizeof(names) / sizeof(char *); ++i) { if (value == values[i]) { return names[i]; } }
	return "UNKNOWN";
}

const char* symtype_to_name(int value) {
	static const char* names[] = (const char* []){"STT_NOTYPE", "STT_OBJECT", "STT_FUNC", "STT_SECTION", "STT_FILE", "STT_COMMON", "STT_TLS", "STT_LOOS", "STT_HIOS", "STT_LOPROC", "STT_HIPROC"};
	static const int values[] = (int []){ STT_NOTYPE, STT_OBJECT, STT_FUNC, STT_SECTION, STT_FILE, STT_COMMON, STT_TLS, STT_LOOS, STT_HIOS, STT_LOPROC, STT_HIPROC };
	for (size_t i = 0; i < sizeof(names) / sizeof(char *); ++i) { if (value == values[i]) { return names[i]; } }
	return "UNKNOWN";
}

const char* section_category_to_name(uint64_t value) {
	static const char* names[] = (const char* []){"SC_SYM_ABS", "SC_SYM_COMMON", "SC_SYM_UNDEF", "SC_GOT", "SC_DATA", "SC_READONLY", "SC_SMALL", "SC_TEXT", "SC_BSS", "SC_UNDEFINED", "SC_OTHER"};
	static const uint64_t values[] = (uint64_t []){ SC_SYM_ABS, SC_SYM_COMMON, SC_SYM_UNDEF, SC_GOT, SC_DATA, SC_READONLY, SC_SMALL, SC_TEXT, SC_BSS, SC_UNDEFINED, SC_OTHER };
	for (size_t i = 0; i < sizeof(names) / sizeof(char *); ++i) { if (value == values[i]) { return names[i]; } }
	return "UNKNOWN";
}

const char* symbol_visibility_to_name(uint64_t value) {
	static const char* names[] = (const char* []){"STV_DEFAULT", "STV_INTERNAL", "STV_HIDDEN", "STV_PROTECTED"};
	static const uint64_t values[] = (uint64_t []){ STV_DEFAULT, STV_INTERNAL, STV_HIDDEN, STV_PROTECTED };
	for (size_t i = 0; i < sizeof(names) / sizeof(char *); ++i) { if (value == values[i]) { return names[i]; } }
	return "UNKNOWN";
}
