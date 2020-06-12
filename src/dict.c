#include <dict.h>
#include<interpreter.h> 

typedef struct {
    char * key;
    void * value;
} KeyValuePair;

struct __Dict {
    List * pairs;
    CleaningType type;
};

KeyValuePair * Kvp_new(char * key, void * value) {
    KeyValuePair * self = malloc(sizeof(KeyValuePair));
    self->key = key;
    self->value = value;
    return self;
}

void Kvp_free(KeyValuePair * self, CleaningType type) {
    switch(type) {
        case CleaningType_FUNCTION: {
            free(self->value);
            break;
        }
        case CleaningType_VARIABLES: {
            Value * val = (Value*)self->value;
            if (val->type == ValueType_ARRAY) {
                Array * arr = val->value;
                for (int i = 0; i < List_count(arr->list); i++) {
                    Value_free(List_at(arr->list, i));
                }
                List_free(arr->list);
            }
            free(self->key);
            break;
        }
        default : {break;}
    }
    free(self);
}

Dict * Dict_new(CleaningType type) {
    Dict * self = malloc(sizeof(Dict));
    self->pairs = List_new();
    self->type = type;
    return self;
}
void Dict_free(Dict * self) {
    size_t count = List_count(self->pairs);
    for (int i = 0; i < count; i++) {
        Kvp_free(List_at(self->pairs, i), self->type);
    }
    List_free(self->pairs);
    free(self);
}

void   Dict_add      (Dict * self, char * key, void * value) {
    KeyValuePair * pair = Kvp_new(key, value);
    List_add(self->pairs, pair);
}
bool   Dict_contains (Dict * self, char * key) {
    for (int i = 0; i < List_count(self->pairs); i++) {
        KeyValuePair * pair = List_at(self->pairs, i);
        if (0 == strcmp(pair->key, key)) return true;
    }
    return false;
}
void * Dict_get      (Dict * self, char * key) {
    for (int i = 0; i < List_count(self->pairs); i++) {
        KeyValuePair * pair = List_at(self->pairs, i);
        if (0 == strcmp(pair->key, key)) return pair->value;
    }
    // @todo error 
    return NULL;
}
char * Dict_getName  (Dict * self, char * key) {
    for (int i = 0; i < List_count(self->pairs); i++) {
        KeyValuePair * pair = List_at(self->pairs, i);
        if (0 == strcmp(pair->key, key)) return pair->key;
    }
    // @todo error 
    return NULL;
}
void * Dict_set      (Dict * self, char * key, void * value) {
    for (int i = 0; i < List_count(self->pairs); i++) {
        KeyValuePair * pair = List_at(self->pairs, i);
        if (0 == strcmp(pair->key, key)) {
            void * oldValue = pair->value;
            pair->value = value;
            return oldValue;
        }
    }
    // @todo error 
    return NULL;
}
void * Dict_remove   (Dict * self, char * key) {
    int index = -1;
    for (int i = 0; i < List_count(self->pairs); i++) {
        KeyValuePair * pair = List_at(self->pairs, i);
        if (0 == strcmp(pair->key, key)) {
            index = i;
            break;
        }
    }
    if (index > -1) {
        KeyValuePair * remuvedPair = List_removeAt(self->pairs, index);
        void * oldValue = remuvedPair->value;
        Kvp_free(remuvedPair, self->type);
        return oldValue;
    }
    // @todo error 
    return NULL;
}
void   Dict_clear    (Dict * self) {
    for (int i = List_count(self->pairs) - 1; i >= 0; i--) {
        KeyValuePair * pair = List_at(self->pairs, i);
        (void)List_removeAt(self->pairs, i);
    } 
}
size_t Dict_count    (Dict * self) {
    return List_count(self->pairs);
}

void   Dict_keys     (Dict * self, List * keys) {
    for (int i = 0; i < List_count(self->pairs); i++) {
        KeyValuePair * pair = List_at(self->pairs, i);
        List_add(keys, pair->key);
    }
}
void   Dict_values   (Dict * self, List * values) {
    for (int i = 0; i < List_count(self->pairs); i++) {
        KeyValuePair * pair = List_at(self->pairs, i);
        List_add(values, pair->value);
    }
}