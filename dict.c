#include <string.h>

#include "git2.h"

#include "git_filter.h"
#include "dict.h"

#define GIT_FILTER_LIST_CHUNK_SIZE 64

typedef struct _dict_elem_t
{
    git_oid key;
    const void *value;
} dict_elem_t;

struct _dict_t 
{
    dict_elem_t *dict;
    unsigned int dict_len;
    unsigned int dict_alloc;
};

dict_t *dict_init()
{
    dict_t *dict;

    dict = (dict_t *)malloc(sizeof(dict_t));
    A(dict == 0, "no memory");

    dict->dict = 0;
    dict->dict_len = 0;
    dict->dict_alloc  = 0;

    return dict;
} 

static int get_pos(dict_t *dict, const void *key)
{
    unsigned int start;
    unsigned int end;

    start = 0;
    end = dict->dict_len;

    for(;;)
    {
        unsigned int middle;
        int cmp;

        if (start == end)
            break;

        middle = (start + end)/2;

        cmp = git_oid_cmp(&dict->dict[middle].key, key);
        if (cmp == 0)
            return middle;
        if (cmp < 0)
            start = middle + 1;
        else
            end = middle;
    }

    return -(int)start-1;
}


void dict_add(dict_t *dict, const git_oid *key, const void *value)
{
    int pos;

    if (dict->dict_len == dict->dict_alloc)
    {
        dict->dict = realloc(dict->dict,
                sizeof(dict_elem_t)*
                    (GIT_FILTER_LIST_CHUNK_SIZE + dict->dict_alloc));
        A(dict->dict == 0, "realloc failed: no memory");
        dict->dict_alloc += GIT_FILTER_LIST_CHUNK_SIZE;
    }

    pos = get_pos(dict, key);
    A(pos >= 0, "key exists\n");
    pos = -pos-1;

    if (dict->dict_len > pos)
    {
        unsigned int remainder = dict->dict_len - pos;
        memmove(&dict->dict[pos + 1], &dict->dict[pos],
                remainder * sizeof(dict_elem_t));

    }

    dict->dict[pos].key = *key;
    dict->dict[pos].value = value;
    dict->dict_len ++;
}

const void *dict_lookup(dict_t *dict, const git_oid *key)
{
    int pos = get_pos(dict, key);
    if (pos < 0)
        return 0;

    return dict->dict[pos].value;
}

void dict_dump(dict_t *dict, dict_dump_t *dump, void *data)
{
    unsigned int i;

    for(i=0; i<dict->dict_len;i++)
        dump(data, &dict->dict[i].key, dict->dict[i].value);
}
