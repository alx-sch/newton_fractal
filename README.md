# Newton Fractal Visualization with ISPC

This project generates visualizations of the **Newton Fractal** for the equation $z^n - 1 = 0$, leveraging Intel's **ISPC** for parallel processing to achieve a ~15x speedup compared to a sequential implementation.

#### Visualization of the Newton Fractal with Different Exponents ($n$):
| $n=5$ | $n=8$ | $n=13$ |
| :---: | :---: | :---: |
| <img src="https://raw.githubusercontent.com/alx-sch/NewtonFractal-ISPC/refs/heads/main/.assets/fractal_5n.png" width="95%" alt="Newton Fractal n=5"> | <img src="https://raw.githubusercontent.com/alx-sch/NewtonFractal-ISPC/refs/heads/main/.assets/fractal_8n.png" width="95%" alt="Newton Fractal n=8"> | <img src="https://raw.githubusercontent.com/alx-sch/NewtonFractal-ISPC/refs/heads/main/.assets/fractal_13n.png" width="95%" alt="Newton Fractal n=13"> |

---

## TOC

- [Getting Started](#-getting-started)
     - [Prerequisites and Setup](#%EF%B8%8F-prerequisites-and-setup)
     - [Building the Project](#%EF%B8%8F-building-the-project)
     - [Configuration Constants](#-configuration-constants)
- [Calculating the Newton Fractal](#-calculating-the-newton-fractal)
- [Parallelization using ISPC](#-parallelization-using-ispc)
     - [Performance and Benchmarking](#%EF%B8%8F-performance-and-benchmarking)
     - [SIMD & ISPC](#%EF%B8%8F-simd--ispc)

--- 

## 🚀 Getting Started

This project is configured to run instantly in a pre-built environment using a **Dev Container** (e.g., this repository's **Codespace**).    
The simplest way to start is to launch the Codespace, as all dependencies are already configured.

---

### ⚙️ Prerequisites and Setup

If you choose to run the project locally outside of a Dev Container, you must manually install the following dependencies:

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

**Note on Editor Support:**   

If you plan on contributing or modifying the ISPC kernel code, please consider installing the official **Intel ISPC** extension for your editor (e.g., VS Code). Doing so makes `.ispc` code readable via syntax highlighting and makes debugging easier thanks to real-time file validation and error reporting.

**Mono Runtime:** If you are using Linux or macOS, the advanced features of the editor extension may require the Mono Runtime to be installed<sup><a href="#footnote1">[1]</a></sup>. This is separate from running the core `ispc` compiler, which should work immediately after Step 2.

**➡️ Dev Container Users:** The ISPC extension and Mono Runtime are preinstalled in the Codespace environment.

---

### 🏗️ Building the Project

The project uses a provided custom `Makefile` to handle the two-stage compilation: ISPC kernel first, then the C++ host, followed by linking.

1. **Build the executable:**   
     ```bash
     make
     ```

2. **Run the executable:**   
     The resulting executable (`newton_fractal`) accepts the degree of the polynomial ($n$) as a command-line argument.
     
     ```bash
     # Usage: ./newton_fractal <degree_n> <optional_width> <optional_height>
     
     # Example 1: Generate a fractal for z^5 - 1 = 0 (5 roots)
     ./newton_fractal 5
     
     # Example 2: Generate a fractal for z^8 - 1 = 0 with custom resolution
     ./newton_fractal 8 1024 768
     ```

     The resulting fractal image (`.ppm`) is saved in the `out/` folder.

3. **Convert the image:**      
     To convert the raw .ppm file to a more common .png format, run:
     
     ```bash
     make png
     ```
     
     This uses `imagemagick` to convert the image and will delete the large original `.ppm` file.

#### Additional Make Targets

In addition to the `make` commands above, the provided `Makefile` includes other useful convenience targets:

| Command | Purpose |
| :--- | :--- |
| `make clean` | **Removes intermediate build files**. Keeps the main executable. |
| `make fclean` | **Deep cleaning**. Removes all built artifacts, including object files, the final executable, and the output files (`.ppm` or `.png` files). Resets the repository to a clean state. |
| `make re` | **Full Rebuild**. Ensures the project is completely cleaned and then rebuilt from scratch. |
| `make seq` | Rebuilds the project using a **sequential (non-ISPC) C++** implementation for comparison. |
| `make debug` | Rebuilds the executable with a flag that enables **verbose runtime logging**. Redirect to a logfile via shell redirection: `newton_fractal 5 2> log.txt`. |
| `make debug_seq` | Rebuilds the program in **sequential mode** *and* with the **debug flag**. As debug prints are invoked during fractal generation in this mode, this allows you to follow the convergence of individual pixels. |

--- 

### 💡 Configuration Constants

The header file `define.hpp` acts as the primary configuration point for the visualization. By modifying the constants within this file, users can **tweak the visual outcome of the fractal**.

Key parameters defined here include:

* **Viewport Limits:** Sets the initial boundaries ($\text{MinRe}$, $\text{MaxIm}$, etc.) in the complex plane.
* **Maximum Iterations:** Controls the maximum calculation depth; adjusting this affects the image's **detail** but also increases computation time.
* **Gamma:** Tweaking this controls the **overall brightness and contrast** of the resulting image.

---

## 🧮 Calculating the Newton Fractal

For a general introduction to Newton Fractals and Newton's Method, check out [this great video](https://www.youtube.com/watch?v=-RdOwhmqP5s) by 3Blue1Brown<sup><a href="#footnote2">[2]</a></sup>.

The Newton Fractal is a type of fractal derived from **Newton's Method** for finding the roots (solutions) of a complex equation $f(z) = 0$. 

The fractal is generated by plotting the complex plane

$$
z_0 = \text{Re}(z_0) + i \cdot \text{Im}(z_0)
$$ 

where:  
- **Hue (Color)**: Determined by which of the $n$ roots the iteration converges to.
- **Brightness/Saturation**: Determined by the number of iterations it takes to converge.

---

#### 🔍 Finding the Roots:

The equation $z^n = 1$ is solved by understanding that the number $1$ can be expressed in the complex exponential form using the identity<sup><a href="#footnote3">[3]</a></sup>:  

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

#### 🔄 The Iterative Process:

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

## ⚡ Parallelization using ISPC

### 🏎️ SIMD & ISPC

This project achieves its massive speedup by mapping the pixel-by-pixel computation onto the **Single Instruction, Multiple Data (SIMD)** model. SIMD enables a single instruction to operate on multiple data elements (e.g., several pixels) simultaneously, leveraging the inherent data parallelism in fractal rendering. A concise introduction to SIMD operations can be found [here](https://ftp.cvut.cz/kernel/people/geoff/cell/ps3-linux-docs/CellProgrammingTutorial/BasicsOfSIMDProgramming.html)<sup><a href="#footnote4">[4]</a></sup>.

This highlights the key distinction between the sequential (scalar) and parallel (vectorized) approaches:

- **Sequential Operation:** The CPU performs one operation on one data element (e.g., `A[0] + B[0]` → `42 + 69`).
- **Vector Operation (SIMD):** The CPU performs one operation on an entire vector of elements within the same clock cycle (e.g., $`[0..3] + B[0..3]` → `[42, -9, 77, 0] + [69, 3, -53, -82]`)<sup><a href="#footnote5">[5]</a></sup>.

#### The Role of the C++ Host Code     
Before ISPC, the host C++ application (`generateSeq()` in `src/Fractal.cpp`) would run a slow, explicit loop over each pixel, calling the complex number iteration function sequentially for all width × height pixels.  

---

**Intel's ISPC (Intel Implicit SPMD Program Compiler)** simplifies the process of writing this SIMD-optimized code<sup><a href="#footnote6">[6]</a></sup>. ISPC uses the **Single Program, Multiple Data (SPMD)** programming model:

1. The programmer writes a single kernel function (in this case, the iterative Newton's method).
2. ISPC automatically compiles this kernel to generate low-level SIMD instructions (like AVX or SSE) specifically for the target CPU architecture<sup><a href="#footnote7">[7]</a></sup>.
3. At runtime, the ISPC system handles the parallel launch of the kernel, with each execution instance (called a "program instance") processing an entire SIMD vector of data simultaneously.

This programming model is particularly effective for the Newton Fractal, since each pixel’s computation is independent — making it a perfect example of data parallelism ideal for SPMD/SIMD execution.

---

### ⏱️ Performance and Benchmarking

A primary goal of this project is to demonstrate the significant speedup gained by using ISPC for parallel computation. Performance is benchmarked using the shell's built-in `time` command to measure the "real" (wall clock) execution time.

#### Baseline (Sequential) Execution

First, we compile a purely sequential version (`make seq`) to establish our performance baseline. This version runs on a single CPU core and does not use ISPC's SIMD optimizations. We measure the time to compute a fractal of order $n=42$:

```bash
~$ time ./newton_fractal 42
[...]

...
real    0m28.632s
user    0m28.492s
sys     0m0.025s
```

The `real` time (wall clock) of **~28.6 seconds** closely matches the `user` time (total CPU time spent). This confirms the computation was executed on a single CPU core.

#### ISPC Parallelization and Speedup

Next, we recompile the program with ISPC enabled (`make re`). This build leverages **SIMD** instructions, allowing the CPU to perform calculations on multiple data points (e.g., 4, 8 or 16 pixels) simultaneously within a single core.

We run the exact same computation:

```bash
~$ time ./newton_fractal 42
[...]

...
real    0m1.973s
user    0m1.954s
sys     0m0.009s
```

The execution time has dropped dramatically from 28.6 seconds to **~1.97 seconds**, that's a **~14.5x speedup**.

---

#### Analysis: How the Speedup Was Achieved

It is important to note how this **~14.5x speedup** is achieved. ISPC provides parallelism in two ways:

1. **SIMD Vectorization:** Performing multiple operations at once *within* a single core.
2. **Multi-threading:** Distributing the work across *multiple* CPU cores (via its tasking system).

In this specific test (run in a GitHub Codespace with 2 CPU cores), notice that the `real` time (`1.973s`) and `user` time (`1.954s`) for the ISPC version are almost identical.
- If ISPC were using multiple cores (e.g., 2 cores fully), we would expect the `user` time (total CPU work) to be roughly double the `real` time (wall clock).
- Since `real` and `user` times are the same, it confirms the program ran on only **one core**.

**Conclusion:** The massive speedup is achieved almost entirely through **SIMD vectorization** on a single core.   

The ISPC compiler vectorized the fractal logic so effectively that it could process ~14-15 data points (pixels) in the same amount of time the sequential C++ code processed one.

Furthermore, this speedup factor becomes even more pronounced for higher values of $n$ (the fractal order), as this increases the computational work that can be parallelized, while the program's overhead remains relatively fixed.

On a local machine with 4, 8 or 16 cores, ISPC would likely leverage *both* SIMD and multi-threading, resulting in an even greater speedup. You can find more details on this concept in the official [ISPC Performance Guide](https://ispc.github.io/perf.html).


---
## References

<a name="footnote1">[1]</a> Mono Project; 2025. [*Install Mono*](https://www.mono-project.com/docs/getting-started/install/)    
<a name="footnote2">[2]</a> Sanderson, G. (3Blue1Brown); YouTube (Oct 12, 2021). [*Newton’s fractal (which Newton knew nothing about)*](https://www.youtube.com/watch?v=-RdOwhmqP5s)  
<a name="footnote3">[3]</a> Rubine, D.; Quora (2017). [*How do I prove Euler Identity for any integer k?*](https://www.quora.com/How-do-I-prove-1-e-2-pi-ik-for-any-integer-k)   
<a name="footnote4">[4]</a> Sony Computer Entertainment Inc. (2008). [*Documents of PS3 Linux Distributor's Starter Kit*](https://ftp.cvut.cz/kernel/people/geoff/cell/ps3-linux-docs/CellProgrammingTutorial/BasicsOfSIMDProgramming.html)   
<a name="footnote5">[5]</a> InfluxData Inc. (2025). [*What is vector processing?*](https://www.influxdata.com/glossary/vector-processing-SIMD/)   
<a name="footnote6">[6]</a> Intel Corporation (2025). [*Intel Implicit SPMD Program Compiler*](https://ispc.github.io/)   
<a name="footnote7">[7]</a> Wikipedia (Oct 6, 2025). [*Streaming SIMD Extensions*](https://en.wikipedia.org/wiki/Streaming_SIMD_Extensions)   
