#include "nm.h"

void	print_error_by_errno() {
	yoyo_dprintf(STDERR_FILENO, "%d, %s\n", errno, strerror(errno));
	exit(1);
}

void	print_error_by_message(t_master* m, const char* message) {
	(void)m;
	yoyo_dprintf(STDERR_FILENO, "error: %s\n", message);
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
