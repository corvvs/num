#ifndef STRUCTURE_H
# define STRUCTURE_H


// type alias
typedef Elf64_Ehdr	t_elf_64_header;
typedef Elf64_Shdr	t_elf_64_section_header;
typedef Elf64_Sym	t_elf_64_symbol;

typedef enum	e_target_category {
	TC_UNKNOWN,
	TC_ELF32,
	TC_ELF64,
}	t_target_category;


// (32/64ビット共通)
// シンボルテーブルの情報を人間(おれ)に優しく整理したもの
typedef struct	s_symbol_table {
	// 実際のシンボルテーブルの先頭アドレス
	void*	head;
	// ELFファイル先頭からのオフセット
	size_t	offset;
	// エントリー数
	size_t	num_entries;
	// 1エントリーのサイズ
	size_t	entry_size;
	// シンボルテーブル全体のサイズ
	size_t	total_size;
}	t_symbol_table;

// (32/64ビット共通)
// 文字列テーブルの情報を人間(おれ)に優しく整理したもの
typedef struct	s_string_table {
	// 実際の文字列テーブルの先頭アドレス
	void*	head;
	// ELFファイル先頭からのオフセット
	size_t	offset;
	// 文字列テーブル全体のサイズ
	size_t	total_size;
}	t_string_table;

// (32/64ビット共通)
// シンボルテーブルセクションと, それに関連する文字列テーブルセクションとをペアにしたもの.
// リンクリストになっている.
typedef struct	s_symbol_unit {
	t_symbol_table	symbol_table;
	t_string_table	string_table;
	struct s_symbol_unit*	next;
}	t_symbol_unit;


// 64ビットの解析用データ
typedef struct	s_analysis_64 {
	// ELFヘッダ(64ビット)
	const t_elf_64_header*	elf_header_64;
	// シンボルテーブル-文字列テーブルのペアリスト
	t_symbol_unit*			symbols;
	// リストの末尾
	t_symbol_unit*			symbols_tail;
}	t_analysis_64;


typedef struct s_target_file {
	const char*		path;
	// memmap された対象ファイルの先頭アドレス
	void*			head;
	// 対象ファイルの真のサイズ
	size_t			size;
}	t_target_file;

// プログラムの実行時の情報をすべて保持する構造体
// 与えられたオプションを解析した結果もここで保持する
typedef struct s_master {
	// 実行ファイル名
	const char*		exec_name;

	// 対象ファイル
	t_target_file	target_file;

	// 64ビットの解析用データ
	t_analysis_64	analysis_64;

}	t_master;

#endif
