#include <complexMath.hpp>
#include <cmath> // For sqrt()

// Complex number subtraction: (a + bi) - (c + di)
Complex	complexSub(const Complex& a, const Complex& b)
{
	Complex	result;
	result.real = a.real - b.real;
	result.imag = a.imag - b.imag;

	return result;
}

// Complex number multiplication: (a + bi) * (c + di)
Complex	complexMul(const Complex& a, const Complex& b)
{
	Complex	result;
	result.real = a.real * b.real - a.imag * b.imag;
	result.imag = a.real * b.imag + a.imag * b.real;

	return result;
}

// Complex number division: (a + bi) / (c + di)
// See expansion of complex divison: https://www.cuemath.com/numbers/division-of-complex-numbers/
Complex	complexDiv(const Complex& a, const Complex& b)
{
	Complex	result;
	double	mag_sq = b.real * b.real + b.imag * b.imag;

	// Avoid divide-by-zero
	if (mag_sq == 0.0)
	{
		result.real = 0.0;
		result.imag = 0.0;
		return result;
	}

	result.real = (a.real * b.real + a.imag * b.imag) / mag_sq;
	result.imag = (a.imag * b.real - a.real * b.imag) / mag_sq;

	return result;
}

// Complex number exponentiation: (a + bi)^n
Complex	complexPow(const Complex& z, int n)
{
	if (n == 0) // z^0 = 1
	{
		return {1.0, 0.0};
	}
	Complex	result = {1.0, 0.0}; // Start with 1

	Complex	base = z;
	for (int i = 0; i < n; ++i)
	{
		result = complexMul(result, base);
	}

	return result;
}

// Returns the magnitude (absolute value) of a complex number z
// |z| = sqrt(a^2 + b^2) for z = a + bi
double	complexAbs(const Complex& z)
{
	return sqrt(z.real * z.real + z.imag * z.imag);
}
