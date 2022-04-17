![Enterprise Engine](readme_logo.png#gh-light-mode-only)
![Enterprise Engine](readme_logo_dark.png#gh-dark-mode-only)

# Enterprise Engine
Enterprise is a "pet project" game engine I'm developing in my free time. It is written in C++17 
for Mac and Windows computers.

To build the engine, use Git to clone this repository, then use CMake in the root directory:

```{.bat}
git clone https://github.com/theOtherMichael/Enterprise.git
cd Enterprise
cmake -S . -B build --DCMAKE_BUILD_TYPE=Release
cmake --build build
```

CMake will automatically clone all of the required submodules. More information about building 
and installing Enterprise will be provided in the documentation. Instructions for generating 
the Enterprise documentation are provided later in this Readme.

## Prerequisites
To build Enterprise, the following must be installed on your system:
* OS: At least **Windows 10** or **macOS 10.15**
* **[CMake](https://cmake.org/download/) version 3.22 or later**
* (Windows developers) **[Visual Studio](https://visualstudio.microsoft.com/downloads/)** or 
**[Build Tools for Visual Studio](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022)** 
with the "Desktop development with C++" workload
* (Windows developers) **[The Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk/)** 
(also available in the Visual Studio Installer under "optional components")
* (Mac developers) **[Xcode](https://apps.apple.com/us/app/xcode/id497799835?mt=12)** or 
**[Command Line Tools for Xcode](https://developer.apple.com/download/all/?q=xcode)** (requires 
free Apple developer account)

## Build Instructions

Enterprise uses CMake to generate its build system. If you have a preferred CMake generator, you 
should be able to use it, so long as:

* Your CMake version is at least **3.22**.
* Your compiler is either **MSVC** or **Clang**.
* You target **64-bit architectures** only.

If you are unfamiliar with CMake, I recommend installing 
[Visual Studio Code](https://code.visualstudio.com) with the 
[C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack). 
With them installed, building and debugging is very simple:

1. Open the Enterprise directory in Visual Studio Code.
2. Open the Command Palette (<kbd>Ctrl</kbd>/<kbd>&#8984;</kbd>+<kbd>Shift</kbd>+<kbd>P</kbd>) and 
select **CMake: Configure**.
3. In the Run panel (<kbd>Ctrl</kbd>/<kbd>&#8984;</kbd>+<kbd>Shift</kbd>+<kbd>D</kbd>), select a 
debugger configuration from the dropdown.
4. Press <kbd>F7</kbd> to build the engine, or <kbd>F5</kbd> to start debugging.

More information about building and debugging Enterprise will be available in the Enterprise 
documentation.

## Documentation Generation
If you have [Doxygen](https://www.doxygen.nl/download.html) installed on your computer, you can use 
it to generate HTML documentation for the engine. To do this, simply run the *gendocs_win.bat* or 
*gendocs_mac.command* script. The generated files will appear in the *docs* folder. Open 
*home.html* to access the documentation.

Due to Apple's security measures, Mac users won't initially be able to run *gendocs_mac.command* 
by double-clicking it. To make it double-clickable, run `chmod u+x gendocs_mac.command` in 
Terminal. Alternatively, you can run the script from Terminal directly by using `sh 
gendocs_mac.command`.

## Current State of Development

At this point, all of Enterprise's basic systems have basic functionality. Up next:
* The Enterprise editor application
* Game project generation
* Scene tools
* Play in editor
* Audio system
* Hot code reloading

## APIs Used
### Windows
* Win32
* Raw Input (for low-level keyboard/mouse input)
* XInput (for gamepad input)
* OpenGL

### macOS
* Cocoa
* Game Controller Framework (for gamepad input)
* OpenGL

## Libraries Used
* Spdlog: <https://github.com/gabime/spdlog>
* Cxx-prettyprint: <http://louisdx.github.io/cxx-prettyprint/>
* Compile-Time SpookyHash: <https://github.com/theOtherMichael/CTSpookyHash>
* Hot Constants: <https://github.com/theOtherMichael/HotConsts>
* Glad: <https://glad.dav1d.de/>
* stb_image: <https://github.com/nothings/stb>
* yaml-cpp: <https://github.com/jbeder/yaml-cpp>
* OpenGL Mathematics (GLM): <https://github.com/g-truc/glm>
* Multi-channel signed distance field atlas generator (msdf-atlas-gen): <https://github.com/Chlumsky/msdf-atlas-gen>

## Other Resources Used
* Roboto (Apache License, Version 2.0): <https://fonts.google.com/specimen/Roboto>
