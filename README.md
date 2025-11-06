# Newton Fractal Visualization with ISPC

This project generates a visualization of the **Newton Fractal** for the equation $z^n - 1 = 0$, utilizing the parallel processing power of the **ISPC** compiler.

#### Visualization of the Newton Fractal with Different Exponents ($n$):
| $n=5$ | $n=8$ | $n=13$ |
| :---: | :---: | :---: |
| <img src="https://raw.githubusercontent.com/alx-sch/NewtonFractal-ISPC/refs/heads/main/.assets/fractal_5n.png" width="95%" alt="Newton Fractal n=5"> | <img src="https://raw.githubusercontent.com/alx-sch/NewtonFractal-ISPC/refs/heads/main/.assets/fractal_8n.png" width="95%" alt="Newton Fractal n=8"> | <img src="https://raw.githubusercontent.com/alx-sch/NewtonFractal-ISPC/refs/heads/main/.assets/fractal_13n.png" width="95%" alt="Newton Fractal n=13"> |

---

## TOC

- [Getting Started](#getting-started)
     - [Prerequisites and Setup](#prerequisites-and-setup)
     - [Building the Project](#building-the-project)
     - [Configuration Constants](#configuration-constants)
- [Calculating the Newton Fractal](#calculating-the-newton-fractal)
- [Parallelization using ISPC](#parallelization-using-ispc)
     - [Performance and Benchmarking](#performance-and-benchmarking)
     - [SIMD & ISCP](#simd--iscp)

--- 

## Getting Started

This project is configured to run instantly in a pre-built environment using a Devcontainer (e.g., **this repository's Codespace**).    
The simplest way to start is to launch the Codespace, as all dependencies are already configured.

---

### Prerequisites and Setup

If you choose to run the project locally outside of a Devcontainer, you must manually install the following dependencies:

* **Make**: The standard build automation tool (the project uses a provided custom `Makefile` to coordinate C++/ISPC compilation)
* **ISPC Compiler**: Required to generate optimized SIMD (Single Instruction, Multiple Data) code from the parallel kernel source.
* **Imagemagick**: Required to convert the raw output files (`.ppm`) into a standard image format (`.png`).

#### Manual Installation Guide (Ubuntu): 
Follow these steps to install the required tools and the ISPC compiler:

1. **Install Standard Dependencies:**
    ```bash
    # Install the core build tools, imagemagick, and file utilities
    sudo apt-get update
    sudo apt-get install -y make g++ imagemagick wget tar
    ```

2. **Install ISPC:**    
   The ISPC compiler must be downloaded and manually placed into the system PATH.
   
    ```bash
    # 1. Download ISPC binary (v1.21.0)
    wget https://github.com/ispc/ispc/releases/download/v1.21.0/ispc-v1.21.0-linux.tar.gz

    # 2. Extract the archive
    tar -xzf ispc-v1.21.0-linux.tar.gz
    
    # 3. Move executable to the system PATH
    sudo mv ispc-v1.21.0-linux/bin/ispc /usr/local/bin/
    
    # 4. Cleanup downloaded files
    rm -rf ispc-v1.21.0-linux*
    ```

3. **Final Verification Check:**     
   After installation, use the following commands to ensure all prerequisites are available and correctly linked in your environment:

   ```bash
    # Check if Make, g++ and Imagemagick are installed
    make --version | head -n 1
    g++ --version | head -n 1
    convert --version | head -n 1
    
    # Check if ISPC is installed and accessible
    ispc --version
    ```

---

### Building the Project

The project uses a provided custom `Makefile` to handle the two-stage compilation: ISPC kernel first, then the C++ host, followed by linking.

From the project root directory, run:

```bash
make
```

The resulting executable (`newton_fractal`) accepts the degree of the polynomial ($n$) as a command-line argument.

```bash
# Usage: ./newton_fractal <degree_n> <optional_width> <optional_height>

# Example 1: Generate a fractal for z^5 - 1 = 0 (5 roots)
./newton_fractal 5

# Example 2: Generate a fractal for z^8 - 1 = 0 with custom resolution
./newton_fractal 8 1024 768
```

The resulting fractal image (`.ppm`) is saved in the `out` folder.

#### Additional Make Targets

In addition to the default `make` command, the provided `Makefile` includes other useful convenience targets:

| Command | Purpose |
| :--- | :--- |
| `make clean` | **Removes intermediate build files** (object files and compiled ISPC kernels). Keeps the main executable. |
| `make fclean` | **Deep cleaning**. Removes all built artifacts, including object files, the final executable, and the output files (`.ppm` or `.png` files). Resets the repository to a clean state. |
| `make re` | **Full Rebuild**. Executes fclean followed by all. This ensures the project is completely cleaned and then rebuilt from scratch. |
| `make png` | **Runs the Imagemagick conversion** on the raw output files (converts `.ppm` to `.png`). Automatically deletes large `.ppm` files upon successful conversion. |
| `make debug` | Builds the executable with a flag that enables **verbose runtime logging** (e.g. iteration details). Redirect to a logfile via shell redirection: `newton_fractal 5 2> log.txt`.

--- 

### Configuration Constants

The header file `define.hpp` acts as the primary configuration point for the visualization. By modifying the constants within this file, users can **tweak the visual outcome of the fractal**.

Key parameters defined here include:

* **Viewport Limits:** Sets the initial boundaries ($\text{MinRe}$, $\text{MaxIm}$, etc.) in the complex plane.
* **Maximum Iterations:** Controls the maximum calculation depth; adjusting this affects the image's **detail** but also increases computation time.
* **Gamma:** Tweaking this controls the **overall brightness and contrast** of the resulting image.

---

## Calculating the Newton Fractal

For a general introduction to Newton Fractals and Newton's Method, check out [this great video](https://www.youtube.com/watch?v=-RdOwhmqP5s) by 3Blue1Brown<sup><a href="#footnote1">[1]</a></sup>.

The Newton Fractal is a type of fractal derived from **Newton's Method** for finding the roots (solutions) of a complex equation $f(z) = 0$. 

The fractal is generated by plotting the complex plane

$$
z_0 = \text{Re}(z_0) + i \cdot \text{Im}(z_0)
$$ 

where:  
- **Hue (Color)**: Determined by which of the $n$ roots the iteration converges to.
- **Brightness/Saturation**: Determined by the number of iterations it takes to converge.

---

#### Finding the Roots:

The equation $z^n = 1$ is solved by understanding that the number $1$ can be expressed in the complex exponential form using the identity<sup><a href="#footnote2">[2]</a></sup>:  

$$
1 = e^{i 2\pi k}
$$

This follows from **Euler’s Formula**

$$
e^{i \theta} = \cos(\theta) + i\sin(\theta)
$$

and the periodicity of sine and cosine, where $k$ is any integer representing full $2\pi$ rotations.

By setting the exponents equal and solving for $z$, we obtain the $n$ distinct roots of unity:

$$
z_k = \cos\left(\frac{2 \pi k}{n}\right) + i \sin\left(\frac{2 \pi k}{n}\right), \quad k = 0, 1, \dots, n-1
$$

---

#### The Iterative Process:

The iterative procedure of Newton’s method is defined as:

$$
z_{k+1} = z_k - \frac{f(z_k)}{f'(z_k)}
$$

The program runs this loop for a given starting complex number $z_0$, computing successive iterations $z_1, z_2, \dots$ until a convergence criterion is met — namely, when the distance between the current value $z_k$ ​and the nearest known root $r_i$ ​ becomes smaller than a tolerance $\epsilon$:

$$
|z_{k+1} - r_i| < \epsilon
$$

For $f(z) = z^n - 1$, the derivative is $f'(z) = n \cdot z^{n-1}$.

Substituting these into the Newton formula gives the specific iteration used in the implementation (and later parallelized in ISPC):

$$
z_{k+1} = z_k - \frac{z_k^n - 1}{n \cdot z_k^{n-1}}
$$

The **root index** ($i$) to which the iteration converges, together with the number of iterations required, determines the color of each pixel in the rendered image

---

## Parallelization using ISPC

### Performance and Benchmarking

A primary goal of this project is to demonstrate the speedup gained by using ISPC for parallel computation. Performance is benchmarked using the shell's built-in `time` command to measure the execution time:

```bash
time ./newton_fractal 12

# time output
real    0m8.422s
user    0m8.391s
sys     0m0.012s
```

#### Baseline (Serial) Execution

The initial serial (single-threaded) execution of the fractal generator provides the baseline for total computational work. We observe that the `user` time (total CPU time spent) closely matches the `real` time (wall clock time), confirming single-core execution.

| Command | Real Time (Wall Clock) | User Time (Total CPU Work) |
| :--- | :--- | :--- |
| `time ./newton_fractal 12` | $\approx 8.4 \text{ s}$ | $\approx 8.4 \text{ s}$ |

This $\approx 8.4 \text{ s}$ represents the **total workload** (the time it takes a single core to complete the computation).

#### ISPC Parallelization and Speedup

The program achieves parallel speedup through two layers:

- **Multi-threading** (managed by the ISPC runtime) to utilize all available CPU cores.
- **SIMD vectorization** to accelerate computation within each core.

You can check the number of available cores using the `nproc` command (on Ubuntu). Since this example runs inside a GitHub Codespace container, only two physical CPUs are available,
while a real machine typically provides 4 or 8 cores.

--- 
### SIMD & ISCP

---
## References

<a name="footnote1">[1]</a> Sanderson, G. (3Blue1Brown); YouTube (Oct 12, 2021). [*Newton’s fractal (which Newton knew nothing about)*](https://www.youtube.com/watch?v=-RdOwhmqP5s)  
<a name="footnote2">[2]</a> Rubine, D.; Quora (2017). [*How do I prove Euler Identity for any integer k?*](https://www.quora.com/How-do-I-prove-1-e-2-pi-ik-for-any-integer-k)     
