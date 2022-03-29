@page Architecture Engine Overview
@tableofcontents

At its most basic level, %Enterprise is a [software framework](https://en.wikipedia.org/wiki/Software_framework): a library containing a partially constructed application.  The library itself implements the application entry point, main loop, and fundamental game systems.  It is statically linked to each game project.

To create games with the library, %Enterprise developers implement "code hook" functions that are intentionally left undefined by the framework.  For example, Game::Init() is intentionally left undefined so it can serve as the developer's "entry point".  Several [core engine constants](@ref Enterprise::Constants) are left to be defined by the developer as well.

It should not be necessary to modify the engine source code when developing a game: most low-level modifications should be possible from the game project itself. While %Enterprise will likely never be distributed as a pre-compiled library, maintaining a line between game code and engine code is still a useful organizational tool that should increase engine portability and reduce game code complexity.

# Application Entry Point

The entry point for an %Enterprise application (its *main* function) is defined in EntryPoint_Win32.cpp for Windows builds and EntryPoint_macOS.mm for Mac builds.  It is only responsible for three tasks:

1. Create and store a map of the command line options provided at launch
2. Instantiate the Enterprise::Application singleton
3. Pump OS messages until Application::Run() returns `false`

All higher-level tasks are encapsulated in Enterprise::Application, which is platform-generic and manages the application lifecycle.  A few notes about this arrangement:

* The game runs on the same thread as the message pump: specifically, messages are pumped between calls to Application::Run().
* The game window is not created here.  Instead, it is created later on by the [Graphics](@ref Graphics) system in Graphics::Init().
* The macOS entry point additionally sets up the dock menu and menu bar.

To understand the execution flow of an %Enterprise application, study the [core calls](@ref Core_Calls) in Application.cpp.

## Menu Bar and Dock Menu (macOS)

Mac apps feature two interaction methods not commonly utilized in games: the menu bar and the dock menu.  %Enterprise automatically sets up a minimal menu bar and dock menu in the MacAppDelegate.  There are presently no plans to allow additional macOS menu options in %Enterprise.

The only option in the menu bar of an %Enterprise game on macOS is "Quit", located in the main menu (the one named after your game).  This triggers a "QuitRequested" event when selected, which can be handled by a custom event handler.  The "Quit" option in the dock behaves identically.  The remaining dock menu options, such as "Keep in Dock", "Show/Hide", and "Show All Windows" behave identically to other apps and exhibit no custom behaviors.

@note If a game does not handle "QuitRequested" events, Application::Quit() is invoked automatically upon selecting a Quit option.  To override this behavior, implement a custom event handler for *QuitRequested* that returns `true`.

# Game Code Entry Point

@note This section covers topics related to [StateManager](@ref StateManager).  At the time of this writing, StateManager is an undeveloped system, so its exact functionality is subject to change.  The section below is written based on the anticipated use of this system.

As a game developer, your “entry point” in an %Enterprise project is Game::Init().  This is the first function to be called after %Enterprise sets up its systems, and it is a suitable place to set up your own systems. Here is a short list of some of the things commonly done in Game::Init():

* Display a splash screen
* Initialize custom game systems, such as health systems, physics, or AI
* Load any art assets needed at launch
* Set up the initial game state (usually a loading screen)

After Game::Init() returns, the application will enter the main loop, and the initial game state will run.  After this, the flow of the game is handled by [StateManager](@ref StateManager), which will transition from state to state until the player chooses to quit.

A game's execution flow is a naturally complex topic.  To learn more about control flow in %Enterprise, see @ref StateManager.
