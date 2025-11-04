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

	return 0;
}
