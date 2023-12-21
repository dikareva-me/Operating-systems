#include "fine_grained_set.h"

#include <string>
#include <iostream>

#include <fcntl.h>


FineGraindedSet::Node* FineGraindedSet::Node::create(int value) {
    return new Node(value);
}

FineGraindedSet::Node::Node(int value) : val{ value }, next{ nullptr } {
    node_mutex = PTHREAD_MUTEX_INITIALIZER;
}

FineGraindedSet::Node::~Node() {
    pthread_mutex_destroy(&node_mutex);
}

bool FineGraindedSet::Node::lock() {
    if (pthread_mutex_lock(&node_mutex) != 0) {
        return false;
    }

    return true;
}

bool FineGraindedSet::Node::unlock() {
    if (pthread_mutex_unlock(&node_mutex) != 0) {
        return false;
    }

    return true;
}

int FineGraindedSet::Node::value() const {
    return val;
}


FineGraindedSet* FineGraindedSet::create() {
    Node* head = Node::create(INT32_MIN);

    if (!head) {
        return nullptr;
    }

    Node* tail = Node::create(INT32_MAX);
    if (!tail) {
        return nullptr;
    }

    head->next = tail;

    return new FineGraindedSet(head);
}

FineGraindedSet::FineGraindedSet(Node* head) : head{ head } {}

FineGraindedSet::~FineGraindedSet() {
    Node* node = head;
  
    while (node != nullptr) {
        Node* tmp = node;
        node = node->next;
        
        // std::cout << tmp->value() << std::endl;

        delete tmp;
    }
}

bool FineGraindedSet::add(int value) {
    head->lock();

    Node* pred = head;
    Node* curr = pred->next;
    curr->lock();

    while (curr->value() < value) {
        pred->unlock();
        pred = curr;
        curr = curr->next;
        curr->lock();
    }

    if (value == curr->value()) {
        pred->unlock();
        curr->unlock();
        return false;
    }

    Node* new_node = Node::create(value);
    if (!new_node) {
        pred->unlock();
        curr->unlock();
        return false;
    }

    pred->next = new_node;
    new_node->next = curr;

    pred->unlock();
    curr->unlock();

    return true;
}

bool FineGraindedSet::remove(int value) {
    head->lock();

    Node* pred = head;
    Node* curr = pred->next;
    curr->lock();

    while (curr->value() < value) {
        pred->unlock();
        pred = curr;
        curr = curr->next;
        curr->lock();
    }
    if (curr->value() == value) {
        Node* tmp = curr;
        pred->next = curr->next;

        delete tmp;

        pred->unlock();
        curr->unlock();

        return true;
    }

    pred->unlock();
    curr->unlock();
    return false;

}

bool FineGraindedSet::contains(int value) {
    Node* pred = head;
    pred->lock();

    Node* curr = pred->next;
    curr->lock();

    while (curr->value() < value) {
        pred->unlock();
        pred = curr;
        curr = curr->next;
        curr->lock();
    }
    if (curr->value() == value) {
        pred->unlock();
        curr->unlock();

        return true;
    }

    pred->unlock();
    curr->unlock();

    return false;
}

FineGraindedSet::Node* FineGraindedSet::find_node(int value) {
    Node* pred = head;
    Node* curr = pred->next;

    pred->lock();
    curr->lock();

    while (value > curr->value()) {
        pred->unlock();
        pred = curr;
        curr = curr->next;
        curr->lock();

    }

    if (value == curr->value()) {
        return pred;
    }

    pred->unlock();
    curr->unlock();

    return nullptr;
}

bool FineGraindedSet::is_empty() const {
    return head->value() == INT32_MIN && head->next->value() == INT32_MAX;
}

ISet* FineGraindedSet::empty() const {
    return create();
}
