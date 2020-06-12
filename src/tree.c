#include <tree.h>
#include <stdlib.h>

Tree * Tree_new(void * value) {
    Tree * self = malloc(sizeof(Tree));

    self->value = value;
    self->children = List_new();

    return self;
}

void Tree_free(Tree * self) {
    free(self->value);
    free(self->children);
    free(self);
}