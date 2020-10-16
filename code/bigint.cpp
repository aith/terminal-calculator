// $Id: bigint.cpp,v 1.2 2020-01-06 13:39:55-08 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue_, bool is_negative_):
                uvalue(uvalue_), is_negative(is_negative_) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

// not sure what this is for
bigint bigint::operator+ () const {
   // cout << "in empty +" << endl;
   return *this;
}

bigint bigint::operator- () const {
   // cout << "in empty -" << endl;
   return {uvalue, not is_negative};
}

// here is the standard +
bigint bigint::operator+ (const bigint& that) const {
   // cout << " bigint+" << endl;
   ///// Start+
   bool sign;
   ubigint result;

   if (is_negative == that.is_negative) { 
      result = uvalue + that.uvalue; 
      sign = is_negative;
   }
   else {
      if (uvalue > that.uvalue) { 
         result = uvalue - that.uvalue; 
         sign = is_negative;
      }
      else {
         result = that.uvalue - uvalue;
         sign = that.is_negative;
      }
   }
   if (result == 0) sign = false;
   return bigint(result, sign);
   ///// End+
   // ubigint result = uvalue + that.uvalue;
   // return result;
}

bigint bigint::operator- (const bigint& that) const {
   // cout << " bigint-" << endl;
   bool sign;
   ubigint result;
   // If: Diff signs
   if (is_negative != that.is_negative) 
      {
      // If: same uvalue
       if (uvalue == that.uvalue) {
         if (is_negative) {
            result = uvalue + that.uvalue;
            sign = true;
         }
         else { 
            result = 0;
            sign = false;
         }
      }
      // If: diff uvalue
      else 
      {
         result = uvalue + that.uvalue; 
         sign = !that.is_negative;
         // sign = that.is_negative;
      }
   }
   // When: Same signs
   else {
      // If: a > b
      if (uvalue > that.uvalue) { 
         result = uvalue - that.uvalue; 
         sign = is_negative;
      }
      // If: b > a 
      else {
         // cout << "here";
         result = that.uvalue - uvalue;
         sign = !that.is_negative;
      }
   }
   if (result == 0) sign = false;
   return bigint(result, sign);
}

bigint bigint::operator* (const bigint& that) const {
   // cout << " bigint*" << endl;
   bool is_neg;
   if (is_negative == true || that.is_negative == true) {
      if (is_negative == that.is_negative) {
         is_neg = false;
      }
      else {
         is_neg = true;
      }
   }
   else is_neg = false;
   bigint result(uvalue * that.uvalue, is_neg);
   return result;
}

bigint bigint::operator/ (const bigint& that) const {
   // cout << " bigint/" << endl;
   bool is_neg;
   if (is_negative == true || that.is_negative == true) {
      if (is_negative == that.is_negative) {
         is_neg = false;
      }
      else {
         is_neg = true;
      }
   }
   else is_neg = false;
   bigint result(uvalue / that.uvalue, is_neg);
   return result;
}

bigint bigint::operator% (const bigint& that) const {
   // cout << " bigint%" << endl;
   bigint result = uvalue % that.uvalue;
   return result;
}

bool bigint::operator== (const bigint& that) const {
   // cout << " bigint==" << endl;
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   // cout << " bigint<" << endl;
   if (is_negative != that.is_negative) return is_negative;
   return uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   // cout << " bigint<<" << endl;
   return out << (that.is_negative ? "-" : "") << that.uvalue;
}

