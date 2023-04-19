#include "nm.h"

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

	DEBUGINFO("e_ident: %s", elf_header->e_ident);
	if (is_32bit(elf_header)) {
		return TC_ELF32;
	}
	if (is_64bit(elf_header)) {
		return TC_ELF64;
	}
	return TC_UNKNOWN;
}

static const char*	get_exec_name(char** argv) {
#ifdef __MACH__
		return yo_basename(argv[0]);
#else
		return argv[0];
#endif
}


int main(int argc, char** argv) {
	// ファイルのオープン
	if (argc < 2) {
		print_error();
	}

	t_master	master = {0};
	master.exec_name = get_exec_name(argv);

	t_target_file*	target = &master.target_file;
	if (!deploy_target_file(argv[1], target)) {
		print_error();
	}

	// この時点で, 対象ファイルは少なくとも 32ビットELFヘッダ以上のサイズを持っていることが確定している.
	t_target_category	category = determine_elf_type_from_header(target);
	switch (category) {
		case TC_ELF32:
			DEBUGINFO("is 32bit: %p", target->head);
			break;
		case TC_ELF64:
			DEBUGINFO("is 64bit: %p", target->head);
			analyze_64bit(&master);
			break;
		default:
			DEBUGINFO("is unknown: %p", target->head);
	}
}
