#ifndef BIG_INT_H
#define BIG_INT_H

using namespace std;

#include "optimized_vector.h"
#include <string>
#include <cstdlib>

struct big_integer {
    big_integer();
    big_integer(big_integer const& other);
    big_integer(int a);
    big_integer(uint32_t a);
    big_integer(uint64_t a);
    explicit big_integer(string const& str);

    big_integer& operator=(big_integer const& other);

    big_integer abs() const;
    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(uint32_t rhs);
    big_integer& operator>>=(uint32_t rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend big_integer operator+(big_integer const &a, big_integer const& b);
    friend big_integer operator-(big_integer const &a, big_integer const& b);
    friend big_integer operator*(big_integer const &a, big_integer const& b);
    friend big_integer operator/(big_integer const &a, big_integer const& b);
    friend big_integer operator%(big_integer const &a, big_integer const& b);

    friend big_integer operator&(big_integer const &a, big_integer const& b);
    friend big_integer operator|(big_integer const &a, big_integer const& b);
    friend big_integer operator^(big_integer const &a, big_integer const& b);
    friend big_integer operator<<(big_integer const &a, uint32_t b);
    friend big_integer operator>>(big_integer const &a, uint32_t b);

    friend string to_string(big_integer const& a);
    void swap(big_integer &other) noexcept;
    bool is_zero() const;
    bool is_negative() const;
    big_integer(bool new_sign, fast_vector const &new_data);
private:
    bool sign;
    fast_vector array;
    size_t size() const;
    uint32_t get_digit(size_t ind) const;
    uint32_t get_real_digit(size_t ind) const;
    void delete_zero();
    void correct();
    big_integer negate() ;
    big_integer dividebi(uint32_t rhs);
    big_integer dividebi(big_integer const &rhs);
};


#endif