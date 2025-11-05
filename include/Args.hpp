#ifndef ARGS_HPP
# define ARGS_HPP

#include <string>

/**
 @brief A class to parse and store command-line arguments.

 This class will parse the arguments in its constructor.
 If parsing fails, it will throw an std::invalid_argument exception.
*/
class Args
{
	public:

		Args();

		int			n;
		int			width;
		int			height;

		void		parse(int argc, char** argv);
		static void	printUsage(const char* progName);

	private:
		static bool	isInteger(const std::string& str);
};

#endif
