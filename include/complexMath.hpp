#ifndef COMPLEX_MATH_HPP
# define COMPLEX_MATH_HPP

# include "defines.hpp" // for Complex struct

Complex	complexSub(const Complex& a, const Complex& b);
Complex	complexMul(const Complex& a, const Complex& b);
Complex	complexDiv(const Complex& a, const Complex& b);
Complex	complexPow(const Complex& z, int n);
double	complexAbs(const Complex& z);

#endif
