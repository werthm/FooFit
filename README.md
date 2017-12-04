# FooFit

Intuitive interface to RooFit.

#### Dependencies
* ROOT 5.34 or 6.08.06 (with RooFit and Minuit2 support)
* CMake 2.8

#### Installation
```
cd /some/directory
git clone https://github.com/werthm/FooFit.git
cd FooFit
mkdir build
cd build
cmake ..
make
export LD_LIBRARY_PATH="/some/directory/FooFit/build/lib:$LD_LIBRARY_PATH"
```
It is recommended to set the `LD_LIBRARY_PATH` variable in your shell configuration file.

#### Usage
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

#### List of classes
```
FFRooModel            : base model pdf class
  FFRooModelPol       : polynomial function model
  FFRooModelExpo      : exponential function model
  FFRooModelGauss     : Gaussian function model
  FFRooModelHist      : histogram-based model
  FFRooModelKeys      : model using kernel estimation
  FFRooModelComp      : base class for composite models
    FFRooModelSum     : sum of models
    FFRooModelProd    : product of models

FFRooFit              : base fit class
  FFRooFitHist        : class for fitting binned data
  FFRooFitTree        : class for fitting unbinned data
    FFRooSPlot        : class for sPlot fits of unbinned data

FFRooFitter           : base high-level fit class
  FFRooFitterTree     : class for high-level fitting of unbinned data
    FFRooFitterSPlot  : class for high-level sPlot fits of unbinned data
FFRooFitterSpecies    : class representing a fit species

FFFooFit              : namespace for utility methods
```

