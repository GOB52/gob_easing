# gob_easing

[日本語](README.ja.md)

## Overview
This is an implementation of a basic easing function.  
They are all constexpr functions. So compile-time computation can be used.  
It is in the form of a PlatformIO and ArduinoIDE library, but can be used with other system in C++11 or later.  
This is header-only library.

See also [https://easings.net/](https://easings.net/)

## How to install
Install in an appropriate way depending on your environment.
* git clone or download zip, and extract into place
* PlatformIO platformio.ini
```ini
lib_deps = https://github.com/GOB52/gob_easing.git
```
* Use library manager on ArduinoIDE

## How to use
Just include gob_easing.hpp.  
namespace is goblib::easing.  
```cpp
#include <gob_easing.hpp>

void foo()
{
    for(int i=0; i <= 100; ++i)
    {
        float t = (float)i / 100; // Clamp [0.0 - 1.0]
        float v = goblib::easing::inBack(t);  //Use this value for what you want to do.
    }
}
```
See examples for details.  
* [graphs, gradients, behavior demo](examples/demo)
* [creating tables with compile-time calculations](examples/lookup_table)


## Document
Can be created from a [configuration file](doc/Doxyfile) and [shell script](doc/doxy.sh) for [Doxygen](https://www.doxygen.nl/).  
Output is to doc/html
```
bash doc/doxy.sh
```
or
See also [GitHub Pages](https://gob52.github.io/gob_easing/)

