FSEvents buggy directories search tool
======================================

As per [rb-fsevent issue #10](https://github.com/thibaudgg/rb-fsevent/issues/10),
this tool tries to find directories that cause realpath to report incorrect case.

Usage:

    find-fsevents-bugs /Users/andreyvit


Building
--------

Clone this Git repository, then run make:

    git clone git://github.com/andreyvit/find-fsevents-bugs.git
    cd find-fsevents-bugs
    make
    sudo make install

You need to have Xcode installed for the build to work.
