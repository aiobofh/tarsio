#include <stdio.h>

void foo(int a) {
  int i;
  for (i = 0; i < a; i++) {
    fprintf(stdout, "Hello world\n");
  }
}

void hello_world() {
  foo(1);
}

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;
  hello_world();
  return 0;
}
