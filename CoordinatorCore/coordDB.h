#include <cstdint>
#include <stdint.h>

#include "../CoordinatorCore/pubsub.h"

publisher_db_entry_t *publisher_db_create(uint32_t pub_id, const std::string &pub_name);

void publisher_db_delete(uint32_t pub_id);


