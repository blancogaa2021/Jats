Guide to Building JASP
======================

JASP depends on [Qt (5+)](http://qt-project.org), [R](http://cran.r-project.org) and [boost](http://boost.org).

Windows
-------

Building JASP under windows is the most temperamental, and the versions listed here are known to work; slight variations on these numbers probably won't work.

Qt: JASP for windows is built as 64-bit and is built with Mingw-W64. The Qt project does not ship a 64-bit version of Qt based on Mingw-W64, and so we rely on a version from [here](http://sourceforge.net/projects/mingwbuilds/files/external-binary-packages/Qt-Builds/). The x64 Qt 5.2.1 version is known to work and is recommended. It also includes Mingw-64. Once downloaded and unzipped to the desired location, it is necessary to run the `QtSDK-x86_64/qtbinpatcher.exe`, so it knows about it's current location.

R: We provide an R build which is known to work [here](http://jasp-stats.org/development/R%20Win64%20for%20JASP%20%282014-09-09%29.zip)

boost: 1.54.0 works, get it from [here](http://www.boost.org/users/history/version_1_54_0.html)

The directory structure should be as follows:

<pre>
[+] jasp-desktop  &lt; from github &gt;
[+] boost_1_54_0
[-] build-JASP- ... &lt; build directory, created by QtCreator &gt;
   [+] R
</pre>

You should also copy the three files libgcc_s_seh-1.dll, libstdc++-6.dll and libwinpthread-1.dll from QtSDK-x86_64/bin into the build directory as well.
 
Mac OS X
--------
Qt: building JASP on OS X is pretty robust, and most versions work. We currently use 5.3.1, but newer versions will probably work too. You can download it from [here](https://qt-project.org/downloads).

R: We provide a specially crafted R.framework [here](http://jasp-stats.org/development/R%20OSX%20for%20JASP%20%282014-09-09%29.zip)

boost: 1.54.0 works, get it from [here](http://www.boost.org/users/history/version_1_54_0.html) (though we do use boost 1.53.0 for 32-bit JASP)

The directory structure should be as follows:

<pre>
 [+] jasp-desktop  &lt; from github &gt;
 [+] boost_1_54_0
 [-] Frameworks
    [+] R.framework
 [+] build-JASP- ... &lt; build directory, created by QtCreator &gt;
</pre>

Linux
-----
This is pretty straight forward