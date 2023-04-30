#include "nm.h"

static const char*	get_exec_name(char** argv) {
#ifdef __MACH__
		return yo_basename(argv[0]);
#else
		return argv[0];
#endif
}

void	parse_option(t_master* m, int argc, char **argv) {
	int i;
	for (i = 1; i < argc; ++i) {
		const char* arg = argv[i];
		if (!(arg[0] == '-' && arg[1])) { break; }
		for (size_t j = 1; arg[j]; ++j) {
			if (arg[j] == 'a') {
				m->option.display_all = true;
			} else if (arg[j] == 'g') {
				m->option.display_only_external = true;
			} else if (arg[j] == 'u') {
				m->option.display_only_undefined = true;
			} else if (arg[j] == 'r') {
				m->option.reversed = true;
			} else if (arg[j] == 'p') {
				m->option.without_sorting = true;
			} else {
				// TODO: invalid option
				break;
			}
		}
	}

	// TODO 本当にこうか？
	if (m->option.display_only_external || m->option.display_only_undefined) {
		m->option.display_all = false;
	}

	m->num_target = argc - i;
	m->target_names = (const char**)(argv + i);
}

const char*	default_target = "a.out";

int main(int argc, char** argv) {
	// ファイルのオープン
	if (argc < 1) {
		print_error_by_errno();
	}

	t_master	master = {
		.exec_name = get_exec_name(argv),
		.option = {0},
	};

	parse_option(&master, argc, argv);

	// TODO: ファイルが指定されていない場合は a.out を対象とする
	// TODO: マルチファイル対応

	// [マスター構造体の初期化]

	// [対象ファイル名の設定]
	DEBUGOUT("num_target: %zu", master.num_target);
	if (master.num_target < 1) {
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
