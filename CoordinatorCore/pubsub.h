/*
 * coordinator needs to store information about registered publisher and
 * subscriber with it. when pub/sub are instantiated, it registers with
 * coordinator.
 */

#ifndef __PUB_SUB__
#define __PUB_SUB__

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "../CoordinatorCore/Config.h"

typedef struct publisher_db_entry_t {
  std::string pub_name;  // Unique name of the publisher
  uint32_t publisher_id; // Unique publisher ID
  uint32_t
      published_msg_ids[MAX_PUBLISHED_MSG]; // List of published message IDs

  publisher_db_entry_t() : pub_name(""), publisher_id(0) {
    std::memset(published_msg_ids, 0, sizeof(published_msg_ids));
  }
} publisher_db_entry_t;

typedef struct subscriber_db_entry_t {
  std::string sub_name;                     // Unique name of the subscriber
  uint32_t subscriber_id;                   // Unique subscriber ID
  std::vector<uint32_t> subscriber_msg_ids; // Subscribed message IDs

  subscriber_db_entry_t() : sub_name(""), subscriber_id(0) {}
} subscriber_db_entry_t;

typedef struct pub_sub_db_entry_t {
  uint32_t publish_msg_code; // Unique message code from a publisher
  std::vector<std::shared_ptr<subscriber_db_entry_t>>
      subscribers; // Subscribers for this message

  pub_sub_db_entry_t() : publish_msg_code(0) {}
} pub_sub_db_entry_t;

#endif // __PUB_SUB__
