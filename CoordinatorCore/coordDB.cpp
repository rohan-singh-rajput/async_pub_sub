#include "pubsub.h"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <unordered_map>

std::unordered_map<uint32_t, publisher_db_entry_t *> pub_db;
std::unordered_map<uint32_t, std::shared_ptr<subscriber_db_entry_t>> sub_db;
std::unordered_map<uint32_t, std::shared_ptr<pub_sub_db_entry_t>> pub_sub_db;

/* Publisher DB Operations */
publisher_db_entry_t *publisher_db_create(uint32_t pub_id, char *pub_name) {
  // create instance of structure of publisher_db_entry_t
  publisher_db_entry_t *pub_entry;

  /* check if publisher already exists */
  if (pub_db.find(pub_id) != pub_db.end()) {
    throw std::runtime_error("Publisher already exits");
    return NULL;
  }

  /* Create a new structure of publisher_db_entry_t */
  pub_entry = new publisher_db_entry_t();

  if (pub_entry == NULL) {
    throw std::runtime_error("Memory allocation failed for publisher");
  }

  pub_entry->publisher_id = pub_id;
  strncpy(pub_entry->pub_name, pub_name, 64);

  pub_db[pub_id] = pub_entry;
  return pub_entry;
}

void publisher_db_delete(uint32_t pub_id) {
  auto it = pub_db.find(pub_id);
  if (it == pub_db.end()) {
    throw std::runtime_error("Publisher not found");
  }

  pub_db.erase(it);
}

bool publisher_publish_msg(uint32_t pub_id, uint32_t published_msg_id) {
  auto it = pub_db.find(pub_id);
  if (it == pub_db.end())
    return false;

  auto &pub_entry = it->second;

  // Check for duplication
  for (int i = 0; i < MAX_PUBLISHED_MSG; ++i) {
    if (pub_entry->published_msg_ids[i] == published_msg_id)
      return false; // Already published
  }

  // Find an empty slot (0 is unused)
  for (int i = 0; i < MAX_PUBLISHED_MSG; ++i) {
    if (pub_entry->published_msg_ids[i] == 0) {
      pub_entry->published_msg_ids[i] = published_msg_id;
      return true;
    }
  }

  return false; // No empty slot available
}

bool publisher_unpublish_msg(uint32_t pub_id, uint32_t published_msg_id) {
  auto it = pub_db.find(pub_id);
  if (it == pub_db.end())
    return false;

  auto &pub_entry = it->second;

  for (int i = 0; i < MAX_PUBLISHED_MSG; ++i) {
    if (pub_entry->published_msg_ids[i] == published_msg_id) {
      pub_entry->published_msg_ids[i] = 0;
      return true;
    }
  }

  return false; // Message not found
}

/* Subscriber DB Operations */
std::shared_ptr<subscriber_db_entry_t> subscriber_db_create(uint32_t sub_id,
                                                            char *sub_name) {
  if (sub_db.find(sub_id) != sub_db.end()) {
    throw std::runtime_error("Subscriber already exists");
  }

  // Create shared_ptr and initialize
  auto sub_entry = std::make_shared<subscriber_db_entry_t>();

  if (sub_entry == NULL) {
    throw std::runtime_error("Memory allocation failed for subscriber");
  }

  sub_entry->subscriber_id = sub_id;
  std::strncpy(sub_entry->sub_name, sub_name, 64);

  // Insert into DB
  sub_db[sub_id] = sub_entry;

  return sub_entry;
}

void subscriber_db_delete(uint32_t sub_id) {
  auto it = sub_db.find(sub_id);
  if (it == sub_db.end()) {
    throw std::runtime_error("subscriber not found");
  }

  sub_db.erase(it);
}

bool subscriber_subscribe_msg(uint32_t sub_id, uint32_t msg_id) {
  auto it = sub_db.find(sub_id);
  if (it == sub_db.end())
    return false; // Subscriber not found

  auto &sub_entry = it->second;

  // Check if already subscribed
  for (int i = 0; i < MAX_SUBSCRIBED_MSG; ++i) {
    if (sub_entry->subscriber_msg_ids[i] == msg_id) {
      return false; // Already subscribed
    }
  }

  // Find empty slot to add the message
  for (int i = 0; i < MAX_SUBSCRIBED_MSG; ++i) {
    if (sub_entry->subscriber_msg_ids[i] == 0) {
      sub_entry->subscriber_msg_ids[i] = msg_id;
      return true; // Successfully subscribed
    }
  }

  return false; // No space to add new subscription
}

bool subscriber_unsubscribe_msg(uint32_t sub_id, uint32_t msg_id) {
  auto it = sub_db.find(sub_id);
  if (it == sub_db.end())
    return false; // Subscriber not found

  auto &sub_entry = it->second;

  for (int i = 0; i < MAX_SUBSCRIBED_MSG; ++i) {
    if (sub_entry->subscriber_msg_ids[i] == msg_id) {
      sub_entry->subscriber_msg_ids[i] = 0; // Mark as unsubscribed
      return true;                          // Successfully unsubscribed
    }
  }

  return false; // Message not found in subscription list
}

/* Pub-Sub DB Operations */
pub_sub_db_entry_t *
pub_sub_db_create(uint32_t msg_id,
                  std::shared_ptr<subscriber_db_entry_t> SubEntry) {
  auto it = pub_sub_db.find(msg_id);
  if (it == pub_sub_db.end()) {
    // Create a new pub-sub entry
    auto entry = std::make_shared<pub_sub_db_entry_t>();
    entry->published_msg_code = msg_id;
    entry->subscribers.push_back(SubEntry);
    pub_sub_db[msg_id] = entry;
    return entry.get();
  } else {
    // Entry already exists – check if subscriber already added
    auto &subs = it->second->subscribers;
    for (const auto &sub : subs) {
      if (sub->subscriber_id == SubEntry->subscriber_id)
        return it->second.get(); // Already subscribed
    }

    // Add new subscriber
    it->second->subscribers.push_back(SubEntry);
    return it->second.get();
  }
}

void pub_sub_db_delete(uint32_t msg_id, uint32_t sub_id) {
  auto it = pub_sub_db.find(msg_id);
  if (it == pub_sub_db.end())
    return;

  auto &subscribers = it->second->subscribers;

  // Remove subscriber by ID
  subscribers.erase(
      std::remove_if(
          subscribers.begin(), subscribers.end(),
          [sub_id](const std::shared_ptr<subscriber_db_entry_t> &sub) {
            return sub->subscriber_id == sub_id;
          }),
      subscribers.end());

  // Optionally, delete entry if no subscribers left
  if (subscribers.empty()) {
    pub_sub_db.erase(it);
  }
}

pub_sub_db_entry_t *pub_sub_db_get(uint32_t msg_id) {
  auto it = pub_sub_db.find(msg_id);
  if (it != pub_sub_db.end()) {
    return it->second.get();
  }
  return nullptr;
}

void coord_db_display() {

  printf("Publisher DB\n");
  for (auto it = pub_db.begin(); it != pub_db.end(); it++) {
    printf("Publisher ID : %u, Publisher Name : %s\n", it->second->publisher_id,
           it->second->pub_name);
  }

  printf("Subscriber DB\n");
  for (auto it = sub_db.begin(); it != sub_db.end(); it++) {
    printf("Subscriber ID : %u, Subscriber Name : %s\n",
           it->second->subscriber_id, it->second->sub_name);
  }

  printf("Pub-Sub DB\n");
  for (auto it = pub_sub_db.begin(); it != pub_sub_db.end(); it++) {
    printf("Message ID : %u\n", it->second->published_msg_code);
    for (auto &sub : it->second->subscribers) {
      printf("Subscriber ID : %u\n", sub->subscriber_id);
    }
  }
}