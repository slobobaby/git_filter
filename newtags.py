#!/usr/bin/python
import sys
import subprocess

dry_run = False
fast = True

# need to be able to filter soon
def gettags(tagfile, filt = None):
    f = open(tagfile, "r")

    tag = {}
    for l in f:
        (k, v) = l.strip().split()
        if not tag.has_key(k):
            tag[k] = []
        tag[k].append(v)

    return tag

def getmaps(mapfile, filt = None):
    f = open(mapfile, "r")

    fmap = {}
    rmap = {}
    for l in f:
        (o, n) = l.strip().split(':')
        old = o.strip()
        new = n.strip()
        if fmap.has_key(old):
            raise Exception("duplicate commit %s" % old)
        if rmap.has_key(new):
            log("duplicate (%s, %s) -> %s" % (old, rmap[new], new))
        fmap[old] = new
        rmap[new] = old

    return fmap

if len(sys.argv) < 3:
    raise Exception("Missing Argument: syntax is '%s <config file> <tagfile>'")

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

cfg = parse_config(sys.argv[1])

cmd = "git --git-dir=%s/.git rev-list --first-parent %s" % (cfg["REPO"], cfg["REVN"][1])
revs = subprocess.check_output(cmd.split()).splitlines()

tags = gettags(sys.argv[2])

def log(msg):
    sys.stderr.write(msg + "\n")

packed_refs_comment = "# added by newtags"

for name in cfg["FILT"]:
    log("STARTING " + name)
    
    tname = cfg["TPFX"]+name

    if fast:
        try:
            prefs_file = tname + "/packed-refs"

            if not dry_run:
                f = open(prefs_file, "r")
                for l in f:
                    line = l.strip()
                    if line == packed_refs_comment:
                        raise Exception("Tagging already done")
                f.close()

                f = open(prefs_file, "a")

                f.write(packed_refs_comment + "\n")
        except:
            log("Tagging already done in %s\n" % tname)
            continue

    map = getmaps(name + ".revinfo")

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

