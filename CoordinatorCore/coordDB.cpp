#include "pubsub.h"
#include <cstdint>
#include <memory>
#include <unordered_map>

std::unordered_map<uint32_t, publisher_db_entry_t *> pub_db;
std::unordered_map<uint32_t, std::shared_ptr<subscriber_db_entry_t>> sub_db;
std::unordered_map<uint32_t, pub_sub_db_entry_t *> pub_sub_db;
