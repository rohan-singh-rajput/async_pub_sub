#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/_select.h>
#include <sys/_types/_fd_def.h>
#include <sys/_types/_socklen_t.h>
#include <sys/_types/_ssize_t.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../Common/cmsgOp.h"
#include "../Common/comm-types.h"

#define COORD_RECV_Q_MAX_MSG_SIZE 2048

extern void coord_db_display();

static void coordinator_reply(int sock_fd, cmsg_t *reply_msg,
                              struct sockaddr_in *client_addr) {
  size_t msg_size_to_send = sizeof(*reply_msg) + reply_msg->tlv_buffer_size;

  int rc = sendto(sock_fd, (char *)reply_msg, msg_size_to_send, 0,
                  (struct sockaddr *)client_addr, sizeof(struct sockaddr));

  if (rc < 0) {
    printf("Coorinator: Error : Feedback  Reply to subscriber failed\n");
  }
}

extern cmsg_t *coordinator_process_publisher_msg(cmsg_t *msg,
                                                 size_t bytes_read);

extern cmsg_t *coordinator_process_subscriber_msg(cmsg_t *msg,
                                                  size_t bytes_read);

static void *coordinator_recv_msg_listen(void *arg __attribute__((unused))) {
  fd_set read_fds;
  int sock_fd, addr_len;
  cmsg_t *reply_msg;

  static char buffer[COORD_RECV_Q_MAX_MSG_SIZE];
  struct sockaddr_in server_addr, client_addr;

  if ((sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    printf("Coordinator: Error: Listener Socket creation failed\n");
    exit(1);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(COORD_UDP_PORT);
  server_addr.sin_addr.s_addr = htonl(COORD_IP_ADDR);
  addr_len = sizeof(struct sockaddr);

  if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) ==
      -1) {
    printf("Coordinator: Error : bind failed\n");
    close(sock_fd);
    exit(1);
  }

  printf("Coordinator: Listening for requests\n");

  while (1) {
    FD_ZERO(&read_fds);
    FD_SET(sock_fd, &read_fds);

    select(sock_fd + 1, &read_fds, NULL, NULL, NULL);

    ssize_t bytes_read =
        recvfrom(sock_fd, buffer, sizeof(buffer), 0,
                 (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);

    buffer[bytes_read] = '\0';

    cmsg_t *msg = (cmsg_t *)buffer;

    switch (msg->msg_type) {
    case PUB_TO_COORD:
      if (msg->id.publisher_id) {
        printf("Coordinator: Recieved message from publisher id = %u\n",
               msg->id.publisher_id);
      } else {
        printf("Coordinator: Recieved message from new publisher\n");
      }

      cmsg_debug_print(msg);

      reply_msg = coordinator_process_publisher_msg(msg, bytes_read);
      if (reply_msg) {
        coordinator_reply(sock_fd, reply_msg, &client_addr);
        free(reply_msg);
      }
      break;

    case SUBS_TO_COORD:
      if (msg->id.subscriber_id) {
        printf("Coordinator: Recieved message from subscriber id = %u\n",
               msg->id.subscriber_id);
      } else {
        printf("Coordinator: Recieved message from new subscriber\n");
      }

      cmsg_debug_print(msg);

      reply_msg = coordinator_process_subscriber_msg(msg, bytes_read);
      if (reply_msg) {
        coordinator_reply(sock_fd, reply_msg, &client_addr);
        free(reply_msg);
      }
      break;

    default:
      break;
    }
  }
  return NULL;
}

static void coordinator_fork_listener_thread() {

  static pthread_t udp_listener_thread;
  pthread_create(&udp_listener_thread, NULL, coordinator_recv_msg_listen, NULL);
}

void coordinator_main() {

  coordinator_fork_listener_thread();
  while (1) {
    coord_db_display();
    getchar();
  }
}