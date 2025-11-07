#ifndef FRACTAL_HPP
# define FRACTAL_HPP

# include "defines.hpp"	// For Color struct, Complex struct
# include <vector>
# include <string>
# include <utility>	// For std::pair

/**
 @brief Manages the state, generation, and output of a Newton fractal for
 the equation `z^n - 1 = 0.`
 
 This class encapsulates all the logic required to create a Newton fractal
 image. It is initialized with the parameters `n`, `width`, `height`.
 
 Its responsibilities are:
 1. Storing all fractal parameters (n, dimensions, tolerance, viewport).
 2. Pre-calculating the 'n' complex roots for `z^n - 1 = 0`.
 3. Pre-generating a color palette.
 4. Running the core `solvePixel` logic for every pixel in the image.
 5. Storing the final image as a vector of `Color` structs.
 6. Saving the final image data to a c.ppm` file.
*/
class Fractal
{
	public:
		Fractal(int n, int width, int height);

		void	generate();	// wrapper for generateSeq / generateISPC
		void	saveImage(const std::string& filename) const;

	private:
		int		n_;
		int		width_;
		int		height_;
		double	tolerance_;
		int		max_iterations_;

		// Viewport boundaries
		double	x_min_, x_max_, y_min_, y_max_;

		// Pre-computed data
		std::vector<Complex>	roots_;		// Holds the 'n' roots
		std::vector<Color>		palette_;	// The 'n' base colors

		// Final result
		std::vector<Color>		pixel_data_;	// 1D vector holding the 2D image

		void				calculateRoots();
		void				setupPalette();
		bool				newtonStep(Complex& z);
		std::pair<int, int>	solvePixel(Complex z_start, int x, int y);
		Color				calculateColor(int root_index, int iterations) const;

		// called from wrapper generate()
		void				generateSeq();
		void				generateISPC();
};

#endif
