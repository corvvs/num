#include "nm.h"



static t_section_category	determine_generic_section_category(const t_section_unit* section) {
	// prefixed by ".debug"
	if (ft_strncmp(section->name, ".debug", 6) == 0) {
		return SC_DEBUG;
	}
	// プログラムデータ（コードやデータなど）を格納するセクション
	if (FLAG_ALL(section->flags, SHF_EXECINSTR | SHF_ALLOC)) {
		// フラグがSHF_EXECINSTR | SHF_ALLOC → テキストセクション
		return SC_TEXT;
	}
	if (FLAG_ALL(section->flags, SHF_WRITE | SHF_ALLOC)) {
		if (ft_strcmp(section->name, ".got") == 0) {
			// フラグがSHF_WRITE | SHF_ALLOC, かつ名前が .got → GOTセクション
			return SC_GOT;
		}
		// フラグがSHF_WRITE | SHF_ALLOC → データセクション
		return SC_DATA;
	}
	if (FLAG_ALL(section->flags, SHF_WRITE | SHF_ALLOC | SHF_MERGE)) {
		// フラグがSHF_WRITE | SHF_ALLOC | SHF_MERGE → スモールオブジェクトセクション
		return SC_SMALL;
	}
	if (FLAG_ALL(section->flags, SHF_ALLOC)) {
		// フラグがSHF_ALLOC → 読み取り専用データセクション
		return SC_READONLY;
	}
	if (FLAG_ALL(section->flags, SHF_MERGE | SHF_STRINGS)) {
		return SC_MERGEABLE_CHARACTER;
	}	
	return SC_OTHER;
}

void	determine_section_category(const t_master* m, const t_analysis* analysis, t_section_unit* section) {
	(void)m;
	(void)analysis;
	switch (section->type) {
		case SHT_NULL: {
			// セクションヘッダテーブルの未使用エントリ
			if (section->flags == 0) {
				section->category = SC_UNDEFINED;
				return;
			}
			break;
		}
		case SHT_INIT_ARRAY:
		case SHT_FINI_ARRAY:
		case SHT_PREINIT_ARRAY:
		case SHT_NOTE:
		case SHT_DYNAMIC:
		case SHT_SYMTAB:
		case SHT_DYNSYM:
		case SHT_STRTAB:
		case SHT_GNU_HASH:
		case SHT_GNU_versym:
		case SHT_GNU_verneed:
		case SHT_GNU_LIBLIST:
		case SHT_RELA:
		case SHT_PROGBITS: {
			t_section_category	category = determine_generic_section_category(section);
			if (category != SC_OTHER) {
				section->category = category;
				return;
			}
			break;
		}
		case SHT_NOBITS: {
			if (FLAG_ALL(section->flags, SHF_WRITE | SHF_ALLOC)) {
				// フラグがSHF_WRITE | SHF_ALLOC → BSSセクション
				section->category = SC_BSS;
				return;
			}
			break;
		}
		default: {
			if (SHT_LOPROC <= section->type && section->type <= SHT_HIPROC) {
				// プロセッサ固有のセクション
				yoyo_dprintf(STDERR_FILENO, "PROC SPECIFIC!! %llx\n", section->type);
				t_section_category	category = determine_generic_section_category(section);
				if (category != SC_OTHER) {
					section->category = category;
					return;
				}
				break;
			}
		}
	}
	section->category = SC_OTHER;
}
