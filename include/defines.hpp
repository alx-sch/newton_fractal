#ifndef DEFINES_HPP
# define DEFINES_HPP

# define YELLOW		"\033[33m"
# define RED		"\033[31m"
# define BOLD		"\033[1m"
# define RESET		"\033[0m"

# define DEF_WIDTH	800	// Default image width
# define DEF_HEIGHT	600	// Default image height

# define MAX_ITERS	50	// Max iterations for Newton's method

constexpr double EPSILON =			1e-10; // Check if floating-point number is zero
constexpr double DEF_TOLERANCE =	1e-6;  // Tolerance for convergence checks

// Debug Print Macro
// Compile with 'make DEBUG=1' to activate
// 'do/while(0)' ensures the macro behaves like a single statement
# ifdef DEBUG
#  define DEBUG_PRINT(x) do { std::cout << x << std::endl; } while (0)
# else
#  define DEBUG_PRINT(x) do {} while (0)
# endif

#endif
