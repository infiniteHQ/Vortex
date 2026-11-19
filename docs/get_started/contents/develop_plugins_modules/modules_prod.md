### Distribute your modules

If you've just finished a version of your module! Perfect, first of all, we recommend creating releases if you are using a git server.

In order to distribute your module you will need to run the build.sh on Linux and build.bat on Windows.

As the result of these scripts, you will have an output tarball and a checksum in the build folder (the final folder is in dist/).

Please remember that modules need to be compiled on every platform you want to distribute your module to.

### From our example

From our HelloWorldModule example, in the scripts folder, you can see the build_vx scripts (.bat for Windows and .sh for Unix/Linux systems). You need to execute them to build Vortex, which is necessary to build your module binary. Then, you just have to execute the build script. This will generate your final tarball.