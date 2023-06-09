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
				yoyo_dprintf(STDERR_FILENO, "%s: invalid option -- '%c'\n", m->exec_name, arg[j]);
				yoyo_dprintf(STDERR_FILENO, "Usage: %s [option(s)] [file(s)]\n", m->exec_name);
				exit(1);
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

static t_endian	get_system_endian(void) {
	uint32_t	x = 1;
	uint8_t*	c = (uint8_t*)&x;
	return *c ? END_LITTLE : END_BIG;
}

const char*	default_target = "a.out";

static void	seed_fuzz(void) {
#ifdef DO_FUZZING
	struct timeval time_now = {};
	gettimeofday(&time_now, NULL);
	unsigned int seed = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);
	// seed = 664490666;
	DEBUGWARN("SEED: %u", seed);
	srand(seed);
#endif
}

int main(int argc, char** argv) {
	seed_fuzz();

	// [想定外の場合ただちに終了]
	if (argc < 1) {
		exit(1);
	}

	// [マスター構造体の初期化]
	t_master	master = {
		.exec_name = get_exec_name(argv),
		.option = {0},
		.system_endian = get_system_endian(),
	};

	// [オプション解析]
	parse_option(&master, argc, argv);

	// [対象ファイル名の設定]
	if (master.num_target < 1) {
		master.num_target = 1;
		master.target_names = &default_target;
	}

	int status = 0;
	// [対象ファイルの解析]
	for (master.i = 0; master.i < master.num_target; ++master.i) {
		if (!analyze_file(&master, master.target_names[master.i])) {
			status = (status + 1) % 256;
		}
	}
	return status;
}
