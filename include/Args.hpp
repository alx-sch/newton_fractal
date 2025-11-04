#ifndef ARGS_HPP
# define ARGS_HPP

#include <string>

/**
 A class to parse and store command-line arguments.
 This class will parse the arguments in its constructor.
 If parsing fails, it will throw an std::invalid_argument exception.
*/
class Args
{
	public:

		Args(int argc, char** argv);

		int			n;
		int			width;
		int			height;

		static void	printUsage(const char* progName);

	private:
		static bool	isInteger(const std::string& str);
};

#endif
