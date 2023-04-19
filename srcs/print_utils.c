#include "nm.h"

void	print_error() {
	yoyo_dprintf(STDERR_FILENO, "%d, %s\n", errno, strerror(errno));
	exit(1);
}
