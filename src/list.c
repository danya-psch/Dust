#include <list.h>

typedef struct __Node {
    struct __Node *next;
    struct __Node *prev;
    void *data;
} Node;

struct __List {
    Node *head;
    Node *tail;
    size_t length;
};

List * List_new(void) {
    List * list = (List *)malloc(sizeof(List));
    list->length = 0;
    list->head = NULL;
    list->tail = NULL;

    return list;
}

void List_insert(List * self, void * value, size_t index) {
    if (self == NULL || index > self->length || value == NULL) return;

    Node * node = (Node*)malloc(sizeof(Node));
    node->data = value;

    if (index == 0) {
        node->next = self->head;
        if (self->head != NULL) {
            self->head->prev = node;
        }
        node->prev = NULL;
        self->head = node;
        if (self->tail == NULL) {
            self->tail = node;
        }
    } else if (index == self->length){
        self->tail->next = node;
        node->prev = self->tail;
        self->tail = node;
    } else {
        Node * cur = self->head;
        for (int i = 0; i < index - 1;i++) {
            cur = cur->next;
        }
        node->next = cur->next;
        cur->next->prev = node;
        cur->next = node;
        node->prev = cur;
    }

    self->length++;
}

void List_free(List * self) {
    if (self == NULL) return;

    if (self->length > 0) {
        Node *cur = self->tail;
        while (cur != NULL) {
            Node * temp = cur;
            cur = cur->prev;
            free(temp);
        }
        self->length = 0;
        self->head = NULL;
        self->tail = NULL;
    }

    free(self);
}

void List_add(List * self, void * value) {
    if (self == NULL || value == NULL) return; // || value == NULL
    Node * node = (Node *)malloc(sizeof(Node));
    node->data = value;
    node->next = NULL;

    if (self->tail == NULL) {
        self->head = node;
        self->head->next = NULL;
        self->head->prev = NULL;
    } else {
        self->tail->next = node;
        node->prev = self->tail;
    }
    self->tail = node;
    self->length++;
}

void * List_at(List * self, size_t index) {
    if (self == NULL || index >= self->length) return NULL;

    Node * cur = self->head;

    for (int i = 0; i < index;i++) {
        cur = cur->next;
    }

    return cur->data;
}

void * List_removeAt(List * self, size_t index) {
    if (self == NULL || index >= self->length) return NULL;

    Node * cur;

    if (index == 0) {
        cur = self->head;
        self->head = self->head->next;
        
        if (self->head == NULL) {
            self->tail = NULL;
        } else {
            self->head->prev = NULL;
        }
    } else {
        Node * temp = self->head;

        for (int i = 0; i < index - 1;i++) {
            temp = temp->next;
        }
        cur = temp->next;
        temp->next = cur->next;
    
        if (cur == self->tail) {
            self->tail = temp;
            temp->next = NULL;
        } else {
            cur->next->prev = temp;
        }
    }

    void * value = cur->data;
    free(cur);
    self->length--;

    return value;
}

void List_copyTo(List * self, List * dest)  {
    List * vals = self;
    for(int i = 0; i < List_count(vals); i++) {
        void * value = List_at(vals, i);
        List_add(dest, value);
    }
}

size_t List_count(List * self) {
    if (self == NULL) return 0;
    return self->length;
}

// iterator

struct __Iterator {
    List * list;
    Node * node;
};

static Iterator * Iterator_new(List * list, Node * node) {
    Iterator * self = malloc(sizeof(Iterator));
    self->list = list;
    self->node = node;
    return self;
}

Iterator * List_getNewBeginIterator (List * self) {
    return Iterator_new(self, self->head);
}

Iterator * List_getNewEndIterator   (List * self) {
    return Iterator_new(self, self->tail->next);
}

void Iterator_free(Iterator * self) {
    free(self);
}

void * Iterator_value    (Iterator * self) {
    return self->node->data;
}

void   Iterator_next     (Iterator * self) {
    self->node = self->node->next;
}

void   Iterator_prev     (Iterator * self) {
    self->node = self->node->prev;
}

void   Iterator_advance  (Iterator * self, IteratorDistance n) {
    for (int i = 0; i < n; i++) {
        if (n > 0) {
            self->node = self->node->next;
        } else {
            self->node = self->node->prev;
        }
    }
}

bool   Iterator_equals   (Iterator * self, Iterator * other) {
    // if (self->list != other->list) return false; 
    return self->node == other->node;

}

IteratorDistance Iterator_distance (Iterator * begin, Iterator * end) {
    IteratorDistance i = 0;
    Node * cur = begin->node; 
    while (cur != end->node) {
        cur = cur->next;
        i++;
    }
    return i;
}
