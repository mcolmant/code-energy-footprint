#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <aio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

static FILE *trace;

long long current_timestamp() {
  struct timeval te; 
  gettimeofday(&te, NULL); 
  return te.tv_sec * 1000LL + te.tv_usec / 1000;
}

void __attribute__ ((constructor)) trace_begin (void) {
  char filename[256];
  snprintf(filename, sizeof(filename), "trace_%d.txt", getpid());
  trace = fopen(filename, "w");
}

void __attribute__ ((destructor)) trace_end (void) {
  fclose(trace);
}

void __cyg_profile_func_enter (void *func,  void *caller) {
  fprintf(trace, "e %p %p %lld\n", func, caller, current_timestamp());
}

void __cyg_profile_func_exit (void *func, void *caller) {
  fprintf(trace, "x %p %p %lld\n", func, caller, current_timestamp());
}

