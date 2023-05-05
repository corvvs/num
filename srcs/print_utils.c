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
