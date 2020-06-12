#pragma once 

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <list.h>

typedef enum {
    CleaningType_FUNCTION,
    CleaningType_VARIABLES,
    CleaningType_SIMPLE
} CleaningType;

typedef struct __Dict Dict;

Dict * Dict_new(CleaningType type);
void Dict_free(Dict * self);

void   Dict_add      (Dict * self, char * key, void * value);
bool   Dict_contains (Dict * self, char * key);
void * Dict_get      (Dict * self, char * key);
char * Dict_getName  (Dict * self, char * key);
void * Dict_set      (Dict * self, char * key, void * value);
void * Dict_remove   (Dict * self, char * key);
void   Dict_clear    (Dict * self);
size_t Dict_count    (Dict * self);

void   Dict_keys     (Dict * self, List * keys);
void   Dict_values   (Dict * self, List * values);
