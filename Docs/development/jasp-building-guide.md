
Guide to Building JASP
======================

The easiest way to build JASP is to use Qt Creator. After cloning the [jasp-desktop project](https://github.com/jasp-stats/jasp-desktop), open the JASP.pro file in the jasp-desktop directory, in Qt Creator. This project contains two runnable sub-projects, *JASP-Desktop* and *JASP-Engine*. In order to run JASP, *JASP-Desktop* is the correct project to run.

For those unfamiliar with using Qt Creator for development, there is some excellent documentation available [here](http://doc.qt.io/qtcreator/index.html).

Alternatively, those that are more comfortable using command line tools can use *QMake*, the manual for which is available [here](http://doc.qt.io/qt-5/qmake-manual.html). *QMake* projects (like JASP) are typically built in two steps; first *QMake* is called, which generates a Makefile for *Make*, and then *Make* is called with this generated Makefile.

We recommend building JASP in a separate directory to it's source directory. This can be achieved by calling *QMake* in a separate directory to the source, for example we might create a separate directory beside the *jasp-desktop* directory (perhaps called *jasp-build*), and then from this directory call:

    qmake ../jasp-desktop/JASP.pro
    make

This generates the Makefile in the *jasp-build* directory, and all resulting object files and executables will be output to this directory.

JASP requires several dependencies which are documented below.

JASP depends on:

 - [Qt (5+)](http://qt-project.org)
 - [R](http://cran.r-project.org)
 - [boost](http://boost.org)
 - [libarchive](http://libarchive.org/)
 - [zlib](http://zlib.net/)

Links to specific versions for each platform are provided below.

Windows
-------

Building JASP under windows is the most temperamental, and the versions listed here are known to work; slight variations on these numbers probably won't work.

 - [Qt 5.2.1 win64 GCC 4.8.2](https://static.jasp-stats.org/development/x64-Qt-5.2.1+QtCreator-3.0.1-(gcc-4.8.2-seh).7z)
 - [R 3.3.3 win64](https://static.jasp-stats.org/development/R3.3%20Win%20JASP%200.8.4.zip)
 - [boost 1.64.0](https://static.jasp-stats.org/development/boost_1_64_0.zip)
 - [boost 1.64.0 binaries, libarchive binaries](https://static.jasp-stats.org/development/Additional%20Binary%20Deps%20Win64%20for%20JASP%20(2017-06-06).zip)

Qt: JASP for windows is built as 64-bit and is built with Mingw-W64. The Qt project does not ship a 64-bit version of Qt based on Mingw-W64, and so we rely on a version from [here](http://sourceforge.net/projects/mingwbuilds/files/external-binary-packages/Qt-Builds/). Once downloaded and unzipped to the desired location, it is necessary to run the `QtSDK-x86_64/qtbinpatcher.exe`, so it knows about it's current location.

dlls: It is necessary to copy the four files libgcc_s_seh-1.dll, libstdc++-6.dll, libwinpthread-1.dll and libgomp-1.dll from QtSDK-x86_64/bin into the build directory as well.

The directory structure should be as follows:

    [+] jasp-desktop  < from github >
	[+] boost_1_64_0
    [-] build-JASP- ... < build directory, created by QtCreator >
       [+] R
        - libgcc_s_seh-1.dll
        - libstdc++-6.dll
        - libwinpthread-1.dll
        - libgomp-1.dll
		- libboost_system-mgw48-mt-1_64.a
		- libboost_filesystem-mgw48-mt-1_64.a
		- libarchive.dll.a
		- libarchive.dll


Mac OS X
--------
To build JASP on mac you need to clone the JASP repository and download the following five packages:

 0. Clone the JASP repository
 1. [XCode](https://developer.apple.com/xcode/) Easiest would be via the App Store.
 2. [Qt 5.4.0](https://download.qt.io/archive/qt/) Newer version not supported yet.
 3. [R 3.3.3](https://static.jasp-stats.org/development/R3.3%20OSX%20JASP%200.8.3.zip) This contains R and the packages
 4. [boost 1.64.0](https://static.jasp-stats.org/development/boost_1_64_0.zip)
 5. [boost 1.64.0 binaries, libarchive binaries](https://static.jasp-stats.org/development/Additional%20Binary%20Deps%20OSX%20for%20JASP%20(2017-06-06).zip)

The directory structure should be as follows:

    [+] jasp-desktop  < from github >
	[+] boost_1_64_0
    [-] Frameworks
       [+] R.framework
    [+] build-JASP- ... < build directory, created by QtCreator >
		- libboost_system-clang-mt-1_64.a
		- libboost_filesystem-clang-mt-1_64.a
		- libarchive.a
		- libz.a

 0. **Clone** the JASP repository into a folder of your choice. Our default choice is *~/desktop/JASP/* and cloning results in the creating of *~/desktop/JASP/jasp-desktop*.
 1. **XCode**: Qt on OS X relies on XCode to function, you can install this through the App Store. It's easiest if you install this, run it, accept the license agreement, and then close it down before installing Qt.
 2. **Qt**: building JASP on OS X is pretty robust, but for the moment we're restricted to QT 5.4. We will upgrade to a newer version soon. **Note**: *For xcode 8 and above, to accept the license, Qt (v5.4) tries to find xcrun instead of xcodebuild, and the following has to be done,*
- Open the file: *Qt_install_folder/5.4/clang_64/mkspecs/features/mac/default_pre.prf*
- Replace the line *isEmpty($$list($$system("/usr/bin/xcrun -find xcrun 2>/dev/null")))* by  *isEmpty($$list($$system("/usr/bin/xcrun -find xcodebuild 2>/dev/null")))*
 3. **R 3.3.3**: Create the folder */Frameworks* in the directory of your choice in step 0. For our default choice this results in *~/desktop/JASP/Frameworks/* first and subsequently *~/desktop/JASP/Frameworks/R.frameworks*.
 4. **boost 1.64.0**: Unzip these files in the JASP folder. In our case, this results in *~/desktop/JASP/boost_1_64_0/*
 5. **boost 1.64.0 binaries, libarchive binaries**: We're almost there! Fire up QT and try to build JASP. QT will automatically create a */build-JASP-....* directory. In our case this leads to  *~/desktop/JASP/build-JASP-Desktop_Qt_5_5_1_clang_64bit-Debug*. QT will stop building JASP as it requires additional files. Thus, unzip the **boost 1.64.0 binaries, libarchive binaries** files and put them into the */build-JASP-....* directory that Qt created.
 6. Build JASP again from Qt. The first build might take a while, but after the first time it'll go very smoothly.


Linux
-----

### Build

#### Ubuntu (and alike)

Jonathon maintains a number of packages for JASP and it's development under linux in his PPA [here](https://launchpad.net/~jonathon-love/+archive/ubuntu/jasp)

To build JASP under Ubuntu, debian, and derivatives, you will need:

 - qt5-default
 - qt5-qmake
 - libqt5widgets5
 - libqt5webkit5-dev
 - libqt5svg5-dev
 - libboost-dev
 - libboost-filesystem-dev
 - libboost-system-dev
 - r-base-core
 - r-cran-rcpp
 - r-cran-rinside *
 - libarchive-dev

#### Fedora
Under Fedora, you need these packages:
 - qt-devel
 - qt5-qtwebkit-devel
 - boost-devel
 - libarchive-devel

And (under fedora only), in R (started as root so packages are installed systemwide), you need to install:

```
install.packages(c("Rcpp","RInside"))
```

Finally, under Fedora only, you need to create a symlink so that R is found:

```
sudo ln -s /usr/lib64/R/ /usr/lib/R
```

### Runtime
#### Ubuntu (and alike)
In order to run, you will need (Ubuntu and alike):

 - r-cran-bayesfactor *
 - r-cran-lme4
 - r-cran-afex *
 - r-cran-car *
 - r-cran-effects *
 - r-cran-logspline *
 - r-cran-hypergeo *
 - r-cran-rjson

Those marked with asterisks are available from Jonathon's PPA.

### Fedora
It works under Fedora, if you install these R packages manually in R:

```
install.packages(c("BayesFactor","lme4","afex","car","effects","logspline","hypergeo","rjson"))
```
