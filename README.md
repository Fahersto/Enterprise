![Enterprise Engine](_Resources/Branding/EPLogo_BlackCaptioned.png)

# Enterprise Engine
Enterprise is a "pet project" game engine I'm developing in C++ in my free time. I'm building it for these reasons:
* Because.
* To learn more about game engines.
* To push my C++ knowledge, and my programming skills overall.
* To practice integrating audio middleware, such as FMOD and Wwise.
* To serve as a testbed for developing new technologies or game ideas.

This project is still in its early stages (I only started working on it at the end of December 2019), but it's at a point now where I figured I'd make my efforts public. Feel free to clone the project to see what I've done, and if you're so inclined, you are welcome to contribute as well.

## Getting started
Use Git to clone the repository:

`git clone --recurse-submodules https://github.com/theMartzSound/Enterprise.git`

This repository makes use of Git submodules, so don't forget to use `--recurse-submodules`.  If you don't do this, you'll be missing files and Enterprise will fail to build.

To build a solution for Visual Studio 2019, run "pm_Win32_VS2019.bat" in the _Scripts folder.  You should then be able to compile immediately.
* Older versions of Visual Studio have not been tested and are presently unsupported.
* macOS and Xcode support have not yet been added.

## Current State of Development
So far, I have developed Enterprise's framework as well as two of its core systems. The framework contains an entry point, a logging system, basic game window handling, custom assertions and exceptions, and a place to add systems. The development of the core systems is ongoing.

Enterprise is being developed with portability in mind, though for now, it is only for Windows.

### The Plan

With the framework largely done, Enterprise's core systems are the focus of development. The systems are the following:

1. Time (done)
2. Events (done (mostly))
3. File (up next)
4. Input
5. Graphics
6. Audio
7. ECS
8. StateStack (probably going to be renamed a few times)

### Extras
Once the core systems are finished, then technically, Enterprise will be, too. However, there are some goals I'd like to eventually meet, even if they aren't a high priority:

* An installation package generator
* Viewers/editors for asset creation
* Support for macOS and development in Xcode
* Support for web players
* A networking system
* Online store integration (Steam, etc.)

## Libraries Used
* Premake: <https://premake.github.io/>
* Spdlog: <https://github.com/gabime/spdlog>
* Cxx-prettyprint: <http://louisdx.github.io/cxx-prettyprint/>