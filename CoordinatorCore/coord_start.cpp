
#include "coordDB.h"

static void coordinator_fork_listener_thread() {}

void initialize_dummy_data() {
  // Create publishers
  publisher_db_create(1, (char *)"PublisherOne");
  publisher_db_create(2, (char *)"PublisherTwo");

  // Create subscribers
  auto sub1 = subscriber_db_create(101, (char *)"SubscriberA");
  auto sub2 = subscriber_db_create(102, (char *)"SubscriberB");

  // Subscribe them to some messages
  subscriber_subscribe_msg(101,
                           1001); // Subscriber 101 subscribes to message 1001
  subscriber_subscribe_msg(102,
                           1002); // Subscriber 102 subscribes to message 1002

  // Link subscriber to messages in pub-sub db
  pub_sub_db_create(1001, sub1);
  pub_sub_db_create(1002, sub2);

  // Publishers publish those messages
  publisher_publish_msg(1, 1001);
  publisher_publish_msg(2, 1002);
}

void coordinator_main() {
  coordinator_fork_listener_thread();

  // coordinator_fork_distribution_threads();

  while (1) {
    // initialize_dummy_data();
    // Add dummy entries
    coord_db_display();
    getchar(); // Press Enter to refresh display
  }
}
