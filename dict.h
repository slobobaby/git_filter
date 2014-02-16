#ifndef LIST_H
#define LIST_H

typedef struct _dict_t dict_t;

/* filter comparison function */
typedef int dict_cmp_t(const void *key1, const void *key2);
typedef void dict_dump_t(void *data, const void *key, const void *val);

dict_t *dict_init(dict_cmp_t *cmp);

void dict_add(dict_t *dict, const void *key, const void *value);

const void *dict_lookup(dict_t *dict, const void *key);

void dict_dump(dict_t *dict, dict_dump_t *dump, void *data);
#endif
