#include "nm.h"

#ifdef NDEBUG
void	yoyo_assert(const char* strexp, bool exp, const char* file, unsigned int line, const char* func) {
	(void)strexp;
	(void)exp;
	(void)file;
	(void)line;
	(void)func;
}
#else
void	yoyo_assert(const char* strexp, bool exp, const char* file, unsigned int line, const char* func) {
	if (exp) { return; }
	yoyo_dprintf(STDERR_FILENO, "%sassertion failed: `%s' at %s:%u %s%s\n", BG_RED, strexp, file, line, func, TX_RST);
	exit(100);
}
#endif
