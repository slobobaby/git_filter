#ifndef LIST_H
#define LIST_H

#include "git2.h"

typedef struct _dict_t dict_t;

/* filter comparison function */
typedef int dict_cmp_t(const void *key1, const void *key2);
typedef void dict_dump_t(void *data, const git_oid *key, const void *val);

dict_t *dict_init(void);

void dict_add(dict_t *dict, const git_oid *key, const void *value);

const void *dict_lookup(dict_t *dict, const git_oid *key);

void dict_dump(dict_t *dict, dict_dump_t *dump, void *data);
#endif
