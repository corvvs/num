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


bool	analyze_header(t_analysis* analysis) {
	const t_target_file* target = &analysis->target;
	t_target_category	category = determine_elf_type_from_header(target);
	analysis->category = category;
	switch (category) {
		case TC_ELF32:
			// DEBUGINFO("is 32bit: %p", target->head);
			map_elf32_header(target->head, &analysis->header);
			break;
		case TC_ELF64:
			// DEBUGINFO("is 64bit: %p", target->head);
			map_elf64_header(target->head, &analysis->header);
			// analyze_64bit(&master);
			break;
		default:
			// DEBUGINFO("is unknown: %p", target->head);
			return false;
	}
	return true;
}

