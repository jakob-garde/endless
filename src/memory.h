#ifndef __MEMORY_H__
#define __MEMORY_H__


// for release builds, we should set DEBUG_BUILD = 0
#ifndef DEBUG_BUILD
#define DEBUG_BUILD 1
#endif


#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <math.h>


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;


//
//  Arena
//


struct MArena {
    u8 *mem;
    u64 cap;
    u64 used;
};

MArena ArenaCreate(void *mem, u64 capacity = 0) {
    MArena a = {};
    a.cap = capacity;
    a.mem = (u8*) mem;
    return a;
}

void *ArenaAlloc(MArena *a, u64 len, bool zerod = true) {
    assert(a->cap >= a->used + len && "ArenaAlloc: capaciry exceeded");

    void *result = a->mem + a->used;
    a->used += len;
    memset(result, 0, len);

    return result;
}

void *ArenaPush(MArena *a, void *data, u32 len) {
    void *dest = ArenaAlloc(a, len);
    memcpy(dest, data, len);
    return dest;
}

void ArenaClear(MArena *a) {
    a->used = 0;
}

// template arg versions
template<typename T> 
inline
T *ArenaAlloc(MArena *a) {
    return (T*) ArenaAlloc(a, sizeof(T));
}

template<typename T> 
inline
T *ArenaPush(MArena *a, T element) {
    return (T*) ArenaPush(a, &element, sizeof(T));
}


//
//  Array
//


// forward declarations for Array method use
template<typename T> struct Array;
template<typename T> Array<T> InitArray(MArena *a, u32 max_len);


template<typename T>
struct Array {
    T *arr = NULL;
    u32 len = 0;
    u32 cap = 0;

    inline
    T *Add(T element) {
        assert(len < cap);

        arr[len++] = element;
        return arr + len - 1;
    }
    T *AddSafe(T element) {
        if (len < cap) {
            return Add(element);
        }
        else {
            return NULL;
        }
    }

    inline
    T *Expand(MArena *a_dest, T element) {
        void *loc = ArenaAlloc(a_dest, sizeof(T));
        assert(arr + len == loc && "must be consecutive");
        cap++;

        return Add(element);
    }

    void InitForExpand(MArena *a_dest) {
        Array<T> result = InitArray<s32>(a_dest, 0);
        arr = result.arr;
    }

    void InitForCapacity(MArena *a_dest, u32 capacity) {
        *this = InitArray<s32>(a_dest, capacity);
    }
};

template<class T>
Array<T> InitArray(MArena *a, u32 max_len) {
    Array<T> _arr = {};
    _arr.len = 0;
    _arr.cap = max_len;
    _arr.arr = (T*) ArenaAlloc(a, sizeof(T) * max_len);
    return _arr;
}


//
//  Hasing
//


u64 Hash(u64 x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x);
    return x;
}


inline
u64 HashDJB2(const char *key) {
    // djb2 - see http://www.cse.yorku.ca/~oz/hash.html
    u64 hash = 5381;

    s32 c;
    u32 i = 0;
    u32 len = strlen(key);
    while (i < len) {
        c = key[i];
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
        i++;
    }
    return hash;
}

inline
u64 HashStringValue(const char *key) {
    u64 hash = HashDJB2( key );
    return hash;
}


// TODO: Str and Str version of hashing


/*
inline
u64 HashDJB2(Str skey) {
    // djb2 - see http://www.cse.yorku.ca/~oz/hash.html
    u64 hash = 5381;

    s32 c;
    u32 i = 0;
    while (i < skey.len) {
        c = skey.str[i];
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
        i++;
    }
    return hash;
}

inline
u64 HashStringValue(Str key) {
    u64 hash = HashDJB2(key);
    return hash;
}
*/


//
//  RNG
//


void Kiss_SRandom(u64 state[7], u64 seed) {
    if (seed == 0) seed = 1;
    state[0] = seed | 1; // x
    state[1] = seed | 2; // y
    state[2] = seed | 4; // z
    state[3] = seed | 8; // w
    state[4] = 0;        // carry
}
u64 Kiss_Random(u64 state[7]) {
    state[0] = state[0] * 69069 + 1;
    state[1] ^= state[1] << 13;
    state[1] ^= state[1] >> 17;
    state[1] ^= state[1] << 5;
    state[5] = (state[2] >> 2) + (state[3] >> 3) + (state[4] >> 2);
    state[6] = state[3] + state[3] + state[2] + state[4];
    state[2] = state[3];
    state[3] = state[6];
    state[4] = state[5] >> 30;
    return state[0] + state[1] + state[3];
}
u64 g_kiss_randstate[7];

#include <sys/time.h>
u64 ReadSystemTimerMySec() {
    u64 systime;
    struct timeval tm;
    gettimeofday(&tm, NULL);
    systime = (u32) tm.tv_sec*1000000 + tm.tv_usec; // microsecs 

    return systime;
}

// WINDOWS:
/*
u64 ReadSystemTimerMySec() {
    // systime (via S.O. 10905892)

    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);
    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;
    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    long tv_sec  = (long) ((time - EPOCH) / 10000000L);
    long tv_usec = (long) (system_time.wMilliseconds * 1000);
    u64 systime = (u32) tv_sec*1000000 + tv_usec; // microsecs 
    return systime;
}
*/

u32 RandInit(u32 seed = 0) {
    if (seed == 0) {
        seed = (u32) Hash(ReadSystemTimerMySec());
    }
    Kiss_SRandom(g_kiss_randstate, seed);
    Kiss_Random(g_kiss_randstate); // flush the first one

    return seed;
}

#ifndef ULONG_MAX
#  define ULONG_MAX ( (u64) 0xffffffffffffffffUL )
#endif

f32 Rand01() {
    f32 randnum = (f32) Kiss_Random(g_kiss_randstate);
    randnum /= (f32) ULONG_MAX + 1;
    return randnum;
}

f32 RandPM1() {
    f32 randnum = (f32) Kiss_Random(g_kiss_randstate);
    randnum = 2.0f * randnum / ((f32) ULONG_MAX + 1) - 1;
    return randnum;
}

bool RandBin() {
    u64 num =  Kiss_Random(g_kiss_randstate) % 2;
    return num == 1;
}

s32 Rand(u64 max_plus_one) {
    u64 num =  Kiss_Random(g_kiss_randstate) % max_plus_one;
    return (s32) num;
}


//
//  Pointer map
//


struct KeyVal {
    u64 key;
    u64 val;
    s64 next;
};

struct HashMap {
    Array<KeyVal> slots;
    u32 collisions;
    u32 load;
    u32 overflows;

    void Print() {
        printf("load: %u, collisions: %u, overflows: %u\n", load, collisions, overflows);
    }
    void PrintElements() {
        for (s32 i = 0; i < slots.len; ++i) {
            KeyVal kv = slots.arr[i];
            printf("%d: key: %lu, val: %lu, next: %ld\n", i, kv.key, kv.val, kv.next);
        }
    }
};

HashMap InitMap(MArena *a_dest, u32 nslots = 1023) {
    HashMap map = {};
    map.slots = InitArray<KeyVal>(a_dest, nslots);
    map.slots.len = nslots;
    return map;
}

void MapClear(HashMap *map) {
    memset(map->slots.arr, 0, sizeof(KeyVal) * map->slots.len);
    map->collisions = 0;
    map->load = 0;
    map->overflows = 0;
}

struct MapIter {
    s32 slot_idx;
    s32 occ_slots_cnt;
};

s64 MapPut(HashMap *map, u64 key, u64 val) {
    assert(key != 0);

    u64 len = (u64) map->slots.len;

    // full-guard
    if (map->load == len) {
        map->overflows++;
        return -1;
    }

    KeyVal *slot0 = map->slots.arr + (key % len);
    KeyVal *slot;

    if (slot0->next || slot0->key) {
        map->collisions++;

        // find the end of the collision chain - or overwrite at matching key within the chain
        while (slot0->next && slot0->key) {

            if (slot0->key == key) {
                // overwrite
                slot0->val = val;

                return slot - map->slots.arr;
            }

            slot0 = slot0 + slot0->next;
        }
        slot = slot0;

        // fint an empty slot
        while (slot->key != 0) {
            slot++;

            // wrap-around
            if (slot == map->slots.arr + len) {
                slot = map->slots.arr;
            }
        }

        // add to the collision chain
        if (slot0 != slot) {
            slot0->next = slot - slot0;
        }

        // sanity check pointer are in range
        assert(slot >= map->slots.arr);
        assert(slot < map->slots.arr + len);
        assert(slot0 >= map->slots.arr);
        assert(slot0 < map->slots.arr + len);
    }
    else {
        slot = slot0;
    }

    slot->key = key;
    slot->val = val;
    map->load++;

    return slot - map->slots.arr;
}

u64 MapGet(HashMap *map, u64 key) {
    if (key == 0) {
        return 0;
    }
    u64 len = (u64) map->slots.len;

    // check the base slot
    KeyVal *slot = map->slots.arr + (key % len);

    if (slot->key == key) {
        return slot->val;
    }

    // iterate the collision chain
    s64 key_stop = slot->key;
    while (slot->next) {
        slot = slot + slot->next;

        if (slot->key == key) {
            return slot->val;
        }
        else if (slot->key == key_stop) {
            break;
        }
    }

    // no takers
    return 0;
}

s64 MapGetIndex(HashMap *map, u64 key, s64 *prev_idx) {
    assert(prev_idx);
    *prev_idx = -1;

    u64 len = (u64) map->slots.len;

    // check the base slot
    KeyVal *slot = map->slots.arr + (key % len);
    if (slot->key == key) {
        return slot - map->slots.arr;
    }

    // iterate the collision chain
    s64 key_stop = slot->key;
    while (slot->next) {
        *prev_idx = slot - map->slots.arr;
        slot = slot + slot->next;

        if (slot->key == key) {
            return slot - map->slots.arr;
        }
        else if (slot->key == key_stop) {
            break;
        }
    }

    // no get
    *prev_idx = -1;
    return -1;
}

s64 MapRemove(HashMap *map, u64 key) {
    u64 len = (u64) map->slots.len;
    s64 prev_idx;
    s64 remove_idx = MapGetIndex(map, key, &prev_idx);

    if (remove_idx == -1) {
        return -1;
    }

    KeyVal *remove = map->slots.arr + remove_idx;
    assert(remove->key == key);

    if (prev_idx >= 0) {
        KeyVal *prev = map->slots.arr + prev_idx;
        assert(prev->next + prev_idx == remove - map->slots.arr);

        if (prev->key == 0 && prev->val == 0) {
            assert(prev->next);
        }
        if (remove->next) {
            prev->next += remove->next;
        }
        else {
            prev->next = 0;
        }
    }

    remove->key = 0;
    remove->val = 0;
    map->load--;

    return remove_idx;
}

inline
s64 MapPut(HashMap *map, void *key, void *val) {
    return MapPut(map, (u64) key, (u64) val);
}
inline
s64 MapPut(HashMap *map, u64 key, void *val) {
    return MapPut(map, key, (u64) val);
}

/*
inline
s64 MapPut(HashMap *map, Str skey, void *val) {
    return MapPut(map, HashStringValue(skey), (u64) val);
}
inline
u64 MapGet(HashMap *map, Str skey) {
    return MapGet(map, HashStringValue(skey));
}
inline
s64 MapRemove(HashMap *map, Str skey) {
    return MapRemove(map, HashStringValue(skey));
}
*/


#endif
