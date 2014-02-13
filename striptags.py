#!/usr/bin/python
#
# Copyright (C) 2014 Brian Murphy <brian@murphy.dk>
#
# This file is part of git_filter, distributed under the GNU GPL v2.
# For full terms see the included COPYING file.
#

import sys
import os
import re

def get_taginfo(fname):
    f = open(fname, "r")

    for l in f:
        print l

def strip_tags(repo, isbare = False, tagfile = "tags"):
    rtagpath = repo
    prefspath = repo
    if not isbare:
        rtagpath += "/.git"
        prefspath += "/.git"
    rtagpath += "/refs/tags"
    prefspath += "/packed-refs"
    prefsnew = prefspath + ".new"
    prefsbak = prefspath + ".bak"

    if os.path.exists(prefsbak):
        raise Exception("%s exists.\n Remove it if you really want to strip tags again." % prefsbak);

    f = open(prefspath, "r")
    n = open(prefsnew, "w")
    t = open(tagfile, "w")

    rtfiles = os.listdir(rtagpath)
    if len(rtfiles) != 0:
        bkdir = rtagpath + ".bak"
        try:
            os.mkdir(bkdir)
        except:
            pass
        for filename in rtfiles:
            stagfile = rtagpath + "/" + filename
            i = open(rtagpath + "/" + filename, "r").read().strip()
            t.write("%s %s\n" % (i, filename))
            os.rename(stagfile, bkdir + "/" + filename)

    tagline = re.compile("^([0-9a-f]*) refs/tags/(.*)")
    indirectline = re.compile("\^.*")
    for line in f:
        m = tagline.match(line)
        if m:
            t.write("%s %s\n" % m.groups())
            continue

        m = indirectline.match(line)
        if m:
            continue

        n.write(line)

    f.close()
    n.close()
    t.close()

    os.rename(prefspath, prefspath + ".bak")
    os.rename(prefsnew, prefspath)

if len(sys.argv) < 2:
    raise Exception("Missing Argument: syntax is '%s <repo>'")

strip_tags(sys.argv[1])
