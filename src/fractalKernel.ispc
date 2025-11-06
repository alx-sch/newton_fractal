// Core parallel kernel. Includes the math utilities from the header.

#include "FractalMath.isph"

// -- Newton Iteration Step --

// Implements z_{k+1} = z_k - f(z_k)/f'(z_k), direct transaltion of C++ newtonStep().
static bool	newtonStep(varying Complex &z, uniform int n, uniform double epsilon)
{
	// f(z) = z^n - 1
	varying	Complex f_z = ComplexSub(ComplexPow(z, n), {1.0, 0.0});

	// f'(z) = n * z^(n-1)
	varying	Complex z_n_minus_1 = ComplexPow(z, n - 1); // f'(z) = n * z^(n-1)
	varying	Complex f_prime_z;
	f_prime_z.real = (double)n * z_n_minus_1.real;
	f_prime_z.imag = (double)n * z_n_minus_1.imag;

	// --- Complex Division: w = f_z / f_prime_z ---

	// C++ version uses abs() here, but no need to use sqrt just to check for "division by zero"
	varying double	mag_sq = f_prime_z.real * f_prime_z.real + f_prime_z.imag * f_prime_z.imag;

	// Check for division by zero (using a small constant for safety)
	if (mag_sq < epsilon)
	{
		return false;
	}

	// Perform the division: w = (A+Bi) / (C+Di)
	// See expansion of complex divison: https://www.cuemath.com/numbers/division-of-complex-numbers/
	varying	Complex w;
	w.real = (f_z.real * f_prime_z.real + f_z.imag * f_prime_z.imag) / mag_sq;
	w.imag = (f_z.imag * f_prime_z.real - f_z.real * f_prime_z.imag) / mag_sq;

	// Update z: z_{k+1} = z_k - w
	z = ComplexSub(z, w);

	return true;
}
