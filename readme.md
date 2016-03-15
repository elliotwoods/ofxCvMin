# Introduction

ofxCvMin began with the Utility and Wrapper functions from Kyle McDonald's ofxCv (https://github.com/kylemcdonald/ofxCv/) and working builds for OSX and Visual Studio.

The addon aims to provide a lightweight implementation of modern OpenCV with cross-platform C++11 support, i.e.:

* Be fast and convenient (equivalence types, e.g. ofVec2f and cv::Point2f are convertible by refernce without copy)
* Be easy to port between platforms (i.e. no extra libraries)
* Provide some commonly used helper functions which allow the user to most of their work with OpenCV directly.

Currently I'm working with:
* XCode 5
* Visual Studio 2012

This addon does not require ofxOpenCv. 

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
