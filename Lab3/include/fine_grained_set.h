#pragma once

#include <string>
#include <semaphore.h>

#include "iset.h"


class FineGraindedSet : public ISet {
private:
    class Node {
    private:
        int val;
        pthread_mutex_t node_mutex;

    public:
        Node* next;

    public:
        static Node* create(int value);

        ~Node();

        bool lock();
        bool unlock();

        int value() const;

    public:
        Node(int value);
    };

private:
    Node* head;

public:
    static FineGraindedSet* create();

    ~FineGraindedSet();

    bool add(int value) override;
    bool remove(int value) override;
    bool contains(int value) override;
    bool is_empty() const override;

    ISet* empty() const override;

private:
    FineGraindedSet(Node* head);

    Node* find_node(int value);

};
