# Brainfuck to x86 Assembly Translator

This is a quick translator that converts
[Brainfuck](https://en.wikipedia.org/wiki/Brainfuck) source into pure x86
assembly for Linux. The resulting assembly is completely free standing.

At present it is naive. In the future, I may compress repeated statements into
single assembly instructions.

## Usage

Assuming you have some brainfuck in `source.bf`, run

```bash
make
./main < source.bf > source.s
as source.s -o source.o
ld source.o -o executable
./executable
```

to build and run your program.

## Language reference

| Token | Semantic meaning |
| --- | --- |
| `>` | Move tape head forward one byte |
| `<`| Move tape head backward one byte |
| `+` | Increment byte at tape head |
| `-` | Decrement byte at tape head |
| `.` | Print byte at tape head |
| `,` | Read one byte and write it to the tape head |
| `[` | Jump to the matching `]`. |
| `]` | If the byte at the tape head is 0, jump to the matching `[`. Otherwise, continue executtion outside the bracket. |
