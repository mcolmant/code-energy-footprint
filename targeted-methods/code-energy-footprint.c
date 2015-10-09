#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <perfmon/pfmlib_perf_event.h>

#include "ancillary.h"
#include "variables.h"
#include "code-energy-footprint.h"

/**
 * Internal struct to represent a piece of code, and the associated opened counters
 */
typedef struct {
  char *label;
  int *fds;
  bool always_fed;
} code;

/**
 * Helps to open the counters only once per label (the label is tracked over the time)
 */
code *codes;
int codes_size = 0;

bool search_label(char *label) {
  int i;
  
  for(i = 0 ; i < codes_size ; i++) {
    if(strcmp(codes[i].label, label) == 0) {
      return true;
    }
  }

  return false;
}

code* create_code(char *label, int *fds) {
  code *c = (code*)calloc(1, sizeof(code));
  c->label = (char*)calloc(strlen(label), sizeof(char));
  c->fds = (int*)calloc(NCPUS * NEVENTS, sizeof(int));
  strcpy(c->label, label);
  memcpy(c->fds, fds, NCPUS * NEVENTS * sizeof(int));
  c->always_fed = false;
  return c;
}

void add_code(code *elt) {
  codes = realloc(codes, (codes_size + 1) * sizeof(code));
  codes[codes_size] = *elt;
  codes_size += 1;
}

int configure_pc(int cpu, char *event) {
  int ret = 0;
  int fd = -1;
  pfm_perf_encode_arg_t *arg;
  
  pfm_initialize();
 
  arg = (pfm_perf_encode_arg_t*)calloc(1, sizeof(pfm_perf_encode_arg_t));
  arg->attr = (struct perf_event_attr*)calloc(1, sizeof(struct perf_event_attr));

  ret = pfm_get_os_event_encoding(event, PFM_PLM0|PFM_PLM3|PFM_PLMH, PFM_OS_PERF_EVENT, arg);
  
  if(ret != PFM_SUCCESS) {
    fprintf(stderr, "pfm_get_os_event_encoding failed: %s\n", pfm_strerror(ret));
    return -1;
  }

  arg->attr->read_format = PERF_FORMAT_TOTAL_TIME_ENABLED|PERF_FORMAT_TOTAL_TIME_RUNNING;
  arg->attr->disabled = 1;
  arg->attr->inherit = 1;
 
  fd = perf_event_open(arg->attr, 0, cpu, -1, 0);
  if(fd == -1) {
    fprintf(stderr, "perf_event_open failed (event: %s, CPU: %d): %s\n", event, cpu, pfm_strerror(fd));
    return -1;
  }
  
  pfm_terminate();
  free(arg->attr);
  free(arg);

  return fd;
}

void configure_all_pc(char *label) {
  struct sockaddr_un control_flow_server_addr, fd_flow_server_addr;
  int control_flow_socket, fd_flow_socket, fd, ievent, cpu, fds[NCPUS * NEVENTS];
  char *message;

  if(!search_label(label)) {
    control_flow_socket = socket(PF_UNIX, SOCK_STREAM, 0);
    fd_flow_socket = socket(PF_UNIX, SOCK_STREAM, 0);

    if(control_flow_socket < 0)  {
      fprintf(stderr, "control_flow_socket failed: %s\n", strerror(errno));
    }

    if(fd_flow_socket < 0)  {
      fprintf(stderr, "fd_flow_socket failed: %s\n", strerror(errno));
    }

    memset(&control_flow_server_addr, 0, sizeof(struct sockaddr_un));
    control_flow_server_addr.sun_family = AF_UNIX;
    strncpy(control_flow_server_addr.sun_path, control_flow_server_path, sizeof(control_flow_server_addr.sun_path) - 1);

    memset(&fd_flow_server_addr, 0, sizeof(struct sockaddr_un));
    fd_flow_server_addr.sun_family = AF_UNIX;
    strncpy(fd_flow_server_addr.sun_path, fd_flow_server_path, sizeof(fd_flow_server_addr.sun_path) - 1);
    
    if(connect(control_flow_socket, (struct sockaddr *) &control_flow_server_addr, sizeof(struct sockaddr_un)) != 0) {
      fprintf(stderr, "connect failed: %s\n", strerror(errno));
    }

    if(connect(fd_flow_socket, (struct sockaddr *) &fd_flow_server_addr, sizeof(struct sockaddr_un)) != 0) {
      fprintf(stderr, "connect failed: %s\n", strerror(errno));
    }
    
    message = (char*)malloc(128 * sizeof(char));
    snprintf(message, 128, "ID:=%s\n", label);
    send(control_flow_socket, message, strlen(message), 0);
    free(message);

    message = (char*)malloc(128 * sizeof(char));
    snprintf(message, 128, "NEVENTS:=%d\n", NEVENTS);
    send(control_flow_socket, message, strlen(message), 0);
    free(message);
  
    message = (char*)malloc(128 * sizeof(char));
    snprintf(message, 128, "NCPUS:=%d\n", NCPUS);
    send(control_flow_socket, message, strlen(message), 0);
    free(message); 

    for(ievent = 0 ; ievent < NEVENTS ; ievent++) {
      message = (char*)malloc(128 * sizeof(char));
      snprintf(message, 128, "EVENT:=%s\n", events[ievent]);
      send(control_flow_socket, message, strlen(message), 0);
      free(message);

      for(cpu = 0 ; cpu < NCPUS ; cpu++) {
        message = (char*)malloc(128 * sizeof(char));
        snprintf(message, 128, "CPU:=%d\n", cpu);
        send(control_flow_socket, message, strlen(message), 0);
        free(message);
        
        fd = configure_pc(cpu, events[ievent]);
        if(ancil_send_fd(fd_flow_socket, fd)) {
          fprintf(stderr,"ancil_send_fd failed: %s\n", strerror(errno));
        }
        
        ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        fds[(ievent * NCPUS) + cpu] = fd;
      }
    }

    add_code(create_code(label, fds));
    
    close(control_flow_socket);
    close(fd_flow_socket);
  }  
}

void start_measure(char *label) {
  int i, j;

  for(i = 0 ; i < codes_size ; i++) {
    if(strcmp(codes[i].label, label) == 0 && !codes[i].always_fed) {
      for(j = 0 ; j < NCPUS * NEVENTS ; j++) {
        ioctl(codes[i].fds[j], PERF_EVENT_IOC_ENABLE, 0);
      }
      
      codes[i].always_fed = true;
      return;
    }
  }
}

void stop_measure(char *label) {
  int i, j;

  for(i = 0 ; i < codes_size ; i++) {
    if(strcmp(codes[i].label, label) == 0 && codes[i].always_fed) {
      for(j = 0 ; j < NCPUS * NEVENTS ; j++) {
        ioctl(codes[i].fds[j], PERF_EVENT_IOC_DISABLE, 0);
      }

      codes[i].always_fed = false;
      return;
    }
  }
}

