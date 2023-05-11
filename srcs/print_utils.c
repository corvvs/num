#include "nm.h"

void	print_recoverable_generic_error_by_errno(const t_master* m, const char* path) {
	print_recoverable_generic_error_by_message(m, path, strerror(errno));
}

void	print_recoverable_generic_error_by_message(const t_master* m, const char* path, const char* message) {
	if (path) {
		yoyo_dprintf(STDERR_FILENO, "%s: '%s': %s\n", m->exec_name, path, message);
	} else {
		yoyo_dprintf(STDERR_FILENO, "%s: %s\n", m->exec_name, message);
	}
}

void	print_unrecoverable_generic_error_by_errno(const t_master* m, const char* path) {
	print_unrecoverable_generic_error_by_message(m, path, strerror(errno));
}

void	print_unrecoverable_generic_error_by_message(const t_master* m, const char* path, const char* message) {
	if (path) {
		yoyo_dprintf(STDERR_FILENO, "%s: '%s': %s\n", m->exec_name, path, message);
	} else {
		yoyo_dprintf(STDERR_FILENO, "%s: %s\n", m->exec_name, message);
	}
	exit(1);
}

void	print_recoverable_file_error_by_message(const t_master* m, const char* path, const char* message) {
	if (path) {
		yoyo_dprintf(STDERR_FILENO, "%s: %s: %s\n", m->exec_name, path, message);
	} else {
		yoyo_dprintf(STDERR_FILENO, "%s: %s\n", m->exec_name, message);
	}
}

#define SPACES "                                                                                                   "

void	print_spaces(int fd, uint64_t n) {
	while (n > 0) {
		uint64_t	m = n > 80 ? 80 : n;
		n -= m;
		yoyo_print_direct(fd, SPACES, m);
	}
}

void	print_chars(int fd, char ch, uint64_t n) {
	while (n-- > 0) {
		yoyo_print_direct(fd, &ch, 1);
	}
}

void	debug_print_section(const t_section_unit* section) {
	(void)section;
	DEBUGINFO("sec: %zx -> %s(t:%llx)\t%s li:%llu\t%c%c%c%c%c%c%c%c%c%c%c%c%c%c %b\t%s",
		section->sec_offset,
		sectiontype_to_name(section->type), section->type,
		section_category_to_name(section->category),
		section->link,
		(section->flags & SHF_WRITE)			? 'w' : '-',
		(section->flags & SHF_ALLOC)			? 'a' : '-',
		(section->flags & SHF_EXECINSTR)		? 'x' : '-',
		(section->flags & SHF_MERGE)			? 'm' : '-',
		(section->flags & SHF_STRINGS)			? 's' : '-',
		(section->flags & SHF_INFO_LINK)		? 'i' : '-',
		(section->flags & SHF_LINK_ORDER)		? 'l' : '-',
		(section->flags & SHF_OS_NONCONFORMING)	? 'o' : '-',
		(section->flags & SHF_GROUP)			? 'g' : '-',
		(section->flags & SHF_TLS)				? 't' : '-',
		(section->flags & SHF_MASKOS)			? 'k' : '-',
		(section->flags & SHF_ORDERED)			? 'r' : '-',
		(section->flags & SHF_EXCLUDE)			? 'e' : '-',
		(section->flags & SHF_MASKPROC)			? 'p' : '-',
		section->flags,
		section->name
	);
}

void	debug_print_symbol(const t_symbol_unit* symbol) {
	(void)symbol;
	DEBUGOUT("%c bind:%s type:%s\t|%s|\tscat:%s vis:%s shndx:%zu b:%llu t:%llu i:%u addr: %p value: %llx size: %llu",
		symbol->symbol_griff,
		symbinding_to_name(symbol->bind),
		symtype_to_name(symbol->type),
		symbol->name,
		symbol->relevant_section ? section_category_to_name(symbol->relevant_section->category) : NULL,
		symbol_visibility_to_name(symbol->visibility),
		symbol->shndx,
		symbol->bind,
		symbol->type,
		symbol->info,
		symbol->address,
		symbol->value,
		symbol->size
	);
}
