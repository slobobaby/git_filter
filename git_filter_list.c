#include <string.h>

#include "git_filter.h"
#include "git_filter_list.h"

#define GIT_FILTER_LIST_CHUNK_SIZE 64

typedef struct _git_filter_list_elem_t
{
    const void *key;
    const void *value;
} git_filter_list_elem_t;

struct _git_filter_list_t 
{
    git_filter_cmp *cmp;
    git_filter_dump *dump;
    git_filter_list_elem_t *list;
    unsigned int list_len;
    unsigned int list_alloc;
};

git_filter_list_t *list_init(git_filter_cmp *cmp, git_filter_dump *dump)
{
    git_filter_list_t *list;

    list = (git_filter_list_t *)malloc(sizeof(git_filter_list_t));
    A(list == 0, "no memory");

    list->cmp = cmp;
    list->dump = dump;
    list->list = 0;
    list->list_len = 0;
    list->list_alloc  = 0;

    return list;
} 

static int get_pos(git_filter_list_t *list, const void *key)
{
    unsigned int start;
    unsigned int end;

    start = 0;
    end = list->list_len;

    for(;;)
    {
        unsigned int middle;
        int cmp;

        if (start == end)
            break;

        middle = (start + end)/2;

        cmp = list->cmp(list->list[middle].key, key);
        if (cmp == 0)
            return middle;
        if (cmp < 0)
            start = middle + 1;
        else
            end = middle;
    }

    return -(int)start-1;
}


void list_add(git_filter_list_t *list, const void *key, const void *value)
{
    int pos;
#if 0
    list->dump("add ", key);
#endif

    if (list->list_len == list->list_alloc)
    {
        list->list = realloc(list->list,
                sizeof(git_filter_list_elem_t)*
                    (GIT_FILTER_LIST_CHUNK_SIZE + list->list_alloc));
        A(list->list == 0, "realloc failed: no memory");
        list->list_alloc += GIT_FILTER_LIST_CHUNK_SIZE;
    }

    pos = get_pos(list, key);
    A(pos >= 0, "key exists\n");
    pos = -pos-1;

    if (list->list_len > pos)
    {
        unsigned int remainder = list->list_len - pos;
        memmove(&list->list[pos + 1], &list->list[pos],
                remainder * sizeof(git_filter_list_elem_t));

    }

    list->list[pos].key = key;
    list->list[pos].value = value;
    list->list_len ++;
}

const void *list_lookup(git_filter_list_t *list, const void *key)
{
    int pos = get_pos(list, key);
    if (pos < 0)
        return 0;

    return list->list[pos].value;
}

void list_dump(git_filter_list_t *list)
{
    unsigned int i;

    for(i=0; i<list->list_len;i++)
    {
        list->dump("key", list->list[i].key);
        list->dump("  v", list->list[i].value);
    }
}
