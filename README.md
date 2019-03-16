# curl_request

Simple wrapper around an HTTP request.

# Building under Linux.

- Install first the required libcurl components with 'apt-get install libcurl4-gnutls-dev';
- Do make -f makefile_linux
- Whatever you use this for, compile with -lcurl.

# Building under Windows 64 bit with mingw-w64.

- Make your life easy and get http://win-builds.org.
- Again, make your life easy and install all packages, even if you won´t use them.
- Modify the makefile_win file so the INCLUDE and LIBPATH variables point to the winbuilds include and lib directories.
- Open a shell. Set your path to the "bin" directory of winbuilds (PATH=%PATH%;path-to-the-bin-directory)
- mingw32-make.exe -f makefile_win
- Whatever you use this for, compile with -lcurl.dll
