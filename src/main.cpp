#include "../include/Args.hpp"
#include "../include/defines.hpp"	// color codes

#include <iostream>

int	main (int argc, char **argv)
{
	try
	{
		Args	args(argc, argv);
	}
	catch(const std::exception& e)
	{
		std::cerr << RED << e.what() << RESET << std::endl;
		Args::printUsage(argv[0]);
		return 1;
	}

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


	return 0;
}
