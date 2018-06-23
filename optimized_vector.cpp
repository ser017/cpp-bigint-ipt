#include "optimized_vector.h"
#include <cassert>

size_t get_new_capacity(const size_t n) {
    if (n == 0) return 4; //small_size
    else return (n * 3) / 2;
}

bool fast_vector::is_big() const {
    return (cur_data != _data.small_data);
}

size_t fast_vector::size() const {
    return _size;
}

fast_vector::fast_vector() : _size(0), cur_data(_data.small_data) {
    memset(cur_data, 0, SMALL_SIZE * sizeof(uint32_t));
}

fast_vector::fast_vector(size_t nsize) : fast_vector() {
    reserve(nsize);
    memset(cur_data, 0, nsize * sizeof(uint32_t));
    _size = nsize;
}

uint32_t* copy_data(const uint32_t* src, size_t cnt, size_t cap) {
    uint32_t* res = static_cast<uint32_t*>(operator new(cap * sizeof(uint32_t)));
    memcpy(res, src, cnt * sizeof(uint32_t));
    memset(res + cnt, 0, (cap - cnt) * sizeof(uint32_t));
    return res;
}

size_t fast_vector::get_capacity() const {
    if (is_big()) return _data.big_data.capacity;
    else return SMALL_SIZE;
}

void fast_vector::prepare_to_new() {
    if (is_big() && !_data.big_data.ptr.unique()) {
        _data.big_data.ptr.reset(copy_data(cur_data, size(), size()), destructor());
        _data.big_data.capacity = size();
        cur_data = _data.big_data.ptr.get();
    }
}

fast_vector::~fast_vector() {
    if (is_big()) _data.big_data.~_big();
}

fast_vector::_big::_big(uint32_t* a, size_t cap) : capacity(cap), ptr(a) {}


void fast_vector::make_big(size_t cap) {
    new(&_data.big_data) _big(copy_data(cur_data, _size, cap), cap);
    cur_data = _data.big_data.ptr.get();
}

void fast_vector::set_capacity(size_t cap) {
    if (is_big() || (cap > SMALL_SIZE)) {
        if (!is_big()) make_big(cap);
        else {
            _data.big_data.ptr.reset(copy_data(cur_data, size(), cap), destructor());
            _data.big_data.capacity = cap;
            cur_data = _data.big_data.ptr.get();
        }
    }
}

void fast_vector::reserve(size_t cap) {
    if (cap > get_capacity()) set_capacity(cap);
}

uint32_t const& fast_vector::operator[](size_t ind) const {
    return cur_data[ind];
}

uint32_t& fast_vector::operator[](size_t ind) {
    assert(!(is_big() && !_data.big_data.ptr.unique()));
    return cur_data[ind];
}

bool operator==(const fast_vector &a, const fast_vector &b) {
    if (a._size != b._size) return 0;
    return (memcmp(a.cur_data, b.cur_data, a._size * sizeof(uint32_t)) == 0);
}

bool fast_vector::is_empty() {
    return (_size == 0);
}

void fast_vector::push_back(uint32_t a) {
    if (get_capacity() < _size + 1) reserve(get_new_capacity(_size));
    cur_data[_size] = a;
    _size++;
}

void fast_vector::pop_back() {
    _size--;
}

uint32_t fast_vector::back() {
    return cur_data[_size - 1];
}

fast_vector::fast_vector(fast_vector const &other) : _size(other._size) {
    if (other.is_big()) {
        new (&_data.big_data) _big(other._data.big_data);
        cur_data = _data.big_data.ptr.get();
    } else {
        memcpy(_data.small_data, other._data.small_data, SMALL_SIZE * sizeof(uint32_t));
        cur_data = _data.small_data;
    }
}

void fast_vector::_big::swap(_big &other) noexcept {
    using std::swap;
    swap(ptr, other.ptr);
    swap(capacity, other.capacity);
}

void fast_vector::swap_big_and_small(typename fast_vector::any_data &a, typename fast_vector::any_data &b) noexcept {
    //a - big
    //b - small
    uint32_t temp[SMALL_SIZE];
    memcpy(temp, b.small_data, SMALL_SIZE * sizeof(uint32_t));
    new(&b.big_data) _big(a.big_data);
    a.big_data.~_big();
    memcpy(a.small_data, temp, SMALL_SIZE * sizeof(uint32_t));
}

void fast_vector::swap(fast_vector &other) noexcept {
    using std::swap;
    if (!is_big() && !other.is_big()) {
        for (size_t i = 0; i < SMALL_SIZE; i++) {
            swap(_data.small_data[i], other._data.small_data[i]);
        }
    } else if (is_big() && other.is_big()) {
        swap(_data.big_data, other._data.big_data);
        cur_data = _data.big_data.ptr.get();
        other.cur_data = other._data.big_data.ptr.get();
    } else if (is_big()) {
        swap_big_and_small(_data, other._data);
        cur_data = _data.small_data;
        other.cur_data = other._data.big_data.ptr.get();
    } else {
        swap_big_and_small(other._data, _data);
        other.cur_data = other._data.small_data;
        cur_data = _data.big_data.ptr.get();
    }
    swap(_size, other._size);
}

fast_vector& fast_vector::operator=(fast_vector const &other) {
    fast_vector temp(other);
    swap(temp);
    return *this;
}