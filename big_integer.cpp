#include "big_integer.h"
#include <algorithm>
#include <iostream>

using namespace std;

const uint32_t BASE_ARRAY = 32;
const int BASE_INT = (int)10e8;

template<typename T>
uint32_t toUint32(T x) {
    return static_cast<uint32_t>(x & 0xffffffff);
}

template<typename T>
uint64_t toUint64(T x) {
    return static_cast<uint64_t>(x);
}

big_integer big_integer::negate(){
    if (is_zero()) return *this;
    else if (size() == 0) return big_integer(1u);
    size_t n = array.size() + 2;
    fast_vector temp(n);
    uint64_t sum = toUint32(~get_digit(0)) + 1ULL, carry = sum >> BASE_ARRAY;
    temp[0] = toUint32(sum);
    for (size_t i = 1; i < n - 2; i++) {
        sum = carry + toUint64(~get_real_digit(i));
        temp[i] = toUint32(sum);
        carry = sum >> BASE_ARRAY;
    }
    for (size_t i = n - 2; i < n; i++) {
        sum = (carry + (sign ? 0 : 0xffffffff));
        temp[i] = toUint32(sum);
        carry = sum >> BASE_ARRAY;
    }
    return big_integer(temp.back() & (1 << (BASE_ARRAY - 1)), temp);
}



bool big_integer::is_zero() const{
    return (!sign) && (size() == 0);
}

size_t big_integer::size() const {
    return array.size();
}

bool big_integer::is_negative() const {
    return sign;
}

big_integer big_integer::abs() const {
    if (sign) return -(*this);
    else return *this;
}

void big_integer::swap(big_integer &num) noexcept {
    std::swap(array, num.array);
    std::swap(sign, num.sign);
}

uint32_t big_integer::get_real_digit(size_t index) const {
    return array[index];
}

uint32_t big_integer::get_digit(size_t index) const {
    if (index < size()) return array[index];
    else if (sign) return 0xffffffff;
    else return 0;
}


void big_integer::delete_zero() {
    while (!array.is_empty() && ((sign && array.back() == 0xffffffff) || (!sign && array.back() == 0))) {
        array.pop_back();
    }
}

big_integer::big_integer(bool new_sign, fast_vector const &new_array) : sign(new_sign), array(new_array) {
    delete_zero();
}

big_integer::big_integer() : sign(false) {}

big_integer::big_integer(big_integer const &num) : sign(num.sign), array(num.array) {
    delete_zero();
}

big_integer::big_integer(int a) : sign(a < 0), array(1) {
    array[0] = toUint32(a);
    delete_zero();
}

big_integer::big_integer(uint32_t a) : sign(0), array(1) {
    array[0] = a;
    delete_zero();
}

big_integer::big_integer(uint64_t a) : sign(0), array(2) {
    array[0] = toUint32(a);
    array[1] = toUint32(a >> BASE_ARRAY);
    delete_zero();
}

big_integer &big_integer::operator=(big_integer const &num) {
    big_integer temp(num);
    swap(temp);
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    big_integer res = (*this);
    big_integer a = res.negate();
    return a;
}

big_integer big_integer::operator~() const {
    big_integer res = (*this);
    big_integer a = res.negate();
    return a - big_integer(1);
}

big_integer& big_integer::operator++() {
    *this += 1;
    return *this;
}

big_integer big_integer::operator++(int) {
    big_integer res(*this);
    *this +=1;
    return res;
}

big_integer& big_integer::operator--() {
    *this -= 1;
    return *this;
}

big_integer big_integer::operator--(int) {
    big_integer temp(*this);
    --(*this);
    return temp;
}

big_integer operator&(big_integer const &a, big_integer const &b) {
    size_t n = max(a.size(), b.size()), m = min(a.size(), b.size());
    fast_vector temp(n);
    for (size_t i = 0; i < m; i++) {
        temp[i] = a.get_real_digit(i) & b.get_real_digit(i);
    }
    for (size_t i = m; i < n; i++) {
        temp[i] = a.get_digit(i) & b.get_digit(i);
    }
    return big_integer(a.sign & b.sign, temp);
}

big_integer operator|(big_integer const &a, big_integer const &b) {
    size_t n = max(a.size(), b.size()), m = min(a.size(), b.size());
    fast_vector temp(n);
    for (size_t i = 0; i < m; i++) {
        temp[i] = a.get_real_digit(i) | b.get_real_digit(i);
    }
    for (size_t i = m; i < n; i++) {
        temp[i] = a.get_digit(i) | b.get_digit(i);
    }
    return big_integer(a.sign | b.sign, temp);
}

big_integer operator^(big_integer const &a, big_integer const &b) {
    size_t n = max(a.size(), b.size()), m = min(a.size(), b.size());
    fast_vector temp(n);
    for (size_t i = 0; i < m; i++) {
        temp[i] = a.get_real_digit(i) ^ b.get_real_digit(i);
    }
    for (size_t i = m; i < n; i++) {
        temp[i] = a.get_digit(i) ^ b.get_digit(i);
    }
    return big_integer(a.sign ^ b.sign, temp);
}

big_integer operator<<(big_integer const &a, uint32_t b) {
    if (b == 0) return big_integer(a);
    size_t div = b >> 5;
    size_t mod = b & (BASE_ARRAY - 1);
    size_t new_size = a.size() + div + 1;
    fast_vector temp(new_size);
    temp[div] = toUint32(uint64_t(a.get_digit(0)) << mod);
    for (size_t i = div + 1; i < new_size; i++) {
        uint64_t x = uint64_t(a.get_digit(i - div)) << mod;
        uint64_t y = uint64_t(a.get_real_digit(i - div - 1)) >> (BASE_ARRAY - mod);
        temp[i] = toUint32(x | y);
    }
    return big_integer(a.sign, temp);
}

big_integer operator>>(big_integer const &a, uint32_t b) {
    if (b == 0) return big_integer(a);
    size_t div = b >> 5;
    size_t mod = b & (BASE_ARRAY - 1);
    size_t new_size = 0;
    if (div < a.size()) new_size = a.size() - div;
    fast_vector temp(new_size);
    for (size_t i = 0; i < new_size; i++) {
        uint64_t x = uint64_t(a.get_real_digit(i + div)) >> mod;
        uint64_t y = uint64_t(a.get_digit(i + div + 1)) << (BASE_ARRAY - mod);
        temp[i] = toUint32(x | y);
    }
    return big_integer(a.sign, temp);
}

int dec_pow(uint32_t st) {
    if (st == 0) return 1;
    if (st % 2) return dec_pow(st - 1) * 10;
    int t = dec_pow(st >> 1);
    return t * t;
}

bool operator==(big_integer const &a, big_integer const &b) {
    return (!a.array.size() && !b.array.size()) || ((a.sign == b.sign) && (a.array == b.array));
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator>(big_integer const &a, big_integer const &b) {
    if (a == b) return false;
    if (a.sign != b.sign) return b.sign;
    if (a.size() != b.size()) return (b.size() < a.size());
    for (size_t i = a.size(); i > 0; i--) {
        uint32_t e = (a.get_digit(i - 1) - b.get_digit(i - 1));
        if (e) return a.get_digit(i - 1) > b.get_digit(i - 1);
    }
    return 0;
}

bool operator<(big_integer const &a, big_integer const &b) {
    return b > a;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(a > b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}

big_integer operator+(big_integer const &a, big_integer const &b) {
    size_t n = max(a.size(), b.size()) + 2, m = min(a.size(), b.size());
    fast_vector temp(n);
    uint64_t carry = 0, sum = 0;
    for (size_t i = 0; i < m; i++) {
        sum = (carry + a.get_real_digit(i)) + b.get_real_digit(i);
        temp[i] = toUint32(sum);
        carry = sum >> BASE_ARRAY;
    }
    for (size_t i = m; i < n; i++) {
        sum = (carry + a.get_digit(i)) + b.get_digit(i);
        temp[i] = toUint32(sum);
        carry = sum >> BASE_ARRAY;
    }
    return big_integer(temp.back() & (1 << (BASE_ARRAY - 1)), temp);
}

big_integer operator-(big_integer const &a, big_integer const &b) {
    size_t n = max(a.size(), b.size()) + 3, m = min(a.size(), b.size());
    fast_vector temp(n);
    uint64_t carry = 0, sum = 0;
    if (m > 0) {
        sum = toUint64(a.get_real_digit(0)) + 1ULL + toUint64(~b.get_real_digit(0));
        temp[0] = toUint32(sum);
        carry = sum >> BASE_ARRAY;
        for (size_t i = 1; i < m; i++) {
            sum = carry + toUint64(a.get_real_digit(i)) + toUint64(~b.get_real_digit(i));
            temp[i] = toUint32(sum);
            carry = sum >> BASE_ARRAY;
        }
        for (size_t i = m; i < n; i++) {
            sum = carry + toUint64(a.get_digit(i)) + toUint64(~b.get_digit(i));
            temp[i] = toUint32(sum);
            carry = sum >> BASE_ARRAY;
        }
    }
    else {
        sum = toUint64(a.get_digit(0)) + 1ULL + toUint64(~b.get_digit(0));
        temp[0] = toUint32(sum);
        carry = sum >> BASE_ARRAY;
        for (size_t i = 1; i < n; i++) {
            sum = carry + toUint64(a.get_digit(i)) + toUint64(~b.get_digit(i));
            temp[i] = toUint32(sum);
            carry = sum >> BASE_ARRAY;
        }
    }
    return big_integer(temp.back() & (1 << (BASE_ARRAY - 1)), temp);

}

fast_vector mul_vector(fast_vector const &a, fast_vector const &b) {
    fast_vector res(a.size() + b.size() + 1);
    for (size_t i = 0; i < a.size(); i++) {
        uint64_t carry = 0, mul = 0, tmp = 0;
        for (size_t j = 0; j < b.size(); j++) {
            size_t k = i + j;
            mul = uint64_t(a[i]) * b[j];
            tmp = (mul & 0xffffffff) + res[k] + carry;
            res[k] = toUint32(tmp);
            carry = (mul >> BASE_ARRAY) + (tmp >> BASE_ARRAY);
        }
        res[i + b.size()] += toUint32(carry);
    }
    return  res;
}

fast_vector mul_big_small(fast_vector const &a, const uint32_t b) {
    fast_vector res(a.size() + 1);
    uint64_t carry = 0, mul = 0, tmp = 0;
    for (size_t i = 0; i < a.size(); i++) {
        mul = uint64_t(a[i]) * b;
        tmp = (mul & 0xffffffff) + carry;
        res[i] = toUint32(tmp);
        carry = (mul >> BASE_ARRAY) + (tmp >> BASE_ARRAY);
    }
    res[a.size()] = toUint32(carry);
    return res;
}

void big_integer::correct() {
    if (!sign) return;
    else if (size() == 0) {
        sign = !sign;
        return;
    }
    array.prepare_to_new();
    size_t n = size();
    uint64_t sum = toUint64(~array[0]) + 1ULL, carry = sum >> BASE_ARRAY;
    array[0] = toUint32(sum);
    for (size_t i = 1; i < n; i++) {
        sum = carry + toUint64(~array[i]);
        array[i] = toUint32(sum);
        carry = sum >> BASE_ARRAY;
    }
    array.push_back(toUint32(carry + 0xffffffff));
    delete_zero();
}

big_integer operator*(big_integer const &a, big_integer const &b) {
    if (a.is_zero() || b.is_zero()) return big_integer(0);
    big_integer apos(a.abs());
    big_integer bpos(b.abs());
    if (apos.size() > bpos.size()) apos.swap(bpos);
    fast_vector temp;
    if (apos.size() == 1) temp = mul_big_small(bpos.array, apos.get_real_digit(0));
    else temp = mul_vector(apos.array, bpos.array);
    big_integer res(a.sign ^ b.sign, temp);
    res.correct();
    return res;
}

int string_to_int(string const &s) {
    int ans = 0;
    for (auto a : s) {
        if (a < '0' || a > '9') throw runtime_error("symbol " + s + " not correct");
        ans = ans * 10 + (a-'0');
    }
    return ans;
}

big_integer string_to_number(string const &s) {
    big_integer res(0);
    size_t begin = 0;
    if (s[begin] == '-') begin++;
    for (size_t i = begin; i + 9 <= s.size(); i += 9) {
        res = res * BASE_INT + string_to_int(s.substr(i, 9));
    }
    uint32_t mod = (s.size() - begin) % 9;
    if (mod > 0) res = res * dec_pow(mod) + string_to_int(s.substr(s.size() - mod, mod));
    return (begin % 2 != 0) ? -res : res;
}

big_integer::big_integer(string const &str) :big_integer(string_to_number(str)) {}

uint32_t get_trial(const uint32_t a, const uint32_t b, const uint32_t c) {
    uint64_t res = a;
    res = ((res << BASE_ARRAY) + b) / c;
    if (res > 0xffffffff) res = 0xffffffff;
    return toUint32(res);
}

void sub_equal_vectors(fast_vector &a, fast_vector const &b) {
    uint64_t sum = toUint64(~b[0]) + toUint64(a[0]) + 1LL, carry = sum >> BASE_ARRAY;
    a[0] = toUint32(sum);
    for (size_t i = 1; i < b.size(); i++) {
        sum = toUint64(~b[i]) + toUint64(a[i]) + carry;
        a[i] = toUint32(sum);
        carry = sum >> BASE_ARRAY;
    }
}

bool compare_equal_vectors(fast_vector const &a, fast_vector const &b) {
    for (size_t i = a.size(); i > 0; i--) {
        if (a[i - 1] != b[i - 1]) return a[i - 1] < b[i - 1];
    }
    return 0;
}



big_integer operator/(big_integer const &a, big_integer const &b) {
    if (b.is_zero()){
        //throw runtime_error("can't divide by zero");
        cout << "zero division was missed";
        return big_integer(0);
    }
    big_integer apos(a.abs());
    big_integer bpos(b.abs());
    
    uint32_t f = toUint32((uint64_t(0xffffffff) + 1) / (uint64_t(bpos.array.back()) + 1));
    size_t n = apos.size();
    size_t m = bpos.size();
    apos.array.prepare_to_new();
    bpos.array.prepare_to_new();
    apos.array = mul_big_small(apos.array, f);
    bpos.array = mul_big_small(bpos.array, f);
    apos.delete_zero();
    bpos.delete_zero();

    uint32_t last = bpos.array.back();
    fast_vector temp(n - m + 1);
    fast_vector dev(m + 1);
    fast_vector div(m + 1);
    for (size_t i = 0; i < m; i++) {
        dev[i] = apos.get_real_digit(n + i - m);
    }
    dev[m] = apos.get_digit(n);

    for (size_t i = 0; i < n - m + 1; i++) {
        dev[0] = apos.get_real_digit(n - m - i);
        uint32_t tq = get_trial(dev[m], dev[m - 1], last);
        div = mul_big_small(bpos.array, tq);
        if (bpos.size() != 1) {
            while ((tq >= 0) && compare_equal_vectors(dev, div)) {
                div = mul_big_small(bpos.array, --tq);
            }
        }
        sub_equal_vectors(dev, div);
        for (size_t j = m; j > 0; j--) {
            dev[j] = dev[j - 1];
        }
        temp[n - m - i] = tq;
    }
    big_integer res(a.sign ^ b.sign, temp);
    res.correct();
    return res;
}

big_integer operator%(big_integer const &a, big_integer const& b) {
    return a - (a / b) * b;
}


big_integer &big_integer::operator+=(big_integer const &b) {
    return *this = *this + b;
}

big_integer &big_integer::operator-=(big_integer const &b) {
    return *this = *this - b;
}

big_integer &big_integer::operator*=(big_integer const &b) {
    return *this = *this * b;
}

big_integer& big_integer::operator/=(big_integer const& b) {
    return *this = *this / b;
}

big_integer& big_integer::operator%=(big_integer const& b) {
    return *this = *this % b;
}

big_integer& big_integer::operator^=(big_integer const &b) {
    return *this = *this ^ b;
}

big_integer& big_integer::operator&=(big_integer const &b) {
    return *this = *this & b;
}

big_integer& big_integer::operator|=(big_integer const &b) {
    return *this = *this | b;
}

big_integer& big_integer::operator<<=(uint32_t b) {
    return *this = *this << b;
}

big_integer& big_integer::operator>>=(uint32_t b) {
    return *this = *this >> b;
}


string to_string(big_integer const& a) {
    if ((!a.sign) && (a.size() == 0)) return "0";
    if (a.size() == 0 && (a.sign)) return "-1";
    string ans = "";
    big_integer apos(a.abs());
    while (!apos.is_zero())
    {
        uint32_t temp = (apos % BASE_INT).get_digit(0);
        for (size_t i = 0; i < 9; i++) {
            ans.push_back('0' + temp % 10);
            temp /= 10;
        }
        apos /= BASE_INT;
    }
    while (!ans.empty() && ans.back() == '0') {
        ans.pop_back();
    }
    if (a.sign) ans.push_back('-');

    reverse(ans.begin(), ans.end());
    return ans;
}