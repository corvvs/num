#ifndef NM_H
# define NM_H

# include <stdio.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <string.h>
# include <errno.h>
# include <stdbool.h>
# include <elf.h>

# include "common.h"
# include "structure.h"
# include "printf_internal.h"
# include "libft.h"

// file.c
bool	deploy_target_file(const char* path, t_target_file* target);
void	destroy_target_file(const t_target_file* target);

// nm.c
void	analyze_64bit(t_master* m);

// structure_mapping.c
void	map_elf64_header(const t_elf_64_header* defined, t_object_header* original);
void	map_elf32_header(const t_elf_32_header* defined, t_object_header* original);
void	map_elf64_section_header(const t_elf_64_section_header* defined, t_section_unit* original);
void	map_elf32_section_header(const t_elf_32_section_header* defined, t_section_unit* original);
void	map_elf64_symbol(const t_elf_64_symbol* defined, const t_string_table_unit* string_table, t_symbol_unit* original);
void	map_elf32_symbol(const t_elf_32_symbol* defined, const t_string_table_unit* string_table, t_symbol_unit* original);
void	map_section_to_symbol_table(const t_section_unit* section, t_symbol_table_unit* table);
void	map_section_to_string_table(const t_section_unit* section, t_string_table_unit* table);

// analysis.c
bool	analyze_file(t_master* m, const char* target_path);

// analysis_header.c
bool	analyze_header(t_analysis* analysis);

// symbol.c
void	determine_section_category(const t_master* m, const t_analysis* analysis, t_section_unit* section);
void	determine_symbol_griff(const t_master* m, const t_analysis* analysis, t_symbol_unit* symbol);

// basic_utils.c
char*	yo_basename(const char* path);

// print_utils.c
void	print_error_by_errno();
void	print_error_by_message(t_master* m, const char* message);
void	print_spaces(int fd, uint64_t n);
void	print_chars(int fd, char ch, uint64_t n);

// printf.c
int	yoyo_dprintf(int fd, const char* format, ...);
int	yoyo_print_direct(int fd, const void* data, size_t size);

// names.c
const char*	elfclass_to_name(int value);
const char*	sectiontype_to_name(int value);
const char*	symbinding_to_name(int value);
const char*	symtype_to_name(int value);
const char* section_category_to_name(uint64_t value);

// assert.c
void	yoyo_assert(const char* strexp, bool exp, const char* file, unsigned int line, const char* func);
#define YOYO_ASSERT(exp) yoyo_assert(#exp, exp, __FILE__, __LINE__, __func__)

#endif

