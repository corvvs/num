#include "nm.h"

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
		case SHT_NOTE:
		case SHT_DYNAMIC:
		case SHT_SYMTAB:
		case SHT_DYNSYM:
		case SHT_STRTAB:
		case SHT_GNU_HASH:
		case SHT_GNU_versym:
		case SHT_GNU_verneed:
		case SHT_RELA:
		case SHT_PROGBITS: {
			if (ft_strncmp(section->name, ".debug", 6) == 0) {
				section->category = SC_DEBUG;
				return;
			}
			// プログラムデータ（コードやデータなど）を格納するセクション
			if (FLAG_ALL(section->flags, SHF_EXECINSTR | SHF_ALLOC)) {
				// フラグがSHF_EXECINSTR | SHF_ALLOC → テキストセクション
				section->category = SC_TEXT;
				return;
			}
			if (FLAG_ALL(section->flags, SHF_WRITE | SHF_ALLOC)) {
				if (ft_strcmp(section->name, ".got") == 0) {
					// フラグがSHF_WRITE | SHF_ALLOC, かつ名前が .got → GOTセクション
					section->category = SC_GOT;
					return;
				}
				// フラグがSHF_WRITE | SHF_ALLOC → データセクション
				section->category = SC_DATA;
				return;
			}
			if (FLAG_ALL(section->flags, SHF_WRITE | SHF_ALLOC | SHF_MERGE)) {
				// フラグがSHF_WRITE | SHF_ALLOC | SHF_MERGE → スモールオブジェクトセクション
				section->category = SC_SMALL;
				return;
			}
			if (FLAG_ALL(section->flags, SHF_ALLOC)) {
				// フラグがSHF_ALLOC → 読み取り専用データセクション
				section->category = SC_READONLY;
				return;
			}
			if (FLAG_ALL(section->flags, SHF_MERGE | SHF_STRINGS)) {
				section->category = SC_MERGEABLE_CHARACTER;
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
	}
	section->category = SC_OTHER;
}

// シンボルの名前 name を決定する
void	determine_symbol_name(
	const t_master* m,
	const t_analysis* analysis,
	const t_table_pair* table_pair,
	t_symbol_unit* symbol
) {
	(void)m;
	if (symbol->type == STT_SECTION) {
		// セクションシンボル
		const t_section_unit* section = &analysis->sections[symbol->shndx];
		symbol->name = section->name;
		return;
	} else {
		const t_string_table_unit* string_table = &table_pair->string_table;
		symbol->name = string_table->head + symbol->name_offset;
	}
}

// グローバルなデバッグ情報シンボル
// ローカルなデバッグ情報シンボル
// スタックまたはその他の未定義セクションシンボル
// グローバルなスモールオブジェクトセクションシンボル	Small
// ローカルなスモールオブジェクトセクションシンボル	small
// 未定義のシンボル	Undefined

// symbol のシンボルグリフを決定する
void	determine_symbol_griff(const t_master* m, const t_analysis* analysis, t_symbol_unit* symbol) {
	(void)m;
	(void)analysis;
	(void)symbol;

	DEBUGOUT("bind:%s\ttype:%s\t|%s|\tsection-cat:%s shndx:%zu %llu %llu %u addr: %p value: %llx size: %llu",
		symbinding_to_name(symbol->bind),
		symtype_to_name(symbol->type),
		symbol->name,
		symbol->relevant_section ? section_category_to_name(symbol->relevant_section->category) : NULL,
		symbol->shndx,
		symbol->bind,
		symbol->type,
		symbol->info,
		symbol->address,
		symbol->value,
		symbol->size
	);

	// [セクションとファイルシンボルはデフォルト表示しない]
	if (!m->option.display_all) {
		switch (symbol->type) {
			case STT_SECTION:
			case STT_FILE:
				return;
			default:
				break;
		}
	}

	// [セクション]
	if (symbol->type == STT_SECTION) {
		const t_section_unit* section = &analysis->sections[symbol->shndx];
		switch (section->category) {
			case SC_DATA: {
				// データセクション
				symbol->symbol_griff = 'd';
				return;
			}
			case SC_TEXT: {
				// テキストセクション
				symbol->symbol_griff = 't';
				return;
			}
			case SC_BSS: {
				// BSSセクション
				symbol->symbol_griff = 'b';
				return;
			}
			case SC_GOT: {
				// GOTセクション
				symbol->symbol_griff = 'd';
				return;
			}
			case SC_SMALL: {
				// スモールオブジェクトセクション
				symbol->symbol_griff = 's';
				return;
			}
			case SC_OTHER: {
				// その他のセクション
				symbol->symbol_griff = 'O';
				return;
			}
			case SC_UNDEFINED: {
				// 未定義のセクション
				symbol->symbol_griff = 'U';
				return;
			}
			case SC_READONLY: {
				// 読み取り専用データセクション
				symbol->symbol_griff = 'r';
				return;
			}
			case SC_DEBUG: {
				symbol->symbol_griff = 'N';
				return;
			}
			case SC_MERGEABLE_CHARACTER: {
				symbol->symbol_griff = 'n';
				return;
			}
			default:
				break;
		}
	}

	// [ウィークシンボル]
	switch (symbol->bind) {
		case STB_WEAK: {
			switch (symbol->type) {
				case STT_OBJECT: {
					const bool maybe_global = symbol->name[0] != '_';
					if (maybe_global) {
						// グローバルなウィークオブジェクト
						symbol->symbol_griff = 'V';
					} else {
						// ローカルなウィークオブジェクト
						symbol->symbol_griff = 'v';
					}
					return;
				}
				default: {
					const bool maybe_global = symbol->name[0] != '_';
					if (maybe_global) {
						// グローバルなウィーク関数
						symbol->symbol_griff = 'W';
					} else {
						// ローカルなウィーク関数
						symbol->symbol_griff = 'w';
					}
					return;
				}
			}
		}
		return;
	}

	switch (symbol->shndx) {
		case SHN_ABS: {
			switch (symbol->bind) {
				case STB_GLOBAL:
					// グローバルな絶対シンボル
					symbol->symbol_griff = 'A';
					return;
				case STB_LOCAL:
					// ローカルな絶対シンボル
					symbol->symbol_griff = 'a';
					return;
				default:
					break;
			}
			break;
		}
		case SHN_COMMON: {
			switch (symbol->bind) {
				case STB_GLOBAL:
					// グローバルなBSSシンボル
					symbol->symbol_griff = 'B';
					return;
				case STB_LOCAL:
					// ローカルなBSSシンボル
					symbol->symbol_griff = 'b';
					return;
				default:
					// コモンシンボル
					symbol->symbol_griff = 'C';
					return;
			}
			break;
		}

		case SHN_UNDEF: {
			// 未定義シンボル??
			symbol->symbol_griff = 'U';
			return;
		}

		default: {
			if (symbol->relevant_section != NULL) {
				switch (symbol->relevant_section->category) {
					case SC_BSS: {
						switch (symbol->bind) {
							case STB_GLOBAL:
								// グローバルなBSSシンボル
								symbol->symbol_griff = 'B';
								return;
							case STB_LOCAL:
								// ローカルなBSSシンボル
								symbol->symbol_griff = 'b';
								return;
						}
						break;
					}

					case SC_DATA: {
						switch (symbol->bind) {
							case STB_GLOBAL:
								// グローバルなデータシンボル
								symbol->symbol_griff = 'D';
								return;
							case STB_LOCAL:
								// ローカルなデータシンボル
								symbol->symbol_griff = 'd';
								return;
						}
						break;
					}

					case SC_READONLY: {
						switch (symbol->bind) {
							case STB_GLOBAL:
								// グローバルな読み取り専用データセクションシンボル
								symbol->symbol_griff = 'R';
								return;
							case STB_LOCAL:
								// ローカルな読み取り専用データセクションシンボル
								symbol->symbol_griff = 'r';
								return;
						}
						break;
					}

					default:
						break;
				}
			}
		}
	}

	// [type で分類]
	switch (symbol->type) {
		case STT_GNU_IFUNC: {
			// インディレクト関数シンボル	Indirect
			symbol->symbol_griff = 'I';
			return;
		}
		case STT_OBJECT: {
			if (symbol->relevant_section != NULL) {
				switch (symbol->relevant_section->category) {
					case SC_GOT: {
						switch (symbol->bind) {
							case STB_GLOBAL:
								// グローバルなGOTシンボル
								symbol->symbol_griff = 'G';
								return;
							case STB_LOCAL:
								// ローカルなGOTシンボル
								symbol->symbol_griff = 'g';
								return;
						}
						break;
					}
					case SC_DATA: {
						switch (symbol->bind) {
							case STB_GLOBAL:
								// グローバルなデータシンボル
								symbol->symbol_griff = 'D';
								return;
							case STB_LOCAL:
								// ローカルなデータシンボル
								symbol->symbol_griff = 'd';
								return;
						}
						break;
					}
					default:
						break;
				}
			}
			break;
		}
		case STT_NOTYPE:
		case STT_FUNC: {
			if (symbol->relevant_section && symbol->relevant_section->category == SC_TEXT) {
				switch (symbol->bind) {
					case STB_GLOBAL:
						// グローバルなテキストシンボル
						symbol->symbol_griff = 'T';
						return;
					case STB_LOCAL:
						// ローカルなテキストシンボル
						symbol->symbol_griff = 't';
						return;
				}
			}
			break;
		}
		default:
			break;
	}
}
