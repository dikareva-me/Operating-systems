#pragma once

class ISet {
public:
    virtual ~ISet() = 0;

    virtual bool add(int val) = 0;
    virtual bool remove(int val) = 0;
    virtual bool contains(int val) = 0;
    virtual bool is_empty() const = 0;

    virtual ISet* empty() const = 0;

};

inline ISet::~ISet() {}
