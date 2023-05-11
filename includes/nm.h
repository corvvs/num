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
bool	deploy_analysis(const t_master* m, const char* path, t_target_file* target);
void	destroy_target_file(const t_master* m, const t_target_file* target);

// nm.c
void	analyze_64bit(t_master* m);

// structure_mapping.c
void	map_elf64_header(const t_analysis* analysis, const t_elf_64_header* defined, t_object_header* original);
void	map_elf32_header(const t_analysis* analysis, const t_elf_32_header* defined, t_object_header* original);
void	map_elf64_section_header(const t_analysis* analysis, const t_elf_64_section_header* defined, t_section_unit* original);
void	map_elf32_section_header(const t_analysis* analysis, const t_elf_32_section_header* defined, t_section_unit* original);
void	map_elf64_symbol(const t_analysis* analysis, const t_elf_64_symbol* defined, t_symbol_unit* original);
void	map_elf32_symbol(const t_analysis* analysis, const t_elf_32_symbol* defined, t_symbol_unit* original);
void	map_section_to_symbol_table(const t_section_unit* section, t_symbol_table_unit* table);
void	map_section_to_string_table(const t_section_unit* section, t_string_table_unit* table);
const t_section_unit*	get_referencing_section(const t_master* m, const t_analysis* analysis, const t_symbol_unit* symbol);

// analysis.c
bool	analyze_file(t_master* m, const char* target_path);

// analysis_header.c
bool	analyze_header(const t_master* m, t_analysis* analysis);

// section.c
void	determine_section_category(const t_master* m, const t_analysis* analysis, t_section_unit* section);

// symbol.c
bool	determine_symbol_name(const t_master* m, const t_analysis* analysis, const t_table_pair* table_pair, t_symbol_unit* symbol);
void	determine_symbol_griff(const t_master* m, const t_analysis* analysis, t_symbol_unit* symbol);

// basic_utils.c
char*	yo_basename(const char* path);


// consistency_utils.c
bool	is_size_mem_aligned(size_t size_val);
# define IS_SIZE_SATISFIED(size_val, type) ((size_val) >= sizeof(type))

// print_utils.c
void	print_recoverable_generic_error_by_errno(const t_master* m, const char* path);
void	print_recoverable_generic_error_by_message(const t_master* m, const char* path, const char* message);
void	print_unrecoverable_generic_error_by_errno(const t_master* m, const char* path);
void	print_unrecoverable_generic_error_by_message(const t_master* m, const char* path, const char* message);
void	print_recoverable_file_error_by_message(const t_master* m, const char* path, const char* message);
void	print_spaces(int fd, uint64_t n);
void	print_chars(int fd, char ch, uint64_t n);
void	debug_print_symbol(const t_symbol_unit* symbol);
void	debug_print_section(const t_section_unit* section);

// printf.c
int	yoyo_dprintf(int fd, const char* format, ...);
int	yoyo_print_direct(int fd, const void* data, size_t size);

// names.c
const char*	objtype_to_name(int value);
const char*	machinetype_to_name(int value);
const char*	elfclass_to_name(int value);
const char*	sectiontype_to_name(int value);
const char*	symbinding_to_name(int value);
const char*	symtype_to_name(int value);
const char* section_category_to_name(uint64_t value);
const char* symbol_visibility_to_name(uint64_t value);

// assert.c
void	yoyo_assert(const char* strexp, bool exp, const char* file, unsigned int line, const char* func);
# define YOYO_ASSERT(exp) yoyo_assert(#exp, exp, __FILE__, __LINE__, __func__)

// endian.c
uint16_t swap_2byte(uint16_t value);
uint32_t swap_4byte(uint32_t value);
uint64_t swap_8byte(uint64_t value);

# define SWAP_BYTE(value) (sizeof(value) < 2 ? (value) : sizeof(value) < 4 ? swap_2byte(value) : sizeof(value) < 8 ? swap_4byte(value) : swap_8byte(value))
# define SWAP_NEEDED(analysis, value) (analysis->system_endian == analysis->endian ? (value) : SWAP_BYTE(value))

#endif

