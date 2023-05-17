#include "nm.h"

// path にある対象ファイルをメモリ上に展開し, 情報を target にセットする.
bool	deploy_analysis(const t_master* m, const char* path, t_target_file* target) {
	int fd = open(path, O_RDONLY);
	if (fd < 0) {
		if (errno == ENOENT) {
			print_recoverable_generic_error_by_message(m, path, "No such file");
		} else {
			print_recoverable_generic_error_by_errno(m, path);
		}
		return false;
	}

	// [ファイルの情報を取得]
	struct stat st;
	if (fstat(fd, &st)) {
		print_recoverable_generic_error_by_errno(m, path);
		close(fd);
		return false;
	}
	if (S_ISDIR(st.st_mode)) {
		yoyo_dprintf(STDERR_FILENO, "%s: Warning: '%s' is a directory\n", m->exec_name, path);
		close(fd);
		return false;
	}
	if (!S_ISREG(st.st_mode)) {
		yoyo_dprintf(STDERR_FILENO, "%s: Warning: '%s' is not an ordinary file\n", m->exec_name, path);
		close(fd);
		return false;
	}

	size_t size = st.st_size;
	if (size == 0) {
		// サイズが0の場合は何も表示しないでエラー終了
		return false;
	}
	if (size < sizeof(Elf32_Ehdr)) {
		// サイズ小さすぎ
		print_recoverable_file_error_by_message(m, path, "file format not recognized");
		close(fd);
		return false;
	}

	// ファイルをメモリにマップする
	void*	mem = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (mem == NULL) {
		print_recoverable_generic_error_by_errno(m, path);
		close(fd);
		return false;
	}
	// fd は閉じてしまっても問題ない.
	if (close(fd)) {
		print_unrecoverable_generic_error_by_errno(m, path);
	}

	target->path = path;
	target->head_addr = mem;
	target->size = size;
	// DEBUGINFO("mmapped: [%p, %p), size: %zu", mem, mem + size, size);
	return true;
}

void	destroy_target_file(const t_master* m, const t_target_file* target) {
	if (munmap(target->head_addr, target->size)) {
		print_unrecoverable_generic_error_by_errno(m, target->path);
	}
}
