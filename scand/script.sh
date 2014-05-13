#!/bin/sh

mipsel-linux-uclibc-gcc scanc.c -o scanc
mipsel-linux-uclibc-strip scanc
tar -c scanc | ssh Segfault@192.168.1.195 -p 666 "cd tmp && tar -x"