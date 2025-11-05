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
	fractal.generate();

	try 
	{
		fractal.saveImage("fractal.ppm");
	}
	catch (const std::exception& e)
	{
		std::cerr << RED << e.what() << RESET << std::endl;
		return 1;
	}

	return 0;
}
