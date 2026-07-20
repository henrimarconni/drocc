#ifndef DOG_CAT_H
#define DOG_CAT_H

void dog_eat();
void dog_sleep();
void dog_speak();

void cat_eat();
void cat_sleep();
void cat_speak();


#ifdef DOG_CAT_IMPLEMENTATION

#include <stdio.h>

void dog_eat() {
  printf("dog_eat\n");
}
void dog_sleep() {
  printf("dog_sleep\n");
}
void dog_speak() {
  printf("dog_speak\n");
}
void cat_eat() {
  printf("cat_eat\n");
}
void cat_sleep() {
  printf("cat_sleep\n");
}
void cat_speak() {
  printf("cat_speak\n");
}

#endif


#endif
