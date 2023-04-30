#include "nm.h"

// 「引数が指定したマクロのいずれかに一致する場合、一致したマクロの名前の文字列を返す関数」を生成するワンライナー
// echo "VALUE_A, VALUE_B, VALUE_C" | ruby -e 'NS = gets.chomp; puts "const char* xxx(int value) {"; puts "\tstatic const char* names[] = (const char* []){#{NS.split(/\s*,\s*/).map{ |s| "\"#{s}\"" }.join(", ")}};"; puts "\tstatic const int values[] = (int []){ #{NS} };"; puts "\tfor (size_t i = 0; i < sizeof(names) / sizeof(char *); ++i) { if (value == values[i]) { return names[i]; } }"; puts "\treturn \"UNKNOWN\";"; puts "}"; ' | pbcopy 

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
