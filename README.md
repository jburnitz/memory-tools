# memory-tools
Some simple tools to view and edit a running process using ptrace, without the overhead of gdb
GDB is the obvious better tool for any real work, but it's a neat insight into how the syscalls work

An example usage would be on a.out with PID 12969, first find the memory location of the image with

joe@debian:~$ cat /proc/12969/maps 
08048000-08049000 r-xp 00000000 08:04 6045666      /home/joe/tmp/a.out
...
joe@debian:~$

The first line of /proc/PID/maps is memory address of the executable section, indicated by the x in the permissions field.
08048000 08049000 are the start and end memory addresses of the a.out executable, which dumpmem and patchmem can use.

