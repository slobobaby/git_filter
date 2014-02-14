#ifndef LIST_H
#define LIST_H

typedef struct _git_filter_list_t git_filter_list_t;

/* filter comparison function */
typedef int git_filter_cmp(const void *key1, const void *key2);
typedef void git_filter_dump(const char *s, const void *key);

git_filter_list_t *list_init(git_filter_cmp *cmp, git_filter_dump *dump);

void list_add(git_filter_list_t *list, const void *key, const void *value);

const void *list_lookup(git_filter_list_t *list, const void *key);

void list_dump(git_filter_list_t *list);
#endif
