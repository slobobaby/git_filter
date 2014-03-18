#!/usr/bin/python
import sys
import subprocess
import os

dry_run = False
fast = True

# need to be able to filter soon
def gettags(repo):
    cmd = "git --git-dir=%s show-ref --tags" % repo
    raw_tags = subprocess.check_output(cmd.split()).splitlines()

    tag_dict = {}
    tag_prefix = "refs/tags/"
    for l in raw_tags:
        (id, full_tag) = l.strip().split()
        if not full_tag.startswith(tag_prefix):
            raise Exception("don't understand tag name %s" % full_tag)

        tag = full_tag[len(tag_prefix):]

        if not tag_dict.has_key(id):
            tag_dict[id] = []
        tag_dict[id].append(tag)

    return tag_dict

def getmaps(mapfile, filt = None):
    f = open(mapfile, "r")

    fmap = {}
    rmap = {}
    for l in f:
        if not l.startswith("r:"):
            continue
        (o, n) = l[2:].strip().split()
        old = o.strip()
        new = n.strip()
        if fmap.has_key(old):
            raise Exception("duplicate commit %s" % old)
        if rmap.has_key(new):
            log("WARNING: duplicate (%s, %s) -> %s" % (old, rmap[new], new))
        fmap[old] = new
        rmap[new] = old

    return fmap

def parse_config(configfile):
    TAGLEN = 6
    config =  {}
    base = ""

    f = open(configfile, "r")
    for l in f:
        if l[:TAGLEN] == "REPO: ":
            config["REPO"] = l[TAGLEN:-1]
        if l[:TAGLEN] == "REVN: ":
            config["REVN"] = l[TAGLEN:-1].split()
        if l[:TAGLEN] == "FILT: ":
            d = l[TAGLEN:-1].split()
            if not config.has_key("FILT"):
                config["FILT"] = []
            config["FILT"].append(d[0])
        if l[:TAGLEN] == "BASE: ":
            base = l[TAGLEN:-1]
        if l[:TAGLEN] == "TPFX: ":
            config["TPFX"] = l[TAGLEN:-1]

    f.close()
    return config

def log(msg):
    sys.stderr.write(msg + "\n")

def find_repo(repo_root):
    gsdir = repo_root + "/.git"
    if os.path.isdir(gsdir):
        return gsdir
    else:
        return repo_root

if len(sys.argv) < 2:
    sys.stderr.write("syntax: %s <repo>\n" % sys.argv[0])
    sys.exit(1)

cfg = parse_config(sys.argv[1])

repo = find_repo(cfg["REPO"])

cmd = "git --git-dir=%s rev-list --first-parent %s" % (repo, cfg["REVN"][1])
revs = subprocess.check_output(cmd.split()).splitlines()

tags = gettags(repo)

packed_refs_comment = "# added by newtags"

if len(sys.argv) < 2:
    raise Exception("Missing Argument: syntax is '%s <config file>'")

for name in cfg["FILT"]:
    log("STARTING " + name)
    
    tname = cfg["TPFX"]+name

    if fast:
        try:
            prefs_file = tname + ".git/packed-refs"

            if not dry_run:
                if os.path.exists(prefs_file):
                    f = open(prefs_file, "r")
                    for l in f:
                        line = l.strip()
                        if line == packed_refs_comment:
                            raise Exception("Tagging already done")
                    f.close()

                    f = open(prefs_file, "a")
                else:
                    f = open(prefs_file, "w")

                f.write(packed_refs_comment + "\n")
        except:
            log("Tagging already done in %s\n" % tname)
            continue

    map = getmaps("%s/%s.revinfo" % (repo, tname))

    log("MAPS done")

    ctags = []
    for rev in revs:
        if tags.has_key(rev):
            ctags += tags[rev]
        if ctags and map.has_key(rev):
            for tag in ctags:
                if fast:
                    if not dry_run:
                        f.write("%s refs/tags/%s\n" % (map[rev], tag))
                else:
                    cmd = "git --git-dir=%s tag %s %s" % (tname, tag, map[rev])
                    if not dry_run:
                        log(cmd)
                        rc = subprocess.call(cmd.split())
            ctags = []
    log("TAGGING done")

    if fast:
        f.close()

