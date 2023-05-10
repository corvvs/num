#include "nm.h"

bool	is_size_mem_aligned(size_t size_val) {
	// TODO: 8 を実行時環境(無理ならコンパイル時環境)のアライメントに置き換える
	return size_val % 8 == 0;
}
