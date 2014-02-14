#!/usr/bin/python
import sys
import subprocess

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

def retag(tagmapfile, t):
    f = open(tagmapfile, "r")

    for l in f:
        (newrev, v) = l.strip().split(':')
        oldrevs = v.split()
        tag = []
        for r in oldrevs:
            if t.has_key(r):
                tag += t[r]

        if tag:
            for ti in tag:
                cmd = "git tag %s %s" % (ti, newrev)
                ret = subprocess.call(cmd.split())
                if ret != 0:
                    print("command failed: %s=%d" % (cmd, ret))
            

if len(sys.argv) < 3:
    raise Exception("Missing Argument: syntax is '%s <revinfo file> <tagfile>'")

tags = gettags(sys.argv[2])
retag(sys.argv[1], tags)
