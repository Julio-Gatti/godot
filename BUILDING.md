# Building

## Windows MinGW

Install Python.

https://www.microsoft.com/store/productId/9NCVDN91XZQP?ocid=pdpshare

PowerShell: `pip install scons`

Figure out where the fuck `scons.exe` went, and add the directory to user path.

In Godot directory:

`scons platform=windows use_mingw=yes`

Or use the `build.bat`.

Possible output:

```
Auto-detected 20 CPU cores available for build parallelism. Using 19 cores by default. You can override it with the -j argument.
Using MinGW, arch x86_64
Building for platform "windows", architecture "x86_64", target "editor".
```

...

```
[Time elapsed: 00:23:44.444]
```
