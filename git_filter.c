/*
 * Copyright (C) 2014 Brian Murphy <brian@murphy.dk>
 *
 * This file is part of git_filter, distributed under the GNU GPL v2.
 * For full terms see the included COPYING file.
 *
 */

#include <stdio.h>
#include <string.h>

#include "git2.h"

#include "git_filter.h"
#include "git_filter_list.h"

#define STACK_MAX 32
#define PATH_MAX STACK_MAX

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

#define tree_equal(tree1, tree2) git_oid_equal(git_tree_id(tree1), git_tree_id(tree2))

#define INCLUDE_CHUNKS 1024

struct include_dirs {
    char **dirs;
    unsigned int alloc;
    unsigned int len;
};

#define TAG_INFO_CHUNK 16
struct rev_info {
    git_oid *id_list;
    unsigned int alloc;
    unsigned int len;
    FILE *file;
};

void rev_info_init(struct rev_info *ti, const char *filename)
{
    char buf[BUFSIZ];
    FILE *f;
    strcpy(buf, filename);
    strcat(buf, ".revinfo");

    memset(ti, 0, sizeof(*ti));

    f = fopen(buf, "w");
    if (!f)
        die("cannot open %s\n", filename);
    ti->file = f;
}

void rev_info_add(struct rev_info *ti, git_oid *oid)
{
    if (ti->len == ti->alloc)
    {
        ti->id_list = (git_oid *)
            realloc(ti->id_list, (ti->alloc + TAG_INFO_CHUNK) *
                    sizeof(struct git_oid));
        A(ti->id_list == 0, "failed to realloc");
        ti->alloc += TAG_INFO_CHUNK;
    }
    git_oid_cpy(&ti->id_list[ti->len], oid);
    ti->len++;
}

void rev_info_clear(struct rev_info *ti)
{
    ti->len = 0;
}

void rev_info_dump(git_oid *o, struct rev_info *ti)
{
    unsigned int i;
    char oid_p[GIT_OID_HEXSZ+1];
    char *n;

    if (ti->len == 0)
        return;

    n = git_oid_tostr(oid_p, GIT_OID_HEXSZ+1, o);
    fprintf(ti->file, "%s:", n);

    for (i = 0; i < ti->len; i++)
    {
        n = git_oid_tostr(oid_p, GIT_OID_HEXSZ+1, &ti->id_list[i]);
        fprintf(ti->file, " %s", n);
    }
    fprintf(ti->file, "\n");

    rev_info_clear(ti);
}


void oid_dump(const char *str, const void *k1)
{
    const git_oid *s1 = (const git_oid *)k1;
    char oid_p[GIT_OID_HEXSZ+1];
    char *n;

    n = git_oid_tostr(oid_p, GIT_OID_HEXSZ+1, s1);

    fprintf(stderr, "%s: %s\n", str, n);
}

int oid_cmp(const void *k1, const void *k2)
{
    const git_oid *s1 = (const git_oid *)k1;
    const git_oid *s2 = (const git_oid *)k2;

    return git_oid_cmp(s1, s2);
}


struct tree_filter {
    const char *name;;
    const char *include_file;
    struct include_dirs id;
    struct rev_info ti;

    /* TODO fix tagging reconstruction and remove these */
    git_commit *parent;
    git_oid parent_oid;

    git_repository *repo;
    git_filter_list_t *revlist;
};

char *git_repo_name = 0;
char *git_tag_prefix = 0;
char *rev_type = 0;
char *rev_string = 0;

#define PG_LEN_MAX 32
unsigned int tf_len = 0;
struct tree_filter tf_list[PG_LEN_MAX];

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

#define BUFLEN 256
void include_dirs_init(struct include_dirs *id, const char *file)
{
    FILE *f;
    char buf[BUFLEN];

    id->dirs = malloc(sizeof (char *) * INCLUDE_CHUNKS);
    id->alloc = INCLUDE_CHUNKS;

    f = fopen(file, "r");
    if (!f)
        die("cannot open %s\n", file);

    while(!feof(f))
    {
        char *e = fgets(buf, BUFLEN, f);
        if (!e)
            break;
        if (id->len == id->alloc)
        {
            id->alloc += INCLUDE_CHUNKS;
            id->dirs = realloc(id->dirs,
                    id->alloc * sizeof (char *));
        }

        e[strlen(e)-1] = '\0';
        id->dirs[id->len] = strdup(e);
        id->len++;
    }

    fclose(f);

#if CHECK_FILES
    int i;

    qsort(id->dirs, id->len, sizeof(char *), sort_string);

    for (i=1; i<id->len; i++)
    {
        if (!strcmp(id->dirs[i], id->dirs[i-1]))
            die("duplicate entries for '%s'", id->dirs[i]);

        unsigned int cmplen = strlen(id->dirs[i-1]);
        if (!strncmp(id->dirs[i], id->dirs[i-1], cmplen)
                && id->dirs[i][cmplen] == '/')
            die("'%s' is a subdir of '%s'", id->dirs[i], id->dirs[i-1]);
    }
#endif
}

void tree_filter_init(struct tree_filter *tf, git_repository *repo)
{
    include_dirs_init(&tf->id, tf->include_file);
    rev_info_init(&tf->ti, tf->name);

    tf->repo = repo;

    tf->revlist = list_init(oid_cmp, oid_dump);

    A(tf->revlist == 0, "failed to allocate list");
}

void tree_filter_fini(struct tree_filter *tf)
{
}

struct dirstack_item {
    git_treebuilder *tb;
    char *name;
};

struct dirstack {
    struct dirstack_item item[STACK_MAX];
    unsigned int depth;
    git_repository *repo;
};

void _stack_close_to(struct dirstack *stack, unsigned int level)
{
    unsigned int i;

    for (i = stack->depth - 1; i >= level; i--)
    {
        struct dirstack_item *cur = &stack->item[i];
        git_oid new_oid;

        C(git_treebuilder_write(&new_oid, stack->repo, cur->tb));
        git_treebuilder_free(cur->tb);

        C(git_treebuilder_insert(0, stack->item[i-1].tb, cur->name,
                    &new_oid, GIT_FILEMODE_TREE));

        free(cur->name);
        cur->name = 0;
        cur->tb = 0;
    }

    stack->depth = level;
}


void _handle_stack(struct dirstack *stack,
        char *path_c[PATH_MAX], unsigned int len)
{
    struct dirstack_item *s;
    unsigned int level;

    if (len == 0)
        return;

    for (level = 1; level <= len; level++)
    {
        s = &stack->item[level];

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

void stack_open(struct dirstack *stack, git_repository *repo)
{
    memset(stack, 0, sizeof(*stack));

    C(git_treebuilder_create(&stack->item[0].tb, 0));
    stack->depth = 1;
    stack->repo = repo;
}

/* modifies path */
unsigned int split_path(char *path_sp[PATH_MAX], char *path)
{
    char *next;
    char *last = path;
    unsigned int cnt;

    cnt = 0;
    while ((next = strchr(last, '/')))
    {
        *next = 0;
        path_sp[cnt++] = last;

	A(cnt > PATH_MAX, "%d path components. overflow", cnt);
        last = next + 1;
    }

    path_sp[cnt++] = last;
    A(cnt > PATH_MAX, "%d path components. overflow", cnt);
    path_sp[cnt] = 0;

    return cnt;
}

void stack_add(struct dirstack *stack, const char *path, 
        const git_tree_entry *ent)
{
    const char *name = git_tree_entry_name(ent);
    const git_oid *t_oid = git_tree_entry_id(ent);
    const git_filemode_t t_fm = git_tree_entry_filemode(ent);
    git_treebuilder *c;

    char *tmppath = strdup(path);
    char *path_sp[PATH_MAX];

    unsigned int cnt = split_path(path_sp, tmppath);

    path_sp[cnt-1] = 0;

    if (cnt > 1)
        _handle_stack(stack, path_sp, cnt - 1);

    c = stack->item[cnt-1].tb;

    C(git_treebuilder_insert(0, c, name, t_oid, t_fm));

    free(tmppath);
}

int stack_close(struct dirstack *stack, git_oid *new_oid)
{
    _stack_close_to(stack, 1);

    C(git_treebuilder_write(new_oid, stack->repo, stack->item[0].tb));

    git_treebuilder_free(stack->item[0].tb);

    return 0;
}

git_tree *filtered_tree(struct include_dirs *id,
        git_tree *tree, git_repository *repo)
{
    git_tree *new_tree;
    git_oid new_oid;
    int i;
    struct dirstack stack[STACK_MAX];

    stack_open(stack, repo);

    for(i=0; i<id->len; i++)
    {
        int error;
        git_tree_entry *out;
        const char *path = id->dirs[i];

        error = git_tree_entry_bypath(&out, tree, path);

        if (error == 0)
        {
            stack_add(stack, path, out);
            git_tree_entry_free(out);
        }
    }

    C(stack_close(stack, &new_oid));

    C(git_tree_lookup(&new_tree, repo, &new_oid));

    return new_tree;
}

#define OIDLIST_MAX 16
typedef struct _commit_list_t
{
    const git_commit *list[OIDLIST_MAX];
    unsigned int len;
} commit_list_t;

/* find parents the parents of the original commit and 
   map them to new commits */
void find_new_parents(git_commit *old, git_filter_list_t *oid_dict, 
        commit_list_t *commit_list)
{
    int cpcount;

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
            const git_commit *newc = list_lookup(oid_dict, old_pid);
            if (newc == 0)
                find_new_parents(old_parent, oid_dict, commit_list);
            else
            {
                commit_list->list[commit_list->len] = newc;
                commit_list->len ++;
            }
        }
    }
}


void create_commit(struct tree_filter *tf, git_tree *tree,
        git_commit *commit, git_oid *commit_id)
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

    commit_list.len = 0;
    find_new_parents(commit, tf->revlist, &commit_list);
    
    /* skip commits which have identical trees but only
       in the simple case of one parent */
    if (commit_list.len == 1)
    {
        git_tree *old_tree;
        C(git_commit_tree(&old_tree, commit_list.list[0]));
        if (tree_equal(old_tree, new_tree))
            return;
    }

    C(git_commit_create(&new_commit_id,
                tf->repo, NULL,
                author, committer,
                NULL,
                message, new_tree,
                commit_list.len, commit_list.list));

    rev_info_dump(&tf->parent_oid, &tf->ti);

    C(git_commit_lookup(&tf->parent, tf->repo,
                &new_commit_id));

    git_oid_cpy(&tf->parent_oid, &new_commit_id);

    git_oid *c_id_cp;

    c_id_cp = (git_oid *)malloc(sizeof(git_oid));
    A(c_id_cp == 0, "no memory");

    *c_id_cp = *commit_id;

    list_add(tf->revlist, c_id_cp, tf->parent);
}


#define CONFIG_KEYLEN 5
void parse_config_file(const char *cfgfile)
{
    FILE *f;
    char buf[BUFLEN];
    char vbuf[BUFLEN];
    unsigned int lineno;
    char *base = 0;

    f = fopen(cfgfile, "r");
    if (!f)
        die("cannot open %s\n", cfgfile);

    lineno = 0;
    while(!feof(f))
    {
        char *e = fgets(buf, BUFLEN, f);
        if (!e)
            break;

        lineno ++;

#define VALUE(buf) (buf+CONFIG_KEYLEN + 1)
#define VALUE_LEN(buf) (strlen(buf)-2-CONFIG_KEYLEN)

        if (buf[0] == '#')
            continue;

        if (!strncmp(e, "REPO: ", CONFIG_KEYLEN))
        {
            if (git_repo_name)
                die("can only specify one repository in %s at %d\n",
                        cfgfile, lineno);
            git_repo_name = strndup(VALUE(buf), VALUE_LEN(buf));
        }
        if (!strncmp(e, "TPFX: ", CONFIG_KEYLEN))
        {
            if (git_tag_prefix)
                die("can only specify one tag prefix in %s at %d\n",
                        cfgfile, lineno);
            git_tag_prefix = strndup(VALUE(buf), VALUE_LEN(buf));
        }
        if (!strncmp(e, "REVN: ", CONFIG_KEYLEN))
        {
            if (rev_type)
                die("can only specify one revision in %s at %d\n",
                        cfgfile, lineno);
            rev_type = strndup(VALUE(buf), VALUE_LEN(buf));
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
            base = strndup(VALUE(buf), VALUE_LEN(buf));
        }
        if (!strncmp(e, "FILT: ", CONFIG_KEYLEN))
        {
            char *name = strndup(VALUE(buf), VALUE_LEN(buf));
            char *file = strchr(name, ' ');
            if (!file)
                die("invalid syntax for filter in %s at %d\n",
                        cfgfile, lineno);
            *file = 0;
            if (tf_len >= PG_LEN_MAX)
            {
                log("tf max length exceeded in %s at %d\n", cfgfile, lineno);
                die("increase PG_LEN_MAX and recompile\n");
            }
            tf_list[tf_len].name = name;

            file ++;
            if (base)
            {
                snprintf(vbuf, BUFLEN, "%s%s", base, file);
                file = strdup(vbuf);
            }
            tf_list[tf_len].include_file = file;

            tf_len ++;
        }
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


int main(int argc, char *argv[])
{
    git_repository *repo;
    git_revwalk *walker;
    git_oid commit_oid;
    git_tree *tree_parent = 0;
    unsigned int count;
    unsigned int i;

    if (argc < 2)
    {
        log("please specify the location of a filter configuration\n");
        log("%s <filter config>\n", argv[0]);
        exit(1);
    }

    parse_config_file(argv[1]);

    C(git_repository_init(&repo, git_repo_name, 0));

    for (i = 0; i < tf_len; i++)
        tree_filter_init(&tf_list[i], repo);

    C(git_revwalk_new(&walker, repo));
    git_revwalk_sorting(walker, GIT_SORT_REVERSE|GIT_SORT_TOPOLOGICAL);

    if (!strcmp(rev_type, "ref"))
    {
        C(git_revwalk_push_ref(walker, rev_string));
    }
    else if (!strcmp(rev_type, "range"))
    {
        C(git_revwalk_push_range(walker, rev_string));
    }
    else
    {
        die("invalid revision type %s in REVN", rev_type);
    }

    count = 0;
    while (!git_revwalk_next(&commit_oid, walker)) {
        git_tree *tree;
        git_commit *commit;

        C(git_commit_lookup(&commit, repo, &commit_oid));
        C(git_commit_tree(&tree, commit));

        /* skip empty input commits */
        if (!tree_parent || !tree_equal(tree_parent, tree))
        {
            for (i = 0; i < tf_len; i++)
                create_commit(&tf_list[i], tree, commit, &commit_oid);
        }

        count ++;
        if (count % 1000 == 0)
            log("count %d\n", count);

        for (i = 0; i < tf_len; i++)
        {
            struct tree_filter *tf = &tf_list[i];
            rev_info_add(&tf->ti, &commit_oid);
        }

        git_commit_free(commit);
        git_tree_free(tree_parent);
        tree_parent = tree;
    }

#define STRLEN 128
    for (i = 0; i < tf_len; i++)
    {
        char oid_p[GIT_OID_HEXSZ+1];
        char buf[STRLEN];
        struct tree_filter *tf = &tf_list[i];
        char *n;
        const git_oid *commit_id;

        commit_id = git_commit_id(tf->parent);
        n = git_oid_tostr(oid_p, GIT_OID_HEXSZ+1, commit_id);

        snprintf(buf, STRLEN, "refs/heads/%s%s", git_tag_prefix, tf->name);
        C(git_reference_create(0, tf->repo, buf, commit_id, 1));
        log("final name %s as %s\n", n, buf);

    	rev_info_dump(&tf->parent_oid, &tf->ti);

        tree_filter_fini(&tf[i]);
    }

    git_revwalk_free(walker);
    git_repository_free(repo);

    return 0;
}
