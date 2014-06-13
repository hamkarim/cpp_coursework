#include "big_integer.h"
#include <algorithm>

big_integer& big_integer::normalize() {
  while (!digits.empty() && digits.back() == 0)
    digits.pop_back();
  if (is_nill())
    sign = 0;
  return *this;
}

bool big_integer::is_nill() const{
  return digits.empty();
}

big_integer::big_integer() : sign(0) {}

big_integer::big_integer(big_integer const& other) : digits(other.digits), sign (other.sign){}

big_integer& big_integer::operator=(big_integer const& other) {
    if (*this == other)
        return *this;
    digits = other.digits;
    sign = other.sign;
    return *this;
}

big_integer::big_integer(int a) {
  sign = (a < 0 ? 1 : 0);
  unsigned tmp = (unsigned) (a < 0 ? -a : a);
  while (tmp > 0) {
    digits.push_back(tmp & (BASE - 1));
    tmp >>= SIZE;
  }
}

big_integer::big_integer(std::string const& str) {
  bool is_negative = str[0] == '-';
  size_t i = (is_negative) ? 1 : 0;
  digits.push_back(0);
  for (; i < str.length(); i++){
    *this *= 10;
    *this += str[i] - '0';
  }
  sign = (is_negative) ? 1 : 0;
  normalize();
}

big_integer::~big_integer() {}

void big_integer::insert_left(unsigned inserted) {
  if (is_nill() && inserted == 0)
    return;
  digits.push_back(0);
  for (size_t i = digits.size() - 1; i >= 1; i--)
    digits[i] = digits[i - 1];
  digits[0] = inserted & (BASE - 1);
}


void big_integer::insert_right() {
  if (is_nill())
    return;
  for (size_t i = 0; i < digits.size() - 1; i++) {
    digits[i] = digits[i + 1];
  }
  digits.pop_back();
  normalize();
}

big_integer& big_integer::operator*=(int value) {
  if (value < 0) {
    sign ^= 1;
    value = -value;
  }
  unsigned long long carry = 0;
  for (size_t i = 0; i < digits.size() || carry; i++) {
    if (i == digits.size())
      digits.push_back(0);
    carry += 1LL * value * digits[i];
    digits[i] = carry & (BASE - 1);
    carry >>= SIZE;
  }
  return normalize();
}

big_integer& big_integer::operator/=(int value) {
  if (value < 0) {
    sign ^= 1;
    value = -value;
  }
  division_remainder = 0;
  for (int i = (int) digits.size() - 1; i >= 0; i--) {
    division_remainder = division_remainder * BASE + digits[i];
    digits[i] = division_remainder / value;
    division_remainder = division_remainder % value;
  }
  return normalize();
}

big_integer& big_integer::add(big_integer const& rhs) {
  size_t max_size = std::max(digits.size(), rhs.digits.size());
  unsigned  carry = 0;
  for (size_t i = 0; i < max_size; i++) {
    if (i >= digits.size())
      digits.push_back(0);
    digits[i] += carry + (i < rhs.digits.size() ? rhs.digits[i] : 0);
    carry = (digits[i] >= BASE ? 1 : 0);
    if (carry)
      digits[i] -= BASE;
  }
  return normalize();
}

big_integer& big_integer::sub(big_integer const& rhs, int& resulting_sign){
  int cmp  = abs_compare(*this, rhs);
  if (cmp == 0){
    return *this = 0;
  }
  if (cmp == 1) {
    unsigned carry = 0;
    resulting_sign = 0;
    for (size_t i = 0; i < digits.size() || carry; i++) {
      int cur = digits[i] - carry - (i < rhs.digits.size() ? rhs.digits[i] : 0);
      carry = (cur < 0 ? 1 : 0);
      if (carry)
	digits[i] = (unsigned) (cur + BASE);
      else
	digits[i] = (unsigned) (cur);
    }
  } else {
    unsigned carry = 0;
    resulting_sign = 1;
    for (size_t i = 0; i < rhs.digits.size() || carry; i++) {
      if (i == digits.size())
	digits.push_back(0);
      int cur = rhs.digits[i] - carry - digits[i];
      carry = (cur < 0 ? 1 : 0);
      if (carry != 0)
	digits[i] = (unsigned) (cur + BASE);
      else
	digits[i] = (unsigned) (cur);
    }
  }
  return normalize();
}

big_integer& big_integer::operator+=(big_integer const& rhs){
  if (rhs.is_nill())
    return *this;
  int cmp = abs_compare(*this, rhs);
  if (sign == rhs.sign){
    return add(rhs);
  } else {
    int resulting_sign;
    if (cmp == 0)
      return *this = 0;
    else {
      sub (rhs, resulting_sign);
      sign ^= resulting_sign;
      return *this;
    }
    return *this;
  }
}


big_integer& big_integer::operator-=(big_integer const& rhs){
  return *this += -rhs;
}


big_integer& big_integer::operator*=(big_integer const& rhs) {
  if (rhs.is_nill())
    return *this = 0;
  small_vector res_digits(digits.size() + rhs.digits.size());
  for (size_t i = 0; i < digits.size(); i++) {
    unsigned long long carry = 0;
    for (size_t j = 0; j < rhs.digits.size() || carry; j++) {
      if (j < rhs.digits.size())
	carry += 1LL * digits[i] * rhs.digits[j];
      carry += res_digits[i + j];
      res_digits[i + j] = carry & (BASE - 1);
      carry >>= SIZE;
    }
  }
  sign = ((sign & rhs.sign) == 1)? 0 : (sign | rhs.sign);
  digits = res_digits;
  return normalize();
}

big_integer& big_integer::divide(big_integer const & rhs) {
  if (is_nill())
    return *this;
  if (rhs.digits.size() == 1) {
    *this /= rhs.digits[0];
    sign = ((sign & rhs.sign) == 1)? 0 : (sign | rhs.sign);
    return * this;
  }
  int normalization_coeff = BASE / (rhs.digits.back() + 1);
  int resulting_sign = ((sign & rhs.sign) == 1)? 0 : (sign | rhs.sign);
  
  *this *= normalization_coeff * ((sign == 0)? 1 : -1);
  big_integer b = rhs;
  b *= normalization_coeff * ((rhs.sign == 0)? 1 : -1);
    
  big_integer remainder;
  unsigned b_last = b.digits.back();
  size_t b_size = b.digits.size();
  
  big_integer tmp;
  for (int i = (int) digits.size() - 1; i >= 0; i--) {
    remainder.insert_left(digits[i]);
    unsigned s1 = (b_size < remainder.digits.size()) ? remainder.digits[b_size] : 0;
    unsigned s2 = (b_size - 1 < remainder.digits.size()) ? remainder.digits[b_size - 1] : 0;
    unsigned quotient = (BASE * 1LL * s1 + s2) / b_last;
    
    tmp = b;
    tmp *= quotient;
    remainder -= tmp;
    
    while (remainder.sign == 1) {
      remainder += b;
      quotient--;
    }
    digits[i] = quotient;
    }
  sign = resulting_sign;
  return normalize();
}

big_integer& big_integer::remainder(big_integer const & rhs) {
  if (is_nill())
    return *this;
  int normalization_coeff = BASE / (rhs.digits.back() + 1);
  int resulting_sign = sign;
  
  *this *= normalization_coeff * ((sign == 0)? 1 : -1);
  big_integer b = rhs;
  b *= normalization_coeff * ((rhs.sign == 0)? 1 : -1);
  
  big_integer remainder;
  unsigned b_last = b.digits.back();
  size_t b_size = b.digits.size();
    
  for (int i = (int) digits.size() - 1; i >= 0; i--) {
    remainder.insert_left(digits[i]);
    unsigned s1 = b_size < remainder.digits.size() ? remainder.digits[b_size] : 0;
    unsigned s2 = b_size - 1 < remainder.digits.size() ? remainder.digits[b_size - 1] : 0;
    unsigned quotient = (BASE * 1LL * s1 + s2) / b_last;
    
    big_integer tmp = b;
    tmp *= quotient;
    remainder -= tmp;
    
    while (remainder.sign == 1) {
      remainder += b;
      quotient--;
    }
  }
  *this = remainder;
  *this /= normalization_coeff;
  sign = resulting_sign;
  return normalize();
}


big_integer& big_integer::operator/=(big_integer const& rhs) {
  return divide(rhs);
}

big_integer& big_integer::operator%=(big_integer const& rhs) {
  return remainder(rhs);
}

void big_integer::invert_bits() {
  if (sign == 1) {
    for (size_t i = 0; i < digits.size(); i++)
      digits[i] ^= BASE - 1;
    --*this;
  }
}

unsigned int p_or(unsigned a, unsigned b){
  return a | b;
}

unsigned int p_and(unsigned a, unsigned b){
  return a & b;
}

unsigned int p_xor(unsigned a, unsigned b){
  return a ^ b;
}

big_integer& big_integer::binary_operation(big_integer const& rhs, unsigned (*fn)(unsigned, unsigned)){
  big_integer b = rhs;
  size_t max_size = std::max(digits.size(), rhs.digits.size());
  digits.resize(max_size);
  b.digits.resize(max_size);
  invert_bits();
  b.invert_bits();
  digits.resize(max_size);
  for (size_t i = 0; i < max_size; i++) {
    digits[i] = fn(digits[i], b.digits[i]);
  }
  sign = fn(sign, rhs.sign);
  invert_bits();
  return normalize();
}

big_integer& big_integer::operator&=(big_integer const& rhs) {
  return binary_operation(rhs, p_and);
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
  return binary_operation(rhs, p_or);
}

big_integer& big_integer::operator^=(big_integer const& rhs) {
  return binary_operation(rhs, p_xor);
}

big_integer big_integer::operator~() const {
  if (is_nill())
    return -1;
  big_integer result = *this + 1;
  result.sign = sign ^ 1;
  return result.normalize();
}


big_integer& big_integer::operator<<=(int rhs) {
  if (is_nill())
    return *this;
  int full = rhs / SIZE;
  rhs = rhs % SIZE;
  for (int i = 0; i < full; i++)
    insert_left(0);
  return *this *= (1 << rhs);
}

big_integer& big_integer::operator>>=(int rhs) {
  if (is_nill())
    return *this;
  int full = rhs / SIZE;
  for (int i = 0; i < full; i++)
    insert_right();
  rhs %= SIZE;
  *this /= (1 << rhs);
  if (sign == 1)
    --*this;
  return *this;
}

big_integer big_integer::operator+() const {
  return *this;
}

big_integer big_integer::operator-() const {
  if (is_nill())
        return *this;
  big_integer result = *this;
  result.sign = sign ^ 1;
  return result;
}


big_integer& big_integer::operator++() {
  *this += 1;
  return *this;
}

big_integer big_integer::operator++(int) {
  big_integer result = *this;
  ++*this;
  return result;
}

big_integer& big_integer::operator--() {
  *this -= 1;
  return *this;
}

big_integer big_integer::operator--(int) {
  big_integer result = *this;
  --*this;
  return result;
}

big_integer operator+(big_integer a, big_integer const& b) {
  return a += b;
}

big_integer operator-(big_integer a, big_integer const& b) {
  return a -= b;
}

big_integer operator*(big_integer a, big_integer const& b) {
  return a *= b;
}

big_integer operator/(big_integer a, big_integer const& b) {
  return a /= b;
}

big_integer operator%(big_integer a, big_integer const& b) {
  return a %= b;
}

big_integer operator&(big_integer a, big_integer const& b) {
  return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b) {
  return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b) {
  return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
  return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
  return a >>= b;
}

int big_integer::abs_compare(big_integer const& a, big_integer const& b){
  if (a.digits.size() > b.digits.size()){
    return 1;
  }
  if (a.digits.size() < b.digits.size()){
    return -1;
  }
  for (int i = a.digits.size() - 1; i >= 0; --i) {
    if (a.digits[i] > b.digits[i]) {
      return 1;
    }
    if (a.digits[i] < b.digits[i]) {
      return -1;
    }
  }
  return 0;
}

int big_integer::compare(big_integer const& a, big_integer const& b){
  int comparison_result = abs_compare(a, b);
  if (comparison_result == 0 && (a.sign != b.sign)){
    return (a.sign == 1) ? -1 : 1;
  }
  if ((a.sign == 1 && b.sign == 1) || (comparison_result > 0 && a.sign == 1) || (comparison_result < 0 && b.sign == 1))
    return -comparison_result;
  return comparison_result;
}

bool operator==(big_integer const& a, big_integer const& b) {
  return big_integer::compare(a,b) == 0;
}

bool operator!=(big_integer const& a, big_integer const& b) {
  return big_integer::compare(a,b) != 0;
}

bool operator<(big_integer const& a, big_integer const& b) {
  return big_integer::compare(a,b) == -1;
}

bool operator>(big_integer const& a, big_integer const& b) {
  return big_integer::compare(a,b) == 1;
}

bool operator<=(big_integer const& a, big_integer const& b) {
  return big_integer::compare(a,b) <= 0;
}

bool operator>=(big_integer const& a, big_integer const& b) {
  return big_integer::compare(a,b) >= 0;
}

std::string to_string(big_integer const& a) {
  if (a.is_nill()) 
    return "0";
  big_integer tmp = a;
  tmp.sign = 0;
  std::string result;
  while (!tmp.is_nill()) {
    tmp /= 10;
    result += '0' + tmp.division_remainder;
  }
  if (a.sign == 1)
    result += "-";
  
  std::reverse(result.begin(), result.end());
  return result;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
  s << to_string(a);
  return s;
}
