@page Architecture Engine Overview
@tableofcontents

At its most basic level, %Enterprise is a [software framework](https://en.wikipedia.org/wiki/Software_framework): a library containing a partially constructed application.  The library itself implements the application entry point, main loop, and fundamental game systems.  It is statically linked to each game project.

To create games with the library, %Enterprise developers implement "code hook" functions that are intentionally left undefined by the framework.  For example, game developers provide their own implementations of Enterprise::Game::Init(), which serves as the game's "entry point".  Several [core engine constants](@ref Enterprise::Constants) are defined this way as well.

It should not be necessary to modify the engine source code when developing a game: most low-level modifications should be possible from the game project itself. While %Enterprise will likely never be distributed as a pre-compiled library, maintaining a line between game code and engine code is still a useful organizational tool that should increase engine portability and reduce game code complexity.

# Application Entry Point

The entry point for an %Enterprise application (its `main` function) is defined in EntryPoint_Win32.cpp for Windows builds and EntryPoint_macOS.mm for Mac builds.  It is very barebones, and is responsible for only three tasks:

1. Create and store a map of the command line options provided at launch
2. Instantiate the Enterprise::Application singleton
3. Pump OS messages until Application::Run() returns `false`

All higher-level tasks are encapsulated in Enterprise::Application, which is platform-generic.  A few notes about this arrangement:

* The game runs on the same thread as the message pump: specifically, messages are pumped between calls to Application::Run().
* The game window is not created here.  Instead, it is created later on by the [Graphics](@ref Graphics) system in Graphics::Init().
* Mac builds have a slightly more complicated entry point because the app menus and dock commands must be set up immediately.  Otherwise, the macOS entry point is very similar to the Windows entry point.

The entire application lifecycle is encapsulated in Enterprise::Application: it sets up the engine, updates all of the systems, then cleans everything up.  Study Application.cpp to gain a clearer understanding of how the runtime works at a low level.

# Game Code Entry Point

@note This section covers topics related to [StateManager](@ref StateManager).  At the time of this writing, StateManager is an undeveloped system, so its exact functionality is subject to change.  The section below is written based on the anticipated use of this system.

As a game developer, your “entry point” in an Enterprise project is [Game::Init()](@ref Enterprise::Game::Init()).  This is the first function to be called after %Enterprise sets up its systems, and it is a suitable place to set up your own systems. Here is a short list of some of the things commonly done in Game::Init():

* Display a splash screen
* Initialize custom game systems, such as health systems, physics, or AI
* Load any art assets needed at launch
* Set up the initial game state (usually a loading screen)

After Game::Init() returns, the application will enter the main loop, at which point the flow of the game is dictated by [StateManager](@ref StateManager).  The exact details of this process go beyond the scope of this introduction, but suffice it to say that the game will go from state to state until the player chooses to quit.  After that, Game::Cleanup() is invoked, and then it's lights out.

A game's execution flow is a naturally complex topic.  To learn more about control flow in %Enterprise, see @ref StateManager.
