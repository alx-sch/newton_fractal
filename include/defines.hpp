#ifndef DEFINES_HPP
# define DEFINES_HPP

# define OUTPUT_DIR	"out"	// Directory to save output files

# define DEF_WIDTH	800	// Default image width
# define DEF_HEIGHT	800	// Default image height

# define MAX_ITERS	100	// Max iterations for Newton's method

// Adjust brightness of colors
// - 0.0: no contrast / brighter,
// - 1.0: original/linear
// - >1.0: more contrast / darker
constexpr double GAMMA =	8.0;

// The viewport defines the area of the complex plane to visualize
// x is the real part, y is the imaginary part
constexpr double DEF_VIEW_MIN_X =	-2.0;	// Default viewport minimum x
constexpr double DEF_VIEW_MAX_X =	2.0;	// Default viewport maximum x
constexpr double DEF_VIEW_MIN_Y =	-2.0;	// Default viewport minimum y
constexpr double DEF_VIEW_MAX_Y =	2.0;	// Default viewport maximum y

constexpr double EPSILON =			1e-10; // Check if floating-point number is zero
constexpr double DEF_TOLERANCE =	1e-6;  // Tolerance for convergence checks

# define YELLOW		"\033[33m"
# define RED		"\033[31m"
# define BOLD		"\033[1m"
# define RESET		"\033[0m"

// DEBUG LOGGING MACRO
// Compile with 'make DEBUG=1' to activate
// 'do/while(0)' ensures the macro behaves like a single statement
# define DEBUG_PIXEL_INTERVAL		100 // Log one pixel every N rows/cols

# ifdef DEBUG
#  define DEBUG_PRINT(x) do { std::cerr << x << std::endl; } while (0)
# else
#  define DEBUG_PRINT(x) do {} while (0)
# endif

// Struct to hold a complex number, shared between C++ host and ISPC kernel
// Share `roots` and `z` arrays.
struct	Complex
{
	double	real;
	double	imag;
};

// Structure to hold RGB color values
struct Color
{
	unsigned char	r; // Red
	unsigned char	g; // Green
	unsigned char	b; // Blue
};

#endif
