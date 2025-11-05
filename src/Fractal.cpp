#include "../include/Fractal.hpp"
#include "../include/defines.hpp"	// DEBUG_PRINT

#include <iostream>
#include <cmath>	// For M_PI, cos, sin
#include <vector>

Fractal::Fractal(int n, int width, int height) :
	n_(n), width_(width), height_(height)
{}

/**
 @brief Calculates all `n` solutions to `z^n = 1` and adds them to the `roots_` vector.

 This function solves `z^n = 1` by finding the `n` complex roots
 using the formula:
 `r_k = cos(2*pi*k/n) + i*sin(2*pi*k/n)`
 for `k = 0` to `n-1`.
*/
void	Fractal::calculateRoots(int n)
{
	roots_.clear(); // Clear vector from previous calculations

	DEBUG_PRINT("--- Calculating " << n << " Roots ---");

	for (int k = 0; k < n; ++k)
	{
		double	theta = 2 * M_PI * static_cast<double>(k) / n;
		Complex	root(cos(theta), sin(theta));
		DEBUG_PRINT("Root " << k << ": " << root);
		roots_.push_back(root);
	}
	DEBUG_PRINT("---------------------------");
}

/**
 @brief Performs one step of the Newton's iteration by updating `z`.

 This is the core formula:
 - `z_{k+1} = z_k - f(z_k)/f'(z_k)`
 - `z_{k+1} = z_k - (z_k^n - 1) / (n*z_k^(n-1))`
*/
bool	Fractal::newtonStep(Complex& z)
{
	Complex	f_z = std::pow(z, n_) - Complex(1, 0); // f(z) = z^n - 1
	Complex	f_prime_z = static_cast<double>(n_) * std::pow(z, n_ - 1); // f'(z) = n*z^(n-1)

	if (std::abs(f_prime_z) < EPSILON)
		return false; // Avoid division by zero

	z = z - f_z / f_prime_z; // Newton's method step -> get z_{k+1} from z_k
	return true;
}
