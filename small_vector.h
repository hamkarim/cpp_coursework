#pragma once
#include "shared_vec.h"
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <climits>
#include <string>
#include <algorithm> 
#include <iostream>
#include <iosfwd>

struct small_vector{
public:
    small_vector();
    small_vector(size_t size);
    small_vector(small_vector const& other);

    small_vector& operator=(small_vector const& other);

    ~small_vector();

    size_t size() const;
    bool empty() const;

    unsigned int front() const;
    unsigned int back() const;
    void push_back(unsigned int a);
    void pop_back();
    void resize(size_t new_size);
    void clear();

    unsigned int& operator[](size_t i);
    const unsigned int& operator[](size_t) const;

private:
    unsigned int small_data;
    shared_vector* data;
    size_t _size;
    void release();
    void aquire(shared_vector* ptr);
    void fork();
};
