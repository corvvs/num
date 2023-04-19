#include "nm.h"

// path にある対象ファイルをメモリ上に展開し, 情報を targer にセットする.
bool	deploy_target_file(const char* path, t_target_file* target) {
	int fd = open(path, O_RDONLY);
	if (fd < 0) {
		print_error();
	}

	// ファイルの情報を取得
	struct stat st;
	if (fstat(fd, &st)) {
		print_error();
	}

	DEBUGINFO("size: %zu", st.st_size);
	size_t size = st.st_size;
	if (size < sizeof(Elf32_Ehdr)) {
		// サイズ小さすぎ
		if (close(fd)) {
			print_error();
		}
		return false;
	}

	// ファイルをメモリにマップする
	void*	mem = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (mem == NULL) {
		print_error();
	}
	// fd は閉じてしまっても問題ない.
	if (close(fd)) {
		print_error();
	}

	target->path = path;
	target->head = mem;
	target->size = size;
	return true;
}
