#include <string.h>

#include "git_filter.h"

#include "git_filter_list.h"

int string_cmp(const void *k1, const void *k2)
{
    const char *s1 = (const char *)k1;
    const char *s2 = (const char *)k2;

#if 0
    printf("cmp %s %s\n", s1, s2);
#endif

    return strcmp(s1, s2);
}

void string_dump(const char *p, const void *k)
{
    const char *s = (const char *)k;

    printf("%s: %s\n", p, s);
}

int main()
{
    git_filter_list_t *list = list_init(string_cmp, string_dump);

    A(list == 0, "null list");

#if 0
    list_add(list, "x", "y");
    list_add(list, "tine", "tmurphy");
    list_add(list, "thomas", "tmurphy");
    list_add(list, "liv", "lmurphy");
    list_add(list, "brian", "murphy");
    list_add(list, "a", "b");

    const void *v = list_lookup(list, "thomas");
    printf("lookup %s\n", (const char *)v);

    v = list_lookup(list, "a");
    printf("lookup %s\n", (const char *)v);

    v = list_lookup(list, "liv");
    printf("lookup %s\n", (const char *)v);
#endif

#if 0
    list_add(list, "a", "y");
    list_add(list, "b", "tmurphy");
    list_add(list, "c", "tmurphy");
    list_add(list, "d", "lmurphy");
    list_add(list, "e", "murphy");
    list_add(list, "f", "b");
#elif 1
    list_add(list, "f", "b");
    list_add(list, "e", "murphy");
    list_add(list, "d", "lmurphy");
    list_add(list, "c", "tmurphy");
    list_add(list, "b", "tmurphy");
    list_add(list, "a", "y");
#else
    list_add(list, "c", "tmurphy");
    list_add(list, "b", "tmurphy");
    list_add(list, "a", "y");
    list_add(list, "f", "b");
    list_add(list, "e", "murphy");
    list_add(list, "d", "lmurphy");
#endif

    list_dump(list);

}
