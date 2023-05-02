#include "nm.h"

// 定義済み構造体を独自構造体にマップする。

// ELF64ヘッダ構造体をオブジェクトヘッダ構造体にマップする
void	map_elf64_header(const t_elf_64_header* defined, t_object_header* original) {
	original->hsize = defined->e_ehsize;
	original->shoff = defined->e_shoff;
	original->shentsize = defined->e_shentsize;
	original->shnum = defined->e_shnum;
	original->shstrndx = defined->e_shstrndx;
}

// ELF32ヘッダ構造体をオブジェクトヘッダ構造体にマップする
void	map_elf32_header(const t_elf_32_header* defined, t_object_header* original) {
	original->hsize = defined->e_ehsize;
	original->shoff = defined->e_shoff;
	original->shentsize = defined->e_shentsize;
	original->shnum = defined->e_shnum;
	original->shstrndx = defined->e_shstrndx;
}

// ELF64セクションヘッダ構造体をセクション構造体にマップする
void	map_elf64_section_header(const t_elf_64_section_header* defined, t_section_unit* original) {
	original->name = NULL;
	original->name_offset = defined->sh_name;
	original->type = defined->sh_type;
	original->flags = defined->sh_flags;
	original->link = defined->sh_link;
	original->offset = defined->sh_offset;
	original->entsize = defined->sh_entsize;
	original->size = defined->sh_size;
	original->info = defined->sh_info;
}

// ELF32セクションヘッダ構造体をセクション構造体にマップする
void	map_elf32_section_header(const t_elf_32_section_header* defined, t_section_unit* original) {
	original->name = NULL;
	original->name_offset = defined->sh_name;
	original->type = defined->sh_type;
	original->flags = defined->sh_flags;
	original->link = defined->sh_link;
	original->offset = defined->sh_offset;
	original->entsize = defined->sh_entsize;
	original->size = defined->sh_size;
	original->info = defined->sh_info;
}

const t_section_unit*	get_referencing_section(const t_master* m, const t_analysis* analysis, const t_symbol_unit* symbol) {
	(void)m;
	(void)analysis;
	(void)symbol;

	switch (symbol->shndx) {
		case SHN_UNDEF:
		case SHN_BEFORE:
		case SHN_AFTER:
		// case SHN_AMD64_LCOMMON:
		// case SHN_SUNW_IGNORE:
		case SHN_ABS:
		case SHN_COMMON:
		case SHN_XINDEX:
			break;
		default:
			YOYO_ASSERT(symbol->shndx < analysis->num_section);
			return &analysis->sections[symbol->shndx];
	}
	return NULL;
}

// ELF64シンボル構造体をシンボル構造体にマップする
void	map_elf64_symbol(const t_elf_64_symbol* defined, t_symbol_unit* original) {
	original->address = defined->st_value;
	original->name = NULL;
	original->name_offset = defined->st_name;
	original->symbol_griff = SYMGRIFF_UNKNOWN;

	original->bind = ELF64_ST_BIND(defined->st_info);
	original->type = ELF64_ST_TYPE(defined->st_info);
	original->info = defined->st_info;
	original->shndx = defined->st_shndx;
	original->value = defined->st_value;
	original->size = defined->st_size;
	original->visibility = ELF64_ST_VISIBILITY(defined->st_other);

	original->is_debug = false;
	original->is_global = false;
	original->is_undefined = false;
}

// ELF32シンボル構造体をシンボル構造体にマップする
void	map_elf32_symbol(const t_elf_32_symbol* defined, t_symbol_unit* original) {
	original->address = defined->st_value;
	original->name = NULL;
	original->name_offset = defined->st_name;
	original->symbol_griff = SYMGRIFF_UNKNOWN;

	original->bind = ELF32_ST_BIND(defined->st_info);
	original->type = ELF32_ST_TYPE(defined->st_info);
	original->info = defined->st_info;
	original->shndx = defined->st_shndx;
	original->value = defined->st_value;
	original->size = defined->st_size;
	original->visibility = ELF32_ST_VISIBILITY(defined->st_other);

	original->is_debug = false;
	original->is_global = false;
	original->is_undefined = false;
}

// セクションユニット構造体をシンボルテーブル構造体にマップする
void	map_section_to_symbol_table(const t_section_unit* section, t_symbol_table_unit* table) {
	table->section = section;
	table->head = section->head;
	table->offset = section->offset;
	table->entry_size = section->entsize;
	table->total_size = section->size;
	if (table->entry_size > 0) {
		table->num_entries = table->total_size / table->entry_size;
	} else {
		table->num_entries = 0;
	}
}

// セクションユニット構造体を文字列テーブル構造体にマップする
void	map_section_to_string_table(const t_section_unit* section, t_string_table_unit* table) {
	table->section = section;
	table->head = section->head;
	table->offset = section->offset;
	table->total_size = section->size;
}


