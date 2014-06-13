#pragme once
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <climits>
#include <string>
#include <algorithm>
#include <iosfwd>
#include <iostream>
#include "small_vector.h"
#include <iosfwd>

struct big_integer {
private:
  static const unsigned SIZE = 31;
  static const unsigned BASE = 1LL << SIZE;
  big_integer& normalize();
  void invert_bits();
  void insert_left(unsigned inserted);
  void insert_right();
  static int abs_compare(big_integer const& a, big_integer const& b);
  static int compare(big_integer const& a, big_integer const& b);
  
  bool is_nill() const;
  big_integer& divide(big_integer const & b);
  big_integer& remainder(big_integer const & b);
  big_integer& add(big_integer const& second);
  big_integer& sub(big_integer const& second, int & resulting_sign);
  big_integer& binary_operation(big_integer const& rhs, unsigned (*fn)(unsigned, unsigned));
  
  unsigned long long division_remainder;
  int sign;
  small_vector digits;
public:
  big_integer();
  big_integer(big_integer const& other);
  big_integer(int a);
  explicit big_integer(std::string const& str);
  ~big_integer();
  
  big_integer& operator=(big_integer const& other);
  
  big_integer& operator+=(big_integer const& rhs);
  big_integer& operator-=(big_integer const& rhs);
  big_integer& operator*=(big_integer const& rhs);
  big_integer& operator/=(big_integer const& rhs);
  big_integer& operator%=(big_integer const& rhs);
  
  big_integer& operator*=(int value);
  big_integer& operator/=(int value);
  
  big_integer& operator&=(big_integer const& rhs);
  big_integer& operator|=(big_integer const& rhs);
  big_integer& operator^=(big_integer const& rhs);
  
  big_integer& operator<<=(int rhs);
  big_integer& operator>>=(int rhs);
  
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
  
  friend std::string to_string(big_integer const& a);
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);
