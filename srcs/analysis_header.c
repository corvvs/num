#include "nm.h"

// [ELFヘッダの解析]

// [e_ident]
// EI_MAG0		0x7f
// EI_MAG1		'E'
// EI_MAG2		'L'
// EI_MAG3		'F'
// EI_CLASS		ファイルのクラス: 32bit or 64bit or 他(エラー)
// EI_DATA		データの符号化: リトルエンディアン or ビッグエンディアン or 他(エラー)
// EI_VERSION	ELFヘッダのバージョン: EV_CURRENT or 他(エラー)
// EI_PAD		パッドバイトの開始: e_ident のうち未使用部分の開始位置
// EI_NIDENT	e_ident[] のサイズ

#define PICK_E_IDENT(e_ident, index) (index < EI_PAD ? e_ident[index] : 0)

typedef unsigned char	*t_elf_identify;

// ファイルが { 0x7f, 'E', 'L', 'F' } で始まるか?
static bool	starts_with_elf(const t_elf_identify e_ident) {
	if (PICK_E_IDENT(e_ident, EI_MAG0) != 0x7f) { return false; }
	if (PICK_E_IDENT(e_ident, EI_MAG1) != 'E') { return false; }
	if (PICK_E_IDENT(e_ident, EI_MAG2) != 'L') { return false; }
	if (PICK_E_IDENT(e_ident, EI_MAG3) != 'F') { return false; }
	return true;
}

// ファイルが32ビットELFであるか?
static bool	is_32bit(const t_elf_identify e_ident) {
	return PICK_E_IDENT(e_ident, EI_CLASS) == ELFCLASS32;
}

// ファイルが64ビットELFであるか?
static bool	is_64bit(const t_elf_identify e_ident) {
	return PICK_E_IDENT(e_ident, EI_CLASS) == ELFCLASS64;
}

static t_target_category	get_elf_class(const t_elf_identify e_ident) {
	if (is_32bit(e_ident)) {
		return TC_ELF32;
	}
	if (is_64bit(e_ident)) {
		return TC_ELF64;
	}
	return TC_UNKNOWN;
}

// ELFヘッダのうち, e_ident[] の整合性をチェックする.
static bool	check_elf_ident_consistency(const t_master* m, const t_analysis* analysis) {
	(void)m;
	const t_elf_identify	e_ident = (t_elf_identify)analysis->target.head;
	if (!starts_with_elf(e_ident)) {
		// NOT A ELF
		return false;
	}
	if (PICK_E_IDENT(e_ident, EI_VERSION) != EV_CURRENT) {
		// NOT A VALID VERSION
		return false;
	}
	if (get_elf_class(e_ident) == TC_UNKNOWN) {
		// NOT A VALID CLASS
		return false;
	}
	return true;
}

bool	analyze_header(const t_master* m, t_analysis* analysis) {
	const t_target_file* target = &analysis->target;
	if (!check_elf_ident_consistency(m, analysis)) {
		print_recoverable_file_error_by_message(m, target->path, "file format not recognized");
		return false;
	}
	const t_elf_identify	e_ident = (t_elf_identify)analysis->target.head;
	const t_target_category	category = get_elf_class(e_ident);
	analysis->category = category;
	switch (category) {
		case TC_ELF32:
			map_elf32_header(target->head, &analysis->header);
			break;
		case TC_ELF64:
			map_elf64_header(target->head, &analysis->header);
			break;
		default:
			print_recoverable_file_error_by_message(m, target->path, "file format not recognized");
			return false;
	}
	return true;
}
