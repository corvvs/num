#include "nm.h"



void	analyze_string_table_section(
	const Elf64_Ehdr* elf_header,
	const Elf64_Shdr* section_header,
	size_t size
) {
	DEBUGWARN("%s", "STRTABLE");
	DEBUGINFO("sh_addr: %p", section_header->sh_addr);
	DEBUGINFO("sh_size: %zu", section_header->sh_size);
	DEBUGINFO("sh_offset: %zu", section_header->sh_offset);
	DEBUGINFO("size: %zu", size);
	const char*	string_table = (void*)elf_header + section_header->sh_offset;
	DEBUGINFO("string_table: %p", string_table);

	// セクションヘッダーの sh_offset は, セクションの先頭アドレスからのオフセット.
	// つまり, セクションの先頭アドレスは, セクションヘッダーテーブルの先頭アドレス + sh_offset で求められる.
	const char*	string_head = NULL;
	size_t	k = 0;
	for (size_t i = 0; i < section_header->sh_size; ++i) {
		char	c = string_table[i];
		if (c == '\0') {
			if (string_head != NULL) {
				DEBUGINFO("%p: str[%zu] = %s", (void*)string_head - (void*)elf_header, k, string_head);
				k += 1;
			}
			string_head = NULL;
		} else if (string_head == NULL) {
			string_head = string_table + i;
		}
	}
}

void	analyze_symbol_table_section(
	const Elf64_Ehdr* elf_header,
	const Elf64_Shdr* section_header,
	size_t size
) {
	DEBUGWARN("%s", "SYMTABLE");
	DEBUGOUT("section_header->sh_size: %zu", section_header->sh_size);
	DEBUGOUT("section_header->sh_entsize: %zu", section_header->sh_entsize);
	DEBUGOUT("section_header->sh_link: %u", section_header->sh_link);
	const size_t	num_entries = section_header->sh_size / section_header->sh_entsize;
	DEBUGOUT("num_entries: %zu", num_entries);
	(void)elf_header;
	(void)size;
	const Elf64_Sym*	symbol_table = (void*)elf_header + section_header->sh_offset;
	DEBUGINFO("symbol_table: %p", symbol_table);

	const Elf64_Sym*	symbol_table_entry = symbol_table;
	size_t				k = 0;
	for (size_t i = 0; i < num_entries; ++i) {
		DEBUGINFO("sym[%zu]: %p", k, (void*)symbol_table_entry - (void*)elf_header);
		DEBUGINFO("sym[%zu]->st_name: %u", k, symbol_table_entry->st_name);
		DEBUGINFO("sym[%zu]->st_value: %p", k, symbol_table_entry->st_value);
		DEBUGINFO("sym[%zu]->st_size: %zu", k, symbol_table_entry->st_size);
		
		unsigned char st_info = symbol_table_entry->st_info;
		unsigned int binding = ELF64_ST_BIND(st_info);
		unsigned int type = ELF64_ST_TYPE(st_info);
		DEBUGINFO("%s", symbinding_to_name(binding));
		DEBUGINFO("%s", symtype_to_name(type));
	
		k += 1;
		symbol_table_entry = (void*)symbol_table_entry + section_header->sh_entsize;
	}
}

// セクションヘッダテーブルの指定されたインデックスが本当に文字列テーブルセクションかどうかをチェックする
static bool	is_string_table_section(
	const t_elf_64_header* elf_header,
	const t_elf_64_section_header* section_header_table,
	size_t index
) {
	// はみ出しチェック
	size_t	section_header_num = elf_header->e_shnum;
	if (section_header_num <= index) {
		DEBUGERR("given index %zu exceeds section_header_num %zu", index, section_header_num);
		return false;
	}
	DEBUGOUT("section_header_table: %p", section_header_table);
	const t_elf_64_section_header*	candidate_header = (void*)section_header_table + index * elf_header->e_shentsize;
	DEBUGOUT("candidate_header: %p", candidate_header);
	DEBUGOUT("candidate_header->sh_type: %d", candidate_header->sh_type);
	// セクションヘッダーの内容チェック
	if (candidate_header->sh_type != SHT_STRTAB) {
		DEBUGERR("given index %zu is not string table section", index);
		return false;
	}
	DEBUGWARN("YES!!: %p", candidate_header);
	return true;
}

// t_string_table 構造体に情報をセットする
static void	set_string_table(
	t_string_table* strtable,
	const t_elf_64_header* elf_header,
	const t_elf_64_section_header* string_header
) {
	strtable->head = (void*)elf_header + string_header->sh_offset;
	strtable->offset = string_header->sh_offset;
	strtable->total_size = string_header->sh_size;
}

// t_symbol_table 構造体に情報をセットする
static void	set_symbol_table(
	t_symbol_table* symtable,
	const t_elf_64_header* elf_header,
	const t_elf_64_section_header* symbol_header
) {
	symtable->head = (void*)elf_header + symbol_header->sh_offset;
	symtable->offset = symbol_header->sh_offset;
	symtable->num_entries = symbol_header->sh_size / symbol_header->sh_entsize;
	symtable->entry_size = symbol_header->sh_entsize;
	symtable->total_size = symbol_header->sh_size;
}


// シンボルテーブルセクションと, 関連する文字列テーブルセクションをペアにして, 解析単位として返す.
static t_symbol_unit*	form_symbol_unit(
	const t_elf_64_header* elf_header,
	const t_elf_64_section_header* section_header_table,
	size_t index_symbol
) {
	const size_t					section_header_entry_size = elf_header->e_shentsize;
	const t_elf_64_section_header*	symbol_header = (void*)section_header_table + section_header_entry_size * index_symbol;
	const t_elf_64_section_header*	string_header = (void*)section_header_table + section_header_entry_size * symbol_header->sh_link;
	t_symbol_unit	unit = {0};
	set_symbol_table(&unit.symbol_table, elf_header, symbol_header);
	set_string_table(&unit.string_table, elf_header, string_header);
	t_symbol_unit* rv = malloc(sizeof(t_symbol_unit));
	YOYO_ASSERT(rv != NULL);
	*rv = unit;
	return rv;
}

// t_symbol_unit 構造体のリストに新しい要素を追加する
static void	push_back_unit_list(
	t_master* m,
	const t_elf_64_section_header* section_header_table,
	size_t index_symbol
) {
	t_analysis_64*			analysis = &m->analysis_64;
	const t_elf_64_header*	elf_header = analysis->elf_header_64;
	t_symbol_unit*			unit = form_symbol_unit(elf_header, section_header_table, index_symbol);
	YOYO_ASSERT(unit != NULL);
	if (analysis->symbols_tail != NULL) {
		analysis->symbols_tail->next = unit;
	}
	analysis->symbols_tail = unit;
	if (analysis->symbols == NULL) {
		analysis->symbols = unit;
	}
}

// t_symbol_unit 構造体のリストを破壊する
void	destroy_unit_list(t_symbol_unit* symbols) {
	t_symbol_unit*	unit = symbols;
	while (unit != NULL) {
		t_symbol_unit*	next = unit->next;
		free(unit);
		unit = next;
	}
}

void	analyze_section_headers(t_master* m) {
	t_analysis_64*	analysis = &m->analysis_64;
	const t_elf_64_header*	elf_header = analysis->elf_header_64;

	// セクションヘッダーテーブルの先頭アドレス
	t_elf_64_section_header* section_header = (t_elf_64_section_header*)((char*)elf_header + elf_header->e_shoff);
	const t_elf_64_section_header* section_header_table = section_header;

	// セクションヘッダーテーブルのエントリー数
	size_t		section_header_num = elf_header->e_shnum;
	// セクションヘッダーテーブルのエントリーのサイズ
	size_t		section_header_size = elf_header->e_shentsize;
	void*		section_header_end = (void*)section_header + section_header_size * section_header_num;

	if ((void*)elf_header + m->target_file.size < section_header_end) {
		// セクションヘッダーテーブルの終わりがファイルをはみ出している
		DEBUGERR("section_header_end %p exceeds file size %zu", section_header_end, m->target_file.size);
		return;
	}

	// セクションヘッダーテーブルのエントリーを1つずつ見ていく
	for (size_t i = 0; i < section_header_num; ++i) {
		switch (section_header->sh_type) {
			case SHT_SYMTAB:
			case SHT_DYNSYM:
				DEBUGOUT("FOUND symbol table index: %zu", i);
				DEBUGOUT("section_header->sh_link: %u", section_header->sh_link);
				// シンボルテーブルが見つかった
				// -> sh_link から文字列テーブルを拾う(本当に文字列テーブルなら...)
				if (!is_string_table_section(elf_header, section_header_table, section_header->sh_link)) {
					DEBUGERR("section header %zu is not string table section", i);
					exit(1);
				}
				push_back_unit_list(m, section_header_table, i);
				// analyze_symbol_table_section(elf_header, section_header, section_header->sh_size);
				break;
		}
		section_header = (void*)section_header + section_header_size;
	}


	// DEBUGINFO("section_header: %p", section_header);
	// DEBUGINFO("section_header_num: %zu", section_header_num);
	// DEBUGINFO("section_header_size: %zu", section_header_size);

	// // セクションヘッダーテーブルの先頭アドレス
	// // セクションヘッダーテーブルは, セクションヘッダーの配列.
	// const Elf64_Shdr* section_header_tab = section_header;

	// DEBUGINFO("section_header_tab: %p", section_header_tab);

	// // セクションヘッダーテーブルをスキャンしていく
	// for (size_t i = 0; i < section_header_num; ++i) {
	// 	// DEBUGINFO("sh[%zu]: %p", i, section_header);
	// 	// DEBUGINFO("sh_type: %s", sectiontype_to_name(section_header->sh_type));



	// 	// SHT_SYMTAB, SHT_DYNSYM
	// 	// これらのセクションは、シンボルテーブルを保持します。
	// 	// 一般に、SHT_SYMTAB セクションはリンク編集に関するシンボルを与えます。
	// 	// このセクションには完全なシンボルテーブルとして、動的リンクに不要な多くのシンボルが存在することがあります。
	// 	// また、オブジェクトファイルには SHT_DYNSYM セクション (最小の一群の動的リンクシンボルを保持して領域を節約している) が存在することがあります。

	// 	// SHT_STRTAB, SHT_DYNSTR
	// 	// これらのセクションは、文字列テーブルを保持します。
	// 	// オブジェクトファイルには、複数の文字列テーブルセクションが存在できます。



	// 	switch (section_header->sh_type) {
	// 		case SHT_SYMTAB:
	// 		case SHT_DYNSYM:
	// 			DEBUGOUT("index: %zu", i);
	// 			analyze_symbol_table_section(elf_header, section_header, section_header->sh_size);
	// 			break;
	// 		case SHT_STRTAB:
	// 			DEBUGOUT("index: %zu", i);
	// 			analyze_string_table_section(elf_header, section_header, section_header->sh_size);
	// 			break;
	// 	}

	// 	section_header = (void*)section_header + section_header_size;
	// }

	destroy_unit_list(analysis->symbols);
}

void	analyze_64bit(t_master* m) {

	t_analysis_64*	analysis = &m->analysis_64;
	analysis->elf_header_64 = m->target_file.head;
	const t_elf_64_header*	elf_header = analysis->elf_header_64;

	DEBUGINFO("%s", elfclass_to_name(elf_header->e_ident[EI_CLASS]));

	DEBUGINFO("elf_header->e_type: %d", elf_header->e_type);

	// この構造体のメンバはオブジェクトファイルタイプを示す:
	// ET_NONE
	// 不明なタイプ。
	// ET_REL
	// 再配置可能ファイル。
	// ET_EXEC
	// 実行可能ファイル。
	// ET_DYN
	// 共有オブジェクト。
	// ET_CORE
	// コアファイル。
	DEBUGINFO("elf_header->e_machine: %d", elf_header->e_machine);

	// このメンバは個々のファイルに必要とされるアーキテクチャーを指定する。
	DEBUGINFO("elf_header->e_version: %d", elf_header->e_version);

	// このメンバは、システムが最初に制御を渡す、 つまりプロセスを開始する仮想アドレスを指定する。
	// ファイルにエントリーポイントが関連付けられていない場合、 このメンバには 0 が入る。
	DEBUGINFO("elf_header->e_entry: %p", elf_header->e_entry);

	// このメンバはプログラムヘッダーテーブルの ファイルオフセット (バイト単位) を保持する。
	// ファイルにプログラムヘッダーテーブルがない場合、 このメンバには 0 が入る。
	DEBUGINFO("elf_header->e_phoff: %zu", elf_header->e_phoff);

	// このメンバはセクションヘッダーテーブルの ファイルオフセット (バイト単位) を保持する。
	// ファイルにセクションヘッダーテーブルがない場合、 このメンバには 0 が入る。
	DEBUGINFO("elf_header->e_shoff: %zu", elf_header->e_shoff);

	// このメンバはセクションヘッダーのサイズ (バイト単位) を保持する。
	// セクションヘッダーはセクションヘッダーテーブルの 1 つのエントリーである; 全てのエントリーは同じサイズである。
	DEBUGINFO("elf_header->e_shentsize: %zu", elf_header->e_shentsize);

	// このメンバはセクションヘッダーテーブルにあるエントリーの数を保持する。
	// よって e_shentsize と e_shnum の積はセクションヘッダーテーブルのサイズ (バイト単位) になる。
	// ファイルにセクションヘッダーテーブルがない場合、 e_shnum は値 0 を保持する。
	// セクションヘッダーテーブルのエントリー数が SHN_LORESERVE (0xff00) 以上の場合、
	// e_shnum には値 0 が入り、セクションヘッダーテーブルのエントリーの実際の数は
	// セクションヘッダーテーブルの最初のエントリーの sh_size メンバに格納される。
	// それ以外の場合、セクションヘッダーテーブルの最初のエントリーの sh_info メンバ には値 0 が格納される。
	DEBUGINFO("elf_header->e_shnum: %zu", elf_header->e_shnum);


	DEBUGINFO("elf_header->e_shstrndx: %d", elf_header->e_shstrndx);


	// このメンバは ELF ヘッダーサイズ (バイト単位) を保持する。
	DEBUGINFO("elf_header->e_ehsize: %zu", elf_header->e_ehsize);

	// このメンバはこのファイルのプログラムヘッダーテーブルの 1 エントリーあたりのサイズ (バイト単位) を保持する;
	// 全てのエントリーは同じサイズである。
	DEBUGINFO("elf_header->e_phentsize: %zu", elf_header->e_phentsize);

	// このメンバはプログラムヘッダーテーブルにあるエントリーの数を保持する。
	// よって e_phentsize と e_phnum の積がテーブルサイズ (バイト単位) になる。
	// ファイルにプログラムヘッダーがない場合、 e_phnum は値 0 を保持する。
	DEBUGINFO("elf_header->e_phnum: %zu", elf_header->e_phnum);


	analyze_section_headers(m);
}
