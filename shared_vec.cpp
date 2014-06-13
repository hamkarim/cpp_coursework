#include "shared_vec.h"

shared_vector::shared_vector() : reference_counter(1) {}

shared_vector::shared_vector(size_t size) : data(size, 0), reference_counter(1) {}

shared_vector::shared_vector(shared_vector const& other) {
    data = other.data;
    reference_counter = 1;
}

shared_vector& shared_vector::operator=(shared_vector const& other){
    if (this == &other)
        return *this;
    data = other.data;
    reference_counter = 1;
    return *this;
}

size_t shared_vector::get_size() const{
    return data.size();
}

bool shared_vector::is_empty() const{
    return data.empty();
}

bool shared_vector::unique() const{
    return reference_counter == 1;
}

unsigned int shared_vector::front() const{
    return data.front();
}

unsigned int shared_vector::back() const{
    return data.back();
}

void shared_vector::push_back(unsigned int a){
    data.push_back(a);
}

void shared_vector::pop_back(){
    data.pop_back();
}

void shared_vector::resize(size_t new_size){
    data.resize(new_size);
}

unsigned int& shared_vector::operator[](size_t i){
    return data[i];
}

const unsigned int& shared_vector::operator[](size_t i) const{
    return data[i];
}

void shared_vector::clear(){
    data.clear();
}
