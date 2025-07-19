
#include "../Common/comm-types.h"
#include <cstddef>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t coord_generate_id() {
  static uint32_t id = 0;
  return ++id;
}

cmsg_t *coordinator_process_publisher_msg(cmsg_t *msg, size_t bytes_read) {

  printf("Coordinator : Processing Publisher Message:\n");
  printf("  Message Type     : %s\n", msg_type_to_string(msg->msg_type));
  printf("  Sub Message Type : %s\n",
         sub_msg_type_to_string(msg->sub_msg_type));
  printf("  Publisher ID     : %u\n", msg->id.publisher_id);
  printf("  Message Code     : %u\n", msg->msg_code);
  printf("  Priority         : %d\n", msg->priority);
  printf("  TLV Size         : %u\n", msg->tlv_buffer_size);
  printf("  Bytes Read       : %zu\n", bytes_read);

  // Log payload if any
  if (msg->tlv_buffer_size > 0) {
    printf("  Payload: %.*s\n", msg->tlv_buffer_size, msg->tlv_buffer);
  }

  // Build ACK reply
  const char *ack = "ACK: Publisher Message Received";
  uint16_t ack_len = strlen(ack) + 1;

  cmsg_t *reply = (cmsg_t *)malloc(sizeof(cmsg_t) + ack_len);
  if (!reply) {
    perror("Coordinator : malloc failed for reply");
    return NULL;
  }

  reply->msg_type = COORD_TO_PUB;
  reply->sub_msg_type = msg->sub_msg_type;
  reply->priority = msg->priority;
  reply->msg_code = msg->msg_code;
  reply->id.publisher_id = msg->id.publisher_id;
  reply->msg_id = msg->msg_id;
  reply->tlv_buffer_size = ack_len;

  memcpy(reply->tlv_buffer, ack, ack_len);

  return reply;
}

cmsg_t *coordinator_process_subscriber_msg(cmsg_t *msg, size_t bytes_read) {

  printf("Coordinator : Processing Subscriber Message:\n");
  printf("  Message Type     : %s\n", msg_type_to_string(msg->msg_type));
  printf("  Sub Message Type : %s\n",
         sub_msg_type_to_string(msg->sub_msg_type));
  printf("  Subscriber ID    : %u\n", msg->id.subscriber_id);
  printf("  Message Code     : %u\n", msg->msg_code);
  printf("  Priority         : %d\n", msg->priority);
  printf("  TLV Size         : %u\n", msg->tlv_buffer_size);
  printf("  Bytes Read       : %zu\n", bytes_read);

  // Log payload if any
  if (msg->tlv_buffer_size > 0) {
    printf("  Payload: %.*s\n", msg->tlv_buffer_size, msg->tlv_buffer);
  }

  // Build ACK reply
  const char *ack = "ACK: Subscriber Message Received";
  uint16_t ack_len = strlen(ack) + 1;

  cmsg_t *reply = (cmsg_t *)malloc(sizeof(cmsg_t) + ack_len);
  if (!reply) {
    perror("Coordinator : malloc failed for reply");
    return NULL;
  }

  reply->msg_type = COORD_TO_SUBS;
  reply->sub_msg_type = msg->sub_msg_type;
  reply->priority = msg->priority;
  reply->msg_code = msg->msg_code;
  reply->id.subscriber_id = msg->id.subscriber_id;
  reply->msg_id = msg->msg_id;
  reply->tlv_buffer_size = ack_len;

  memcpy(reply->tlv_buffer, ack, ack_len);

  return reply;
}
