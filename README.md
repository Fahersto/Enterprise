![Enterprise Engine](_resources/Branding/EPLogo_BlackCaptioned.png)

# Enterprise Engine
Enterprise is a "pet project" game engine I'm developing for Windows and macOS in my free time. I'm building 
it for these reasons:
* Because.
* To learn more about game engines.
* To push my C++ knowledge, and my programming skills overall.
* To practice integrating audio middleware, such as FMOD and Wwise.
* To serve as a testbed for developing new technologies or game ideas.

The engine is written in C++.  Feel free to clone the project and build on it, or use it as a point of 
reference in your own projects.

## Getting started
1. Use Git to clone the repository with `--recurse-submodules` set.  If you're cloning with HTTPS, your 
script should look similar to the following:

    `git clone --recurse-submodules https://github.com/theOtherMichael/Enterprise.git`

2. In *_scripts*, run the script file for your platform to build your project files.
    * Windows: **pm_Win32_VS2019.bat**
    * macOS: **pm_Mac_XCode11.command**
3. In the root directory, open the newly generated `.sln` file (Visual Studio) or `.xcworkspace` file (Xcode) 
and build it.

At present, only Visual Studio 2019 (Windows) and Xcode 11 (macOS) have been tested.  I have no plans to 
add support for other compilers or platforms.

### For Visual Studio Users
Ensure you have installed the Windows 10 SDK.  It can be installed in the optional components of the Visual 
Studio 2019 Installer, or as a direct download [here](https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk/).

### For Xcode Users
Enterprise uses [Premake](https://premake.github.io/) to generate project files.  At present, Premake does 
not support custom Xcode schemes.  A default scheme is set up for a test "Sandbox" application, but one is 
also generated for every library in the solution.  You may need to manually switch to the scheme for Sandbox.

Eventually, I will find a better way to generate schemes, but for now, I recommend making the following 
manual changes to the "Sandbox" scheme:
* Set **Run -> Options -> Console** to **Use Terminal**.  Xcode's built-in console does not support colored
  text.
* In **Run -> Arguments -> Environmental Variables**, set `OS_ACTIVITY_MODE = disable`.  This disables
  OS-level logging.

## Current State of Development
The engine is still pretty barebones, but it is now on a solid foundation and is ready for higher systems 
like `Graphics` and `Audio`.  What I'm calling the "Foundation" systems (`Time`, `Events`, and `File`) aren't 
completely *finished*, but are all useable MVP's.

The `Input` system is pretty much done, excepting mouse input.  Up next is the `Graphics` system (finally), 
after which will be `Audio` and the gameplay helper systems, `ECS` and `StateManager`.

### Roadmap
Progress towards an MVP Enterprise Engine can be tracked by the development status of its main systems, which 
are:

1. The `Time` system (done*)
2. The `Events` system (done*)
3. The `File` system (done*)
4. The `Input` system (done*)
5. The `Graphics` system (in progress)
6. The `Audio` system
7. The `ECS` system
8. The `StateManager`

\* That is to say, good enough to move on from.

### Extras
The following are "stretch goals" I may implement after having used the Enterprise MVP for a while:

* An installation package generator
* Viewers/editors for asset creation
* Support for Linux and web players
* Online store integration (Steam, etc.)
* A Network system

## APIs Used
### Windows
* Win32
* Raw Input (for low-level keyboard/mouse input)
* XInput (for gamepad input)
* OpenGL (DirectX support coming later)

### macOS
* Cocoa
* Game Controller Framework (for gamepad input)
* OpenGL (Metal support coming later)

## Libraries Used
* Premake: <https://premake.github.io/>
* Spdlog: <https://github.com/gabime/spdlog>
* Cxx-prettyprint: <http://louisdx.github.io/cxx-prettyprint/>
* Compile-Time SpookyHash: <https://github.com/theOtherMichael/CTSpookyHash>
* Hot Constants: <https://github.com/theOtherMichael/HotConsts>
* Glad: <https://glad.dav1d.de/>
* stb_image: <https://github.com/nothings/stb>
