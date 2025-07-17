
#include "coordDB.h"

static void coordinator_fork_listener_thread() {}

void coordinator_main() {
  coordinator_fork_listener_thread();

  // coordinator_fork_distribution_threads();

  while (1) {
    coord_db_display();
    getchar(); // Press Enter to refresh display
  }
}
