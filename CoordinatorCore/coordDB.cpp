#include "pubsub.h"
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <unordered_map>

std::unordered_map<uint32_t, publisher_db_entry_t *> pub_db;
std::unordered_map<uint32_t, std::shared_ptr<subscriber_db_entry_t>> sub_db;
std::unordered_map<uint32_t, pub_sub_db_entry_t *> pub_sub_db;

/* Publisher DB operations */
publisher_db_entry_t *publisher_db_create(uint32_t pub_id,
                                          const std::string &pub_name) {
  // Check if publisher already exists
  if (pub_db.find(pub_id) != pub_db.end()) {
    throw std::runtime_error("Publisher already exists");
  }

  // Allocate new publisher entry
  publisher_db_entry_t *pub_entry = new publisher_db_entry_t;
  if (!pub_entry) {
    throw std::runtime_error("Memory allocation failed");
  }

  // Assign values
  pub_entry->publisher_id = pub_id;
  pub_entry->pub_name = pub_name;

  // Add to publisher DB
  pub_db[pub_id] = pub_entry;

  return pub_entry;
}

void publisher_db_delete(uint32_t pub_id) {
  // Check if publisher exists
  auto it = pub_db.find(pub_id);
  if (it == pub_db.end()) {
    throw std::runtime_error("Publisher not found");
  }

  // Delete the publisher entry
  delete it->second;
  pub_db.erase(it);
  // Note: If there are any published messages, they should also be cleaned up
  //       but this is not handled in this function.
  //       You may need to implement additional logic to handle published
  //       messages. This is a placeholder for future cleanup logic.
}

bool publisher_publish_msg(uint32_t pub_id, uint32_t published_msg_id) {
  // Check if publisher exists
  auto it = pub_db.find(pub_id);
  if (it == pub_db.end()) {
    throw std::runtime_error("Publisher with ID " + std::to_string(pub_id) +
                             " not found");
  }

  publisher_db_entry_t *pub_entry = it->second;

  if (!pub_entry) {
    throw std::runtime_error("Publisher entry is null for ID " +
                             std::to_string(pub_id));
  }

  if (pub_entry->pub_name.empty()) {
    throw std::runtime_error("Publisher name is empty for ID " +
                             std::to_string(pub_id));
  }

  if (pub_entry->publisher_id == 0) {
    throw std::runtime_error("Invalid publisher ID for publisher: " +
                             pub_entry->pub_name);
  }

  if (published_msg_id == 0) {
    throw std::runtime_error("Invalid message ID (0) for publisher: " +
                             pub_entry->pub_name);
  }

  // TODO: Add message to queue or notify subscribers
  // This is a simulated publish
  return true;
}

bool publisher_unpublish_msg(uint32_t pub_id, uint32_t published_msg_id) {
  // Check if publisher exists
  auto it = pub_db.find(pub_id);
  if (it == pub_db.end()) {
    throw std::runtime_error("Publisher with ID " + std::to_string(pub_id) +
                             " not found");
  }

  // Unpublish the message
  publisher_db_entry_t *pub_entry = it->second;
  if (pub_entry == nullptr) {
    throw std::runtime_error("Publisher entry is null for ID " +
                             std::to_string(pub_id));
  }
  if (pub_entry->pub_name.empty()) {
    throw std::runtime_error("Publisher name is empty for ID " +
                             std::to_string(pub_id));
  }
  // Note: Actual message unpublishing logic is not implemented in this
  // placeholder
  return true;
}

/* Subscriber DB Operations */
std::shared_ptr<subscriber_db_entry_t>
subscriber_db_create(uint32_t sub_id, const std::string &sub_name) {
  // Check if subscriber already exists
  if (sub_db.find(sub_id) != sub_db.end()) {
    throw std::runtime_error("Subscriber already exists");
  }

  // Allocate new subscriber entry
  auto sub_entry = std::make_shared<subscriber_db_entry_t>();
  if (!sub_entry) {
    throw std::runtime_error("Memory allocation failed");
  }

  // Assign values
  sub_entry->subscriber_id = sub_id;
  sub_entry->sub_name = sub_name;

  // Add to subscriber DB
  sub_db[sub_id] = sub_entry;

  return sub_entry;
}

void subscriber_db_delete(uint32_t sub_id) {
  // Check if subscriber exists
  auto it = sub_db.find(sub_id);
  if (it == sub_db.end()) {
    throw std::runtime_error("Subscriber not found");
  }

  // Delete the subscriber entry
  sub_db.erase(it);
  // To-Do If there are any subscriptions, they should also be cleaned up
  //       but this is not handled in this function.
  //       Need to implement additional logic to handle subscriptions.
}

bool subscriber_subscribe_msg(uint32_t sub_id, uint32_t msg_id) {
  // Check if subscriber exists
  auto it = sub_db.find(sub_id);
  if (it == sub_db.end()) {
    throw std::runtime_error("Subscriber with ID " + std::to_string(sub_id) +
                             " not found");
  }

  auto sub_entry = it->second;

  if (!sub_entry) {
    throw std::runtime_error("Subscriber entry is null for ID " +
                             std::to_string(sub_id));
  }

  if (sub_entry->sub_name.empty()) {
    throw std::runtime_error("Subscriber name is empty for ID " +
                             std::to_string(sub_id));
  }

  if (sub_entry->subscriber_id == 0) {
    throw std::runtime_error("Invalid subscriber ID for subscriber: " +
                             sub_entry->sub_name);
  }

  if (msg_id == 0) {
    throw std::runtime_error("Invalid message ID (0) for subscriber: " +
                             sub_entry->sub_name);
  }
}

bool subscriber_unsubscribe_msg(uint32_t sub_id, uint32_t msg_id) {
  // Check if subscriber exists
  auto it = sub_db.find(sub_id);
  if (it == sub_db.end()) {
    throw std::runtime_error("Subscriber with ID " + std::to_string(sub_id) +
                             " not found");
  }

  auto sub_entry = it->second;

  if (!sub_entry) {
    throw std::runtime_error("Subscriber entry is null for ID " +
                             std::to_string(sub_id));
  }

  if (sub_entry->sub_name.empty()) {
    throw std::runtime_error("Subscriber name is empty for ID " +
                             std::to_string(sub_id));
  }

  // To-Do Actual message unsubscription logic is not implemented in this
  // placeholder
  return true;
}

/* Operations on PUB-SUB DB */
pub_sub_db_entry_t *
pub_sub_db_create(uint32_t msg_id,
                  std::shared_ptr<subscriber_db_entry_t> SubEntry) {
  // Check if message already exists
  if (pub_sub_db.find(msg_id) != pub_sub_db.end()) {
    throw std::runtime_error("Message already exists");
  }

  // Allocate new pub-sub entry
  auto pub_sub_entry = new pub_sub_db_entry_t;
  if (!pub_sub_entry) {
    throw std::runtime_error("Memory allocation failed");
  }

  // Assign values
  pub_sub_entry->publish_msg_code = msg_id;
  pub_sub_entry->subscribers.push_back(SubEntry);

  // Add to pub-sub DB
  pub_sub_db[msg_id] = pub_sub_entry;

  return pub_sub_entry;
}

void pub_sub_db_delete(uint32_t msg_id, uint32_t sub_id) {
  // Check if message exists
  auto it = pub_sub_db.find(msg_id);
  if (it == pub_sub_db.end()) {
    throw std::runtime_error("Message not found");
  }

  // Check if subscriber exists
  auto sub_it = sub_db.find(sub_id);
  if (sub_it == sub_db.end()) {
    throw std::runtime_error("Subscriber not found");
  }

  // Delete the pub-sub entry
  delete it->second;
  pub_sub_db.erase(it);
}

pub_sub_db_entry_t *pub_sub_db_get(uint32_t msg_id) {
  // Check if message exists
  auto it = pub_sub_db.find(msg_id);
  if (it == pub_sub_db.end()) {
    throw std::runtime_error("Message not found");
  }

  return it->second;
}