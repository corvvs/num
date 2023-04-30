#ifndef STRUCTURE_H
#define STRUCTURE_H

# include <stdlib.h>
# include <stdbool.h>
# include <stdbool.h>
# include <elf.h>

// 事前宣言
struct s_section_unit;

// type alias
typedef Elf64_Ehdr t_elf_64_header;
typedef Elf64_Shdr t_elf_64_section_header;
typedef Elf64_Sym t_elf_64_symbol;

typedef Elf32_Ehdr t_elf_32_header;
typedef Elf32_Shdr t_elf_32_section_header;
typedef Elf32_Sym t_elf_32_symbol;

typedef enum e_target_category
{
	TC_UNKNOWN,
	TC_ELF32,
	TC_ELF64,
} t_target_category;

typedef enum e_section_category {
	// 絶対シンボル
	SC_SYM_ABS,
	// コモン or BSSシンボル
	SC_SYM_COMMON,
	// 未定義シンボル
	SC_SYM_UNDEF,
	// gotセクション
	SC_GOT,
	// データセクション
	SC_DATA,
	// 読み取り専用セクション
	SC_READONLY,
	// スモールオブジェクトセクション
	SC_SMALL,
	// テキストセクション
	SC_TEXT,
	// BSSセクション
	SC_BSS,
	// 
	SC_MERGEABLE_CHARACTER,
	// デバッグセクション
	SC_DEBUG,
	// 未定義セクション
	SC_UNDEFINED,
	SC_OTHER,
}	t_section_category;

# define SYMGRIFF_UNKNOWN '?'

typedef struct s_symbol_unit {
	size_t		address;      // シンボルのアドレス
	char		symbol_griff; // シンボルグリフ
	const char*	name;         // シンボルの名前; 文字列テーブル上のアドレスを指している。
	size_t		name_offset;  // name の文字列オフセット

	uint64_t	bind;
	uint64_t	type;
	unsigned char	info;
	size_t		shndx;
	uint64_t	value;
	uint64_t	size;

	bool		is_debug;     // デバッグシンボルかどうか(-a)
	bool		is_global;    // 外部シンボルかどうか(-g)
	bool		is_undefined; // 未定義シンボルかどうか(-u)

	const struct s_section_unit*	relevant_section;
} t_symbol_unit;

// (32/64ビット共通)
// セクションヘッダから抽出したセクションの情報
typedef struct s_section_unit {
	const char*	name; // セクション名; 文字列テーブル上のある位置を指す
	size_t		name_offset; // セクション名の文字列テーブルオフセット
	uint64_t	type; // タイプ
	uint64_t	flags; // フラグ
	uint64_t	link; // リンク先
	void*		head; // セクションのメモリマップアドレス
	size_t		offset; // セクションのファイルオフセット
	size_t		entsize; // エントリーサイズ
	size_t		size; // セクションのサイズ
	uint64_t	info;

	t_section_category category;
} t_section_unit;

// (32/64ビット共通)
// シンボルテーブルの情報を人間(おれ)に優しく整理したもの
typedef struct s_symbol_table_unit
{
	const t_section_unit*	section;
	// 実際のシンボルテーブルの先頭アドレス
	void *head;
	// ELFファイル先頭からのオフセット
	size_t offset;
	// エントリー数
	size_t num_entries;
	// 1エントリーのサイズ
	size_t entry_size;
	// シンボルテーブル全体のサイズ
	size_t total_size;
} t_symbol_table_unit;

// (32/64ビット共通)
// 文字列テーブルの情報を人間(おれ)に優しく整理したもの
typedef struct s_string_table_unit
{
	const t_section_unit*	section;
	// 実際の文字列テーブルの先頭アドレス
	void *head;
	// ELFファイル先頭からのオフセット
	size_t offset;
	// 文字列テーブル全体のサイズ
	size_t total_size;
} t_string_table_unit;

// (32/64ビット共通)
// シンボルテーブルセクションと, それに関連する文字列テーブルセクションとをペアにしたもの.
// リンクリストになっている.
typedef struct s_symbol_list_node
{
	t_symbol_table_unit symbol_table;
	t_string_table_unit string_table;
	struct s_symbol_list_node *next;
} t_table_pair;

typedef struct s_object_header {
	size_t	hsize;     // ヘッダのサイズ
	size_t	shoff;     // シンボルヘッダーテーブルのファイルオフセット
	size_t	shentsize; // シンボルヘッダーテーブルの要素サイズ
	size_t	shnum;     // シンボルヘッダーテーブルの要素数
	size_t	shstrndx;  // セクション名を持つ文字列テーブルセクションのインデックス
} t_object_header;

typedef struct s_target_file
{
	const char *path;
	// memmap された対象ファイルの先頭アドレス
	void *head;
	// 対象ファイルの真のサイズ
	size_t size;
} t_target_file;

// 64ビットの解析用データ
typedef struct s_analysis
{
	t_target_file	target;      // 対象ファイル
	int				error_no;    // このファイルで最後に発生したエラーの errno

	t_target_category	category;
	
	t_object_header	header;      // このオブジェクトファイル(ELFファイル)のヘッダ情報
	
	size_t			num_section; // セクションヘッダーテーブルに存在するであろうセクションの数
	t_section_unit*	sections;    // セクション構造体の配列; 要素数は num_section に等しい
	
	size_t				num_symbol_table;
	t_table_pair*	symbol_tables; // シンボルテーブルの配列

	size_t			num_symbol;           // このファイルに存在するであろうシンボルの総数
	size_t			num_symbol_effective; // ↑ のうち, 表示対象となるシンボルの数
	t_symbol_unit*	symbols;     // シンボル構造体の配列; 要素数は num_symbol に等しい
	t_symbol_unit**	sorted_symbols; // ソート後の symbols; ポインタの配列であることに注意

	// セクション名を格納する文字列テーブル
	size_t			section_name_str_table_index;
	t_string_table_unit	section_name_str_table;
	bool			found_section_name_str_table;
} t_analysis;

typedef struct s_option {
	bool	display_all;
	bool	display_only_external;
	bool	display_only_undefined;
	bool	reversed;
} t_option;

// プログラムの実行時の情報をすべて保持する構造体
// 与えられたオプションを解析した結果もここで保持する
typedef struct s_master
{
	const char*	exec_name;            // 実行ファイル名
	t_option		option;           // オプション情報
	size_t			num_target;       // 対象ファイル数
	const char**	target_names;     // 対象ファイルの名前の配列; 要素数は num_target に等しい
	size_t			i;                // 現在解析しているファイルのインデックス
	t_analysis		current_analysis; // 解析構造体
} t_master;

#endif
