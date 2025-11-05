#ifndef NEWTON_FRACTAL_HPP
# define NEWTON_FRACTAL_HPP

# include <vector>
# include <complex>

class Fractal
{
	public:
		using Complex = std::complex<double>; // Short-hand for complex numbers

		Fractal(int n, int width, int height);

		std::vector<Complex>	roots_;

		void	calculateRoots(int n);
		bool	newtonStep(Complex& z);

	private:
		int		n_;
		int		width_;
		int		height_;

};

#endif
