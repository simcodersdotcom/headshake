# HeadShake plugin for X-Plane 10/11

HeadShake is an X-Plane camera plugin wich adds some nice POV effects while flying in virtual cockpit view.
It is free, opensource and available for Windows, Linux and Mac in both 64 and 32 bit versions.
This plugin was developed by [SimCoders.com](https://www.simcoders.com) and maintained by the community.

# Download

You can download the plugin from https://www.simcoders.com/headshake/features-and-download

# Compile the source code

To compile the source code you need cmake and g++ (or MinGW).

Make sure to specify the [SDK](http://www.xsquawkbox.net/xpsdk/mediawiki/Main_Page) folder path in the CMakeLists.txt file.

Type the following commands in your favourite unix shell.

```
cd xp-plugins-headshake
mkdir build
cd build
cmake -DCMP_LIN=ON ..
make
```

Use `CMP_LIN=ON` to compile the Linux version, `CMP_WIN_64=ON` or `CMP_WIN_32=ON` to compile the Windows version using MinGW or `CMP_MAC=ON` to compile the Mac OSX version.
