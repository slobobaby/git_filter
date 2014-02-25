/*
 * Copyright (C) 2014 Brian Murphy <brian@murphy.dk>
 *
 * This file is part of git_filter, distributed under the GNU GPL v2.
 * For full terms see the included COPYING file.
 *
 */

#define DEBUG 0
#define MALLOC_STATS 0

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#if MALLOC_STATS
#include <malloc.h>
#endif
#include <regex.h>

#include "git2.h"

#include "git_filter.h"
#include "dict.h"

#if 1
const char *oid_list[] = {
"ef8d9f70aec935c5e0c898d92f2b18da7cad8b22",
"6a7acb578f0db4ede36b60bbae761a5b428b21f5",
"7d38773b790b6d3d95e7956ce7598b6907387c41",
"635a336addd811d45ce372037ce55551073e8c6a",
"d557c99975a624c4be98219a8a1048af6f4c5130",
"0a2aa5c39eb7064933cb14ad8eb492cbd4fe7fb5",
"c3c8ef2d99d93ea5dec5557a58df9619dcf6d0e7",
"0487738c1eac740d55cb898dd0e31affa567714a",
"c3501f769f3f924d7e27e94dc6a5c3343d77b7d8",
"bc26b170c8c81c8b26607eea31c61af552b77344",
"2104814361a95252c905df0dda8ca38036d0b4ba",
"486378cc1be6b310832d119ef5a39709767c5561",
"98d6a316ee34763a9b2cb104352b9afc7f543cee",
"ae8b2dbb9dcdf27b41f5b05db7b4c48b24a06043",
"3b7cd63936d0420a367218f7d927f492ea09beb3",
"8f74aa2a1571adab33b007981b145713a1d70e71"
};

#define LIST_LEN (sizeof(oid_list)/sizeof(oid_list[0]))

static git_oid search_oid[LIST_LEN];
#endif

#define STACK_CHUNKS 32
#define INCLUDE_CHUNKS 1024
#define TAG_INFO_CHUNKS 128
#define TF_LIST_CHUNKS 32
#define CL_CHUNKS 16
#define BUFLEN 128

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

#define tree_equal(tree1, tree2) git_oid_equal(git_tree_id(tree1), \
            git_tree_id(tree2))

typedef struct _include_dirs {
    char **dirs;
    unsigned int alloc;
    unsigned int len;
} include_dirs_t;

typedef struct _tree_filter {
    const char *name;;
    const char *include_file;
    include_dirs_t id;

    git_oid last;

    git_repository *repo;
    dict_t *revdict;

    dict_t *deleted_merges;
    dict_t *deleted_commits;

    char first;
} tree_filter_t;

typedef struct _rev_info_dump_t
{
    FILE *f;
    const char *prefix;
} rev_info_dump_t;

typedef struct _dirstack_item_t {
    git_treebuilder *tb;
    char *name;
} dirstack_item_t;

typedef struct _dirstack_t {
    dirstack_item_t *item;
    unsigned int alloc;
    unsigned int depth;
    git_repository *repo;
} dirstack_t;

typedef struct _commit_list_t
{
    const git_commit **list;
    unsigned int len;
    unsigned int alloc;
} commit_list_t;

static char *git_repo_name = 0;
static char *git_repo_suffix = "";
static char *git_tag_prefix = 0;
static char *rev_type = 0;
static char *rev_string = 0;
static char continue_run = 0;

static unsigned int tf_len = 0;
static tree_filter_t *tf_list;
static unsigned int tf_list_alloc = 0;

static void tf_list_new(const char *name, const char *file)
{
    if (tf_len >= tf_list_alloc)
    {
        tf_list_alloc += TF_LIST_CHUNKS;
        tf_list = realloc(tf_list, tf_list_alloc *
                sizeof(tree_filter_t));
        memset(&tf_list[tf_list_alloc - TF_LIST_CHUNKS], 0,
                TF_LIST_CHUNKS * sizeof(tree_filter_t));
    }

    tf_list[tf_len].name = name;
    tf_list[tf_len].include_file = file;

    tf_len ++;
}

char *local_sprintf(const char *format, ...)
{
    va_list ap;
    int cnt;
    char *out = malloc(BUFLEN);

    A(out == 0, "no memory");

    va_start(ap, format);
    cnt = vsnprintf(out, BUFLEN, format, ap);
    va_end(ap);

    if (cnt > BUFLEN)
    {
        cnt ++;
        out = realloc(out, cnt);
        A(out == 0, "no memory");

        va_start(ap, format);
        vsnprintf(out, cnt, format, ap);
        va_end(ap);
    }

    return out;
}

static char *savefile(const char *name, const char *suffix)
{
    return local_sprintf("%s/%s%s%s%s",
            git_repo_name, git_repo_suffix, git_tag_prefix, name, suffix);
}

char *local_fgets(FILE *f)
{
    char *line = 0;
    char *r;
    unsigned int len = BUFLEN;
    size_t slen = 0;

    do {
        line = realloc(line, len);
        A(line == 0, "no memory");

        r = fgets(&line[slen], len-slen, f);
        if (!r)
        {
            if (slen != 0)
                break;
            free(line);
            return 0;
        }
        slen = strlen(line);
        len += BUFLEN;
    } while(line[slen-1] != '\n');

    line[slen - 1] = 0;
    return line;
}

void _rev_info_dump(void *d, const git_oid *k, const void *v)
{
    rev_info_dump_t *ri = (rev_info_dump_t *)d;
    char oids1[GIT_OID_HEXSZ+1];
    char oids2[GIT_OID_HEXSZ+1];
    const git_oid *o = (const git_oid *)k;
    const git_oid *cid = git_commit_id((const git_commit *)v);

    fprintf(ri->f, "%s%s %s\n", ri->prefix,
            git_oid_tostr(oids1, GIT_OID_HEXSZ+1, o),
            git_oid_tostr(oids2, GIT_OID_HEXSZ+1, cid)
           );
}

void rev_info_dump(tree_filter_t *tf)
{
    FILE *f;
    rev_info_dump_t ri;
    char *full_path = savefile(tf->name, ".revinfo");

    f = fopen(full_path, "w");
    if (!f)
        die("Cannot open %s.\n", full_path);

    ri.f = f;
    ri.prefix = "r:";
    dict_dump(tf->revdict, _rev_info_dump, &ri);
    ri.prefix = "m:";
    dict_dump(tf->deleted_merges, _rev_info_dump, &ri);
    ri.prefix = "d:";
    dict_dump(tf->deleted_commits, _rev_info_dump, &ri);

    fclose(f);

    free(full_path);
}

/* string sorting callback for qsort */
int sort_string(const void *a, const void *b)
{
    const char **stra = (const char **)a;
    const char **strb = (const char **)b;

    return strcmp(*stra, *strb);
}

void include_dirs_init(include_dirs_t *id, const char *file)
{
    FILE *f;
    int i;

    id->dirs = malloc(sizeof (char *) * INCLUDE_CHUNKS);
    id->alloc = INCLUDE_CHUNKS;
    id->len = 0;

    f = fopen(file, "r");
    if (!f)
        die("Cannot open %s.\n", file);

    while(!feof(f))
    {
        char *e = local_fgets(f);
        if (!e)
            break;
        if (id->len == id->alloc)
        {
            id->alloc += INCLUDE_CHUNKS;
            id->dirs = realloc(id->dirs,
                    id->alloc * sizeof (char *));
        }

        id->dirs[id->len] = e;
        id->len++;
    }

    fclose(f);

    qsort(id->dirs, id->len, sizeof(char *), sort_string);

    for (i=1; i<id->len; i++)
    {
        if (!strcmp(id->dirs[i], id->dirs[i-1]))
            die("%s: duplicate entries for '%s'", file, id->dirs[i]);

        unsigned int cmplen = strlen(id->dirs[i-1]);
        if (!strncmp(id->dirs[i], id->dirs[i-1], cmplen)
                && id->dirs[i][cmplen] == '/')
            die("%s: '%s' is a subdir of '%s'",
                    file, id->dirs[i], id->dirs[i-1]);
    }
}

static void save_last_commit(git_oid *commit_id, const char *filename)
{
        char oids[GIT_OID_HEXSZ+1];

        FILE *f = fopen(filename, "w");
        if (!f)
        {
            printf("WARNING: could not write last commit id file %s", filename);
            return;
        }

        fprintf(f, "%s\n", git_oid_tostr(oids, GIT_OID_HEXSZ+1, commit_id));

        fclose(f);
}


static void read_last_commit(git_oid *commit_id, const char *filename)
{
        FILE *f = fopen(filename, "r");
        if (!f)
            die("Could not open last commit id file %s", filename);

        char *e = local_fgets(f);
        if (!e)
            die("Could not read last commit id file %s", filename);

        C(git_oid_fromstr(commit_id, e));

        free(e);

        fclose(f);
}


static unsigned int read_revinfo(
        dict_t *revdict, dict_t *deleted_merges, dict_t *deleted_commits,
        git_repository *repo, const char *filename)
{
    unsigned int lineno = 0;
    FILE *f = fopen(filename, "r");
    if (!f)
        die("Could not open %s", filename);

    for (;;)
    {
        char *e = local_fgets(f);
        char *v;
        char *k;
        git_oid coid;
        git_commit *commit;

        if (e == 0)
            break;

        lineno ++;

        k = e + 2;
        v = strstr(e, " ");
        if (!v)
            die("could not parse line %d of %s", lineno, filename);
        v += 1;

        git_oid oida;

        if (git_oid_fromstr(&oida, k) != 0)
            die("could not parse line %d of %s", lineno, filename);

        if (git_oid_fromstr(&coid, v) != 0)
            die("could not parse line %d of %s", lineno, filename);

        C(git_commit_lookup(&commit, repo, &coid));

        switch (e[0])
        {
            case 'r':
                dict_add(revdict, &oida, commit);
                break;
            case 'm':
                dict_add(deleted_merges, &oida, commit);
                break;
            case 'd':
                dict_add(deleted_commits, &oida, commit);
                break;
            default:
                die("illegal entry at line %d of %s", lineno, filename);
        }

        free(e);
    }

    fclose(f);

    return lineno;
}

void tree_filter_init(tree_filter_t *tf, git_repository *repo)
{
    int count = 0;
    include_dirs_init(&tf->id, tf->include_file);

    tf->repo = repo;

    tf->revdict = dict_init();

    tf->deleted_merges = dict_init();

    tf->deleted_commits = dict_init();

    if (continue_run)
    {
        char *full_path = savefile(tf->name, ".revinfo");
        count = read_revinfo(tf->revdict, tf->deleted_merges,
                       tf->deleted_commits, repo, full_path);
        free(full_path);
    }

    if (count == 0)
        tf->first = 1;
}

void tree_filter_fini(tree_filter_t *tf)
{
}

dirstack_item_t *stack_get_item(dirstack_t *stack, int level)
{
    if (stack->alloc <= level)
    {
        stack->alloc += STACK_CHUNKS;

        stack->item = realloc(stack->item,
                stack->alloc * sizeof(dirstack_item_t));
        A(stack->item == 0, "no memory");

        memset(&stack->item[stack->alloc-STACK_CHUNKS], 0,
                STACK_CHUNKS * sizeof(dirstack_item_t));
    }
    return &stack->item[level];
}

void _stack_close_to(dirstack_t *stack, unsigned int level)
{
    unsigned int i;

    for (i = stack->depth - 1; i >= level; i--)
    {
        dirstack_item_t *cur = stack_get_item(stack, i);
        dirstack_item_t *prev = stack_get_item(stack, i-1);
        git_oid new_oid;

        C(git_treebuilder_write(&new_oid, stack->repo, cur->tb));
        git_treebuilder_free(cur->tb);

        C(git_treebuilder_insert(0, prev->tb, cur->name,
                    &new_oid, GIT_FILEMODE_TREE));

        free(cur->name);
        cur->name = 0;
        cur->tb = 0;
    }

    stack->depth = level;
}

void _handle_stack(dirstack_t *stack, char **path_c, unsigned int len)
{
    dirstack_item_t *s;
    unsigned int level;

    if (len == 0)
        return;

    for (level = 1; level <= len; level++)
    {
        s = stack_get_item(stack, level);

        if (!s->name)
        {
            s->name = strdup(path_c[level-1]);
            C(git_treebuilder_create(&s->tb, 0));
            stack->depth = level + 1;
            continue;
        }

        if (!strcmp(s->name, path_c[level-1]))
            continue;

        A(s->tb == 0, "stack overflow\n");

        _stack_close_to(stack, level);

        A(stack->depth != level, "stack error");
        A(s->name != 0, "stack error");
        A(s->tb != 0, "stack error");

        C(git_treebuilder_create(&s->tb, 0));
        s->name = strdup(path_c[level-1]);
        stack->depth = level + 1;
    }
}

void stack_open(dirstack_t *stack, git_repository *repo)
{
    dirstack_item_t *di;
    memset(stack, 0, sizeof(*stack));

    di = stack_get_item(stack, 0);

    C(git_treebuilder_create(&di->tb, 0));
    stack->depth = 1;
    stack->repo = repo;
}


#define add_pathc(p, item) do { \
    p[cnt++] = last; \
    if (cnt == path_size) \
    { \
        *p += STACK_CHUNKS; \
        p = realloc(p, path_size * sizeof(char *)); \
        A(p == 0, "no memory"); \
    } \
} while(0)


/* modifies path */
unsigned int split_path(char ***path_sp, char *path)
{
    char *next;
    char *last = path;
    unsigned int cnt;
    unsigned int path_size = STACK_CHUNKS;
    char **p = malloc(path_size * sizeof(char *));
    A(p == 0, "no memory");

    cnt = 0;
    while ((next = strchr(last, '/')))
    {
        *next = 0;

        add_pathc(p, last);

        last = next + 1;
    }

    add_pathc(p, last);

    p[cnt] = 0;

    *path_sp = p;

    return cnt;
}

void stack_add(dirstack_t *stack, const char *path, 
        const git_tree_entry *ent)
{
    const char *name = git_tree_entry_name(ent);
    const git_oid *t_oid = git_tree_entry_id(ent);
    const git_filemode_t t_fm = git_tree_entry_filemode(ent);

    char *tmppath = strdup(path);
    char **path_sp;
    dirstack_item_t *di;

    unsigned int cnt = split_path(&path_sp, tmppath);

    path_sp[cnt-1] = 0;

    if (cnt > 1)
        _handle_stack(stack, path_sp, cnt - 1);

    di = stack_get_item(stack, cnt-1);

    C(git_treebuilder_insert(0, di->tb, name, t_oid, t_fm));

    free(path_sp);
    free(tmppath);
}

int stack_close(dirstack_t *stack, git_oid *new_oid)
{
    dirstack_item_t *di;

    _stack_close_to(stack, 1);

    di = stack_get_item(stack, 0);

    C(git_treebuilder_write(new_oid, stack->repo, di->tb));
    git_treebuilder_free(di->tb);

    free(stack->item);

    return 0;
}

#if 0
git_tree *filtered_tree(include_dirs *id,
        git_tree *tree, git_repository *repo)
{
    git_tree *new_tree;
    git_oid new_oid;
    int i;
    dirstack_t stack;

    stack_open(&stack, repo);

    for(i=0; i<id->len; i++)
    {
        int error;
        git_tree_entry *out;
        const char *path = id->dirs[i];

        error = git_tree_entry_bypath(&out, tree, path);

        if (error == 0)
        {
            stack_add(&stack, path, out);
            git_tree_entry_free(out);
        }
    }

    C(stack_close(&stack, &new_oid));

    C(git_tree_lookup(&new_tree, repo, &new_oid));

    return new_tree;
}
#else
#define MAX_REGEX 1
struct filter_data_t {
    dirstack_t *stack;
    regex_t regex[MAX_REGEX];
    unsigned int regex_len;
};

typedef struct _s2_e_t
{
    git_tree *tree;
    size_t idx;
    size_t count;
    size_t change_count;
    const char *entname;
    git_treebuilder *tb;
} s2_e_t;

#define STACK_MAX 512
typedef struct _s2_t
{
    s2_e_t stack[STACK_MAX];
    unsigned int depth;
} s2_t;

static void copy_entry(git_treebuilder *tb, git_tree *tree, size_t idx)
{
        const git_tree_entry *e = git_tree_entry_byindex(tree, idx);
        const git_oid *t_oid = git_tree_entry_id(e);
        const git_filemode_t t_fm = git_tree_entry_filemode(e);
        const char *n = git_tree_entry_name(e);

        C(git_treebuilder_insert(0, tb, n, t_oid, t_fm));
}

git_tree *tree_walk(git_tree *tree, git_repository *repo,
    struct filter_data_t *fd)
{
    size_t count;
    size_t i;
    s2_t st;
    git_oid new_tree_id;
    size_t idx = 0;
    git_treebuilder *tb = 0;
    const char *entname = 0;
    size_t change_count = 0;

    memset(&st, 0, sizeof(st));
    tb = 0;

    count = git_tree_entrycount(tree);
    for (;;)
    {
start:
        for (i = idx; i < count; i++)
        {
            const git_tree_entry *e = git_tree_entry_byindex(tree, i);
            const git_oid *t_oid = git_tree_entry_id(e);
            git_otype type = git_tree_entry_type(e);
            const char *n = git_tree_entry_name(e);

            if (type == GIT_OBJ_TREE)
            {
                s2_e_t *st_ent = &st.stack[st.depth];

                st_ent->tb = tb;
                st_ent->entname = n;
                st_ent->tree = tree;
                st_ent->idx = i + 1;
                st_ent->count = count;
                st_ent->change_count = change_count;

                st.depth ++;

                C(git_tree_lookup(&tree, repo, t_oid));

                count = git_tree_entrycount(tree);
                idx = 0;
                change_count = 0;
                tb = 0;

                goto start;
            }

            if (type == GIT_OBJ_BLOB)
            {
#if 0
                if (regexec(&fd->regex[0], n, 0, 0, 0) == 0)
#else
                int x,m;

                m = 0;

                for (x = 0; x < LIST_LEN; x++)
                {
                    if (!git_oid_cmp(&search_oid[x], t_oid))
                    {
                        m = 1;
                        break;
                    }
                }
#endif
                if (m)
                {
                    printf("remove %s\n", n);
                    change_count ++;
                    if (!tb)
                    {
                        size_t j;

                        C(git_treebuilder_create(&tb, 0));
                        for (j=0; j < i; j++)
                        {
                            copy_entry(tb, tree, j);
                        }
                        if (st.depth > 0)
                        {
                            for (j=0; j < st.depth; j++)
                            {
                                if (!st.stack[j].tb)
                                {
                                    size_t k;
                                    C(git_treebuilder_create(&st.stack[j].tb, 0));
                                    for (k=0; k < st.stack[j].idx - 1; k++)
                                    {
                                        copy_entry(st.stack[j].tb,
                                                st.stack[j].tree, k);
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (tb)
                        copy_entry(tb, tree, i);
                }
            }
        }

        int new_commit = 0;
        if (change_count)
        {
            /* close current directory */
            C(git_treebuilder_write(&new_tree_id, repo, tb));
            git_treebuilder_free(tb);
            tb = 0;
            new_commit = 1;
        }

        if (st.depth == 0)
            break;

        git_tree_free(tree);
        tree = 0;

        s2_e_t *st_ent = &st.stack[st.depth];

        memset(st_ent, 0, sizeof(s2_e_t));

        /* pop stack */
        st.depth --;

        st_ent = &st.stack[st.depth];

        tree = st_ent->tree;
        idx = st_ent->idx;
        count = st_ent->count;
        tb = st_ent->tb;
        entname = st_ent->entname;

        if (change_count)
            change_count = st_ent->change_count + 1;
        else
            change_count = st_ent->change_count;
            
        if (new_commit)
        {
            C(git_treebuilder_insert(0, tb, entname,
                        &new_tree_id, GIT_FILEMODE_TREE));
        }
        else
        {
            if (tb)
                copy_entry(tb, tree, idx-1);
        }
    }

    if (change_count)
    {
        git_tree *new_tree;
        git_tree_lookup(&new_tree, repo, &new_tree_id);

        return new_tree;
    }
    else
        return tree;
}

git_tree *filtered_tree(struct include_dirs *id,
        git_tree *tree, git_repository *repo)
{
    git_tree *new_tree;
    struct filter_data_t fd;
    int err;

    err = regcomp(&fd.regex[0], "^Makefile$", REG_NOSUB);
    //err = regcomp(&fd.regex[0], "^aero_flex_dll.vcxproj$", REG_NOSUB);
    //err = regcomp(&fd.regex[0], "^usim.cpp$", REG_NOSUB);
    if (err < 0)
    {
        die("error compiling regular expression %d\n", err);
    }
    fd.regex_len = 1;

    new_tree = tree_walk(tree, repo, &fd);

    regfree(&fd.regex[0]);

    return new_tree;
}
#endif

void commit_list_init(commit_list_t *cl)
{
    memset(cl, 0, sizeof(*cl));
}

void commit_list_add(commit_list_t *cl, const git_commit *c)
{
    unsigned int i;

    for (i = 0; i < cl->len; i++)
    {
        if (!git_oid_cmp(git_commit_id(c), git_commit_id(cl->list[i])))
                return;
    }
    if (cl->len == cl->alloc)
    {
        cl->list = realloc(cl->list,
                (cl->alloc + CL_CHUNKS) * sizeof(git_commit *));
        cl->alloc += CL_CHUNKS;
        A(cl->list == 0, "no memory");
    }
    cl->list[cl->len] = c;
    cl->len ++;
}

void commit_list_free(commit_list_t *cl)
{
    free(cl->list);
    cl->list = 0;
}

/* helper function to find object id in one of the dictionaries */
static const git_commit *dict_lookup_all(const git_oid *id,
        dict_t *oid_dict, dict_t *deleted_merges, dict_t *deleted_commits)
{
    const git_commit *newc = dict_lookup(oid_dict, id);
    if (newc != 0)
        return newc;
    newc = dict_lookup(deleted_merges, id);
    if (newc != 0)
        return newc;
    newc = dict_lookup(deleted_commits, id);
    return newc;
}


/* find the parents of the original commit and
   map them to new commits */
void find_new_parents(git_commit *old, dict_t *oid_dict, 
        dict_t *deleted_merges, dict_t *deleted_commits,
               commit_list_t *commit_list)
{
    int cpcount;

    if (old == 0)
        return;

    cpcount = git_commit_parentcount(old);

    if (cpcount)
    {
        /* find parents */
        unsigned int n;
        for (n = 0; n < cpcount; n++)
        {
            git_commit *old_parent;
            const git_oid *old_pid;
            const git_commit *newc;

            C(git_commit_parent(&old_parent, old, n));
            old_pid = git_commit_id(old_parent);

            newc = dict_lookup_all(old_pid, oid_dict,
                    deleted_merges, deleted_commits);
            if (newc == 0)
                find_new_parents(old_parent, oid_dict,
                        deleted_merges, deleted_commits,
                        commit_list);
            else
                commit_list_add(commit_list, newc);

            git_commit_free(old_parent);
        }
    }
}

int parent_of(git_repository *repo, const git_oid *aid, const git_oid *oid)
{
    unsigned int cpcount;
    git_commit *commit;
#if DEBUG
    char oids1[GIT_OID_HEXSZ+1];
    char oids2[GIT_OID_HEXSZ+1];
#endif

    dlog("%s parent_of %s\n",
            git_oid_tostr(oids1, GIT_OID_HEXSZ+1, aid),
            git_oid_tostr(oids2, GIT_OID_HEXSZ+1, oid));

    C(git_commit_lookup(&commit, repo, oid));

    for(;;)
    {
        git_commit *parent = 0;
        unsigned int i;

        cpcount = git_commit_parentcount(commit);
        if (cpcount == 0)
        {
            git_commit_free(commit);
            return 0;
        }

        dlog("    %d parents\n", cpcount);
        for (i = 1; i < cpcount; i++)
        {
            dlog("    index %d\n", i);
            C(git_commit_parent(&parent, commit, i));

            oid = git_commit_id(parent);
            dlog("    parent %s\n", git_oid_tostr(oids1, GIT_OID_HEXSZ+1, oid));

            if (!git_oid_cmp(oid, aid))
            {
                dlog("    yes!\n");
                git_commit_free(commit);
                git_commit_free(parent);
                return 1;
            }
            git_commit_free(parent);
        }

        C(git_commit_parent(&parent, commit, 0));

        oid = git_commit_id(parent);
        dlog("    parent %s\n", git_oid_tostr(oids1, GIT_OID_HEXSZ+1, oid));

        if (!git_oid_cmp(oid, aid))
        {
            dlog("    yes!\n");
            git_commit_free(commit);
            git_commit_free(parent);
            return 1;
        }

        git_commit_free(commit);
        commit = parent;
    }
}


void create_commit(tree_filter_t *tf, git_tree *tree,
        git_commit *commit, const git_oid *commit_id)
{
    git_tree *new_tree;
    git_oid new_commit_id;
    const char *message;
    const git_signature *committer;
    const git_signature *author;
    commit_list_t commit_list;
    git_commit *new_commit;

    new_tree = filtered_tree(&tf->id, tree, tf->repo);

    if (git_tree_entrycount(new_tree) == 0)
    {
        if (new_tree != tree)
            git_tree_free(new_tree);
        return;
    }

    commit_list_init(&commit_list);
    if (tf->first)
        tf->first = 0;
    else
        find_new_parents(commit, tf->revdict, tf->deleted_merges,
                        tf->deleted_commits, &commit_list);

    /* skip commits which have identical trees but only
       in the simple case of one parent */
    if (commit_list.len == 1)
    {
        git_tree *old_tree;

        C(git_commit_tree(&old_tree, commit_list.list[0]));

        if (tree_equal(old_tree, new_tree))
        {
            /* cache this commit's parent so we can use it later */
            dict_add(tf->deleted_commits, commit_id, commit_list.list[0]);
            if (tree != new_tree)
                git_tree_free(new_tree);
            git_tree_free(old_tree);
            commit_list_free(&commit_list);
            return;
        }
        git_tree_free(old_tree);
    }
    else if (commit_list.len > 1)
    {
        unsigned int simplified = 0;
        unsigned int index = 1;
        unsigned int i;

        for(i = 1; i < commit_list.len; i++)
        {
            if (parent_of(tf->repo,
                        git_commit_id(commit_list.list[i]),
                        git_commit_id(commit_list.list[0])))
            {
                simplified ++;
            }
            else
            {
                if (index < i)
                {
                    commit_list.list[index] = commit_list.list[i];
                }
                index++;
            }
        }
        commit_list.len -= simplified;
        /* this is a merge commit that has collapsed to nothing cache this 
           information */
        if (commit_list.len == 1)
        {
            dict_add(tf->deleted_merges, commit_id, commit_list.list[0]);
            if (tree != new_tree)
                git_tree_free(new_tree);
            commit_list_free(&commit_list);
            return;
        }
    }

    message = git_commit_message(commit);
    committer = git_commit_committer(commit);
    author = git_commit_author(commit);

    C(git_commit_create(&new_commit_id,
                tf->repo, NULL,
                author, committer,
                NULL,
                message, new_tree,
                commit_list.len, commit_list.list));

    if (tree != new_tree)
        git_tree_free(new_tree);

    commit_list_free(&commit_list);

    C(git_commit_lookup(&new_commit, tf->repo, &new_commit_id));

    dict_add(tf->revdict, commit_id, new_commit);
    tf->last = new_commit_id;
}


#define CONFIG_KEYLEN 5
void parse_config_file(const char *cfgfile)
{
    FILE *f;
    unsigned int lineno;
    char *base = strdup("");

    f = fopen(cfgfile, "r");
    if (!f)
        die("cannot open %s\n", cfgfile);

    lineno = 0;
    while(!feof(f))
    {
        char *e = local_fgets(f);
        if (!e)
            break;

        lineno ++;

#define VALUE(buf) (buf+CONFIG_KEYLEN+1)

        if (e[0] == '#')
        {
            free(e);
            continue;
        }

        if (!strncmp(e, "REPO: ", CONFIG_KEYLEN))
        {
            if (git_repo_name)
                die("can only specify one repository in %s at %d\n",
                        cfgfile, lineno);
            git_repo_name = strdup(VALUE(e));
        }
        if (!strncmp(e, "TPFX: ", CONFIG_KEYLEN))
        {
            if (git_tag_prefix)
                die("can only specify one tag prefix in %s at %d\n",
                        cfgfile, lineno);
            git_tag_prefix = strdup(VALUE(e));
        }
        if (!strncmp(e, "REVN: ", CONFIG_KEYLEN))
        {
            if (rev_type)
                die("can only specify one revision in %s at %d\n",
                        cfgfile, lineno);
            rev_type = strdup(VALUE(e));
            rev_string = strchr(rev_type, ' ');
            if (!rev_string)
                die("can't find revision %s at %d\n", cfgfile, lineno);
            *rev_string = 0;
            rev_string ++;
        }
        if (!strncmp(e, "BASE: ", CONFIG_KEYLEN))
        {
            free(base);
            base = strdup(VALUE(e));
        }
        if (!strncmp(e, "FILT: ", CONFIG_KEYLEN))
        {
            char *name = VALUE(e);
            char *file = strchr(name, ' ');
            if (!file)
                die("invalid syntax for filter in %s at %d\n",
                        cfgfile, lineno);
            *file = 0;
            file ++;

            file = local_sprintf("%s%s", base, file);

            tf_list_new(strdup(name), file);
        }

        free(e);
    }

    if (rev_string == 0)
        die("no REVN: line found in %s\n", cfgfile);
    if (git_tag_prefix == 0)
        die("no TPFX: line found in %s\n", cfgfile);
    if (git_repo_name == 0)
        die("no REPO: line found in %s\n", cfgfile);
    if (tf_len == 0)
        die("no fiter specified in %s\n", cfgfile);

    free(base);

    fclose(f);
}

static void revwalk_init(git_revwalk *walker, const git_oid *last_commit_id)
{
    git_revwalk_sorting(walker, GIT_SORT_REVERSE|GIT_SORT_TOPOLOGICAL);

    if (!strcmp(rev_type, "ref"))
    {
        C(git_revwalk_push_ref(walker, rev_string));
        if (continue_run)
            C(git_revwalk_hide(walker, last_commit_id));
    }
    else if (!strcmp(rev_type, "range"))
    {
        if (continue_run)
            die("cannot continue from a range");

        C(git_revwalk_push_range(walker, rev_string));
    }
    else
    {
        die("invalid revision type %s in REVN", rev_type);
    }
}

void display_progress(char *s, unsigned int count,
        unsigned int total, time_t start, int force)
{
    time_t now;
    unsigned int percent;
    unsigned int time_taken;
    unsigned int time_remaining;
    static time_t last = 0;

    now = time(0);

    if (!force && now - last == 0)
        return;

    percent = count * 100 / total;
    time_taken = now - start;
    time_remaining = time_taken * (total - count)/ count;

    printf("%s: %d%% (%d/%d) time %ds(%ds)    \r",
            s, percent, count, total, time_taken, time_remaining);
    fflush(stdout);

    last = now;
}


int main(int argc, char *argv[])
{
    git_repository *repo;
    git_revwalk *walker;
    git_oid commit_oid;
    unsigned int count;
    unsigned int commit_count;
    unsigned int i;
    git_oid last_commit_id;
    char *last_commit_path = 0;
    time_t start;

#if 1
    for (i=0; i<LIST_LEN; i++)
    {
        C(git_oid_fromstr(&search_oid[i], oid_list[i]));
    }
#endif

    if (argc < 2)
    {
        printf("please specify the location of a filter configuration\n");
        printf("%s <filter config> [continue]\n", argv[0]);
        exit(1);
    }

    parse_config_file(argv[1]);

    C(git_repository_open(&repo, git_repo_name));
    if (!git_repository_is_bare(repo))
    {
        git_repo_suffix = ".git/";
    }

    last_commit_path = savefile("last_commit", "");

    if (argc > 2)
    {
        if(!strcmp(argv[2], "continue"))
        {    
            printf("Continuing from previous runs.\n");
            continue_run = 1;
            read_last_commit(&last_commit_id, last_commit_path);
        }
        else
        {
            die("second argument '%s' given. Did you mean 'continue'?",
                    argv[2]);
        }

    }

    for (i = 0; i < tf_len; i++)
        tree_filter_init(&tf_list[i], repo);

    C(git_revwalk_new(&walker, repo));

    revwalk_init(walker, &last_commit_id);

    commit_count = 0;
    while (!git_revwalk_next(&commit_oid, walker))
        commit_count ++;

    if (commit_count == 0)
    {
        printf("No new commits.\n");
        exit(0);
    }

    revwalk_init(walker, &last_commit_id);

    count = 0;
    start = time(0);

    while (!git_revwalk_next(&commit_oid, walker)) {
        git_tree *tree;
        git_commit *commit;

        C(git_commit_lookup(&commit, repo, &commit_oid));
        C(git_commit_tree(&tree, commit));

        for (i = 0; i < tf_len; i++)
            create_commit(&tf_list[i], tree, commit, &commit_oid);

        count ++;
        display_progress("Processing commits", count, commit_count, start, 0);

        git_commit_free(commit);
        git_tree_free(tree);
    }

    display_progress("Processing commits", count, commit_count, start, 1);

    printf("\n");

    start = time(0) - start;

    printf("Processed %d new commit%s in %lds.\n",
            count, count > 1 ? "s" : "", start);

    for (i = 0; i < tf_len; i++)
    {
        char oids[GIT_OID_HEXSZ+1];
        char *tag;
        tree_filter_t *tf = &tf_list[i];

        if (!git_oid_iszero(&tf->last))
        {
            tag = local_sprintf("refs/heads/%s%s", git_tag_prefix, tf->name);
            C(git_reference_create(0, tf->repo, tag, &tf->last, 1));
            printf("branch %s%s is now at %s\n",
                    git_tag_prefix, tf->name,
                    git_oid_tostr(oids, GIT_OID_HEXSZ+1, &tf->last));
            free(tag);
        } else {
            printf("%s%s unchanged\n", git_tag_prefix, tf->name);
        }

        rev_info_dump(tf);

        tree_filter_fini(&tf[i]);
    }

    save_last_commit(&commit_oid, last_commit_path);
    free(last_commit_path);

    git_revwalk_free(walker);
    git_repository_free(repo);

#if MALLOC_STATS
    malloc_stats();
#endif

    return 0;
}
