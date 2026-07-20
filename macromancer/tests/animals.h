#ifndef MM_Animal_H__
#define MM_Animal_H__
#include "dog_cat.h"


struct MM_AnimalInterface {
  typeof(dog_eat)* _eat;
  typeof(dog_speak)* _speak;
  typeof(dog_sleep)* _sleep;
};

extern struct MM_AnimalInterface mm_Animal_iface;

extern const struct MM_AnimalInterface mm_Dog_vt;
extern const struct MM_AnimalInterface mm_Cat_vt;

#define eat mm_Animal_iface._eat
#define speak mm_Animal_iface._speak
#define sleep mm_Animal_iface._sleep


#ifdef MM_Animal_IMPLEMENTATION

const struct MM_AnimalInterface mm_Dog_vt = {
  ._eat = dog_eat,
  ._speak = dog_speak,
  ._sleep = dog_sleep
};

const struct MM_AnimalInterface mm_Cat_vt = {
  ._eat = cat_eat,
  ._speak = cat_speak,
  ._sleep = cat_sleep
};

struct MM_AnimalInterface mm_Animal_iface = {
  ._eat = dog_eat,
  ._speak = dog_speak,
  ._sleep = dog_sleep
};


#endif
#endif

