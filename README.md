FooFit
======

Intuitive interface to RooFit

## Table of contents
* [Release notes](#release-notes)
* [Installation](#installation)
* [Quick start](#quick-start)
* [Class hierarchy](#class-hierarchy)
## Release notes

### 0.1.0
November 5, 2019
* initial public release

## Installation

### Dependencies
* ROOT 6 (with RooFit and Minuit2 support)
* CMake 2.8

### Installation
```
cd /some/directory
git clone https://github.com/werthm/FooFit.git
cd FooFit
mkdir build
cd build
cmake ..
make
make install
export LD_LIBRARY_PATH="/some/directory/FooFit/build/lib:$LD_LIBRARY_PATH"
export ROOT_INCLUDE_PATH="/some/directory/FooFit/build/include:$ROOT_INCLUDE_PATH"
```
It is recommended to set the `LD_LIBRARY_PATH` and `ROOT_INCLUDE_PATH` variables
in your shell configuration file.

## Quick start
If `LD_LIBRARY_PATH` and `ROOT_INCLUDE_PATH` are set, root should be able to
find the library automatically. If not, there are several ways to load the
library:

ROOT command line interface:
```
root [0] gSystem->Load("libFooFit.so");
```
Interpreted macros:
```
void FooFitMacro()
{
  gSystem->Load("libFooFit.so");
  ...
}
```
Compiled macros:
```
root [0] .include /some/directory/FooFit/build/include
root [1] .x FooFitMacro.C++
```

## Class hierarchy
```
FFRooModel            : base model pdf class
  FFRooModelPol       : polynomial function model
  FFRooModelChebychev : Chebychev polynomial function model
  FFRooModelExpo      : exponential function model
  FFRooModelGauss     : Gaussian function model
  FFRooModelHist      : histogram-based model
  FFRooModelKeys      : model using kernel estimation
  FFRooModelComp      : base class for composite models
    FFRooModelSum     : sum of models
    FFRooModelProd    : product of models

FFRooFit              : base fit class
  FFRooFitHist        : class for fitting histograms
  FFRooFitTree        : class for fitting trees
    FFRooSPlot        : class for sPlot fits of trees

FFRooFitter           : base high-level fit class
  FFRooFitterUnbinned : class for high-level fitting of unbinned data
    FFRooFitterSPlot  : class for high-level sPlot fits of unbinned data
  FFRooFitterBinned   : class for high-level fitting of binned data
FFRooFitterSpecies    : class representing a fit species

FFFooFit              : namespace for utility methods
```

