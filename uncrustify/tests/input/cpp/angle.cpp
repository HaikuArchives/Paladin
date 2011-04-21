#include <vector>
typedef std::vector<std::vector<int> > Table;  // OK
typedef std::vector<std::vector<bool>> Flags;  // Error

void func(List<B>= default_val1);
void func(List<List<B>>= default_val2);

void foo()
{
  A<(X>Y)> a;
  a = static_cast<List<B>>(ld);
}

template<int i> class X { /* ... */ };
X< 1>2 > x1;    // Syntax error.
X<(1>2)> x2;    // Okay.

template<class T> class Y { /* ... */ };
Y<X<1>> x3;     // Okay, same as "Y<X<1> > x3;".
Y<X<6>>1>> x4;  // Syntax error. Instead, write "Y<X<(6>>1)>> x4;".
Y<X<(6>>1)>> x4;

