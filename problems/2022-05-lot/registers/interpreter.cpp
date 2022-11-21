#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_BUF 1000         // maximum size of words in the input program
#define MAX_LITERALS 100'000 // maximum size of program literals
#define MAX_REGISTERS 12     // or is it 15? do X, Y and Out count towards the limit?
#define MAX_LABELS 1'000
#define MAX_INSTRUCTIONS 100'000
#define MAX_RUNTIME 100'000'000

#define NONE -1

#define OP_EOF 1
#define OP_REGISTERS 2
#define OP_INC 3
#define OP_DEC 4
#define OP_LABEL 5
#define OP_JEQ 6

typedef struct {
  int op;
  int arg1, arg2, arg3;
} instruction;

typedef struct {
  char* name; // pointer inside lit[]
  int value;
} reg;

typedef struct {
  char* name; // pointer inside lit[]
  int ptr;    // pointer inside in[]
} label;

FILE* prog;        // global for convenience
char buf[MAX_BUF]; // including the terminating \0
char lit[MAX_LITERALS];
reg r[MAX_REGISTERS];
label l[MAX_LABELS];
instruction in[MAX_INSTRUCTIONS];
int lit_size, num_registers, num_labels, num_instructions;
int line_no;

void die(const char* format, ...) {
  va_list args;
  va_start(args, format);
  if (line_no) {
    fprintf(stderr, "Error on line %d: ", line_no);
  }
  vfprintf(stderr, format, args);
  fprintf(stderr, "\n");
  va_end(args);
  _exit(1);
}

// Reads a token into buf[], which can be:
//   * newline;
//   * any string of non-space characters (including tabs!);
// Returns true for success, false for EOF.
bool read_token() {
  char c;
  // skip white space
  do {
    c = fgetc(prog);
  } while (c == ' ');

  if (c == EOF) {
    return false;
  } else if (c == '\n' || c == '\r') {
    // consume \n\r, \r\n, \r or \n
    buf[0] = '\n';
    buf[1] = '\0';
    line_no++;

    char next = fgetc(prog);
    if (next != '\n' || next != '\r' || next == c) {
      ungetc(next, prog);
    }
    return true;
  } else {
    int pos = 0;
    do {
      buf[pos++] = c;
      c = fgetc(prog);
    } while (c != EOF && c != ' ' && c != '\n' && c != '\r');
    buf[pos] = '\0';
    ungetc(c, prog);
    return true;
  }
}

void expect_token() {
  if (!read_token()) {
    die("Unexpected end of file.");
  }

  if (!strcmp(buf, "\n")) {
    die("Unexpected end of line.");
  }
}

// EOF is also acceptable.
void expect_new_line() {
  if (read_token() && strcmp(buf, "\n")) {
    die("Expected end of line, got <%s>.", buf);
  }
}

// Skips empty lines and comments until it encounters an instruction. Returns
// the opcode.
int read_instruction() {
  int result = NONE;

  while (result == NONE) {
    if (!read_token()) {
      result = OP_EOF;
    } else if (buf[0] == '\n') {
      // ignore empty lines
    } else if (buf[0] == '#') {
      do {
        read_token();
      } while (buf[0] != '\n');
    } else if (!strcmp(buf, "registers")) {
      result = OP_REGISTERS;
    } else if (!strcmp(buf, "inc")) {
      result = OP_INC;
    } else if (!strcmp(buf, "dec")) {
      result = OP_DEC;
    } else if (!strcmp(buf, "jeq")) {
      result = OP_JEQ;
    } else if (buf[strlen(buf) - 1] == ':') {
      result = OP_LABEL;
    } else {
      die("Unknown instruction <%s>.", buf);
    }
  }

  return result;
}

// Copies a literal string from src[] to lit[] and returns a pointer to it.
char* add_literal(const char* src) {
  char* result = strcpy(lit + lit_size, src);
  lit_size += strlen(src) + 1; // count the \0
  return result;
}

// @defined: If true, we assert that the register is defined. If false, we
// assert that it is not.
int get_register_index(const char* name, bool defined) {
  int i = 0;
  while (i < num_registers && strcmp(r[i].name, name)) {
    i++;
  }

  if (defined && (i == num_registers)) {
    die("Unknown register <%s>", name);
  } else if (!defined && (i < num_registers)) {
    die("Register <%s> is already defined.", name);
  }

  return i;
}

void init_register(const char* name, int value) {
  if (num_registers == MAX_REGISTERS) {
    die("Register limit (%d) exceeded.", MAX_REGISTERS);
  }

  get_register_index(name, false);
  r[num_registers++] = { add_literal(name), value };
}

void instr_registers() {
  int op = read_instruction();
  if (op != OP_REGISTERS) {
    die("The first instruction must be <registers>.");
  }

  do {
    read_token();
    if (buf[0] != '\n') {
      init_register(buf, 0);
    }
  } while (buf[0] != '\n');
}

void instr_inc_dec(int op) {
  expect_token(); // register name

  int i = get_register_index(buf, true);
  in[num_instructions++] = { op, i };

  expect_new_line();
}


// Reminder: Labels can be defined before or after they are used in a <jeq>.
int get_label_index(const char* name) {
  int i = 0;
  while (i < num_labels && strcmp(l[i].name, name)) {
    i++;
  }

  return (i < num_labels) ? i : NONE;
}

// Assumes that the label name does not already exist in l[].
void add_label(const char* name, int ptr) {
  if (num_labels == MAX_LABELS) {
    die("Label limit (%d) exceeded.", MAX_LABELS);
  }

  l[num_labels++] = { add_literal(name), ptr };
}

// Adds the label if it is undefined. Returns its index.
int get_or_add_label(const char* name) {
  int i = get_label_index(name);
  if (i == NONE) {
    i = num_labels;
    add_label(name, NONE);
  }

  return i;
}

// The label name is in buf[].
void instr_label() {
  buf[strlen(buf) - 1] = '\0'; // clear the final ':'

  int i = get_label_index(buf);
  if (i == NONE) {
    add_label(buf, num_instructions);
  } else if (l[i].ptr == NONE) {
    l[i].ptr = num_instructions;
  } else {
    die("Label <%s> is already defined.", buf);
  }

  expect_new_line();
}

void instr_jeq() {
  expect_token();
  int r1 = get_register_index(buf, true);
  expect_token();
  int r2 = get_register_index(buf, true);
  expect_token();
  int i = get_or_add_label(buf);
  in[num_instructions++] = { OP_JEQ, r1, r2, i };

  expect_new_line();
}

void read_program() {
  line_no = 1;
  instr_registers();

  int op;
  while ((op = read_instruction()) != OP_EOF) {
    switch (op) {
      case OP_INC:
      case OP_DEC:
        instr_inc_dec(op);
        break;

      case OP_JEQ:
        instr_jeq();
        break;

      case OP_LABEL:
        instr_label();
        break;

      case OP_REGISTERS:
        die("<registers> should only be called once, at the beginning.");

      default:
        die("Unknown opcode %d.", op);
    }
  }
}

// After reading the entire program, make sure that any labels referenced by
// jeq are defined.
void check_labels() {
  for (int i = 0; i < num_labels; i++) {
    if (l[i].ptr == NONE) {
      die("Label <%s> is used by jeq, but never defined.", l[i].name);
    }
  }
}

// @percent: The chance (0-100) of executing an inc/dec instruction
// Returns the number of instructions executed.
int run_program(int percent) {
  int ptr = 0;
  int counter = 0;

  while (ptr < num_instructions && counter <= MAX_RUNTIME) {
    int r1 = in[ptr].arg1;
    int r2 = in[ptr].arg2;
    int dest = in[ptr].arg3;
    switch (in[ptr].op) {
      case OP_INC:
        if (rand() % 100 < percent) {
          r[r1].value++;
          counter++;
        }
        // printf("%s = %d\n", r[r1].name, r[r1].value);
        ptr++;
        break;

      case OP_DEC:
        if (rand() % 100 < percent) {
          if (r[r1].value) {
            r[r1].value--;
          }
          counter++;
        }
        // printf("%s = %d\n", r[r1].name, r[r1].value);
        ptr++;
        break;

      case OP_JEQ:
        // printf("%s = %d, %s = %d\n", r[r1].name, r[r1].value, r[r2].name, r[r2].value);
        if (r[r1].value == r[r2].value) {
          ptr = l[dest].ptr;
        } else {
          ptr++;
        }
        counter++;
        break;
    }
  }

  if (counter > MAX_RUNTIME)  {
    die("Runtime exceeds %d cycles.", MAX_RUNTIME);
  }

  return counter;
}

int main(int argc, char** argv) {
  // read command line arguments
  if (argc != 5) {
    die("Usage: interpreter <program file> <x> <y> <%%success>");
  }

  char* prog_filename = argv[1];
  int x = atoi(argv[2]);
  int y = atoi(argv[3]);
  int percent = atoi(argv[4]); // [0-100]

  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  // initialize the registers
  init_register("X", x);
  init_register("Y", y);
  init_register("Out", 0);

  // parse the program
  if (!(prog = fopen(prog_filename, "r"))) {
    die("Could not open program file <%s>.", prog_filename);
  }
  read_program();
  fclose(prog);
  check_labels();

  // execute the program
  int counter = run_program(percent);
  int max = 13.5 * (x + 1) * (y + 1);

  printf("%d instructions executed.\n", counter);
  printf("Max instructions for 100p: %d\n", max);
  printf("Value of the Out register: %d\n", r[2].value);
  if (r[2].value != x * y) {
    die("**** INCORRECT answer.");
  }

  return 0;
}
