build: find-fsevents-bugs

clean:
	-rm find-fsevents-bugs

find-fsevents-bugs: find-fsevents-bugs.c
	gcc -o find-fsevents-bugs -framework CoreFoundation -framework CoreServices find-fsevents-bugs.c

install: build
	cp find-fsevents-bugs /usr/local/bin

.PHONY: build clean install
