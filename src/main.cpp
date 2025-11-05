#include "../include/Args.hpp"
#include "../include/Fractal.hpp"
#include "../include/defines.hpp"	// color codes

#include <iostream>
#include <iomanip>	// For formatting output

int	main (int argc, char **argv)
{
	Args	args;

	// Parse command-line arguments
	try
	{
		args.parse(argc, argv);
	}
	catch(const std::exception& e)
	{
		std::cerr << RED << e.what() << RESET << std::endl;
		Args::printUsage(argv[0]);
		return 1;
	}

	Fractal	fractal(args.n, args.width, args.height);

	// Test for a pixel, like top-left corner with viewport -2 to +2 on both axes
	Fractal::Complex	z(-2.0, 2.0);
	DEBUG_PRINT(std::fixed << std::setprecision(2) << "\n--- Tracing Pixel " << z << " ---");

	fractal.solvePixel(z);

	// find solutions (roots) to z^n - 1 = 0
	// z is a complex number, conisting of real and imaginary parts
	// let's make real part x and imaginary part y --> these go from
	// x: pixel column:	0 - width -1
	// y: pixel row:	0 - height-1
	// --> that's where the image comes in
	// z --> can be written down as a 2D point: z = x + iy
	// Need a viewport in the complex plane
	// Let's say we want to see from -2 to +2 on both axes
	// -- x =400, y = 300 would be the very center of the image -> z = 0 + 0i (would fail, because division by zero
	// -- x = 0, y = 0 would be the top left corner -> z = -2 + 2i

	// Newton Method formula:
	// z_(k+1) = z_k - f(z_k) / f'(z_k)
	// for our function:
	// f(z) = z^n - 1
	// f'(z) = n*z^(n-1)
	// so:
	// z_(k+1) = z_k - (z_k^n - 1) / (n*z_k^(n-1))
	// use n = 3 (example):
	// z_(k+1) = z_k - (z_k^3 - 1) / (3*z_k^2); same as:
	// (3*z_k^2) / (3*z_k^2) - (z_k^3 - 1) / (3*z_k^2)
	// = (2*z_k^3 + 1) / (3*z_k^2)
	// = (2*z_k^3) / (3*z_k^2) + 1 / (3*z_k^2)
	// = (2/3)*z_k + 1/(3*z_k^2)
	// = 1/3 * (2*z_k + 1/z_k^2)

	// Roots for n=3 are (z^3 = 1):
	// root 1: 1 + 0i (always a solution)
	// ok and the other roots? Using Euler's formula, we can write "1" in a generalized way that accounts for all 360 rotations (periodicity)
	// https://www.quora.com/How-do-I-prove-1-e-2-pi-ik-for-any-integer-k
	// -> e^(2 PI i k) = 1
	// where k is any integer (periodicity)
	// plugin our equation z^n = 1 = e^(2 PI i k)
	// -> z = e^i (2 PI k / n)
	// How to write e^i something in code?
	// Euler's formula says: e^(i theta) = cos(theta) + i sin(theta)
	// So our roots are:
	// z_k = cos(2 PI k / n) + i sin(2 PI k / n), for k = 0, 1, ..., n-1
	// create function!

	// OK Funtions gives:
	// --- Calculating 3 Roots ---
	// Root 0: (1,0)
	// Root 1: (-0.5,0.866025)
	// Root 2: (-0.5,-0.866025)
	// ---------------------------
	// Ok now, check for a given z, which root does it converge to?
	// E.g. let's say the viewport is -2 to +2 on both axes, use the top left corner: z = -2 + 2i
	// Using n = 3 again, this gives:
	// z_(k+1) = 1/3 * (2*z_k + 1/z_k^2)
	// Iteration 0: z_0 = -2 + 2i --> Close to any root (1,0) -> No!
	// Iteration 1: z_1 = 1/3 * (2*(-2 + 2i) + 1/(-2 + 2i)^2) = -1.33333 + 1.33333i --> Close to any root (1,0), (-0.5,0.866025), (-0.5,-0.866025) -> No!
	// Iteration 2: z_2 = 1/3 * (2*(-1.33333 + 1.33333i) + 1/(-1.33333 + 1.33333i)^2) = -0.790123 + 0.790123i --> Close to any root (-0.5,0.866025) -> No!



	return 0;
}
