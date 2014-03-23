#!/usr/bin/python

import re

size = {}
bom = re.compile("size (\d*) oid (.*)")
bo_lines = open("bigobjs").read().splitlines()
for l in bo_lines:
    m = bom.match(l)
    if m:
        size[m.group(2)] = int(m.group(1))

rmlog_lines = open("rmlogs").read().splitlines()
for l in rmlog_lines:
    oid = l[0:40]
    filename = l[41:]
    print size[oid], filename, oid
