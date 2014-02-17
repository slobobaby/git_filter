#include <string.h>

#include "git_filter.h"

#include "dict.h"

int string_cmp(const void *k1, const void *k2)
{
    const char *s1 = (const char *)k1;
    const char *s2 = (const char *)k2;

#if 0
    printf("cmp %s %s\n", s1, s2);
#endif

    return strcmp(s1, s2);
}

void string_dump(void *p, const void *k, const void *v)
{
    const char *sk = (const char *)k;
    const char *sv = (const char *)v;

    printf("%s: %s\n", sk, sv);
}

int main()
{
    dict_t *dict = dict_init(string_cmp);

    A(dict == 0, "null dict");

#if 0
    dict_add(dict, "x", "y");
    dict_add(dict, "tine", "tmurphy");
    dict_add(dict, "thomas", "tmurphy");
    dict_add(dict, "liv", "lmurphy");
    dict_add(dict, "brian", "murphy");
    dict_add(dict, "a", "b");

    const void *v = dict_lookup(dict, "thomas");
    printf("lookup %s\n", (const char *)v);

    v = dict_lookup(dict, "a");
    printf("lookup %s\n", (const char *)v);

    v = dict_lookup(dict, "liv");
    printf("lookup %s\n", (const char *)v);
#endif

#if 0
    dict_add(dict, "a", "y");
    dict_add(dict, "b", "tmurphy");
    dict_add(dict, "c", "tmurphy");
    dict_add(dict, "d", "lmurphy");
    dict_add(dict, "e", "murphy");
    dict_add(dict, "f", "b");
#elif 1
    dict_add(dict, "f", "b");
    dict_add(dict, "e", "murphy");
    dict_add(dict, "d", "lmurphy");
    dict_add(dict, "c", "tmurphy");
    dict_add(dict, "b", "tmurphy");
    dict_add(dict, "a", "y");
#else
    dict_add(dict, "c", "tmurphy");
    dict_add(dict, "b", "tmurphy");
    dict_add(dict, "a", "y");
    dict_add(dict, "f", "b");
    dict_add(dict, "e", "murphy");
    dict_add(dict, "d", "lmurphy");
#endif

    dict_dump(dict, string_dump, 0);

    return 0;
}
