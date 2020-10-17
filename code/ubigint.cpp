// $Id: ubigint.cpp,v 1.9 2020-07-02 15:38:57-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

#include "math.h"
#include "ubigint.h"
#include "debug.h"

ubigint::ubigint (unsigned long that){
   while (that > 0) {
      ubig_value.push_back(that % 10);
      that /= 10;
   }
   if (ubig_value.size() == 0) ubig_value.push_back(0);
}

ubigint::ubigint (const string& that) {
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      ubig_value.push_back(digit-'0');
   }
   if (ubig_value.size() == 0) ubig_value.push_back(0);
   reverse(ubig_value.begin(), ubig_value.end());
}

// Helper for reversing order in ubigint
ubigint ubigint::reverse_ubigint (const ubigint& that) const {
   // TF
   ubigint reversed_result("");
   //bc we cant edit const
   vector<udigit_t> input_vector = that.ubig_value;     
   for (long unsigned int itr = 0; 
    !input_vector.empty(); itr++) {
      auto digit = input_vector.back();
      input_vector.pop_back();
      reversed_result.ubig_value.push_back(digit);
   }
   return reversed_result;
}

ubigint ubigint::operator+ (const ubigint& that) const {
   vector<udigit_t> a = ubig_value;
   vector<udigit_t> b = that.ubig_value;
   reverse(a.begin(), a.end());
   reverse(b.begin(), b.end());
   ubigint result;
   int carry = 0;
   while (!a.empty() || !b.empty() || carry == 1) {
      udigit_t a_digit = 0;
      udigit_t b_digit = 0;
      if (!a.empty()) { a_digit = a.back(); a.pop_back(); }
      if (!b.empty()) { b_digit = b.back(); b.pop_back(); }
      int sum = (a_digit) + (b_digit) + carry;
      carry = 0;
      if (sum >= 10) {
         carry = 1;
         sum -= 10;
      }
      char result_digit = sum;
      result.ubig_value.push_back(result_digit);
   }
   while (result.ubig_value.back()==0 && result.ubig_value.size()!=1) 
      result.ubig_value.pop_back();
   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   vector<udigit_t> a = ubig_value;
   vector<udigit_t> b = that.ubig_value;
   reverse(a.begin(), a.end());
   reverse(b.begin(), b.end());
   ubigint result;
   int carry = 0;
   while (!a.empty() || !b.empty() || carry == 1) {
      int a_digit = 0;
      int b_digit = 0;
      if (!a.empty()) { a_digit = a.back(); a.pop_back(); }
      if (!b.empty()) { b_digit = b.back(); b.pop_back(); }
      int sum = a_digit - b_digit - carry;
      carry = 0;
      if (sum < 0) {
         carry = 1;
         sum = 10 + sum;
      }
      char result_digit = sum;
      result.ubig_value.push_back(result_digit);
   }
   while (result.ubig_value.back()==0 && result.ubig_value.size()!=1) 
      result.ubig_value.pop_back();
   return result;
}

ubigint ubigint::operator* (const ubigint& that) const {
   string initializeString = "";
   for (long unsigned int s = 0; 
    s < (ubig_value.size() + that.ubig_value.size()); s++) {
       initializeString += '0';
   }
   ubigint p (initializeString);
   auto m = ubig_value.size();
   auto n = that.ubig_value.size();
   for (long unsigned i = 0; i < m; i++) {
      int c = 0;
      for (long unsigned j = 0; j < n; j++) {
         auto d = p.ubig_value.at(i+j) 
         + (ubig_value.at(i) * that.ubig_value.at(j))
         + c;
         p.ubig_value.at(i+j) = d%10;
         c = floor(d / 10);
      }
      p.ubig_value.at(i+n) = c;
   }
   while (p.ubig_value.back() == 0 && p.ubig_value.size() != 1) 
      p.ubig_value.pop_back();
   return p;
}

void ubigint::multiply_by_2() {
   int size = this->ubig_value.size();
   int carry = 0;
   for (int i = 0; i < size; i++) {
      int product = this->ubig_value[i] * 2 + carry;
      carry = 0;
      if (product >= 10) {
         product -= 10;
         if (i+1 != size) carry = 1;
         else this->ubig_value.push_back(1);
      }
      this->ubig_value[i] = product;
   }
}

void ubigint::divide_by_2() {
   int size = this->ubig_value.size();
   for (int i = 0; i < size-1; i++) {
      this->ubig_value[i] = (this->ubig_value[i] / 2)
                           + (5 * (this->ubig_value[i+1] & 1));
   }
   this->ubig_value[size-1] = floor(this->ubig_value[size-1]/2);
   while (this->ubig_value.back()==0 && this->ubig_value.size()!=1) 
      this->ubig_value.pop_back();
}

struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; 
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   if (ubig_value.size() != that.ubig_value.size()) return false;
   long unsigned int itr = 0;
   while (itr < ubig_value.size()) {
      if (ubig_value[itr] != that.ubig_value[itr]) return false;
      itr++;
   }
   return true;
}

bool ubigint::operator< (const ubigint& that) const {
   if (ubig_value.size() < that.ubig_value.size()) return true;
   if (ubig_value.size() > that.ubig_value.size()) return false;
   for (int i = ubig_value.size() - 1 ; i >= 0; i--) {
      if (ubig_value[i] < that.ubig_value[i]) return true;
      if (ubig_value[i] > that.ubig_value[i]) return false;
   }
   return false; // if ==
}

ostream& operator<< (ostream& out, const ubigint& that) { 
   if (that.ubig_value.size() < 1) {
      return out << 0;
   }
   int char_counter = 0;
   for (int i = that.ubig_value.size()-1; i >= 0; i-- ) {
      if (char_counter >= 69) {
         out << '\\' << endl;
         char_counter = 0;
      }
      out << static_cast<int>(that.ubig_value[i]);
      char_counter++;
   }
   return out;
}
