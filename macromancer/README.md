# Macromancer

Macromancer is a code generator for C that provides a simple interface/implementation system without changing the language or requiring compiler extensions.

You describe interfaces and implementations in a small configuration file, and Macromancer generates a single header that connects them together.

The generated code is ordinary C. There is no runtime library or dependency on Macromancer after code generation.

## Static Interfaces

A static interface is resolved at compile time and has no runtime overhead.

Input:

```text
$interface Animal as Static {
    eat
    speak
    sleep
}

$impl Dog as Animal {
    $header = "dog_cat.h"
    eat = dog_eat
    speak = dog_speak
    sleep = dog_sleep
}

$impl Cat as Animal {
    $header = "dog_cat.h"
    eat = cat_eat
    speak = cat_speak
    sleep = cat_sleep
}

$export Animal as Dog
```

Generated header:

```c
#ifndef MM_Animal_H__
#define MM_Animal_H__

#include "dog_cat.h"

#define eat   dog_eat
#define speak dog_speak
#define sleep dog_sleep

#endif
```

Your code simply calls

```c
eat();
speak();
sleep();
```

Changing the implementation only requires changing the export.

Instead of editing the configuration file, it can also be overridden from the command line:

```sh
macromancer animals.mm --export Animal=Cat -o animals.h
```

This is useful when selecting implementations from a build system.

## Dynamic Interfaces

A dynamic interface generates a vtable so the implementation can be changed at runtime.

Input:

```text
$interface Animal as Dynamic {
    eat
    speak
    sleep
}

$impl Dog as Animal {
    $header = "dog_cat.h"
    eat = dog_eat
    speak = dog_speak
    sleep = dog_sleep
}

$impl Cat as Animal {
    $header = "dog_cat.h"
    eat = cat_eat
    speak = cat_speak
    sleep = cat_sleep
}

$export Animal as Dog
```

Generated header:

```c
#ifndef MM_Animal_H__
#define MM_Animal_H__

#include "dog_cat.h"

struct MM_AnimalInterface {
    typeof(dog_eat)   *_eat;
    typeof(dog_speak) *_speak;
    typeof(dog_sleep) *_sleep;
};

extern struct MM_AnimalInterface mm_Animal_iface;

extern const struct MM_AnimalInterface mm_Dog_vt;
extern const struct MM_AnimalInterface mm_Cat_vt;

#define eat   mm_Animal_iface._eat
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
```

Switching implementations is just an assignment:

```c
mm_Animal_iface = mm_Dog_vt;

/* ... */

mm_Animal_iface = mm_Cat_vt;
```

Calls to `eat()`, `speak()`, and `sleep()` will use whichever implementation is currently assigned.

## Available commands
```sh
macromancer --export Interface=Implementation
```
(-e shorthand exists for --export)

```sh
macromancer inputfile -o outputfile
```
(--output works too)

If no -o argument is provided, it by default prints the output to stdout
```sh
macromancer inputfile
```

To view help:
```sh
macromancer --help
```

## Notes

- Static interfaces have no runtime overhead.
- Dynamic interfaces use a single vtable.
- Generated code is ordinary C.
- Doesn't need any additional tooling.
- No compiler extensions are required.
- Macromancer only generates code. Compilation is left to your normal build system.
