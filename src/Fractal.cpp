#include "Fractal.hpp"
#include "defines.hpp"		// DEBUG_PRINT, Default values
#include "complexMath.hpp"	// For Complex operations
#include "fractal.h"		// ISPC generated header

#include <iostream>
#include <cmath>		// For M_PI, cos, sin, std::pow (brightness calculation)
#include <iomanip>		// Formatte output debug prints
#include <fstream>		// For file output
#include <stdexcept>	// For std::runtime_error

/**
 @brief Constructor for the Fractal.

 @param n 		The `n` in `z^n - 1`.
 @param width	The target image width.
 @param height	The target image height.
*/
Fractal::Fractal(int n, int width, int height) :
	n_(n), width_(width), height_(height),
	tolerance_(DEF_TOLERANCE), max_iterations_(MAX_ITERS),
	x_min_(DEF_VIEW_MIN_X), x_max_(DEF_VIEW_MAX_X),
	y_min_(DEF_VIEW_MIN_Y), y_max_(DEF_VIEW_MAX_Y)
{
	calculateRoots();
	setupPalette();
	pixel_data_.resize(width_ * height_); // Allocate space for pixel data

	DEBUG_PRINT("--- Fractal Object Created ---");
	DEBUG_PRINT("  image size: " << width_ << " x " << height_);
	DEBUG_PRINT("  fractal order (n): " << n_);
	DEBUG_PRINT(std::fixed << std::setprecision(2) << 
				"  real axis (x): [" << x_min_ << ", " << x_max_ << "]");
	DEBUG_PRINT(std::fixed << std::setprecision(2) << 
				"  imag axis (y): [" << y_min_ << ", " << y_max_ << "]\n");
}

///////////////////
// MAIN FUNCTION //
///////////////////

/**
 @brief Generates the Newton fractal for all pixels in the image.

 This function serves as a wrapper that chooses the appropriate
 generation method based on compilation flags:
  - If compiled with `-DSEQ` (`make seq`), it runs the sequential CPU version (`generateSeq()`).
  - Otherwise, it runs the ISPC parallel version (`generateISPC()`).

 The function handles mapping each pixel to a complex number, computing
 its convergence using Newton's method, and storing the final colors
 in the `pixel_data_` vector.
*/
void	Fractal::generate()
{
#ifdef SEQ
	generateSeq();	// sequential CPU version
#else
	generateISPC();	// ISPC parallel version
#endif
}

/**
 @brief Runs the main fractal generation loop on the CPU sequentially.

 This function iterates over every `(x, y)` pixel in the image, 
 maps each pixel to a complex number (`z_start`) within the viewport,
 calls `solvePixel()` to determine the root and iteration count,
 then uses `calculateColor()` to determine the final color.
 The resulting color is stored in the 1D `pixel_data_` vector.

 This is the sequential (single-threaded) version.
*/
void	Fractal::generateSeq()
{
	// Main loop: Iterate over each every row
	for (int y = 0; y < height_; ++y)
	{
		for (int x = 0; x < width_; ++x)
		{
			// -- MAP --
			// Convert the pixel (x, y) to a complex number z_start within the viewport
			
			// x (real part) scales from [0, width_-1] to [x_min_, x_max_]
			double	real = x_min_ + (static_cast<double>(x) / (width_ - 1)) * (x_max_ - x_min_);

			// y (imaginary part) scales from [0, height_-1] to [y_max_, y_min_]
			double	imag = y_max_ - (static_cast<double>(y) / (height_ - 1)) * (y_max_ - y_min_);

			Complex	z_start = {real, imag};

			// -- SOLVE --
			std::pair<int, int>	solution = solvePixel(z_start, x, y);

			// -- COLOR --
			Color	pixel_color = calculateColor(solution.first, solution.second);

			// Debug logging for some pixels
			if (x % DEBUG_PIXEL_INTERVAL == 0 && y % DEBUG_PIXEL_INTERVAL == 0)
			{
				DEBUG_PRINT("  Color: ("
							<< static_cast<int>(pixel_color.r) << ", "
							<< static_cast<int>(pixel_color.g) << ", "
							<< static_cast<int>(pixel_color.b) << ")\n");
			}

			// --- STORE --- Save color in 1D pixel vector
			pixel_data_[y * width_ + x] = pixel_color;
		}
	}
}

/**
 @brief Generate the Newton fractal using the ISPC parallel kernel.

 This function sets up the necessary buffers for ISPC, calls the ISPC
 `calculateFractal` kernel to compute the fractal in parallel, and
 stores the resulting pixel colors in `pixel_data_`.

 Steps performed:
 1.	Allocate temporary integer vectors (`root_indices` and `iterations`)
	to hold the results for each pixel.
 2.	Call the ISPC kernel `calculateFractal`, using pointers to the output arrays
	(`.data()` is used to get raw pointers from the vectors,
	because ISPC requires C-style arrays).
 3.	Convert the integer results from ISPC into `Color` objects for each pixel,
	storing them in `pixel_data_` for later saving to an image file.
*/
void	Fractal::generateISPC()
{
	// Allocate output buffers for ISPC lanes
	std::vector<int>	root_indices(width_ * height_);
	std::vector<int>	iterations(width_ * height_);

	// Call ISPC kernel; use .data() to get raw pointer to vector memory
	// This call populates 'root_indices' and 'iterations' with all pixel results
	ispc::calculateFractal(
		width_, height_, n_, roots_.data(), tolerance_, EPSILON, max_iterations_,
		x_min_, x_max_, y_min_, y_max_, root_indices.data(), iterations.data()
	);

	// Convert results to Color vector
	for (int i = 0; i < width_ * height_; ++i)
	{
		pixel_data_[i] = calculateColor(root_indices[i], iterations[i]);
	}
}

///////////////
// SAVE FILE //
///////////////

/**
 @brief Saves the generated fractal data to a `.ppm` file.

 @param filename	The name of the output file.
*/
void	Fractal::saveImage(const std::string& filename) const
{
	// Open filestream for writing
	std::ofstream	outFile(filename);
	if (!outFile.is_open())
	{
		throw std::runtime_error("Error: Could not open file '" + filename + "' for writing.");
	}

	// Write the PPM Header. This tells the image viewer
	// it's a text-based (P3) RGB image, of width/height dimensions,
	// with max color value of 255.
	outFile << "P3\n";
	outFile << width_ << " " << height_ << "\n";
	outFile << "255\n";

	// Write all the pixel data
	// We Loop through our 1D vector 'pixel_data_' from start to end.
	for (const Color& pixel : pixel_data_)
	{
		outFile << static_cast<int>(pixel.r) << " "
				<< static_cast<int>(pixel.g) << " "
				<< static_cast<int>(pixel.b) << "\n";
	}

	outFile.close();

	// Print summary to console
	std::cout	<< BOLD << "Fractal image saved to '" << YELLOW << filename
				<< RESET << BOLD << "'"
				<< RESET << std::endl;
	std::cout	<< "  image size: " << width_ << " x " << height_ << std::endl;
	std::cout	<< "  fractal order (n): " << n_ << std::endl;
	std::cout	<< std::fixed << std::setprecision(2) <<
				"  real axis (x): [" << x_min_ << ", " << x_max_ << "]" << std::endl;
	std::cout	<< std::fixed << std::setprecision(2) <<
				"  imag axis (y): [" << y_min_ << ", " << y_max_ << "]" << std::endl;
	std::cout	<< "\nUse '" << YELLOW << "make png" << RESET << "' to convert the .ppm file to .png format."
				<< std::endl;
}

///////////////////////////////
// PRE-COMPUTATION FUNCTIONS //
///////////////////////////////

/**
 @brief Calculates all `n` solutions to `z^n = 1` and adds them to the `roots_` vector.

 This function solves `z^n = 1` by finding the `n` complex roots using the formula:
 `r_k = cos(2*pi*k/n) + i*sin(2*pi*k/n)`
*/
void	Fractal::calculateRoots()
{
	roots_.clear(); // Clear vector from previous calculations

	DEBUG_PRINT("--- Calculating " << n_ << " Roots ---");

	for (int k = 0; k < n_; ++k)
	{
		double	theta = 2 * M_PI * static_cast<double>(k) / n_;
		Complex	root = { cos(theta), sin(theta) };
		DEBUG_PRINT("  Root " << k << ": (" << root.real << ", " << root.imag << ")");
		roots_.push_back(root);
	}
	DEBUG_PRINT("");
}

/**
 @brief Populates the `palette_` vector with `n` base colors.

 This assigns a color to each of the `n` roots for visualization.
 It used a pre-defined set of 13 distinct colors and will repeat (wrap around)
 if `n` is larger that the number of available colors.
*/
void	Fractal::setupPalette()
{
	palette_.clear();

	// A pre-defined list of distinct colors
	const std::vector<Color> master_palette =
	{
		{255, 0, 0},		// 1. Red
		{0, 255, 0},		// 2. Green
		{0, 0, 255},		// 3. Blue
		{255, 255, 0},		// 4. Yellow
		{255, 0, 255},		// 5. Magenta
		{0, 255, 255},		// 6. Cyan
		{255, 165, 0},		// 7. Orange
		{255, 105, 180},	// 8. Hot Pink
		{173, 255, 47},		// 9. Lime Green
		{0, 191, 255},		// 10. Deep Sky Blue
		{148, 0, 211},		// 11. Violet
		{255, 20, 147},		// 12. Deep Pink
		{255, 215, 0}		// 13. Gold
	};

	// Add the first 'n_' colors from master_palette to palette_
	for (int i = 0; i < n_; ++i)
	{
		// Use modulo to wrap around if n_ > master_palette.size()
		Color	color = master_palette[i % master_palette.size()];
		palette_.push_back(color);
	}

	DEBUG_PRINT("--- Palette Setup ---");
	DEBUG_PRINT("  Created a palette with " << palette_.size()
				<< " colors (distinct: "
				<< (static_cast<int>(master_palette.size()) < n_ ?
					master_palette.size() : n_)
				<< ")\n");
}

//////////////////////
// HELPER FUNCTIONS //
//////////////////////

/**
 @brief Performs one step of the Newton's iteration by updating `z`.

 This is the core formula:
 `z_{k+1} = z_k - f(z_k)/f'(z_k)`

 For `f(z_k) = z^n - 1`, this becomes:
 `z_{k+1} = z_k - (z_k^n - 1) / (n*z_k^(n-1))`

 @return	`true` if the step was successful,
 			`false` if the derivative was too small.
*/
bool	Fractal::newtonStep(Complex& z)
{
	Complex	f_z = complexSub(complexPow(z, n_), Complex{1, 0}); // f(z) = z^n - 1
	Complex	z_n_minus_1 = complexPow(z, n_ - 1);
	Complex	n_complex = {static_cast<double>(n_), 0.0};
	Complex	f_prime_z = complexMul(n_complex, z_n_minus_1); // f'(z) = n*z^(n-1)

	// Checking '== 0.0' is tricky with floating-point numbers
	if (complexAbs(f_prime_z) < EPSILON)
		return false; // Avoid division by zero

	z = complexSub(z, complexDiv(f_z, f_prime_z)); // Newton's method step -> get z_{k+1} from z_k
	return true;
}

/**
 @brief Determines which root the starting point `z_start` converges to
		and how many iterations it took.

 This function applies Newton's method iteratively starting from `z_start`
 until convergence to one of the known roots or until the maximum number
 of iterations is reached or a division by zero occurs.

 @param z_start	The initial complex number to start the iteration from.
 @param x		The x-coordinate of the pixel (column).
 @param y		The y-coordinate of the pixel (row).

 @return	A pair where the first element is the index of the converged root
			(or -1 if no convergence) and the second element is the number of
			iterations taken.
*/
std::pair<int, int>	Fractal::solvePixel(Complex z_start, int x, int y)
{
	Complex	z = z_start;

	// Only log one pixel every N rows/cols
	bool	log_this_pixel = (x % DEBUG_PIXEL_INTERVAL == 0
							&& y % DEBUG_PIXEL_INTERVAL == 0);

	if (log_this_pixel)
		DEBUG_PRINT("--- Solving Pixel (" << x << ", " << y << ") ---");

	// Loop through max allowed iterations until convergence is found (or division by zero)
	for (int iter = 0; iter < max_iterations_; ++iter)
	{
		if (log_this_pixel)
			DEBUG_PRINT(std::fixed << std::setprecision(4) <<"  z_" << iter
						<< ": (" << z.real << ", " << z.imag << ")");
		// CHECK FOR CONVERGENCE
		// Check if 'z' is close enough to any of the pre-calculated 'n' roots
		for (int k = 0; k < n_; ++k)
		{
			// If distance between between z and root is less than tolerance, we have converged (root found!)
			if (complexAbs(complexSub(z, roots_[k])) < tolerance_)
			{
				if (log_this_pixel)
					DEBUG_PRINT("  Converged to root " << k << " in " << iter << " iterations");
				return std::make_pair(k, iter);
			}
		}

		// NOT CONVERGED YET - PERFORM NEWTON STEP
		if (!newtonStep(z))
		{
			if (log_this_pixel)
				DEBUG_PRINT("  Iter " << iter << ": Derivative too small, stopping iteration");
			return std::make_pair(-1, iter); // -1 indicates no convergence
		}
	}

	// NO CONVERGENCE WITHIN max_iterations_ -> Also a failure
	if (log_this_pixel)
		DEBUG_PRINT("Did not converge within " << max_iterations_ << " iterations");
	return std::make_pair(-1, max_iterations_); // Did not converge
}

/**
 @brief Calculates the color for a pixel based on the root index and iterations.

 This function uses the base color for the root and adjusts its brightness
 based on the number of iterations taken to converge.

 @param root_index	The index of the root to which the pixel converged.
 @param iterations	The number of iterations taken to converge.
 @return			The calculated Color for the pixel.
*/
Color	Fractal::calculateColor(int root_index, int iterations) const
{
	// If solvePixel() returned -1 (no convergence), use black color
	if (root_index == -1)
		return {0, 0, 0};

	// Get base color
	Color	base_color = palette_[root_index];

	// Calculate brightness factor (more iterations -> darker color)
	// 0 iterations: 100% bright (1.0); max_iterations_: 0% bright (0.0)
	double	linear_brightness = 1.0 - static_cast<double>(iterations) / max_iterations_;

	// Spice it up with gamma correction for better visual contrast
	double	brightness = std::pow(linear_brightness, GAMMA);

	// Apply shading to base color
	// Multiply each color component by brightness factor
	unsigned char	r = static_cast<unsigned char>(base_color.r * brightness);
	unsigned char	g = static_cast<unsigned char>(base_color.g * brightness);
	unsigned char	b = static_cast<unsigned char>(base_color.b * brightness);

	return {r, g, b};
}