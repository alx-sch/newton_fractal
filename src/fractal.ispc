// Core parallel kernel.
// It's job is to run the Newton iteration loop for one pixel
// (finding root and counting iterations).
// The C++ host program launches this kernel and the ISPC runtin then runs
// thousands of copies of this kernel at the same time, each copy handling
// a different pixel in the viewport.
// This replaces the slow, sequential C++ that iterates over every pixel with
// a single, massive parallel operation, speeding up the computation.

#include "fractalMath.isph"

// -- Newton Iteration Step --

// Implements z_{k+1} = z_k - f(z_k)/f'(z_k), direct transaltion of C++ newtonStep().
static bool	newtonStep(varying Complex &z, uniform int n, uniform double epsilon)
{
	Complex	one;
	one.real = 1.0;
	one.imag = 0.0;

	// f(z) = z^n - 1
	varying	Complex f_z = complexSub(complexPow(z, n), one);

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
	varying	Complex quot;
	quot.real = (f_z.real * f_prime_z.real + f_z.imag * f_prime_z.imag) / mag_sq;
	quot.imag = (f_z.imag * f_prime_z.real - f_z.real * f_prime_z.imag) / mag_sq;

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
	uniform	Complex		roots[/*number of roots*/],
	uniform double		tolerance,
	uniform double		epsilon,
	uniform int			max_iterations,
	uniform double		x_min,
	uniform double		x_max,
	uniform double		y_min,
	uniform double		y_max,

	// Pointers are uniform, but data access will be varying
	uniform int			out_root_indices[/*width * height*/],
	uniform int			out_iterations[/*width * height*/]
)
{
	uniform int	total_pixels = width * height;
	uniform double	tolerance_sq = tolerance * tolerance;

	// Pre-calculate uniform values for mapping (to avoid division inside loop)
	uniform double	inv_width = 1.0 / (double)width;
	uniform double	map_x_range = (x_max - x_min) / (double)(width - 1);
	uniform double	map_y_range = (y_max - y_min) / (double)(height - 1);

	// PARALLEL LOOP OVER
	// Each lane starts at its programIndex and jumps by the total number of lanes (programCount)
	for (uniform int i = 0; i < total_pixels; i += programCount)
	{
		// GET VARYING PIXEL INDEX
		// pixel_index will be [0, 1, 2... 15], then [16, 17... 31], etc.
		varying int	pixel_index = i + programIndex;

		// Safety check: don't let lanes compute pixels beyond the image boundary
		if (pixel_index >= total_pixels)
		{
			break; // Lanes that are out of bounds stop working
		}

		// MAP: Convert varying index to (x, y) coordinates
		varying double	y_double = floor((double)pixel_index * inv_width);
		varying double	x_double = (double)pixel_index - y_double * (double)width;

		// Map (x, y) to a varying complex number z
		varying	Complex z;
		z.real = x_min + x_double * map_x_range;
		z.imag = y_max - y_double * map_y_range; // y axis is inverted

		varying int		converged_root = -1;
		varying int		iterations = 0;
		varying bool	done = false; // Flag for structured control flow

		// SOLVE: Newton Iteration Loop
		for (iterations = 0; iterations < max_iterations; ++iterations)
		{
			// Check convergence against all roots
			for (uniform int k = 0; k < n; ++k)
{
				// Use squared magnitude for an efficient check (avoids sqrt)
				varying double	dist_sq_real = z.real - roots[k].real;
				varying double	dist_sq_imag = z.imag - roots[k].imag;
				varying double	dist_sq = (dist_sq_real * dist_sq_real) + (dist_sq_imag * dist_sq_imag);

				// Only update if this lane is not already done
				if (dist_sq < tolerance_sq && !done)
				{
					converged_root = k;
					done = true;
				}
			}

			// If this lane is done, or if newtonStep fails, break from iteration loop
			if (done || !newtonStep(z, n, epsilon)) 
			{
				break;
			}
		}

		// STORE: Write the varying results to the correct varying slots
		out_root_indices[pixel_index] = converged_root;
		out_iterations[pixel_index] = iterations;
	}
}
