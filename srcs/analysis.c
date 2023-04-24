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

void	extract_sections(t_master* m, t_analysis* analysis, const void* section_header_table) {
	const void* current_header = section_header_table;
	for (size_t i = 0; i < analysis->num_section; ++i) {
		t_section_unit*	section = &analysis->sections[i];

		// [セクション配列を埋める]
		switch (analysis->category) {
			case TC_ELF32:
				map_elf32_section_header(current_header, section);
				break;
			case TC_ELF64:
				map_elf64_section_header(current_header, section);
				break;
			default:
				// 何かがおかしい
				print_error_by_message(m, "SOMETHING WRONG");
				break;
		}
		section->head = analysis->target.head + section->offset;

		// [セクションが「セクション名文字列テーブル」だった場合, 控えておく]
		if (analysis->header.shstrndx == i) {
			analysis->section_name_str_table_index = i;
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
}

void extract_symbol_tables(t_analysis* analysis) {
	size_t	i_symbol_table = 0;
	for (size_t i = 0; i < analysis->num_section; ++i) {
		t_section_unit*	section = &analysis->sections[i];
		switch (section->type) {
			case SHT_SYMTAB:
			case SHT_DYNSYM: {
				size_t	string_table_index = section->link;
				YOYO_ASSERT(string_table_index < analysis->num_section);
				t_section_unit*	strtab_section = &analysis->sections[string_table_index];
				t_symbol_list_node* symbol_pair = &analysis->symbol_tables[i_symbol_table];

				// [シンボルテーブルと文字列テーブルをペアにする]
				map_section_to_symbol_table(section, &symbol_pair->symbol_table);
				map_section_to_string_table(strtab_section, &symbol_pair->string_table);
				analysis->num_symbol += symbol_pair->symbol_table.num_entries;
				i_symbol_table += 1;
			}
		}
	}
}

void	extract_symbols(t_master* m, t_analysis* analysis) {
	size_t i_symbol = 0;
	for (size_t i_symbol_table = 0; i_symbol_table < analysis->num_symbol_table; ++i_symbol_table) {
		t_symbol_list_node*	node = &analysis->symbol_tables[i_symbol_table];
		t_symbol_table_unit* symbol_table = &node->symbol_table;
		t_string_table_unit* string_table = &node->string_table;
		void*	current_symbol = symbol_table->head;
		for (size_t k = 0; k < node->symbol_table.num_entries; ++k, ++i_symbol) {
			t_symbol_unit*	symbol = &analysis->symbols[i_symbol];
			switch (analysis->category) {
				case TC_ELF32:
					map_elf32_symbol(current_symbol, string_table, symbol);
					break;
				case TC_ELF64:
					map_elf64_symbol(current_symbol, string_table, symbol);
					break;
				default:
					// 何かがおかしい
					print_error_by_message(m, "SOMETHING WRONG");
					break;
			}

			current_symbol += symbol_table->entry_size;

		}
	}
}

int		compare_by_address(const t_symbol_unit* a, const t_symbol_unit* b) {
	if (a->address < b->address) {
		return -1;
	}
	if (a->address > b->address) {
		return 1;
	}
	return 0;
}

int		compare_by_name(const t_symbol_unit* a, const t_symbol_unit* b) {
	return ft_strcmp(a->name, b->name);
}

void	quicksort_symbols(t_symbol_unit** list, size_t n) {
	if (n <= 1) {
		return;
	}
	size_t	i = 0;
	size_t	j = n - 1;
	t_symbol_unit*	pivot = list[n / 2];
	while (i <= j) {
		while (compare_by_name(list[i], pivot) < 0) {
			i += 1;
		}
		while (compare_by_name(list[j], pivot) > 0) {
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
	quicksort_symbols(list, j + 1);
	quicksort_symbols(list + i, n - i);
}

void	sort_symbols(t_master* m, t_analysis* analysis) {
	for (size_t i = 0; i < analysis->num_symbol; ++i) {
		analysis->sorted_symbols[i] = &analysis->symbols[i];
	}
	(void)m;
	// analysis->sorted_symbols の中身を address で昇順にソートする
	quicksort_symbols(analysis->sorted_symbols, analysis->num_symbol);
}

bool	analyze_file(t_master* m, const char* target_path) {
	t_analysis*	analysis = &m->current_analysis;
	*analysis = (t_analysis){0};

	t_target_file*	target = &m->current_analysis.target;
	// [ファイルの展開]
	if (!deploy_target_file(target_path, target)) {
		print_error_by_errno();
	}

	// この時点で, 対象ファイルは少なくとも 32ビットELFヘッダ以上のサイズを持っていることが確定している.
	// [32/64ビットかどうかを判定し, 結果に応じてヘッダのマッピングを行う]
	if (!analyze_header(analysis)) {
		return false;
	}

	// [セクションヘッダーテーブルを見る]
	void* current_header = analysis->target.head + analysis->header.shoff;
	const void* section_header_table = current_header;
	(void)section_header_table;
	size_t shsize = analysis->header.shnum * analysis->header.shentsize;
	YOYO_ASSERT(analysis->header.shoff + shsize <= analysis->target.size);

	// [セクション配列を用意する]
	analysis->num_section = analysis->header.shnum;
	analysis->sections = malloc(sizeof(t_section_unit) * analysis->num_section);
	YOYO_ASSERT(analysis->sections != NULL);
	analysis->num_symbol_table = 0;

	extract_sections(m, analysis, section_header_table);

	// シンボルユニット配列を用意する
	YOYO_ASSERT(analysis->num_symbol_table > 0);
	analysis->symbol_tables = malloc(sizeof(t_symbol_list_node) * analysis->num_symbol_table);
	YOYO_ASSERT(analysis->symbol_tables != NULL);
	
	// [シンボルテーブルがあったら, 対応する文字列テーブルとペアにして配列に入れていく]
	analysis->num_symbol = 0;
	extract_symbol_tables(analysis);

	// [シンボルを配列化する]
	YOYO_ASSERT(analysis->num_symbol > 0);
	analysis->symbols = malloc(sizeof(t_symbol_unit) * analysis->num_symbol);
	YOYO_ASSERT(analysis->symbols != NULL);
	analysis->sorted_symbols = malloc(sizeof(t_symbol_unit *) * analysis->num_symbol);
	YOYO_ASSERT(analysis->sorted_symbols != NULL);

	extract_symbols(m, analysis);

	// [必要ならシンボルをソート]
	sort_symbols(m, analysis);

	// [表示]
	const size_t	addr_width = 16;
	for (size_t i = 0; i < analysis->num_symbol; ++i) {
		t_symbol_unit*	symbol = analysis->sorted_symbols[i];

		// [アドレスの表示]
		if (symbol->address == 0) {
			print_spaces(STDOUT_FILENO, addr_width);
		} else {
			uint32_t	width = number_width(symbol->address, 16);
			if (width < addr_width) {
				print_chars(STDOUT_FILENO, '0', addr_width - width);
			}
			yoyo_dprintf(STDOUT_FILENO, "%x", symbol->address);
		}
		// [グリフの表示]
		yoyo_dprintf(STDOUT_FILENO, " %c ", symbol->symbol_griff);
		// [名前の表示]
		yoyo_dprintf(STDOUT_FILENO, "%s\n", symbol->name);
	}


	free(analysis->sections);
	free(analysis->symbol_tables);
	free(analysis->symbols);
	free(analysis->sorted_symbols);

	destroy_target_file(target);

	return true;
}
