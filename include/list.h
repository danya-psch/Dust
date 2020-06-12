#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<iterator.h>

typedef struct __List List;

List * List_new(void);
void List_free(List * self);

void List_insert(List * self, void * value, size_t index);
void List_add(List * self, void * value);
void * List_at(List * self, size_t index);
void * List_removeAt(List * self, size_t index);
void List_copyTo(List * self, List * dest);
size_t List_count(List * self);

Iterator * List_getNewBeginIterator (List * self);
Iterator * List_getNewEndIterator   (List * self);

List * List_cpy(List * a);