#include "nm.h"

// [定義済み構造体を独自構造体にマップする]
// システムで定義されているELF関連の構造体(Elf32_Ehdr や Elf64_Sym など)は, 32ビット/64ビットとで異なる定義を持つ。
// これを単一の構造体への代入によって吸収することを「マップする」と称している。
// なおこの際、エンディアンによる差異も同時に吸収する。
// (ELFのエンディアンとシステムのエンディアンが異なる場合、システムのエンディアンに合わせる)

// ELF64ヘッダ構造体をオブジェクトヘッダ構造体にマップする
void	map_elf64_header(const t_analysis* analysis, const t_elf_64_header* defined, t_object_header* original) {
	// DEBUGOUT("e_ident:\t%p", &defined->e_ident);
	// DEBUGOUT("e_type:\t%s", objtype_to_name(defined->e_type));
	// DEBUGOUT("e_machine:\t%s", machinetype_to_name(defined->e_machine));
	original->hsize     = SWAP_NEEDED(analysis, defined->e_ehsize);
	original->shoff     = SWAP_NEEDED(analysis, defined->e_shoff);
	original->shentsize = SWAP_NEEDED(analysis, defined->e_shentsize);
	original->shnum     = SWAP_NEEDED(analysis, defined->e_shnum);
	original->shstrndx  = SWAP_NEEDED(analysis, defined->e_shstrndx);
}

// ELF32ヘッダ構造体をオブジェクトヘッダ構造体にマップする
void	map_elf32_header(const t_analysis* analysis, const t_elf_32_header* defined, t_object_header* original) {
	original->hsize     = SWAP_NEEDED(analysis, defined->e_ehsize);
	original->shoff     = SWAP_NEEDED(analysis, defined->e_shoff);
	original->shentsize = SWAP_NEEDED(analysis, defined->e_shentsize);
	original->shnum     = SWAP_NEEDED(analysis, defined->e_shnum);
	original->shstrndx  = SWAP_NEEDED(analysis, defined->e_shstrndx);
}

// ELF64セクションヘッダ構造体をセクション構造体にマップする
void	map_elf64_section_header(const t_analysis* analysis, const t_elf_64_section_header* defined, t_section_unit* original) {
	original->name        = NULL;
	original->name_offset = SWAP_NEEDED(analysis, defined->sh_name);
	original->type        = SWAP_NEEDED(analysis, defined->sh_type);
	original->flags       = SWAP_NEEDED(analysis, defined->sh_flags);
	original->link        = SWAP_NEEDED(analysis, defined->sh_link);
	original->offset      = SWAP_NEEDED(analysis, defined->sh_offset);
	original->entsize     = SWAP_NEEDED(analysis, defined->sh_entsize);
	original->size        = SWAP_NEEDED(analysis, defined->sh_size);
	original->info        = SWAP_NEEDED(analysis, defined->sh_info);
}

// ELF32セクションヘッダ構造体をセクション構造体にマップする
void	map_elf32_section_header(const t_analysis* analysis, const t_elf_32_section_header* defined, t_section_unit* original) {
	original->name        = NULL;
	original->name_offset = SWAP_NEEDED(analysis, defined->sh_name);
	original->type        = SWAP_NEEDED(analysis, defined->sh_type);
	original->flags       = SWAP_NEEDED(analysis, defined->sh_flags);
	original->link        = SWAP_NEEDED(analysis, defined->sh_link);
	original->offset      = SWAP_NEEDED(analysis, defined->sh_offset);
	original->entsize     = SWAP_NEEDED(analysis, defined->sh_entsize);
	original->size        = SWAP_NEEDED(analysis, defined->sh_size);
	original->info        = SWAP_NEEDED(analysis, defined->sh_info);
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
void	map_elf64_symbol(const t_analysis* analysis, const t_elf_64_symbol* defined, t_symbol_unit* original) {
	original->name          = NULL;
	original->symbol_griff  = SYMGRIFF_UNKNOWN;
	original->address       = SWAP_NEEDED(analysis, defined->st_value);
	original->name_offset   = SWAP_NEEDED(analysis, defined->st_name);
	original->info          = SWAP_NEEDED(analysis, defined->st_info);
	original->shndx         = SWAP_NEEDED(analysis, defined->st_shndx);
	original->value         = SWAP_NEEDED(analysis, defined->st_value);
	original->size          = SWAP_NEEDED(analysis, defined->st_size);
	original->other         = SWAP_NEEDED(analysis, defined->st_other);
	original->bind          = ELF64_ST_BIND(original->info);
	original->type          = ELF64_ST_TYPE(original->info);
	original->visibility    = ELF64_ST_VISIBILITY(original->other);

	original->is_debug = false;
	original->is_global = false;
	original->is_undefined = false;
}

// ELF32シンボル構造体をシンボル構造体にマップする
void	map_elf32_symbol(const t_analysis* analysis, const t_elf_32_symbol* defined, t_symbol_unit* original) {
	original->name          = NULL;
	original->symbol_griff  = SYMGRIFF_UNKNOWN;
	original->address       = SWAP_NEEDED(analysis, defined->st_value);
	original->name_offset   = SWAP_NEEDED(analysis, defined->st_name);
	original->info          = SWAP_NEEDED(analysis, defined->st_info);
	original->shndx         = SWAP_NEEDED(analysis, defined->st_shndx);
	original->value         = SWAP_NEEDED(analysis, defined->st_value);
	original->size          = SWAP_NEEDED(analysis, defined->st_size);
	original->other         = SWAP_NEEDED(analysis, defined->st_other);
	original->bind          = ELF32_ST_BIND(original->info);
	original->type          = ELF32_ST_TYPE(original->info);
	original->visibility    = ELF32_ST_VISIBILITY(original->other);

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


