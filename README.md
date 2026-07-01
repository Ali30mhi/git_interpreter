# Forth Language

A small stack-based Forth interpreter, written in C.

## Files

- `forth.c` — the current, actively maintained interpreter. A clean rewrite of the original with a polished terminal REPL: colored output, a live stack display after every command, help/words commands, and safe error handling.
- `forth language` — the original prototype. Kept as-is for history; superseded by forth.c.
- `example.fs` — a short demo script. Run it with ./forth < example.fs

## Build & run

gcc -Wall -Wextra -std=c11 -o forth forth.c
./forth

No external dependencies.

## Implemented

+ - * / mod /mod = < > and or invert dup drop swap over rot . .s emit cr empty? reset, single-level if/else/then, negative numbers, backslash comments.

## Not yet implemented

Word definitions (: name ... ;), loops, variable/constant, string printing. Recognized but intentionally left unimplemented rather than faked.

## License

GPL-3.0 — see LICENSE.
