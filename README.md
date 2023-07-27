introduction 

Forth Language
Forth is a stack-based programming language known for its simplicity and efficiency. This project is a Forth language interpreter and compiler, written in C.

Setup
First you have to make sure you have git, gcc and make install on your machine then you can clone this project:

git clone https://github.com/alimoghaddam2000/Forth
after cloning you must install sdl2 and ncurses. if you are in ubuntu22 you have to run following command otherwise you should lookup how to install libsdl2 on your specific OS in google.

Features
This interpreter supports following key word:
do, loop, +loop, begin, until, i, i', j
+, * , /, %, mod
if, else, then, =, <, >, and, or, invert
variable, constant, allot, @
dup, drop, swap, over, rot,
key, last-key - emit, cr, ."
Sleep, random - (, )

These are possible Errors:
Stack underflow
Stack overflow










































