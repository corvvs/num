#include "nm.h"

// path にある対象ファイルをメモリ上に展開し, 情報を target にセットする.
bool	mmap_target_file(const t_master* m, const char* path, t_target_file* target) {
	int fd = open(path, O_RDONLY);
	if (fd < 0) {
		if (errno == ENOENT) {
			print_recoverable_error_by_message(m, path, "No such file");
		} else {
			print_recoverable_error_by_errno(m, path);
		}
		return false;
	}

	// [ファイルの情報を取得]
	struct stat st;
	if (fstat(fd, &st)) {
		print_recoverable_error_by_errno(m, path);
		close(fd);
		return false;
	}

	DEBUGINFO("size: %zu", st.st_size);
	size_t size = st.st_size;
	if (size < sizeof(Elf32_Ehdr)) {
		// サイズ小さすぎ
		// TODO: なんか出す
		close(fd);
		return false;
	}

	// ファイルをメモリにマップする
	void*	mem = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (mem == NULL) {
		print_recoverable_error_by_errno(m, path);
		close(fd);
		return false;
	}
	// fd は閉じてしまっても問題ない.
	if (close(fd)) {
		print_unrecoverable_error_by_errno(m, path);
	}

	target->path = path;
	target->head = mem;
	target->size = size;
	return true;
}

void	destroy_target_file(const t_master* m, const t_target_file* target) {
	if (munmap(target->head, target->size)) {
		print_unrecoverable_error_by_errno(m, target->path);
	}
}
