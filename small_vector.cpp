#include "small_vector.h"
#include "shared_vec.h"

small_vector::small_vector() : _size(0), small_data(0), data(NULL) {}

small_vector::small_vector(size_t init_size) : _size(init_size), small_data(0){
    data = new shared_vector(init_size);
}

small_vector::small_vector(small_vector const& other) {
    if (other.data != NULL) {
		data = other.data;
		data->reference_counter++;
	} else {
		data = NULL;
		small_data = other.small_data;
	}
	_size = other._size;
}

void small_vector::aquire(shared_vector* ptr){
    data = ptr;
    ptr->reference_counter++;
}

void small_vector::release(){
    if (data->unique())
        delete data;
    else
        data->reference_counter--;
    data = NULL;
}

small_vector& small_vector::operator=(small_vector const& other){
    if (this == &other)
        return *this;
    if (data != NULL)
        release();
    if (other.data != NULL)
        aquire(other.data);
    else {
        data = NULL;
        small_data = other.small_data;
    }
    _size = other._size;
    return *this;
}

small_vector::~small_vector(){
    if (data != NULL)
        release();
}

size_t small_vector::size() const{
    return _size;
}

bool small_vector::empty() const{
    return _size == 0;
}

unsigned int small_vector::front() const{
    if (data != NULL)
        return data->front();
    return small_data;
}

unsigned int small_vector::back() const{
    if (data != NULL)
        return data->back();
    else {
        return small_data;
    }
}

void small_vector::fork(){
    if (data != NULL){
        if (!data->unique()){
            data->reference_counter--;
            data = new shared_vector(*data);
            data->reference_counter = 1;
            _size = data->get_size();
        }
    } else {
        data = new shared_vector();
    }
}

void small_vector::push_back(unsigned int a) {
    if (data != NULL){
        fork();
        data->push_back(a);
        _size++;
    }
    else if (_size == 0){
        small_data = a;
        _size = 1;
    }
    else if (_size == 1){
        data = new shared_vector();
        data->push_back(small_data);
        data->push_back(a);
        _size++;
    }
}

void small_vector::pop_back(){
    if (data != NULL){
        if (_size > 0){
            fork();
            data->pop_back();
            _size--;
        }
    } else {
        _size = 0;
    }
}

void small_vector::resize(size_t new_size){
    if (data != NULL || new_size > 1){
        fork();
        data->resize(new_size);
        if (_size == 1)
            (*data)[0] = small_data;
        _size = new_size;
    }
    else
        _size = new_size;
}

void small_vector::clear(){
    if (data != NULL){
        if (data->unique())
            delete data;
        else
            release();
    }
    _size = 0;
}

unsigned int& small_vector::operator[](size_t i){
    if (data != NULL){
        fork();
        return (*data)[i];
    }
    return small_data;
}

const unsigned int& small_vector::operator[](size_t i) const{
    if (data != NULL){
        return (*data)[i];
    }
    return small_data;
}
