#include "nm.h"

// [ELFヘッダの解析]

static bool	is_32bit(const Elf32_Ehdr* elf_header) {
	return elf_header->e_ident[EI_CLASS] == ELFCLASS32;
}

static bool	is_64bit(const Elf32_Ehdr* elf_header) {
	return elf_header->e_ident[EI_CLASS] == ELFCLASS64;
}


t_target_category	determine_elf_type_from_header(const t_target_file* target) {
	// unsigned char	e_ident[EI_NIDENT];
	// の部分は 32/64 ビットで共通なので, 
	// とりあえず仮に64ビットとして e_ident の部分を読む.
	// ELFヘッダの情報を表示する

	Elf32_Ehdr* elf_header = (Elf32_Ehdr*)target->head;
	if (ft_memcmp(elf_header, (const char []){0x7f, 'E', 'L', 'F'}, 4)) {
		// NOT A ELF
		return TC_UNKNOWN;
	}

	// DEBUGINFO("e_ident: %s", elf_header->e_ident);
	if (is_32bit(elf_header)) {
		return TC_ELF32;
	}
	if (is_64bit(elf_header)) {
		return TC_ELF64;
	}
	return TC_UNKNOWN;
}


bool	analyze_header(const t_master* m, t_analysis* analysis) {
	const t_target_file* target = &analysis->target;
	t_target_category	category = determine_elf_type_from_header(target);
	analysis->category = category;
	switch (category) {
		case TC_ELF32:
			map_elf32_header(target->head, &analysis->header);
			break;
		case TC_ELF64:
			map_elf64_header(target->head, &analysis->header);
			break;
		default:
			print_recoverable_generic_error_by_message(m, target->path, "file format not recognized");
			return false;
	}
	return true;
}

