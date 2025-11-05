#include "../include/Fractal.hpp"
#include "../include/defines.hpp"	// DEBUG_PRINT

#include <iostream>
#include <cmath>	// For M_PI, cos, sin
#include <iomanip>	// Formatte output debug prints

Fractal::Fractal(int n, int width, int height) :
	n_(n), width_(width), height_(height),
	tolerance_(DEF_TOLERANCE), max_iterations_(MAX_ITERS)
{
	calculateRoots(n_);
}

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

 @return	`true` if the step was successful,
 			`false` if the derivative was too small.
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

/**
 @brief Determines which root the starting point `z_start` converges to
		and how many iterations it took.

 This function applies Newton's method iteratively starting from `z_start`
 until convergence to one of the known roots or until the maximum number
 of iterations is reached.

 @param z_start The initial complex number to start the iteration from.
 @return A pair where the first element is the index of the converged root
		 (or -1 if no convergence) and the second element is the number of
		 iterations taken.
*/
std::pair<int, int>	Fractal::solvePixel(Complex z_start)
{
	Complex	z = z_start;

	// Loop through max allowed iterations until convergence is found (or division by zero)
	for (int iter = 0; iter < max_iterations_; ++iter)
	{
		DEBUG_PRINT(std::fixed << std::setprecision(4) <<"z_" << iter << ": " << z);
		// CHECK FOR CONVERGENCE
		// Check if 'z' is close enough to any of the pre-calculated 'n' roots
		for (int k = 0; k < n_; ++k)
		{
			// If distance between between z and root is less than tolerance, we have converged (root found!)
			if (std::abs(z - roots_[k]) < tolerance_)
			{
				DEBUG_PRINT("Converged to root " << k << " in " << iter << " iterations.");
				DEBUG_PRINT("---------------------------");
				return std::make_pair(k, iter);
			}
		}

		// NOT CONVERGED YET - PERFORM NEWTON STEP
		if (!newtonStep(z))
		{
			DEBUG_PRINT("Iter " << iter << ": Derivative too small, stopping iteration.");
			DEBUG_PRINT("---------------------------");
			return std::make_pair(-1, iter); // -1 indicates no convergence
		}
	}

	// NO CONVERGENCE WITHIN max_iterations_ -> Also a failure
	DEBUG_PRINT("Did not converge within " << max_iterations_ << " iterations.");
	DEBUG_PRINT("---------------------------");
	return std::make_pair(-1, max_iterations_); // Did not converge
}