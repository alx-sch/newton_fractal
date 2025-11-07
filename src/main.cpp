#include "Args.hpp"
#include "Fractal.hpp"
#include "defines.hpp"		// color codes

#include <iostream>
#include <iomanip>	// For formatting output
#include <string>
#include <ctime>	// Helper: For std::time_t, std::tm, std::localtime
#include <sstream>	// Helper: For std::stringstream

static std::string	genOutputFilename(int n);

/**
 @brief Main entry point for the Newton Fractal generator.

 This program generates a Newton fractal for the equation `z^n - 1 = 0`
 and saves it as a `.ppm` image file.

 Command-line arguments:
 - `<n>` (required): The degree of the polynomial.
 - `[width]` (optional): The width of the output image (default: 800).
 - `[height]` (optional): The height of the output image (default: 800).

 Example usage:
 ```
 ./newton_fractal 5 1024 1024
 ```
 This generates a fractal for `z^5 - 1 = 0` with a resolution of 1024x1024 pixels.
*/
int	main (int argc, char **argv)
{
	try
	{
		// Parse and validate command-line arguments
		Args	args(argc, argv);

		// Create Fractal object and generate the fractal data
		Fractal	fractal(args.n, args.width, args.height);
		fractal.generate();

		// Save fractal data to file
		std::string	outputFilename = genOutputFilename(args.n);
		fractal.saveImage(outputFilename);
	}
	catch(const std::exception& e)
	{
		std::cerr << RED << e.what() << RESET << std::endl;
		Args::printUsage(argv[0]);
		return 1;
	}

	return 0;
}

/**
 @brief Generates a unique output filename based on the current timestamp.

 The filename format is:
 `out/fractal_<n>n_YYYYMMDD_HHMMSS.ppm`

 @param n	The order of the fractal (used in the filename).
 @return	The generated output filename as a string.
*/
static std::string	genOutputFilename(int n)
{
	// Get current time
	std::time_t	now = std::time(nullptr);
	std::tm*	local_tm = std::localtime(&now);

	// Build name in stringstream
	std::stringstream	ss;
	ss	<< OUTPUT_DIR << "/fractal_" << n << "n_"	// Add fractal order
		<< std::put_time(local_tm, "%Y%m%d_%H%M%S")	// Add timestamp (YYYYMMDD_HHMMSS)
		<< ".ppm";	// Add file extension

	return ss.str();
}
