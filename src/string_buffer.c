#include<string_buffer.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include<stdarg.h>
#include<string_dup.h>


struct __StringBuffer {
    char * array;
    size_t capacity;
    size_t length;
};

static const size_t INITIAL_CAPASITY = 100;

StringBuffer * StringBuffer_new(void) {
    StringBuffer * self = malloc(sizeof(StringBuffer));
    self->capacity = INITIAL_CAPASITY; 
    self->array = malloc(sizeof(char) * self->capacity);
    StringBuffer_clear(self);
    return self;
}

void StringBuffer_free(StringBuffer * self) {
    free(self->array);
    free(self);
}

static void ensureCapacity(StringBuffer * self, size_t appendLength) {
    while (self->length + appendLength > self->capacity) {
        size_t newCapacity = self->capacity * 2;
        char * newMem = realloc(self->array, sizeof(char) * newCapacity);
        if (newMem == NULL) {
            // @todo error
        }
        self->array = newMem;
        self->capacity = newCapacity;
    }
}

void StringBuffer_append(StringBuffer * self, const char * str) {
    size_t strLen = strlen(str); 
    ensureCapacity(self, strLen);
    strcat(self->array + self->length - 1, str);
    self->length += strLen;
}

void StringBuffer_appendChar(StringBuffer * self, char ch) {
    ensureCapacity(self, 1);
    self->array[self->length - 1] = ch;
    self->array[self->length] = '\0';
    self->length++;
}

void StringBuffer_appendFormat(StringBuffer * self, const char * fmt, ...) {
    va_list vlist;
    va_start(vlist, fmt);
    size_t bufsz = vsnprintf(NULL, 0, fmt, vlist);
    char * buf = malloc(bufsz + 1);
    va_start(vlist, fmt);
    vsnprintf(buf, bufsz + 1, fmt, vlist);
    va_end(vlist);
    StringBuffer_append(self, buf);
    free(buf);
}

void StringBuffer_clear(StringBuffer * self) {
    self->array[0] = '\0';
    self->length = 1;
}

// https://stackoverflow.com/a/37132824
// static char * strdup(char *src) {
//     char *str;
//     char *p;
//     int len = 0;

//     while (src[len])
//         len++;
//     str = malloc(len + 1);
//     p = str;
//     while (*src)
//         *p++ = *src++;
//     *p = '\0';
//     return str;
// }

char * StringBuffer_toNewString(StringBuffer * self) {
    char * str = self->array;
    return strdup(str); 
}
