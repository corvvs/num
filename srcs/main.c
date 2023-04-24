#include "nm.h"

static const char*	get_exec_name(char** argv) {
#ifdef __MACH__
		return yo_basename(argv[0]);
#else
		return argv[0];
#endif
}

const char*	default_target = "a.out";

int main(int argc, char** argv) {
	// ファイルのオープン
	if (argc < 2) {
		print_error_by_errno();
	}

	// TODO: オプション解析

	// TODO: ファイルが指定されていない場合は a.out を対象とする
	// TODO: マルチファイル対応

	// [マスター構造体の初期化]
	t_master	master = {
		.exec_name = get_exec_name(argv),
	};

	// [対象ファイル名の設定]
	if (argc >= 2) {
		size_t arg_offset = 1;
		// 対象ファイルが1つ以上与えられている場合
		master.num_target = argc - arg_offset;
		master.target_names = (const char**)(argv + arg_offset);
	} else {
		master.num_target = 1;
		master.target_names = &default_target;
	}

	for (master.i = 0; master.i < master.num_target; ++master.i) {
		if (!analyze_file(&master, master.target_names[master.i])) {
			// エラー発生時
			break;
		}
	}
}
