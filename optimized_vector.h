#ifndef VECTOR_H
#define VECTOR_H

#include <cstdlib>
#include <cstring>
#include <variant>
#include <memory>

struct fast_vector {
public:
    fast_vector();
    ~fast_vector();
    explicit fast_vector(size_t nsize);
    fast_vector(fast_vector const &other);

    void reserve(size_t capacity);
    size_t size() const;
    bool is_empty();
    

    uint32_t& operator[](size_t ind);
    uint32_t const& operator[](size_t ind) const;

    fast_vector& operator=(fast_vector const &other);

    void pop_back();
    void push_back(uint32_t a);
    uint32_t back();

    void swap(fast_vector &other) noexcept;
    friend bool operator==(const fast_vector &a, const fast_vector &b);
    void prepare_to_new();

private:
    size_t get_capacity() const;
    static const size_t SMALL_SIZE = 4;

    size_t _size;
    struct _big {
        size_t capacity;
        std::shared_ptr<uint32_t> ptr;
        _big(uint32_t* a, size_t capacity);
        void swap(_big &other) noexcept;
    };

    union any_data {
        uint32_t small_data[SMALL_SIZE];
        _big big_data;
        any_data() {};
        ~any_data() {};
    } _data;

    struct destructor {
        void operator()(uint32_t* p) {
            operator delete(p);
        }
    };

    uint32_t* cur_data;

    bool is_big() const;
    void make_big(size_t capacity);
    void set_capacity(size_t capacity);

    void swap_big_and_small(any_data &a, any_data &b) noexcept;
};

#endif
