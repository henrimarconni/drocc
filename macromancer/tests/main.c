#define DOG_CAT_IMPLEMENTATION
#define MM_Animal_IMPLEMENTATION
#include "animals.h"

int main() {
  speak();
  eat();
  sleep();
  mm_Animal_iface = mm_Cat_vt;
  speak();
  eat();
  sleep();
}
