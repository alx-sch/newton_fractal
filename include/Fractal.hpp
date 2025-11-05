#ifndef NEWTON_FRACTAL_HPP
# define NEWTON_FRACTAL_HPP

#include <vector>
#include <complex>	// to save roots as complex numbers

class Fractal
{
	public:
		Fractal(int n, int width, int height);

		std::vector<std::complex<double>>	roots_;

		void	calculateRoots(int n);

	private:
		int			n_;
		int			width_;
		int			height_;

};

#endif
