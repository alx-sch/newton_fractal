// Core parallel kernel.
// It's job is to run the Newton iteration loop for one pixel
// (finding root and counting iterations).
// The C++ host program launches this kernel and the ISPC runtin then runs
// thousands of copies of this kernel at the same time, each copy handling
// a different pixel in the viewport.
// This replaces the slow, sequential C++ that iterates over every pixel with
// a single, massive parallel operatio, speeding up the compilation.

#include "FractalMath.isph"

// -- Newton Iteration Step --

// Implements z_{k+1} = z_k - f(z_k)/f'(z_k), direct transaltion of C++ newtonStep().
static bool	newtonStep(varying Complex &z, uniform int n, uniform double epsilon)
{
	// f(z) = z^n - 1
	varying	Complex f_z = complexSub(complexPow(z, n), {1.0, 0.0});

	// f'(z) = n * z^(n-1)
	varying	Complex z_n_minus_1 = complexPow(z, n - 1); // f'(z) = n * z^(n-1)
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
	varying	Complex quot = complexDiv(f_z, f_prime_z);

	// Update z: z_{k+1} = z_k - w
	z = complexSub(z, quot);

	return true;
}

// --- The Main Parallel Kernel ---
// This function is exported so it can be called from the C++ host (Fractal.cpp).
// Translated from C++ Fractal::generate()
export void calculateFractal(
	// UNIFORM INPUTS (Same for all threads/lanes)
	uniform int			width,
	uniform int			height,
	uniform int			n,
	uniform	Complex*	roots,
	uniform double		tolerance,
	uniform double		epsilon,
	uniform int			max_iterations,
	uniform double		x_min,
	uniform double		x_max,
	uniform double		y_min,
	uniform double		y_max,

	// VARYING OUTPUTS (Each lane writes its unique result)
	varying int*		out_root_indices,
	varying int*		out_iterations
)
{
	uniform int	total_pixels = width * height;

	// "Stride Loop"
	// provided by ISPC:
	// programIndex: gives the index of the SIMD-lane being used for running each program instance
	// programCount: gives the total number of instances in the gang
	for (uniform int i = programIndex; i < total_pixels; i += programCount)
	{
		// Convert linear index 'i' to (x, y) coordinates
		int	x = i % width;
		int	y = i / width;

		// Map (x, y) to Complex z_start
		varying double	real = x_min + ((double)x / (width - 1)) * (x_max - x_min);
		varying double	imag = y_max - ((double)y / (height - 1)) * (y_max - y_min);

		// Init values
		varying Complex	z = {real, imag};
		varying int		converged_root = -1;
		varying int		iterations = 0;

		// Solve: Run Newton's Method -> C++'s solvePixel()
		for (iterations = 0; iterations < max_iterations; ++iterations)
		{
			// Check for convergence
			for (uniform int k = 0; k < n; ++k)
			{
				// Not using complexAbs here to avoid sqrt() and extra struct creation
				// --> better performance!
				varying double	dx = z.real - roots[k].real;
				varying double	dy = z.imag - roots[k].imag;
				if ((dx*dx + dy*dy) < tolerance)
				{
					converged_root = k;
					goto end_loop; // 'break' would end for all lanes
				}
			}

			// Not converged: Perform Newton Step
			if (!newtonStep(z, n, epsilon))
			{
				goto end_loop;
			}
		}

	end_loop:
		// Store: Write results back to the output arrays
		out_root_indices[i] = converged_root;
		out_iterations[i] = iterations;
	}
}
