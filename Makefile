build: find-fsevents-bugs

clean:
	-rm find-fsevents-bugs

fseventsmon: find-fsevents-bugs.c
	gcc -o find-fsevents-bugs find-fsevents-bugs.c

install: build
	cp find-fsevents-bugs /usr/local/bin

.PHONY: build clean install
