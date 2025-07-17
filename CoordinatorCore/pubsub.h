/*
 * coordinator needs to store information about registered publisher and
 * subscriber with it. when pub/sub are instantiated, it registers with
 * coordinator.
 */

#ifndef __PUB_SUB__
#define __PUB_SUB__

#include "Config.h"
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

typedef struct publisher_db_entry_ {
  char pub_name[64];
  uint32_t publisher_id;
  uint32_t published_msg_ids[MAX_PUBLISHED_MSG];

  publisher_db_entry_() {
    pub_name[0] = '\0';
    publisher_id = 0;
    // set 0s to array -> published_msg_ids of
    // size uint32_t
    memset(published_msg_ids, 0, sizeof(published_msg_ids));
  }

} publisher_db_entry_t;

typedef struct subscriber_db_entry_ {
  char sub_name[64];
  uint32_t subscriber_id;
  uint32_t subscriber_msg_ids[MAX_SUBSCRIBED_MSG];

  subscriber_db_entry_() {
    sub_name[0] = '\0';
    subscriber_id = 0;
    memset(subscriber_msg_ids, 0, sizeof(subscriber_msg_ids));
  }

} subscriber_db_entry_t;

typedef struct pub_sub_db_entry_ {
  uint32_t published_msg_code;

  std::vector<std::shared_ptr<subscriber_db_entry_t>> subscribers;

  pub_sub_db_entry_() {
    published_msg_code = 0;
    subscribers.clear();
  }

} pub_sub_db_entry_t;

#endif // __PUB_SUB__
