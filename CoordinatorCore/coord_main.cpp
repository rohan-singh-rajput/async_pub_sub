/* Coordinator = Listener thread + {set of distribution threads} */

#include <pthread.h>

extern void coordinator_main();

int main(int argc, char **argv) {

  (void)argc;
  (void)argv;

  // Coordinator thread - starts listener threads + distribution threads
  coordinator_main();

  pthread_exit(0);
  // all child threads will no tbe terminated when main is terminated

  return 0;
}