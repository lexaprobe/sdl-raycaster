# SDL Raycaster

A retro-style raycaster made with SDL3. I started this project because I was interested how some of the first FPS games were made :)

The main program logic is from 3D Sage's [Make Your Own Raycaster](https://www.youtube.com/watch?v=gYRrGTC7GtA&t=557s) series on YouTube.

## Requirements

To run this program for yourself, you will need the following:

- A current version of [SDL](https://www.libsdl.org/).
- A C compiler such as [GCC](https://gcc.gnu.org/) or [Clang/LLVM](https://clang.llvm.org/)
  - MacOS and Linux machines will already have one of the above installed.
  - For Windows, run `gcc --version` and `clang --version` in the command prompt to see which is already installed.
- [`make`](https://www.gnu.org/software/make/manual/make.html) to easily compile the program and manage dependencies.

**Note:** This project assumes SDL is installed in `/Library/Frameworks`. You may need to edit the Makefile to adjust for your SDL install location.

Once you've met the above requirements, either clone this repository onto your local machine with

```bash
git clone https://github.com/lexaprobe/sdl-raycaster.git
```

**OR** navigate to the main page of this repository, click the green Code button, and select "Download ZIP".

## How To Run

From the project directory, run:

```bash
make
./main
```

Then use WASD to move around!
