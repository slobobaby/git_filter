PROGS = git_filter
CFLAGS = -O2 -Wall -Werror
#CFLAGS += -ggdb
LIBGIT2_DIR = libgit2-0.20.0
CFLAGS += -I$(LIBGIT2_DIR)/include
LDLIBS = -L $(LIBGIT2_DIR) -lgit2
UNAME = $(shell uname)
ifneq ($(UNAME), Darwin)
LDLIBS += -lrt
endif

LIBGIT2_SRC = https://github.com/libgit2/libgit2/archive/v0.20.0.tar.gz
LIBGIT2_LOCAL = $(LIBGIT2_DIR).tar.gz
LIBGIT2_PATCHES = libgit2.patch

all: $(PROGS)
 
$(LIBGIT2_LOCAL):
	wget -O $@ $(LIBGIT2_SRC) || curl -L $(LIBGIT2_SRC) -o $@

$(LIBGIT2_DIR)/.unpacked: $(LIBGIT2_LOCAL)
	tar -xvf $(LIBGIT2_LOCAL)
	touch $@

$(LIBGIT2_DIR)/.patched: $(LIBGIT2_DIR)/.unpacked
	patch -p0 < $(LIBGIT2_PATCHES)
	touch $@

$(LIBGIT2_DIR)/.built: $(LIBGIT2_DIR)/.patched
	make -C libgit2-0.20.0 -f Makefile.embed
	touch $@

git_filter.o: $(LIBGIT2_DIR)/.built

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(PROGS): %: %.o
	$(CC) $(LDFLAGS) $< $(LDLIBS) -o $@

clean:
	rm -f *.o
	rm -f $(PROGS)
	rm -fr $(LIBGIT2_DIR)

reallyclean: clean
	rm -f $(LIBGIT2_LOCAL)
