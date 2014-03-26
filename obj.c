/*
 * Copyright (C) 2014 Brian Murphy <brian@murphy.dk>
 *
 * This file is part of git_filter, distributed under the GNU GPL v2.
 * For full terms see the included COPYING file.
 *
 */

#define DEBUG 0

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "git2.h"

#include "git_filter.h"
#include "dict.h"

#define C(git2_call) do { \
    int _error = git2_call; \
    if (_error < 0) \
    { \
        const git_error *e = giterr_last(); \
        log("error %d at %d calling %s\n", _error, __LINE__, #git2_call); \
        log("%d: %s\n", e->klass, e->message); \
        exit(1); \
    } \
} while(0)

typedef struct _odb_cb_data_t
{
    git_repository *repo;
    unsigned int count;
    unsigned int blob_count;
    size_t size;
} odb_cb_data_t;

int odb_cb(const git_oid *id, void *payload)
{
    odb_cb_data_t *data = (odb_cb_data_t *)payload;
#if 1
    git_object *o;
#endif

#if 1
    char oids1[GIT_OID_HEXSZ+1];

#if 0
    printf("o: %s\n", git_oid_tostr(oids1, GIT_OID_HEXSZ+1, id));
#endif
#endif

#if 1
    C(git_object_lookup(&o, data->repo, id, GIT_OBJ_ANY));

    git_otype t = git_object_type(o);

    if (t == GIT_OBJ_BLOB)
    {
        git_off_t bsize = git_blob_rawsize((git_blob *)o);
        if (bsize > data->size)
        {
            printf("size %lld oid %s\n",
                    bsize, git_oid_tostr(oids1, GIT_OID_HEXSZ+1, id));
        }

        data->blob_count ++;
    }

    git_object_free(o);
#endif

    data->count ++;


    return 0;
}

int main(int argc, char *argv[])
{
    git_repository *repo;
    char *git_repo_name;
    git_odb *odb;
    odb_cb_data_t cb_data;

    if (argc < 2)
    {
        printf("please specify the location of a repository\n");
        printf("%s <repo>\n", argv[0]);
        exit(1);
    }

    git_repo_name = argv[1];

    C(git_repository_open(&repo, git_repo_name));

    C(git_repository_odb(&odb, repo));

    cb_data.repo = repo;
    cb_data.count = 0;
    cb_data.blob_count = 0;
    cb_data.size = 500000;

    C(git_odb_foreach(odb, odb_cb, &cb_data));

    printf("blobs: %d objects: %d\n", cb_data.blob_count, cb_data.count);

    git_odb_free(odb);

    return 0;
}
