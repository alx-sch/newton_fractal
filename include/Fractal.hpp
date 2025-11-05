#ifndef NEWTON_FRACTAL_HPP
# define NEWTON_FRACTAL_HPP

# include <vector>
# include <complex>
# include <utility>	// For std::pair

class Fractal
{
	public:
		using Complex = std::complex<double>; // Short-hand for complex numbers

		Fractal(int n, int width, int height);

		std::vector<Complex>	roots_;

		std::pair<int, int>		solvePixel(Complex z_start);

	private:
		int		n_;
		int		width_;
		int		height_;
		double	tolerance_;
		int		max_iterations_;

		void	calculateRoots(int n);
		bool	newtonStep(Complex& z);


};

#endif
