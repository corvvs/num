#include "nm.h"

static uint64_t	number_width(uint64_t i, uint32_t base) {
	if (i == 0) {
		return 1;
	}
	uint64_t	n = 0;
	while (i) {
		i /= base;
		n += 1;
	}
	return n;
}

// check [in_from, in_from + in_size) is within [ex_from, ex_from + ex_size)
static bool is_region_within_another(
	const void* in_from,
	size_t in_size,
	const void* ex_from,
	size_t ex_size
) {
	if (in_from < ex_from) {
		return false;
	}
	if (ex_size < in_size) {
		return false;
	}
	if ((size_t)(in_from - ex_from) > (ex_size - in_size)) {
		return false;
	}
	return true;
}

static bool	is_sec_within_mmapped_region(const t_analysis* analysis, const t_section_unit* section) {
	// DEBUGOUT("ELF: [%p, %p)", analysis->target.head_addr, analysis->target.head_addr + analysis->target.size);
	// DEBUGOUT("sec: [%p, %p)", section->head_addr, section->head_addr + section->size);
	return is_region_within_another(section->head_addr, section->size, analysis->target.head_addr, analysis->target.size);
}

// アドレス from, ELFファイルの中に収まっているかどうか
static bool	is_mem_within_mmapped_region(
	size_t elf_size,
	size_t offset,
	size_t size
) {
	// DEBUGOUT("elf_size: %zu, offset: %zu, size: %zu", elf_size, offset, size);
	// サイズ がELFサイズを超えていないかどうかチェック
	if (size > elf_size) { return false; }
	// オフセット + サイズ がELFサイズを超えていないかどうかチェック
	if (elf_size - size < offset) { return false; }
	return true;
}

// セクションヘッダーテーブルの終端がELFファイルの中に収まっているかどうか
static bool	is_sht_within_mmapped_region(const t_object_header* header, const t_target_file* target) {
	// オーバーフローチェック
	if (header->shentsize == 0 || header->shnum > SIZE_MAX / header->shentsize) { return false; }
	const size_t shsize = header->shnum * header->shentsize;
	return is_mem_within_mmapped_region(target->size, header->shoff, shsize);
}

static bool	extract_sections(t_master* m, t_analysis* analysis) {
	analysis->num_section = analysis->header.shnum;
	analysis->sections = malloc(sizeof(t_section_unit) * analysis->num_section);
	YOYO_ASSERT(analysis->sections != NULL);
	analysis->num_symbol_table = 0;
	const void* current_header = analysis->target.head_addr + analysis->header.shoff;
	for (size_t i = 0; i < analysis->num_section; ++i) {
		t_section_unit*	section = &analysis->sections[i];

		// [セクション配列を埋める]
		switch (analysis->category) {
			case TC_ELF32:
				map_elf32_section_header(analysis, current_header, section);
				break;
			case TC_ELF64:
				map_elf64_section_header(analysis, current_header, section);
				break;
			default:
				// 何かがおかしい
				print_unrecoverable_generic_error_by_message(m, analysis->target.path, "SOMETHING WRONG");
				return false;
		}
		section->head_addr = analysis->target.head_addr + section->sec_offset;
		// [セクションのオーバーフローチェック]
		if (!is_sec_within_mmapped_region(analysis, section)) {
			print_recoverable_file_error_by_message(m, analysis->target.path, "file format not recognized");
			return false;
		}

		// [セクションが「セクション名文字列テーブル」だった場合, 控えておく]
		if (analysis->header.shstrndx == i) {
			analysis->section_name_str_table_index = i;
			map_section_to_string_table(section, &analysis->section_name_str_table);
			if (!analysis->section_name_str_table.is_terminated) {
				print_recoverable_file_error_by_message(m, analysis->target.path, "file format not recognized");
				return false;
			}
			analysis->found_section_name_str_table = true;
		}
		// [セクションが「シンボルテーブル」だった場合, カウントしておく]
		switch (section->type) {
			case SHT_SYMTAB:
			case SHT_DYNSYM:
				analysis->num_symbol_table += 1;
		}
		current_header += analysis->header.shentsize;
	}
	// この時点で「セクション名文字列テーブル」が見つからなかった場合は失敗
	if (!analysis->found_section_name_str_table) {
		print_recoverable_file_error_by_message(m, analysis->target.path, "no symbols");
		return false;
	}

	// セクション名文字列テーブルを使ってセクション名をセットする
	const t_string_table_unit* sec_strtab = &analysis->section_name_str_table;
	for (size_t i = 0; i < analysis->num_section; ++i) {
		t_section_unit*	section = &analysis->sections[i];
		if (analysis->found_section_name_str_table) {
			// DEBUGOUT("section->name_offset: %zu, sec_strtab->total_size: %zu", section->name_offset, sec_strtab->total_size);
			if (section->name_offset > sec_strtab->total_size) {
				return false;
			}
			section->name = sec_strtab->head_addr + section->name_offset;
		} else {
			section->name = NULL;
		}
		determine_section_category(m, analysis, section);
	}
	return true;
}

static bool extract_symbol_tables(t_analysis* analysis) {
	analysis->symbol_tables = malloc(sizeof(t_table_pair) * analysis->num_symbol_table);
	YOYO_ASSERT(analysis->symbol_tables != NULL);
	size_t	i_symbol_table = 0;
	analysis->num_symbol = 0;
	for (size_t i = 0; i < analysis->num_section; ++i) {
		t_section_unit*	section = &analysis->sections[i];
		switch (section->type) {
			case SHT_SYMTAB:
			case SHT_DYNSYM: {
				size_t	string_table_index = section->link;
				YOYO_ASSERT(string_table_index < analysis->num_section);
				t_section_unit*	strtab_section = &analysis->sections[string_table_index];
				t_table_pair* symbol_pair = &analysis->symbol_tables[i_symbol_table];
				t_symbol_table_unit*	symtab = &symbol_pair->symbol_table;
				t_string_table_unit*	strtab = &symbol_pair->string_table;

				// [シンボルテーブルと文字列テーブルをペアにする]
				map_section_to_symbol_table(section, symtab);
				if (!is_mem_within_mmapped_region(analysis->target.size, symtab->offset, symtab->total_size)) {
					return false;
				}
				map_section_to_string_table(strtab_section, strtab);
				if (!is_mem_within_mmapped_region(analysis->target.size, strtab->offset, strtab->total_size)) {
					return false;
				}
				analysis->num_symbol += symtab->num_entries;
				i_symbol_table += 1;
			}
		}
	}
	return true;
}

static bool	extract_symbols(t_master* m, t_analysis* analysis) {
	YOYO_ASSERT(analysis->num_symbol > 0);
	analysis->symbols = malloc(sizeof(t_symbol_unit) * analysis->num_symbol);
	YOYO_ASSERT(analysis->symbols != NULL);
	analysis->sorted_symbols = malloc(sizeof(t_symbol_unit *) * analysis->num_symbol);
	YOYO_ASSERT(analysis->sorted_symbols != NULL);
	analysis->num_symbol_effective = 0;
	size_t i_symbol = 0;
	for (size_t i_symbol_table = 0; i_symbol_table < analysis->num_symbol_table; ++i_symbol_table) {
		t_table_pair*			node = &analysis->symbol_tables[i_symbol_table];
		t_symbol_table_unit*	symbol_table = &node->symbol_table;
		if (symbol_table->section->type != SHT_SYMTAB) { continue; }
		void*					current_symbol = symbol_table->head_addr;
		for (size_t k = 0; k < node->symbol_table.num_entries; ++k, ++i_symbol) {
			const size_t	sec_offset = current_symbol - analysis->target.head_addr;
			if (sec_offset > analysis->target.size) {
				return false;
			}
			t_symbol_unit*	symbol_unit = &analysis->symbols[i_symbol];
			switch (analysis->category) {
				case TC_ELF32:
					map_elf32_symbol(analysis, current_symbol, symbol_unit);
					break;
				case TC_ELF64:
					map_elf64_symbol(analysis, current_symbol, symbol_unit);
					break;
				default:
					// 何かがおかしい
					print_unrecoverable_generic_error_by_message(m, analysis->target.path, "SOMETHING WRONG");
					break;
			}
			symbol_unit->offset = (size_t)(current_symbol - analysis->target.head_addr);
			symbol_unit->relevant_section = get_referencing_section(m, analysis, symbol_unit);

			// [シンボル名をセットする]
			if (!determine_symbol_name(m, analysis, node, symbol_unit)) {
				yoyo_dprintf(STDERR_FILENO, "%s: %s: %s\n", m->exec_name, analysis->target.path, "file format not recognized");
				return false;
			}

			// [シンボルグリフを決定する]
			determine_symbol_griff(m, analysis, symbol_unit);

			current_symbol += symbol_table->entry_size;
			analysis->num_symbol_effective += 1;
		}
	}
	return true;
}

// シンボル名による比較
static int		compare_by_name(const t_symbol_unit* a, const t_symbol_unit* b, bool rev) {
	int rv;
	if (a->name == NULL || b->name == NULL) {
		rv = (a->name == NULL ? 0 : 1) - (b->name == NULL ? 0 : 1);
	} else {
		rv = ft_strcmp(a->name, b->name);
	}
	return rev ? -rv : rv;
}

// シンボルのオフセットによる比較
// (アドレスではないことに注意)
static int		compare_by_offset(const t_symbol_unit* a, const t_symbol_unit* b, bool rev) {
	int rv = a->offset > b->offset ? +1 : a->offset < b->offset ? -1 : 0;
	return rev ? -rv : rv;
}

static int		compare_complex(const t_symbol_unit* a, const t_symbol_unit* b, bool rev) {
	int rv = compare_by_name(a, b, rev);
	if (rv == 0) {
		rv = compare_by_offset(a, b, false); // TODO: 環境依存?
	}
	return rv;
}

static void	quicksort_symbols(
	const t_master* m,
	t_symbol_unit** list,
	size_t n,
	int (*compare)(const t_symbol_unit*, const t_symbol_unit*, bool rev)
) {
	if (n <= 1) {
		return;
	}
	size_t	i = 0;
	size_t	j = n - 1;
	t_symbol_unit*	pivot = list[n / 2];
	while (i <= j) {
		while (compare(list[i], pivot, m->option.reversed) < 0) {
			i += 1;
		}
		while (compare(list[j], pivot, m->option.reversed) > 0) {
			j -= 1;
		}
		if (i <= j) {
			t_symbol_unit*	tmp = list[i];
			list[i] = list[j];
			list[j] = tmp;
			i += 1;
			j -= 1;
		}
	}
	quicksort_symbols(m, list, j + 1, compare);
	quicksort_symbols(m, list + i, n - i, compare);
}

static void	sort_symbols(const t_master* m, t_analysis* analysis) {
	for (size_t i = 0; i < analysis->num_symbol_effective; ++i) {
		analysis->sorted_symbols[i] = &analysis->symbols[i];
	}
	if (!m->option.without_sorting) {
		quicksort_symbols(m, analysis->sorted_symbols, analysis->num_symbol_effective, compare_complex);
	}
}

static void	destroy_analysis(const t_master* m, t_analysis* analysis) {
	free(analysis->sections);
	free(analysis->symbol_tables);
	free(analysis->symbols);
	free(analysis->sorted_symbols);
	destroy_target_file(m, &analysis->target);
}

static bool	should_print_address(const t_symbol_unit* symbol) {
	return symbol->shndx != SHN_UNDEF;
}

static bool should_print_symbol(const t_symbol_unit* symbol) {
	if (symbol->symbol_griff == SYMGRIFF_UNKNOWN) { return false; }
	// if (symbol->symbol_griff == 'U' && (symbol->name == NULL || ft_strnlen(symbol->name, 1) == 0)) { return false; }
	if (symbol->symbol_griff == 'U' && symbol->bind == STB_LOCAL) { return false; }
	if (symbol->name != NULL) {
		if (symbol->symbol_griff == 'U' && ft_strnlen(symbol->name, 1) == 0) { return false; }
		if (symbol->name[0] == '$' && ft_strnlen(symbol->name, 3) == 2) { return false; }
	}
	return true;
}

static void print_symbol_address(const t_analysis* analysis, const t_symbol_unit* symbol) {
	const size_t	addr_width = analysis->category == TC_ELF64 ? 16 : 8;
	if (!should_print_address(symbol)) {
		print_spaces(STDOUT_FILENO, addr_width);
	} else {
		uint32_t	width = number_width(symbol->address, 16);
		if (width < addr_width) {
			print_chars(STDOUT_FILENO, '0', addr_width - width);
		}
		yoyo_dprintf(STDOUT_FILENO, "%x", symbol->address);
	}
}

static void	print_symbols(const t_master* m, const t_analysis* analysis) {
	if (m->num_target > 1) {
		yoyo_dprintf(STDOUT_FILENO, "\n");
		yoyo_dprintf(STDOUT_FILENO, "%s:\n", analysis->target.path);
	}
	if (analysis->num_symbol_effective == 0) {
		yoyo_dprintf(STDERR_FILENO, "%s: %s: %s\n", m->exec_name, analysis->target.path, "no symbols");
		return;
	}
	for (size_t i = 0; i < analysis->num_symbol_effective; ++i) {
		t_symbol_unit*	symbol = analysis->sorted_symbols[i];
		if (!should_print_symbol(symbol)) {
			// debug_print_symbol(symbol);
			// if (symbol->relevant_section) {
			// 	debug_print_section(symbol->relevant_section);
			// }
			continue;
		}

		// [アドレスの表示]
		print_symbol_address(analysis, symbol);
		// [グリフの表示]
		yoyo_dprintf(STDOUT_FILENO, " %c ", symbol->symbol_griff);
		// [名前の表示]
		yoyo_dprintf(STDOUT_FILENO, "%s", symbol->name);
		yoyo_dprintf(STDOUT_FILENO, "\n", symbol->name);
	}
}

static bool	analyze(t_master* m, t_analysis* analysis) {
	// この時点で, 対象ファイルは少なくとも 32ビットELFヘッダ以上のサイズを持っていることが確定している.
	// [32/64ビットかどうかを判定し, 結果に応じてヘッダのマッピングを行う]
	if (!analyze_header(m, analysis)) {
		return false;
	}

	// [セクションヘッダーテーブルを見る]
	if (!is_sht_within_mmapped_region(&(analysis->header), &(analysis->target))) {
		yoyo_dprintf(STDERR_FILENO, "%s: %s: %s\n", m->exec_name, analysis->target.path, "file format not recognized");
		return false;
	}

	// [セクションを入れた配列を作る]
	if (!extract_sections(m, analysis)) {
		return false;
	}

	// シンボルユニット配列を用意する
	if (analysis->num_symbol_table == 0) {
		yoyo_dprintf(STDERR_FILENO, "%s: %s: %s\n", m->exec_name, analysis->target.path, "no symbols");
		return false;
	}

	// [シンボルテーブルがあったら, 対応する文字列テーブルとペアにして配列に入れていく]
	if (!extract_symbol_tables(analysis)) {
		yoyo_dprintf(STDERR_FILENO, "%s: %s: %s\n", m->exec_name, analysis->target.path, "no symbols");
		return false;
	}

	// [シンボルを配列化する]
	extract_symbols(m, analysis);

	// [必要ならシンボルをソート]
	sort_symbols(m, analysis);

	// [表示]
	print_symbols(m, analysis);
	return true;
}

bool	analyze_file(t_master* m, const char* target_path) {
	t_analysis*	analysis = &m->current_analysis;
	*analysis = (t_analysis){0};
	analysis->target_index = m->i;

	t_target_file*	target = &m->current_analysis.target;
	// [ファイルの展開]
	if (!deploy_analysis(m, target_path, target)) {
		return false;
	}
	// [ファイルの解析]
	bool	result = analyze(m, analysis);
	// [後始末]
	destroy_analysis(m, analysis);
	return result;
}
