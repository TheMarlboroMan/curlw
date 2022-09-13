# curlw

Wrappers around curl HTTP requests. Formerly known as "curl_request".

# Building under Linux.

- Install first the required libcurl components with 'apt-get install libcurl4-gnutls-dev';
- Do the out of source build:
	- mkdir build
	- cd build
	- cmake ..
	- make
- Whatever you use this for, compile with -lcurl.

# Building under Windows 64 bit with mingw-w64.

- Make your life easy and get http://win-builds.org.
- Again, make your life easy and install all packages, even if you won´t use them.
- TODO: See how cmake does this...
- Modify the makefile_win file so the INCLUDE and LIBPATH variables point to the winbuilds include and lib directories.
- Open a shell. Set your path to the "bin" directory of winbuilds (PATH=%PATH%;path-to-the-bin-directory)
- mingw32-make.exe -f makefile_win
- Whatever you use this for, compile with -lcurl.dll

# licenses

This library makes no sense without libcurl itself, whose license can be read on the LICENSE_THIRD_PARTY file.
