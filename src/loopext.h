#ifndef __LOOP_EXT_H__
#define __LOOP_EXT_H__


#include "memory.h"
#include "entities.h"
#include "globals.h"
#include "assets.h"

// app specific
#include "map.h"


const char *title = "Endless Forest";


void InitExt() {
    // ...

    Map *map = InitForestMap(&a_life);
}


void UpdateExt(Entity *ent, f32 dt) {
    // ...
}


void EntityDrawExt(Array<Animation> animations, Entity *ent) {
    // ...
}


#endif
