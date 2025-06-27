#include "pubsub.h"
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <unordered_map>

std::unordered_map<uint32_t, publisher_db_entry_t *> pub_db;
std::unordered_map<uint32_t, std::shared_ptr<subscriber_db_entry_t>> sub_db;
std::unordered_map<uint32_t, pub_sub_db_entry_t *> pub_sub_db;

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


