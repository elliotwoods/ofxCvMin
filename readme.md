# Introduction

ofxCvMin began with:

* Modern OpenCv libs
* C++11 support on OSX
* Utility and Wrapper functions from Kyle McDonald's ofxCv (https://github.com/kylemcdonald/ofxCv/) (although many have been removed / changed signifcantly. Compatability is not expected)

The aims of this addon are to:

* Be minimal (no significant helper classes)
* Be fast and convenient (equivalence types, e.g. ofVec2f and cv::Point2f are convertible by refernce without copy)
* Give you a working OpenCV environment with the least fuss
* Provide some commonly used helper functions
* 
Currently I'm working with:
* XCode 5
* Visual Studio 2012

This addon does not require ofxOpenCv. 

## Side note

This addon may become less useful when a proper c++11 release of oF arrives.

# Notes to self

## building opencv

* use sources from opencv website
* cmake gui
	* XCode
	* `CMAKE_CXX_FLAGS -m32`
	* `CMAKE_OSX_ARCHITECTURES i386`
	* `CMAKE_OSX_DEPLOYMENT_TARGET 10.8`
* XCode
	* Project settings
		* C++ Language dialect `-std=c++11`
		* Deployment target `10.8` (probably already is)
	* Set to release

A working opencv 2.4.9 build can be found at:
* http://www.kimchiandchips.com/files/openFrameworks/opencv-2.4.9-c++11-osx-libc++-x86.zip
(note some paths are hard-coded for elliot's machine)

then go into the `modules` folder and run:

```bash
for D in *; do
    if [ -d "${D}" ]; then
        cp -r "${D}/include" ../include   # your processing here
    fi
done
```

to gather the includes into the `./includes/` folder.
