// $Id: ubigint.cpp,v 1.9 2020-07-02 15:38:57-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "ubigint.h"
#include "debug.h"

ubigint::ubigint (unsigned long that): uvalue (that) {
   DEBUGF ('~', this << " -> " << uvalue)
}

ubigint::ubigint (const string& that) {
   DEBUGF ('~', "that = \"" << that << "\"");
   for (char digit : that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      ubig_value.push_back(digit);
   }
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
   // TF
   vector<udigit_t> a = ubig_value;
   vector<udigit_t> b = that.ubig_value;
   ubigint result("");
   int carry = 0;
   while (!a.empty() || !b.empty() || carry == 1) {
      udigit_t a_digit = '0';
      udigit_t b_digit = '0';
      if (!a.empty()) { a_digit = a.back(); a.pop_back(); }
      if (!b.empty()) { b_digit = b.back(); b.pop_back(); }
      int sum = (a_digit - '0') + (b_digit - '0') + carry;
      carry = 0;
      if (sum >= 10) {
         carry = 1;
         sum -= 10;
      }
      char result_digit = sum + '0';
      result.ubig_value.push_back(result_digit);
   }
   DEBUGF ('u', *this << "non-reversed result+ is:" << result << endl);
   ubigint reversed_result = reverse_ubigint(result);
   return reversed_result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   // TF

   // DEBUGF ('u', ubig_value);
   // DEBUGF ('u', that.ubig_value);
   // cout << ubig_value;
   // for (auto thing : ubig_value ){
   //    cout << thing;
   // }

   // vector<udigit_t> a = ubig_value;
   // vector<udigit_t> b = that.ubig_value;
   // ubigint result("");
   // int carry = 0;
   // while (!a.empty() || !b.empty() || carry == 1) {
   //    udigit_t a_digit = '0';
   //    udigit_t b_digit = '0';
   //    if (!a.empty()) { a_digit = a.back(); a.pop_back(); }
   //    if (!b.empty()) { b_digit = b.back(); b.pop_back(); }
   //    // DEBUGF ('u', *this << "thing- is:" << a_digit << endl);
   //    // DEBUGF ('u', *this << "thing- is:" << b_digit << endl);
   //    int sum = (a_digit - '0') - (b_digit - '0') - carry;
   //    // DEBUGF ('u', *this << "sum- is:" << b_digit << endl);
   //    carry = 0;
   //    if (sum < 0) {
   //    // DEBUGF ('u', *this << "in carry" << endl);
   //       carry = 1;
   //       sum = 10 + sum;
   //    }
   //    char result_digit = sum + '0';
   //    result.ubig_value.push_back(result_digit);
   // }
   // // trim leading 0, if it exists
   // if (result.ubig_value.back() == '0') result.ubig_value.pop_back();
   // DEBUGF ('u', 
   //*this << "non-reversed result- is:" << result << endl);
   // ubigint reversed_result = reverse_ubigint(result);
   // return reversed_result;
}

ubigint ubigint::operator* (const ubigint& that) const {
   return ubigint (uvalue * that.uvalue);
}

void ubigint::multiply_by_2() {
   uvalue *= 2;
}

void ubigint::divide_by_2() {
   uvalue /= 2;
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
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
   if (ubig_value.size() != that.ubig_value.size()) 
      return false;
   for (long unsigned int i = 0; i < ubig_value.size(); i++) {
      if (ubig_value.at(i) != that.ubig_value.at(i))
         return false;
   }
   return true;
}

bool ubigint::operator< (const ubigint& that) const {
   // join each vec 
   string a_string = "";
   for (auto a_digit : ubig_value) {
      a_string += a_digit;
   }

   string b_string = "";
   for (auto b_digit : that.ubig_value) {
      b_string += b_digit;
   }

   // turn into int
   int a_int = stoi(a_string);
   int b_int = stoi(b_string);

   // cmp
   if (a_int < b_int) return true;
   else return false;
}

ostream& operator<< (ostream& out, const ubigint& that) { 
   // return out << "ubigint(" << that.uvalue << ")";
   for (auto i : that.ubig_value) {
      out << i;
   }
   return out;
}

