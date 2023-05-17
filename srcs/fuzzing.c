#include "nm.h"

// ファジング
#ifdef DO_FUZZING
const int	fuzz_values[] = {0, 1, 2, 7, 8, 16, 24, 32, 63, 64, -1};
# define FUZZ_VALUE_(type) ((type)(fuzz_values[rand() % (sizeof(fuzz_values) / sizeof(int))] * sizeof(type)))
# define FUZZ_VALUE(type) (rand() % 2 == 0 ? FUZZ_VALUE_(type) : -FUZZ_VALUE_(type))

static bool	do_fuzz(void) {
	return rand() % 1007 < 10;
}

uint8_t	fuzz_1byte(uint8_t value) {
	if (!do_fuzz()) { return value; }
	return value + FUZZ_VALUE(uint8_t);
}

uint16_t fuzz_2byte(uint16_t value) {
	if (!do_fuzz()) { return value; }
	return value + FUZZ_VALUE(uint16_t);
}

uint32_t fuzz_4byte(uint32_t value) {
	if (!do_fuzz()) { return value; }
	return value + FUZZ_VALUE(uint32_t);
}

uint64_t fuzz_8byte(uint64_t value) {
	if (!do_fuzz()) { return value; }
	return value + FUZZ_VALUE(uint64_t);
}
#endif
