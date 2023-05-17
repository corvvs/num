#include "nm.h"

// path の basename 部分先頭のポインタを返す
char*	yo_basename(const char* path) {
	char*	last_slash = ft_strrchr(path, '/');
	if (last_slash == NULL) {
		return (char *)path;
	} else {
		return (char *)(last_slash + 1);
	}
}

// 文字列s1, s2が等しいことをチェックする(どっちもNULLの場合はtrue)
bool	yo_str_equal(const char* s1, const char* s2) {
	if (s1 == s2) { return true; }
	if (s1 == NULL || s2 == NULL) { return false; }
	return ft_strcmp(s1, s2) == 0;
}
