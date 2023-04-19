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

// nm.c
void	analyze_64bit(t_master* m);

// basic_utils.c
char*	yo_basename(const char* path);

// print_utils.c
void	print_error();

// printf.c
int	yoyo_dprintf(int fd, const char* format, ...);

// names.c
const char*	elfclass_to_name(int value);
const char*	sectiontype_to_name(int value);
const char*	symbinding_to_name(int value);
const char*	symtype_to_name(int value);

// assert.c
void	yoyo_assert(const char* strexp, bool exp, const char* file, unsigned int line, const char* func);
#define YOYO_ASSERT(exp) yoyo_assert(#exp, exp, __FILE__, __LINE__, __func__)

#endif

