# memory-tools
Some simple tools to view and edit a running process using ptrace, without the overhead of gdb
GDB is the obvious better tool for any real work, but it's a neat insight into how the syscalls work

An example usage would be on a.out with PID 12969, first find the memory location of the image with

joe@debian:~$ cat /proc/12969/maps 
08048000-08049000 r-xp 00000000 08:04 6045666                            /home/joe/tmp/a.out
08049000-0804a000 rw-p 00000000 08:04 6045666                            /home/joe/tmp/a.out
f75b7000-f75b8000 rw-p 00000000 00:00 0 
f75b8000-f7701000 r-xp 00000000 08:02 395520                             /lib/i386-linux-gnu/libc-2.13.so
f7701000-f7703000 r--p 00149000 08:02 395520                             /lib/i386-linux-gnu/libc-2.13.so
f7703000-f7704000 rw-p 0014b000 08:02 395520                             /lib/i386-linux-gnu/libc-2.13.so
f7704000-f7707000 rw-p 00000000 00:00 0 
f7721000-f7724000 rw-p 00000000 00:00 0 
f7724000-f7725000 r-xp 00000000 00:00 0                                  [vdso]
f7725000-f7741000 r-xp 00000000 08:02 392062                             /lib/i386-linux-gnu/ld-2.13.so
f7741000-f7742000 r--p 0001b000 08:02 392062                             /lib/i386-linux-gnu/ld-2.13.so
f7742000-f7743000 rw-p 0001c000 08:02 392062                             /lib/i386-linux-gnu/ld-2.13.so
ffa31000-ffa52000 rw-p 00000000 00:00 0                                  [stack]
joe@debian:~$

The first line is the memory address of the executable section, indicated by the x in the permissions field.
08048000 08049000 are the start and end memory addresses of the a.out executable, which dumpmem and patchmem can use.

