# OOTuple

[![Build Status](https://travis-ci.org/tenphase/OOTuple.svg?branch=master)](https://travis-ci.org/tenphase/OOTuple)
[![License](https://img.shields.io/badge/license-mit-brightgreen.svg?style=flat)](LICENSE.md)
![Status](https://img.shields.io/badge/status-experimental-yellow.svg?style=flat)

OOTuple is an object oriented implementation of a tuple made in C++11.  
It uses Template Meta Programming to ensure that accesses to any element is always 'direct'.  
This means that there's no runtime computation to access elements.  
**Access complexity is always O(1) - (constant)**.

## Project Status

**OOTuple works well as is.**  
**It has not been extensively tested** and it misses features that `std::tuple` has.  
**It is not complete and it is unlikely for it to be someday...**

It has been done as an exercice to better understand:
- how to use variadic template parameters.
- how to implement recursive instantiation of such a template.
- how to declare a recursive accessor trait.

## Compile & Test

- _The OOTuple project uses `cmake` to generate needed the `makefile`s (or the IDE project of your choice)
  and make it easy to compile and test it on a wide range of configurations.  
- You must have at least version 2.8.7 of `cmake` to compile._


##### [ With cmake command line ]

_(Open a terminal and type)_
```
cd build/
cmake ../
make
./OOTuple
```


##### [ With Qt Creator ]

_(Open Qt Creator)_
- Select "File" > "Open File or Project"
- Browse to the OOTuple directory and select "CMakeLists.txt"  
  _(This will open the "CMake Wizard")_  
  It will propose to create a "Build directory" named `OOTuple-build`.  
  **Do not accept**.  
- Press the `Choose...` button and select `OOTuple/build` as build directory.  
  _(because this directory will contain generated headers needed for the compilation)_
- Press `Continue...`
- Then, select the generator of your choice.
- Hit `Run CMake`, then `Done`.
- Test by pressing the `Run` button...


##### [ With CMake GUI ]

_(Open CMake GUI)_
- Select the project directory: `OOTuple`
- Select the build directory: `OOTuple/build`
- Press the `Configure` button
- Then, select the generator of your choice and press `Done`.
- Hit the `Generate` button.
- Finally, use the generated project as intended...


#### Note for Mac users

- _To compile on Mac, you need the Xcode command line tools.  
  (for the system headers to be found by the compiler)_

_(Open a terminal and type)_
```
xcode-select -—install
```

## Examples

- For usage example, see [ootuple.cpp](ootuple.cpp)
