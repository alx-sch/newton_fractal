#include "../include/Args.hpp"
#include "../include/defines.hpp"	// color codes

#include <iostream>
#include <stdexcept>	// For std::invalid_argument
#include <cctype>		// For std::isdigit
#include <string>		// For std::stoi

// `Args` constructor checks and parses command-line arguments
Args::Args(int argc, char** argv)
	: width(DEFAULT_WIDTH), height(DEFAULT_HEIGHT) // Set defaults
{
	// Check 'n'
	if (argc < 2)
		throw std::invalid_argument("Error: Missing required argument <n>");

	if (!isInteger(argv[1]))
		throw std::invalid_argument("Error: <n> must be a valid integer");

	n = std::stoi(argv[1]);
	if (n == 0 || n == 1 || n == -1)
		throw std::invalid_argument("Error: <n> must not be 0, 1 or -1 to create a fractal");

	// Check for optional 'width'
	if (argc >= 3)
	{
		if (!isInteger(argv[2]))
			throw std::invalid_argument("Error: [width] must be a valid integer");
		width = std::stoi(argv[2]);
		if (width <= 0)
			throw std::invalid_argument("Error: [width] must be a positive number");
	}

	// Check for optional 'height'
	if (argc >= 4)
	{
		if (!isInteger(argv[3]))
		{
			throw std::invalid_argument("Error: [height] must be a valid integer");
		}
		height = std::stoi(argv[3]);
		if (height <= 0)
		{
			throw std::invalid_argument("Error: [height] must be a positive number");
		}
	}
}

// Prints usage information
void	Args::printUsage(const char* progName)
{
	std::cout	<< BOLD << YELLOW << "Usage: " << progName << " <n> [width] [height]" << RESET << std::endl;
	std::cout	<< "  <n>      : Degree of the polynomial (integer, not 0, 1 or -1)" << std::endl;
	std::cout	<< "  [width]  : Width of the output image (optional, positive integer, default: "
				<< DEFAULT_WIDTH << ")" << std::endl;
	std::cout	<< "  [height] : Height of the output image (optional, positive integer, default: "
				<< DEFAULT_HEIGHT << ")" << std::endl;
}

/**
 Checks if a string contains only a valid integer.
 Allows an optional '+' or '-' sign at the beginning.
 Does NOT allow whitespace or any other characters.
*/
bool	Args::isInteger(const std::string& str)
{
	if (str.empty())
		return false;

	size_t start = 0;
	if (str[0] == '-' || str[0] == '+')
		start = 1;

	for (size_t i = start; i < str.length(); ++i)
	{
		if (!std::isdigit(str[i]))
			return false;
	}
	return true;
}
