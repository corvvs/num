#include "nm.h"

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
		if (symbol->shndx < analysis->num_section) {
			const t_section_unit* section = &analysis->sections[symbol->shndx];
			symbol->name = section->name;
		} else {
			symbol->name = NULL;
		}
	} else {
		const t_string_table_unit* string_table = &table_pair->string_table;
		if (string_table->is_terminated) {
			symbol->name = string_table->head_addr + symbol->name_offset;
		} else {
			symbol->name = NULL;
		}
	}
}

static t_visibility	infer_symbol_visibility(const t_symbol_unit* symbol) {
	switch (symbol->bind) {
		case STB_GLOBAL:
			return V_GLOBAL;
		case STB_LOCAL:
			return V_LOCAL;
		case STB_WEAK: {
			if (symbol->relevant_section != NULL) {
				return V_GLOBAL;
			} else {
				return V_LOCAL;
			}
		}
		default:
			break;
	}
	const bool maybe_global = symbol->name[0] != '_';
	return maybe_global ? V_GLOBAL : V_LOCAL;
}

// symbol のシンボルグリフを決定する
void	determine_symbol_griff(const t_master* m, const t_analysis* analysis, t_symbol_unit* symbol) {
	// debug_print_symbol(symbol);

	t_visibility	visibility = infer_symbol_visibility(symbol);

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
	if (m->option.display_only_external) {
		if (symbol->bind != STB_GLOBAL && symbol->bind != STB_WEAK) {
			return;
		}
	}
	if (m->option.display_only_undefined) {
		if (symbol->shndx != SHN_UNDEF) {
			return;
		}
	}

	// [セクション]
	if (symbol->type == STT_SECTION && symbol->shndx < analysis->num_section) {
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
			// case SC_OTHER: {
			// 	// その他のセクション
			// 	symbol->symbol_griff = 'O';
			// 	return;
			// }
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
					if (visibility == V_GLOBAL) {
						// グローバルなウィークオブジェクト
						symbol->symbol_griff = 'V';
					} else {
						// ローカルなウィークオブジェクト
						symbol->symbol_griff = 'v';
					}
					return;
				}
				default: {
					if (visibility == V_GLOBAL) {
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
