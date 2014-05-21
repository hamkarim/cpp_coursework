//#include "stdafx.h"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <climits>
#include <string>
#define min(a, b) (((a) < (b))? a : b)
#define UINT_BITS (sizeof (unsigned int) * 8)

class big_integer
{
public:
    std::vector<unsigned int> digits;

    big_integer(){
        digits.push_back(0);
    }

    big_integer(int value){
        digits.push_back(value);
    }

    big_integer(big_integer const & other){
      std::copy(other.digits.begin(), other.digits.end(), std::back_inserter(digits));
    }

    big_integer(std::string const& str){
        digits.push_back(0);
        bool is_negative = str[0] == '-';
        int i = (is_negative) ? 1 : 0;
        for (; i < str.length(); i++){
            *this *= 10;
            *this += str[i] - '0';
        }
        if (is_negative){
            *this = ~*this + 1;
        }
    }

  ~big_integer(){}
  
    big_integer& operator=(big_integer const& other){
        digits = other.digits;
        return *this;
    }

    friend bool operator==(big_integer const& a, big_integer const& b){
        if (a.digits.size() != b.digits.size())
            return false;
        for (int i = a.digits.size() - 1; i >= 0; i--){
            if (a.digits[i] != b.digits[i])
                return false;
        }
        return true;
    }

    friend bool operator!=(big_integer const& a, big_integer const& b){
        return !(a == b);
    }

    friend bool operator<(big_integer const& a, big_integer const& b){
        int a_sign = get_sign(a);
        int b_sign = get_sign(b);
        if (a_sign < b_sign){
	  return true;
	}
        else if (a_sign > b_sign){
	  return false;
	}
        else {
	  if (a.digits.size() != b.digits.size()){
	    return (a_sign == 0) ? a.digits.size() < b.digits.size() : a.digits.size() > b.digits.size();
	  }
	  for (int i = a.digits.size() - 1; i >= 0; i--){
	    if ((a_sign == 1 && a.digits[i] < b.digits[i]) || (a_sign == 0 && a.digits[i] > b.digits[i]))
	      return false;
	  }   
	}
        return true;
    }

    friend bool operator>(big_integer const& a, big_integer const& b){
      int a_sign = get_sign(a);
      int b_sign = get_sign(b);
      if (a_sign > b_sign){
	return true;
      }
      else if (a_sign < b_sign){
	return false;
      }
      else {
	if (a.digits.size() != b.digits.size()){
	  return (a_sign == 0) ? a.digits.size() > b.digits.size() : a.digits.size() < b.digits.size();	
	}
	for (int i = a.digits.size() - 1; i >= 0; i--){
	  if ((a_sign == 1 && a.digits[i] > b.digits[i]) || (a_sign == 0 && a.digits[i] < b.digits[i]))
	    return false;
	}
      }
      return true;
    }

    friend bool operator<=(big_integer const& a, big_integer const& b){
        return (a < b) || (a == b);
    }

    friend bool operator>=(big_integer const& a, big_integer const& b){
      return (a > b) || (a == b);
    }

    big_integer& operator&=(big_integer const& rhs){
        int sign = digits[digits.size() - 1] & (1 << 31);
        unsigned int bits = (sign == 0) ? 0 : UINT_MAX;
        for (int i = 0; i < digits.size() || i < rhs.digits.size(); i++){
            if (i >= digits.size()){
                digits.push_back(bits);
            }
            digits[i] &= rhs.digits[i];
        }
        return *this;
    }

    big_integer& operator|=(big_integer const& rhs){
        int sign = digits[digits.size() - 1] & (1 << 31);
        unsigned int bits = (sign == 0) ? 0 : UINT_MAX;
        for (int i = 0; i < digits.size() || i < rhs.digits.size(); i++){
            if (i >= digits.size()){
                digits.push_back(bits);
            }
            digits[i] |= rhs.digits[i];
        }
        return *this;
    }

    big_integer& operator^=(big_integer const& rhs){
        int sign = digits[digits.size() - 1] & (1 << 31);
        unsigned int bits = (sign == 0) ? 0 : UINT_MAX;
        for (int i = 0; i < digits.size() || i < rhs.digits.size(); i++){
            if (i >= digits.size()){
                digits.push_back(bits);
            }
            digits[i] ^= rhs.digits[i];
        }
        return *this;
    }

    big_integer& operator+=(big_integer const& rhs){
        unsigned int carry = 0;
        for (int i = 0; i < digits.size() || i < rhs.digits.size(); i++){
            if (i >= digits.size()){
                digits.push_back(get_bits(i));
            }
            unsigned long long sum = digits[i] + rhs.digits[i] + carry;
            digits[i] = sum % (1LL << (sizeof(unsigned int)* 8));
            carry = sum / (1LL << (sizeof(unsigned int)* 8));
        }
        if (carry != 0){
            digits.push_back(carry);
        }
        normalize();
        return *this;
    }

    big_integer& operator-=(big_integer const& rhs){
        *this += -rhs;
        return *this;
    }

    big_integer& operator*=(big_integer const& rhs){
        std::vector<unsigned int> res;
        unsigned long long product = 0;
        for (int i = 0; i < digits.size(); i++){
            unsigned int carry = 0;
            for (int j = 0; j < rhs.digits.size() || carry; j++){
                if (i + j >= res.size()){
                    res.push_back(0);
                }
                product = res[i + j] + (unsigned long long)  digits[i] * (j < rhs.digits.size() ? rhs.digits[j] : 0) + carry;
                res[i + j] = product % (1LL << UINT_BITS);
                carry = product / (1LL << UINT_BITS);
            }
        }
        digits = res;
        normalize();
        return *this;
    }

    big_integer& operator/=(big_integer const& rhs){
        big_integer res;
        big_integer first = *this;
        big_integer second = rhs;
        bool is_negative_first = false;
        bool is_negative_second = false;
        if (*this < 0){
            first = -*this;
            is_negative_first = true;
        }
        if (rhs < 0){
            second = -rhs;
            is_negative_second = true;
        }
        if (first.digits.size() == second.digits.size() == 1){
            return *this = first.digits[0] / second.digits[0];
        }
        int shift = leading_zeros(second);
        if (shift > 32){
            first <<= shift - UINT_BITS / 2;
            second <<= shift - UINT_BITS / 2;
        }
        if (first < second){
            *this = 0;
            return *this;
        }
        int m = first.digits.size() - second.digits.size();
        int n = second.digits.size();
        if (first >= (second << (UINT_BITS * m))){
            res += big_integer(1) << (UINT_BITS * m);
            first -= (second << UINT_BITS * m);
        }
        for (int i = m - 1; i >= 0; i--){
            unsigned long long possible_quotient = ((((unsigned long)long (digits[n + i]) << UINT_BITS) + digits[n + i - 1]) / second.digits[n - 1]);
            unsigned long long quotient = min(possible_quotient, 1LL << UINT_BITS);
            first -= (second * quotient) << (UINT_BITS * i);
            while (first < 0){
                quotient--;
                first += second << UINT_BITS * i;
            }
            res += quotient << UINT_BITS * i;
        }
        if (is_negative_first ^ is_negative_first){
            return *this = -res;
        }
        else {
            return *this = res;
        }
    }

    big_integer& operator%=(big_integer const& rhs){
      big_integer res;
	bool sign_lhs = get_sign(*this);
	bool sign_rhs = get_sign(rhs);
        big_integer first = (sign_lhs)? -*this : *this;
        big_integer second = (sign_rhs)? -rhs : rhs;

        if (first.digits.size() == second.digits.size() == 1){
            return *this = first.digits[0] % second.digits[0];
        }
        int shift = leading_zeros(second);
        if (shift > 16){
            first <<= shift - UINT_BITS / 2;
            second <<= shift - UINT_BITS / 2;
        }
        if (first < second){
            *this = 0;
            return *this;
        }
        int m = first.digits.size() - second.digits.size();
        int n = second.digits.size();
        if (first >= (second << (UINT_BITS * m))){
            res += big_integer(1) << (UINT_BITS * m);
            first -= (second << UINT_BITS * m);
        }
        for (int i = m - 1; i >= 0; i--){
            unsigned long long possible_quotient = ((((unsigned long)long (digits[n + i]) << UINT_BITS) + digits[n + i - 1]) / second.digits[n - 1]);
            unsigned long long quotient = min(possible_quotient, 1LL << UINT_BITS);
            first -= (second * quotient) << (UINT_BITS * i);
            while (first < 0){
                quotient--;
                first += second << UINT_BITS * i;
            }
            res += quotient << UINT_BITS * i;
        }
        if (sign_lhs ^ sign_rhs){
            return *this -= -res * rhs;
        }
        else {
            return *this -= res * rhs;
        }
    }

    big_integer& operator<<=(int n){
        std::vector<unsigned int>::iterator iter;
        int i = 0;
        while (n >= 32){
            iter = digits.begin();
            n -= 32;
            i++;
            digits.insert(iter, 0);
        }
        unsigned long long mask = 0;
        for (int j = i; j < digits.size(); j++){
            unsigned int tmp = digits[j];
            digits[j] <<= n;
            digits[j] |= mask / (1LL << UINT_BITS);
            mask = (unsigned long long) tmp << n;
        }
        digits.push_back(mask / (1LL << UINT_BITS));
	normalize();
        return *this;
    }

    big_integer& operator>>=(int n){
        unsigned int sign = digits[digits.size() - 1] & (1 << (UINT_BITS - 1));
        std::vector<unsigned int>::iterator iter;
        while (n >= 32){
            iter = digits.begin();
            n -= 32;
            digits.erase(iter);
        }
        unsigned long long mask = ((unsigned long long) digits[digits.size() - 1] << UINT_BITS) >> n;
        digits[digits.size() - 1] = (int)digits[digits.size() - 1] >> n;
        for (int j = digits.size() - 2; j >= 0; j--){
            unsigned int tmp = digits[j];
            digits[j] >>= n;
            digits[j] |= mask % (1LL << UINT_BITS);
            mask = ((unsigned long long) tmp << UINT_BITS) >> n;
        }
	return *this;
    }

    friend big_integer operator*(big_integer first, big_integer const& second){
        return first *= second;
    }

    friend big_integer operator+(big_integer first, big_integer const& second)
    {
        return first += second;
    }

    friend big_integer operator-(big_integer first, big_integer const& second)
    {
        return first -= second;
    }

    friend big_integer operator&(big_integer first, big_integer const& second){
        return first &= second;
    }

    friend big_integer operator|(big_integer first, big_integer const& second){
        return first |= second;
    }

    friend big_integer operator^(big_integer first, big_integer const& second){
        return first ^= second;
    }

    friend big_integer operator/(big_integer first, big_integer const& second){
        return first /= second;
    }

    friend big_integer operator%(big_integer first, big_integer const& second){
        return first %= second;
    }

    friend big_integer operator>>(big_integer first, int n){
        return first >>= n;
    }

    friend big_integer operator<<(big_integer first, int n){
        return first <<= n;
    }

    unsigned int mod_10(){
        big_integer tmp = *this;
        tmp %= 10;
        return tmp.digits[0];
    }

    big_integer operator+() const{
        return *this;
    }

    big_integer operator~() const{
      big_integer res (digits.size());
        for (int i = digits.size() - 1; i >= 0; i--){
            res.digits[i] = ~digits[i];
        }
        return res;
    }

    big_integer operator-() const{
        big_integer res = ~*this;
        res += 1;
        return res;
    }

    int leading_zeros(big_integer const& divisor){
        int count = 0;
        int shift = 1;
        unsigned int mask = 1 << UINT_BITS - 1;
        while (divisor.digits[divisor.digits.size() - 1] & mask == 0){
            mask >>= 1;
            count++;
        }
        return count;
    }

    unsigned int get_bits(int number){
        return (get_sign(*this) == 1) ? UINT_MAX : 0;
    }

    void normalize(){
        int i = digits.size() - 1;
        while (i >= 1){
            if ((digits[i] == 0 && ((digits[i - 1] & (1 << UINT_BITS - 1)) == 0)) || (digits[i] == UINT_MAX && ((digits[i - 1] & (1 << UINT_BITS - 1)) != 0))){
                digits.pop_back();
                i--;
            }
            else {
                return;
            }
        }
    }

    static unsigned int get_sign(big_integer const& a){
        return ((a.digits[a.digits.size() - 1] & (1 << UINT_BITS - 1)) == 0) ? 0 : 1;
    }

    friend std::string to_string(big_integer const& a){
        std::string res;
        big_integer tmp = a;
        if (tmp < 0){
            tmp = ~tmp + 1;
        }
        while (tmp >= 10){
            res += '0' + tmp.mod_10();
            tmp /= 10;
        }
        res.push_back(tmp.digits[0] + '0');
        if (get_sign(a) == 1){
            res += "-";
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

};



int main()
{
  big_integer a ("683421414244");
  big_integer v("423112423245235");
  //  a += v;
  std::cout<<to_string(v)<<"\n";
  std::cout<<std::endl;
}
