#pragma once
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <climits>
#include <string>
#include <algorithm> 
#include <iostream>
#include <iosfwd>

struct shared_vector{
    std::vector<unsigned int> data;
public:
    size_t reference_counter;
    shared_vector();
    shared_vector(size_t size);
    shared_vector(shared_vector const& other);

    shared_vector& operator=(shared_vector const& other);

    size_t get_size() const;
    bool is_empty() const;
    unsigned int front() const;
    unsigned int back() const;
    bool unique() const;

    void push_back(unsigned int a);
    void pop_back();
    void resize(size_t new_size);

    unsigned int& operator[](size_t i);
    const unsigned int& operator[](size_t i) const;
    void clear();
};
