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

#define STACK_CHUNKS 32
#define INCLUDE_CHUNKS 1024
#define TAG_INFO_CHUNKS 128
#define TF_LIST_CHUNKS 32
#define CL_CHUNKS 16

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

struct include_dirs {
    char **dirs;
    unsigned int alloc;
    unsigned int len;
};

struct tree_filter {
    const char *name;;
    const char *include_file;
    struct include_dirs id;

    /* TODO fix tagging reconstruction and remove these */
    git_commit *last;

    git_repository *repo;
    dict_t *revdict;

    dict_t *deleted_merges;

    char first;
};

static char *git_repo_name = 0;
static char *git_repo_suffix = "";
static char *git_tag_prefix = 0;
static char *rev_type = 0;
static char *rev_string = 0;
static char delete_merges = 1;


static unsigned int tf_len = 0;
static struct tree_filter *tf_list;
static unsigned int tf_list_alloc = 0;

static char continue_run = 0;

#define BUFLEN 128
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

void _rev_info_dump(void *d, const void *k, const void *v)
{
    FILE *f = (FILE *)d;
    char oids1[GIT_OID_HEXSZ+1];
    char oids2[GIT_OID_HEXSZ+1];
    const git_oid *o = (const git_oid *)k;
    const git_oid *cid = git_commit_id((const git_commit *)v);

    fprintf(f, "%s: %s\n",
            git_oid_tostr(oids1, GIT_OID_HEXSZ+1, o),
            git_oid_tostr(oids2, GIT_OID_HEXSZ+1, cid)
           );
}

void rev_info_dump(dict_t *d, const char *filename)
{
    FILE *f;
    char *full_path = local_sprintf("%s%s%s%s.revinfo",
            git_repo_name, git_repo_suffix, git_tag_prefix, filename);

    f = fopen(full_path, "w");
    if (!f)
        die("Cannot open %s.\n", filename);

    dict_dump(d, _rev_info_dump, f);

    fclose(f);

    free(full_path);
}

int oid_cmp(const void *k1, const void *k2)
{
    const git_oid *s1 = (const git_oid *)k1;
    const git_oid *s2 = (const git_oid *)k2;

    return git_oid_cmp(s1, s2);
}


#define CHECK_FILES 1

#if CHECK_FILES
/* string sorting callback for qsort */
int sort_string(const void *a, const void *b)
{
    const char **stra = (const char **)a;
    const char **strb = (const char **)b;

    return strcmp(*stra, *strb);
}
#endif

void include_dirs_init(struct include_dirs *id, const char *file)
{
    FILE *f;

    id->dirs = malloc(sizeof (char *) * INCLUDE_CHUNKS);
    id->alloc = INCLUDE_CHUNKS;

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

#if CHECK_FILES
    int i;

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
#endif
}


static void save_last_commit(git_oid *commit_id, const char *filename)
{
        char oids[GIT_OID_HEXSZ+1];

        FILE *f = fopen(filename, "w");
        if (!f)
        {
            log("WARNING: could not write last commit id file %s", filename);
            return;
        }

        printf("saved last commit %s to %s\n",
                git_oid_tostr(oids, GIT_OID_HEXSZ+1, commit_id),
                filename);

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


static unsigned int read_revinfo(dict_t *revdict,
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
        git_oid coid;
        git_commit *commit;

        if (e == 0)
            break;

        lineno ++;

        v = strstr(e, ": ");
        if (!v)
            die("could not parse line %d of %s", lineno, filename);
        v += 2;

        git_oid *oida = malloc(sizeof(git_oid));
        A(oida == 0, "no memory");

        C(git_oid_fromstr(oida, e));
        C(git_oid_fromstr(&coid, v));

        C(git_commit_lookup(&commit, repo, &coid));

        dict_add(revdict, oida, commit);

        free(e);
    }

    fclose(f);

    return lineno;
}

void tree_filter_init(struct tree_filter *tf, git_repository *repo)
{
    int count = 0;
    include_dirs_init(&tf->id, tf->include_file);

    tf->repo = repo;

    tf->revdict = dict_init(oid_cmp);
    A(tf->revdict == 0, "failed to allocate list");

    tf->deleted_merges = dict_init(oid_cmp);
    A(tf->deleted_merges == 0, "failed to allocate list");

    if (continue_run)
    {
        char *full_path = local_sprintf("%s%s%s%s.revinfo",
                git_repo_name, git_repo_suffix, git_tag_prefix, tf->name);
        count = read_revinfo(tf->revdict, repo, full_path);
        free(full_path);
    }

    if (count == 0)
        tf->first = 1;
}

void tree_filter_fini(struct tree_filter *tf)
{
}

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

git_tree *filtered_tree(struct include_dirs *id,
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

typedef struct _commit_list_t
{
    const git_commit **list;
    unsigned int len;
    unsigned int alloc;
} commit_list_t;

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

/* find the parents of the original commit and
   map them to new commits */
void find_new_parents(git_commit *old, dict_t *oid_dict, 
        dict_t *deleted_merges, commit_list_t *commit_list)
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
            C(git_commit_parent(&old_parent, old, n));
            old_pid = git_commit_id(old_parent);
            const git_commit *newc = dict_lookup(oid_dict, old_pid);
            if (newc == 0) {
                if (delete_merges)
                    newc = dict_lookup(deleted_merges, old_pid);
                if (newc != 0)
                    commit_list_add(commit_list, newc);
                else
                    find_new_parents(old_parent, oid_dict, 
                            deleted_merges, commit_list);
            }
            else
                commit_list_add(commit_list, newc);
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


void create_commit(struct tree_filter *tf, git_tree *tree,
        git_commit *commit, const git_oid *commit_id)
{
    git_tree *new_tree;
    git_oid new_commit_id;
    const char *message;
    const git_signature *committer;
    const git_signature *author;
    commit_list_t commit_list;

    message = git_commit_message(commit);
    committer = git_commit_committer(commit);
    author = git_commit_author(commit);

    new_tree = filtered_tree(&tf->id, tree, tf->repo);

    if (git_tree_entrycount(new_tree) == 0)
        return;

    author = git_commit_author(commit);

    commit_list_init(&commit_list);
    if (tf->first)
        tf->first = 0;
    else
        find_new_parents(commit, tf->revdict, tf->deleted_merges, &commit_list);

    /* skip commits which have identical trees but only
       in the simple case of one parent */
    if (commit_list.len == 1)
    {
        git_tree *old_tree;
        C(git_commit_tree(&old_tree, commit_list.list[0]));
        if (tree_equal(old_tree, new_tree))
            return;
    }
    else if (delete_merges && commit_list.len > 1)
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
            git_oid *c_id_cp;

            c_id_cp = (git_oid *)malloc(sizeof(git_oid));
            A(c_id_cp == 0, "no memory");

            *c_id_cp = *commit_id;

            dict_add(tf->deleted_merges, c_id_cp, commit_list.list[0]);
            return;
        }
    }

    C(git_commit_create(&new_commit_id,
                tf->repo, NULL,
                author, committer,
                NULL,
                message, new_tree,
                commit_list.len, commit_list.list));

    commit_list_free(&commit_list);

    C(git_commit_lookup(&tf->last, tf->repo, &new_commit_id));

    git_oid *c_id_cp;

    c_id_cp = (git_oid *)malloc(sizeof(git_oid));
    A(c_id_cp == 0, "no memory");

    *c_id_cp = *commit_id;

    dict_add(tf->revdict, c_id_cp, tf->last);
}


#define CONFIG_KEYLEN 5
void parse_config_file(const char *cfgfile)
{
    FILE *f;
    unsigned int lineno;
    char *base = 0;

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
            continue;

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
            if (base)
            {
                free(base);
            }
            base = strdup(VALUE(e));
        }
        if (!strncmp(e, "FILT: ", CONFIG_KEYLEN))
        {
            char *name = strdup(VALUE(e));
            char *file = strchr(name, ' ');
            if (!file)
                die("invalid syntax for filter in %s at %d\n",
                        cfgfile, lineno);
            *file = 0;

            if (tf_len >= tf_list_alloc)
            {
                tf_list_alloc += TF_LIST_CHUNKS;
                tf_list = realloc(tf_list, tf_list_alloc * 
                        sizeof(struct tree_filter));
            }
            tf_list[tf_len].name = name;

            file ++;

            if (base)
                file = local_sprintf("%s%s", base, file);
            tf_list[tf_len].include_file = file;

            tf_len ++;
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

#define LEN 128
void display_progress(unsigned int count, unsigned int total, time_t *last)
{
    time_t now;
    char buf[LEN];
    unsigned int i;
    unsigned int percent;
    static int last_count_len = 0;

    now = time(0);
    if (now - *last == 0)
        return;

    for (i=0; i<last_count_len; i++)
        printf("\b");
    fflush(stdout);
    percent = count * 100 / total;
    last_count_len = snprintf(buf, LEN, "%02d%% (%d/%d)",
            percent, count, total);
    if (last_count_len > LEN)
        last_count_len = LEN;
    printf("%s", buf);
    fflush(stdout);
    *last = now;
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
    time_t last, start;

    if (argc < 2)
    {
        log("please specify the location of a filter configuration\n");
        log("%s <filter config> [continue]\n", argv[0]);
        exit(1);
    }

    parse_config_file(argv[1]);

    C(git_repository_open(&repo, git_repo_name));
    if (!git_repository_is_bare(repo))
    {
        git_repo_suffix = "/.git/";
    }

    last_commit_path = local_sprintf("%s%s%slast_commit",
            git_repo_name, git_repo_suffix, git_tag_prefix);

    if (argc > 2 && !strcmp(argv[2], "continue"))
    {
        log("Continuing from previous runs.\n");
        continue_run = 1;
        read_last_commit(&last_commit_id, last_commit_path);
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

    printf("Processing commits: ");

    count = 0;
    start = time(0);
    last = start;

    while (!git_revwalk_next(&commit_oid, walker)) {
        git_tree *tree;
        git_commit *commit;

        C(git_commit_lookup(&commit, repo, &commit_oid));
        C(git_commit_tree(&tree, commit));

        for (i = 0; i < tf_len; i++)
            create_commit(&tf_list[i], tree, commit, &commit_oid);

        count ++;
        display_progress(count, commit_count, &last);

        git_commit_free(commit);
        git_tree_free(tree);
    }

    display_progress(count, commit_count, &last);

    printf("\n");

    start = time(0) - start;

    printf("Processed %d new commit%s in %lds.\n",
            count, count > 1 ? "s" : "", start);

    for (i = 0; i < tf_len; i++)
    {
        char oids[GIT_OID_HEXSZ+1];
        char *tag;
        struct tree_filter *tf = &tf_list[i];
        const git_oid *commit_id;

        if (tf->last)
        {
            commit_id = git_commit_id(tf->last);

            tag = local_sprintf("refs/heads/%s%s", git_tag_prefix, tf->name);
            C(git_reference_create(0, tf->repo, tag, commit_id, 1));
            log("final name %s as %s\n",
                    git_oid_tostr(oids, GIT_OID_HEXSZ+1, commit_id), tag);
            free(tag);
        } else {
            log("no change for %s\n", tf->name);
        }

        rev_info_dump(tf->revdict, tf->name);

        tree_filter_fini(&tf[i]);
    }

    save_last_commit(&commit_oid, last_commit_path);
    free(last_commit_path);

    git_revwalk_free(walker);
    git_repository_free(repo);

    return 0;
}
