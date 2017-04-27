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

