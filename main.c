#include <stdio.h>

static int label_index;

/*
 * The grammar is simple. The entry point is parse.
 *
 * Exprs
 *      : Expr
 *      | Expr Exprs
 *
 * Expr
 *      : '>'
 *      | '<'
 *      | '+'
 *      | '-'
 *      | '.'
 *      | ','
 *      | '[' Exprs ']'
 */

void parse(FILE *in, FILE *out);
void loop(FILE *in, FILE *out);
void exprs(FILE *in, FILE *out);

void parse(FILE *in, FILE *out) {
	fprintf(out, ".global _start\n.text\n_start:\n");
	fprintf(out, "sub $0x4000, %%rsp\n"); // allocate 4KB on the stack
	fprintf(out, "add $0x2000, %%rsp\n"); // and start in the middle of it
	exprs(in, out);
	fprintf(out, "mov $60, %%rax\nxor %%rdi, %%rdi\nsyscall\n");
}

void loop(FILE *in, FILE *out) {
	char c;
	int this_loop_label;

	/*
	 * Each loop needs two labels. Loops inside this one might use other labels,
	 * so we need to remember which one is ours.
	 */
	label_index += 2;
	this_loop_label = label_index;

	fprintf(out, "jmp L%d\n", this_loop_label+1);
	fprintf(out, "L%d:\n", this_loop_label);

	exprs(in, out);
	c = getc(in);
	if (c != ']') {
		fprintf(stderr, "parse error: expected ']', got %c\n", c);
		return;
	}

	fprintf(out, "L%d:\n", this_loop_label+1);
	fprintf(out, "cmpb $0, 0(%%rsp)\n");
	fprintf(out, "jne L%d\n", this_loop_label);
}

void exprs(FILE *in, FILE *out) {
	char c;
	while ((c = getc(in)) != EOF) {
		switch (c) {
			case '>':
				fprintf(out, "dec %%rsp\n");
				break;
			case '<':
				fprintf(out, "inc %%rsp\n");
				break;
			case '+':
				fprintf(out, "incb 0(%%rsp)\n");
				break;
			case '-':
				fprintf(out, "decb 0(%%rsp)\n");
				break;
			case '.':
				fprintf(out, "mov $1, %%rax\n"); // write syscall
				fprintf(out, "mov $1, %%rdi\n"); // to stdout
				fprintf(out, "mov %%rsp, %%rsi\n"); // printing the byte at head
				fprintf(out, "mov $1, %%rdx\n"); // only one character
				fprintf(out, "syscall\n");
				break;
			case ',':
				fprintf(out, "mov $0, %%rax\n"); // read syscall is 0
				fprintf(out, "mov $0, %%rdi\n"); // from stdin
				fprintf(out, "mov %%rsp, %%rsi\n"); // reading to the head
				fprintf(out, "mov $1, %%rdx\n"); // only one character
				fprintf(out, "syscall\n");
				break;
			case '[':
				loop(in, out);
				break;
			case ']':
				// End brackets are not recognized in this rule
				ungetc(c, in);
				return;
				break;
			default:
				// Extra characters are ignored in brainfuck
				break;
		}
	}
}

int main() {
	parse(stdin, stdout);
}
