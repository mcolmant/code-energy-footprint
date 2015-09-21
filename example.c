#include <stdlib.h>
#include <math.h>

#include "code-energy-footprint.h"

void stress() {
  int i;
  configure_all_pc("label1");
  start_measure("label1");
  while(i < 100000000) {
    sqrt(rand());
    i++;
  }
  stop_measure("label1");
}

int main() {
  stress();
  return 0;
}

