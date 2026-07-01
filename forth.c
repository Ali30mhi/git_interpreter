/* =====================================================================
 * A tiny stack-based FORTH interpreter with a polished terminal REPL.
 *
 * This is a rewrite of an earlier version. The language semantics are
 * intentionally kept small (arithmetic, comparisons, stack shuffling,
 * basic I/O, and single-level IF/ELSE/THEN) -- word definitions
 * (": name ... ;"), loops (DO/LOOP, BEGIN/UNTIL) and variables are NOT
 * implemented yet. See README.md for details and the "help"/"words"
 * commands at runtime.
 * ===================================================================== */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>   /* strcasecmp */
#include <ctype.h>
#include <unistd.h>    /* isatty */

/* ----------------------------- Config ------------------------------- */
#define STACK_MAX      1000
#define MAX_INPUT_LEN  256
#define MAX_TOKENS     64
#define MAX_TOKEN_LEN  64

/* ----------------------------- Colors -------------------------------- */
#define COLOR_RESET   "\x1b[0m"
#define COLOR_BOLD    "\x1b[1m"
#define COLOR_DIM     "\x1b[2m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_CYAN    "\x1b[36m"

static int use_color = 1; /* decided at startup based on isatty() */

static const char *C(const char *code) { return use_color ? code : ""; }

/* ------------------------------ Stack -------------------------------- */
struct stack {
    int top;
    int array[STACK_MAX];
};

static int is_empty(struct stack *s) { return s->top == -1; }
static int stack_size(struct stack *s) { return s->top + 1; }

static void print_error(const char *msg) {
    fprintf(stderr, "%s  ! %s%s\n", C(COLOR_RED), msg, C(COLOR_RESET));
}

static void push(struct stack *s, int n) {
    if (s->top >= STACK_MAX - 1) {
        print_error("stack overflow");
        return;
    }
    s->top++;
    s->array[s->top] = n;
}

/* Returns 0 on underflow (and leaves the stack untouched) instead of
 * reading out of bounds, which is what the original code did. */
static int pop(struct stack *s) {
    if (is_empty(s)) {
        print_error("stack underflow");
        return 0;
    }
    int n = s->array[s->top];
    s->array[s->top] = 0;
    s->top--;
    return n;
}

static void clear_stack(struct stack *s) { s->top = -1; }

static void print_stack(struct stack *s) {
    printf("%s  <%d> %s", C(COLOR_DIM), stack_size(s), C(COLOR_RESET));
    printf("%s[ ", C(COLOR_CYAN));
    for (int i = 0; i <= s->top; i++) {
        printf("%d ", s->array[i]);
    }
    printf("]%s\n", C(COLOR_RESET));
}

/* --------------------------- Word primitives -------------------------- */
/* All words share the same signature so they can live in one dispatch
 * table. Every op checks it has enough operands before popping, so a
 * bad command prints a clean message instead of corrupting memory. */

#define REQUIRE(n) \
    if (stack_size(s) < (n)) { print_error("stack underflow: not enough values"); return; }

static void w_add(struct stack *s)      { REQUIRE(2); int b = pop(s), a = pop(s); push(s, a + b); }
static void w_sub(struct stack *s)      { REQUIRE(2); int b = pop(s), a = pop(s); push(s, a - b); }
static void w_mul(struct stack *s)      { REQUIRE(2); int b = pop(s), a = pop(s); push(s, a * b); }

static void w_div(struct stack *s) {
    REQUIRE(2);
    int b = pop(s), a = pop(s);
    if (b == 0) { print_error("division by zero"); return; }
    push(s, a / b);
}

static void w_moddiv(struct stack *s) { /* simplified /mod: pushes remainder only */
    REQUIRE(2);
    int b = pop(s), a = pop(s);
    if (b == 0) { print_error("division by zero"); return; }
    push(s, a % b);
}

static void w_mod(struct stack *s) { /* floored modulo, always same sign as divisor */
    REQUIRE(2);
    int b = pop(s), a = pop(s);
    if (b == 0) { print_error("division by zero"); return; }
    int m = a % b;
    if (m != 0 && ((m < 0) != (b < 0))) m += b;
    push(s, m);
}

static void w_eq(struct stack *s) { REQUIRE(2); int b = pop(s), a = pop(s); push(s, a == b ? -1 : 0); }
static void w_lt(struct stack *s) { REQUIRE(2); int b = pop(s), a = pop(s); push(s, a <  b ? -1 : 0); }
static void w_gt(struct stack *s) { REQUIRE(2); int b = pop(s), a = pop(s); push(s, a >  b ? -1 : 0); }
static void w_and(struct stack *s){ REQUIRE(2); int b = pop(s), a = pop(s); push(s, (a && b) ? -1 : 0); }
static void w_or(struct stack *s) { REQUIRE(2); int b = pop(s), a = pop(s); push(s, (a || b) ? -1 : 0); }
static void w_invert(struct stack *s) { REQUIRE(1); int a = pop(s); push(s, a ? 0 : -1); }

static void w_dup(struct stack *s)  { REQUIRE(1); push(s, s->array[s->top]); }
static void w_drop(struct stack *s) { REQUIRE(1); pop(s); }
static void w_swap(struct stack *s) {
    REQUIRE(2);
    int tmp = s->array[s->top];
    s->array[s->top] = s->array[s->top - 1];
    s->array[s->top - 1] = tmp;
}
static void w_over(struct stack *s) { REQUIRE(2); push(s, s->array[s->top - 1]); }
static void w_rot(struct stack *s) { /* (a b c -- b c a) */
    REQUIRE(3);
    int c = s->array[s->top];
    int b = s->array[s->top - 1];
    int a = s->array[s->top - 2];
    s->array[s->top - 2] = b;
    s->array[s->top - 1] = c;
    s->array[s->top]     = a;
}

static void w_dot(struct stack *s) {
    REQUIRE(1);
    printf("%s%d%s\n", C(COLOR_CYAN), pop(s), C(COLOR_RESET));
}
static void w_dot_s(struct stack *s) { print_stack(s); /* non-destructive */ }
static void w_emit(struct stack *s) {
    REQUIRE(1);
    putchar((char)pop(s));
    fflush(stdout);
}
static void w_cr(struct stack *s) { (void)s; putchar('\n'); }
static void w_reset(struct stack *s) { clear_stack(s); }
static void w_is_empty(struct stack *s) { push(s, is_empty(s) ? -1 : 0); }

#undef REQUIRE

/* ---------------------------- Dispatch table --------------------------- */
typedef void (*word_fn)(struct stack *);

typedef struct {
    const char *name;
    word_fn fn;
    const char *doc; /* stack effect + short description, for help/words */
} word_entry;

static const word_entry WORDS[] = {
    { "+",       w_add,      "( a b -- a+b )    add" },
    { "-",       w_sub,      "( a b -- a-b )    subtract" },
    { "*",       w_mul,      "( a b -- a*b )    multiply" },
    { "/",       w_div,      "( a b -- a/b )    divide" },
    { "mod",     w_mod,      "( a b -- a%b )    floored modulo" },
    { "/mod",    w_moddiv,   "( a b -- rem )    remainder (simplified)" },
    { "=",       w_eq,       "( a b -- flag )   equal?" },
    { "<",       w_lt,       "( a b -- flag )   less than?" },
    { ">",       w_gt,       "( a b -- flag )   greater than?" },
    { "and",     w_and,      "( a b -- flag )   logical and" },
    { "or",      w_or,       "( a b -- flag )   logical or" },
    { "invert",  w_invert,   "( a -- flag )     logical not" },
    { "dup",     w_dup,      "( a -- a a )      duplicate top" },
    { "drop",    w_drop,     "( a -- )          discard top" },
    { "swap",    w_swap,     "( a b -- b a )    swap top two" },
    { "over",    w_over,     "( a b -- a b a )  copy 2nd item to top" },
    { "rot",     w_rot,      "( a b c -- b c a) rotate top three" },
    { ".",       w_dot,      "( a -- )          pop and print" },
    { ".s",      w_dot_s,    "( -- )            print stack, non-destructive" },
    { "emit",    w_emit,     "( c -- )          pop and print as ASCII char" },
    { "cr",      w_cr,       "( -- )            print a newline" },
    { "empty?",  w_is_empty, "( -- flag )       is the stack empty?" },
    { "reset",   w_reset,    "( -- )            clear the whole stack" },
};
#define NUM_WORDS (int)(sizeof(WORDS) / sizeof(WORDS[0]))

static const word_entry *find_word(const char *name) {
    for (int i = 0; i < NUM_WORDS; i++) {
        if (strcasecmp(WORDS[i].name, name) == 0) return &WORDS[i];
    }
    return NULL;
}

/* --------------------------- IF / ELSE / THEN --------------------------
 * Single-level conditional support (no nesting), tracked with a small
 * state machine. This mirrors what the original code attempted, but the
 * gating now applies to *every* token (numbers included), not just
 * recognized words -- previously a number literal inside a skipped
 * branch would still get pushed onto the stack. */
enum { IF_NONE = 0, IF_SKIP = 1, IF_EXEC_THEN = 2 };
static int if_state = IF_NONE;

/* ------------------------------ Parsing --------------------------------- */

/* Tokenize on whitespace, bounds-checked, handles repeated spaces/tabs. */
static int tokenize(const char *input, char tokens[MAX_TOKENS][MAX_TOKEN_LEN]) {
    int count = 0;
    int len = (int)strlen(input);
    int i = 0;
    while (i < len && count < MAX_TOKENS) {
        while (i < len && isspace((unsigned char)input[i])) i++;
        if (i >= len) break;
        int j = 0;
        while (i < len && !isspace((unsigned char)input[i]) && j < MAX_TOKEN_LEN - 1) {
            tokens[count][j++] = input[i++];
        }
        tokens[count][j] = '\0';
        count++;
    }
    return count;
}

/* Parses an (optionally negative) integer literal. Returns 1 on success
 * and writes the value through out_value; returns 0 if the token is not
 * a number at all (so it can be tried as a word instead). Using an
 * out-parameter avoids the old bug where -1 was both "not a number" and
 * a perfectly valid value to push. */
static int shafaq(const char *token, int *out_value) {
    int len = (int)strlen(token);
    if (len == 0) return 0;
    int i = 0, negative = 0;
    if (token[0] == '-') {
        if (len == 1) return 0; /* lone "-" is the subtract word */
        negative = 1;
        i = 1;
    }
    long value = 0;
    for (; i < len; i++) {
        if (!isdigit((unsigned char)token[i])) return 0;
        value = value * 10 + (token[i] - '0');
        if (value > 2147483647L) return 0; /* overflow guard */
    }
    *out_value = (int)(negative ? -value : value);
    return 1;
}

static void run_word(const char *tok, struct stack *s) {
    const word_entry *w = find_word(tok);
    if (w) {
        w->fn(s);
        return;
    }
    char msg[96];
    snprintf(msg, sizeof(msg), "unknown word: '%s' (try 'words' or 'help')", tok);
    print_error(msg);
}

static void execute_line(const char *input, struct stack *s) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
    int count = tokenize(input, tokens);

    for (int t = 0; t < count; t++) {
        char *tok = tokens[t];

        if (strcmp(tok, "\\") == 0) {
            break; /* '\' comments out the rest of the line */
        }

        if (if_state == IF_SKIP) {
            if (strcasecmp(tok, "else") == 0) { if_state = IF_EXEC_THEN; continue; }
            if (strcasecmp(tok, "then") == 0) { if_state = IF_NONE; continue; }
            continue; /* swallow tokens (numbers and words alike) in the skipped branch */
        }

        if (strcasecmp(tok, "if") == 0) {
            if (is_empty(s)) { print_error("stack underflow: 'if' needs a flag"); continue; }
            int cond = pop(s);
            if_state = (cond == 0) ? IF_SKIP : IF_EXEC_THEN;
            continue;
        }
        if (strcasecmp(tok, "else") == 0) {
            if (if_state == IF_EXEC_THEN) if_state = IF_SKIP;
            else print_error("'else' without matching 'if'");
            continue;
        }
        if (strcasecmp(tok, "then") == 0) {
            if (if_state == IF_NONE) print_error("'then' without matching 'if'");
            else if_state = IF_NONE;
            continue;
        }
        if (strcmp(tok, ":") == 0 || strcmp(tok, ";") == 0) {
            print_error("word definitions (: ... ;) aren't supported in this build yet");
            continue;
        }

        int value;
        if (shafaq(tok, &value)) {
            push(s, value);
            continue;
        }

        run_word(tok, s);
    }
}

/* -------------------------------- REPL UI -------------------------------- */

static void print_banner(void) {
    printf("%s%s", C(COLOR_BOLD), C(COLOR_CYAN));
    printf("+----------------------------------------+\n");
    printf("|               F O R T H                 |\n");
    printf("+----------------------------------------+\n");
    printf("%s", C(COLOR_RESET));
    printf("A tiny stack-based Forth REPL.\n");
    printf("Type %shelp%s for commands, %swords%s for the vocabulary, %sexit%s to quit.\n\n",
           C(COLOR_YELLOW), C(COLOR_RESET), C(COLOR_YELLOW), C(COLOR_RESET),
           C(COLOR_YELLOW), C(COLOR_RESET));
}

static void print_help(void) {
    printf("\n%sHow this works:%s\n", C(COLOR_BOLD), C(COLOR_RESET));
    printf("  Numbers are pushed onto the stack. Words operate on the stack.\n");
    printf("  Example:  3 4 +  .        -> pushes 3, pushes 4, adds them, prints 7\n\n");
    printf("%sREPL commands:%s\n", C(COLOR_BOLD), C(COLOR_RESET));
    printf("  help            show this message\n");
    printf("  words           list every available word\n");
    printf("  clear / cls     clear the screen\n");
    printf("  exit/quit/bye   leave the REPL\n\n");
    printf("%sControl flow:%s\n", C(COLOR_BOLD), C(COLOR_RESET));
    printf("  <flag> if ... then\n");
    printf("  <flag> if ... else ... then   (single level only, no nesting)\n\n");
    printf("%sComments:%s  \\ ignores the rest of the line\n\n", C(COLOR_BOLD), C(COLOR_RESET));
    printf("%sNot implemented yet:%s  word definitions (: ... ;), do/loop, begin/until,\n"
           "  variable, constant, string printing (.\") -- see README.md\n\n",
           C(COLOR_DIM), C(COLOR_RESET));
}

static void print_words(void) {
    printf("\n%sAvailable words:%s\n", C(COLOR_BOLD), C(COLOR_RESET));
    for (int i = 0; i < NUM_WORDS; i++) {
        printf("  %s%-8s%s %s\n", C(COLOR_YELLOW), WORDS[i].name, C(COLOR_RESET), WORDS[i].doc);
    }
    printf("  %s%-8s%s %s\n", C(COLOR_YELLOW), "if/else/then", C(COLOR_RESET), "conditional (single level)");
    printf("\n");
}

int main(void) {
    use_color = isatty(fileno(stdout));

    struct stack s;
    s.top = -1;

    print_banner();

    char input[MAX_INPUT_LEN];
    while (1) {
        printf("%s%sforth> %s", C(COLOR_BOLD), C(COLOR_GREEN), C(COLOR_RESET));
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) {
            printf("\n%sGoodbye!%s\n", C(COLOR_YELLOW), C(COLOR_RESET));
            break; /* EOF, e.g. Ctrl-D */
        }

        size_t len = strlen(input);
        while (len > 0 && (input[len - 1] == '\n' || input[len - 1] == '\r')) {
            input[--len] = '\0';
        }

        char *trimmed = input;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
        if (*trimmed == '\0') continue; /* blank line, just reprompt */

        if (strcasecmp(trimmed, "exit") == 0 || strcasecmp(trimmed, "quit") == 0 ||
            strcasecmp(trimmed, "bye") == 0) {
            printf("%sGoodbye!%s\n", C(COLOR_YELLOW), C(COLOR_RESET));
            break;
        }
        if (strcasecmp(trimmed, "help") == 0) { print_help(); continue; }
        if (strcasecmp(trimmed, "words") == 0) { print_words(); continue; }
        if (strcasecmp(trimmed, "clear") == 0 || strcasecmp(trimmed, "cls") == 0) {
            printf("\x1b[2J\x1b[H");
            continue;
        }

        execute_line(trimmed, &s);
        print_stack(&s);
    }

    return 0;
}