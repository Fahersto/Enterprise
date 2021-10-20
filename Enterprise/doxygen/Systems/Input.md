@page Input Input (System)
@tableofcontents

Enterprise::Input is %Enterprise's input system.  It is one of the three "interface" systems.  It offers:

* Support for popular gamepads
* %Input mapping via configuration files
* Simultaneous input stream management (local multiplayer)
* Chain-of-responsibility input response patterns

# Supported Devices

At the time of this writing, %Enterprise supports the following input devices:

* Keyboard and mouse
* Xbox 360, Xbox One, and Xbox Series X/S controllers
* PlayStation 4 and PlayStation 5 controllers (macOS only for now)
* MFi (Made for iOS) controllers (macOS only)

On Windows, gamepad support is provided through the XInput API, and on macOS, it is provided through the %Game Controller framework.  While Xbox controllers are presently the only supported gamepad type on Windows, %Enterprise will eventually support non-Xbox gamepads as well, including PlayStation controllers, through the DirectInput API.  Only gamepads that support the [GCExtendedGamepad](https://developer.apple.com/documentation/gamecontroller/gcextendedgamepad) profile are supported on Mac, though that is subject to change as well.

# Input Contexts

%Enterprise games respond to player input using dynamically loaded input mapping schemes called **input contexts**.  %Input contexts automatically handle converting raw input data into game-specific values and offer a natural layer of abstraction between game code and input hardware.

Contexts are defined in configuration files that ship with the final game.  This allows input bindings to be tweaked by the player, either through in-game menus or by editing the files directly.  It also makes game input easier to program, because the lowest-level details are not part of your game code.

@anchor Actions_and_Axes
## Actions and Axes

Contexts are composed of *action mappings* and *axis mappings*.  The terms "action" and "axis" have very specific meanings here:

* **Axis (plural: Axes)**: A single `float` value calculated from one or more hardware controls.
* **Action**: An event triggered by a mapped hardware control.

Axes are generally used for real-time controls, and actions for discrete controls.  For example, a "jump" button would be best implemented as an *action* because it triggers a one-off event in the game.  A real-time camera, however, is best controlled with *axes*, because they allow player input to be integrated over time.

Buttons and keys have a natural association with actions, and likewise, triggers and thumbsticks have a natural relationship with axes.  However, it is important to understand that every control (except for mouse movement) can be mapped to both actions and axes. The following table covers the behavior of each control when mapped to an action or axis:

| Control Type  | Down Action           | Up Action           | Raw Axis Value                           |
| ------------- | --------------------- | ------------------- | ---------------------------------------- |
| Keyboard Keys | Key *pressed*         | Key *released*      | `1.0f` while held, `0.0f` while released |
| Mouse Buttons | Button *clicked*      | Button *released*   | `1.0f` while held, `0.0f` while released |
| Mouse Scroll Wheel | Wheel clicked *down* one increment | Wheel clicked *up* one increment | `1.0f` * number of clicked increments (scrolling down produces negative values) |
| Mouse delta axes | *N/A*              | *N/A*               | Delta movement this frame (not normalized) |
| Mouse pointer axes | *N/A*            | *N/A*               | Cursor coordinates in pixel space        |
| Gamepad buttons | Button *pressed*    | Button *released*   | `1.0f` while held, `0.0f` while released |
| Gamepad analog triggers | Value crosses threshold moving in the *positive* direction | Value crosses threshold moving in the *negative* direction | Value in [`0.0f`, `1.0f`], where `0.0f` is released and `1.0f` is fully depressed |
| Gamepad thumbstick axes | Value crosses threshold moving *away* from `0.0f` | Value crosses threshold moving *towards* `0.0f` | Value in [`-1.0f`, `1.0f`], where `0.0f` is centered and `-1.0f` and `1.0f` are axis extremes |

@note Because different mice have different DPI values, all games that use mouse deltas should include a "mouse sensitivity" setting via an in-game menu.  Mouse deltas are not normalized.

@anchor Defining_a_Context
## Defining a Context

To define an input context, simply create a section for it in an INI file.  The section must be created in the "Input" section group and contain at least one `ActionMapping` or `AxisMapping` key.

The following example shows a context definition for basic platformer input:

```ini
; Input.ini
[Input.PlayerMovement]
AxisMapping   = (name="MoveX", control=GP_LStick_X,        scale=1.0)
AxisMapping   = (name="MoveY", control=GP_LStick_Y,        scale=1.0)
ActionMapping = (name="Jump",  control=GP_FaceButton_Down, dir=down)

AxisMapping   = (name="MoveX", control=KB_A,               scale=-1.0)
AxisMapping   = (name="MoveX", control=KB_D,               scale= 1.0)
AxisMapping   = (name="MoveY", control=KB_S,               scale=-1.0)
AxisMapping   = (name="MoveY", control=KB_W,               scale= 1.0)
ActionMapping = (name="Jump",  control=KB_Space,           dir=down)
```

When the above file is loaded with Input::LoadContextFile(), a context called "PlayerMovement" is registered with the input system.  Once it’s loaded, it can be added to the context stack using Input::BindContext(), then used to drive gameplay.

@see @ref File

### Axis Mappings

%Input axes are defined by one or more `AxisMapping` keys.  This key takes a *dictionary* value, which is a comma-separated list of key-value pairs.

```ini
AxisMapping=(name="MoveX", control=GP_LStick_X, scale=1.0)
```

`AxisMapping` takes three sub-values:

* **name**: The name of the axis.
* **control**: The name of the physical control being mapped.  This must be the name of an Enterprise::ControlID.
* **scale (optional)**: A factor for pre-scaling the raw control value.  Defaults to `1.0f` if unspecified.

If you map multiple controls to the same axis, the final axis value will be *the sum of all mappings*.  This is useful in scenarios such as *ASWD* movement, where the 'A' and 'D' keys and 'S' and 'W' keys "push" the same axes in opposite directions.

@note Be sure to clamp your axis values before using them.  If a player modifies their INI files, they can potentially cheat in your game by modifying the scale of their controls!

### Action Mappings

Actions are defined with `ActionMapping` keys.  As with `AxisMapping`, `ActionMapping` accepts dictionary values.

```ini
ActionMapping=(name="FireWeapon", control=GP_RTrigger, dir=down, threshold=0.4)
```

`ActionMapping` takes four sub-values:

* **name**: The name of the action.
* **control**: The name of the physical control being mapped.  This must be the name of an Enterprise::ControlID.
* **dir**: The direction of control use that should trigger the action.  Valid values are "up" and "down".
* **threshold (optional)**: For non-binary controls, the value at which the action is triggered.  Defaults to `0.5f` if unspecified.

`dir` specifies the direction of the action mapping.  In general, a "down" action triggers when a control is *pressed*, and an "up" action triggers upon its *release*.  Refer to the table in [Actions and Axes](@ref Actions_and_Axes) for a full list of how each control maps to actions.

When defining actions for non-binary controls, `threshold` determines the point at which the action is triggered.  For example, if you map a gamepad trigger to a "down" action with a `0.25f` threshold, the action will fire when the trigger is squeezed a quarter of the way.  Adjusting the threshold value is a good way to adjust the "feel" of certain controls.

# Controllers and Streams

## Controllers

In %Enterprise, input devices are referred to as **controllers**.  Controllers can either be gamepads or the keyboard and mouse.

%Enterprise enumerates all connected controllers using an integer `typedef`, Input::ControllerID (not to be confused with Enterprise::ControlID).  ControllerIDs are assigned according to the following rules:

* ControllerID 0 represents a *null* connection.
* ControllerID 1 represents the keyboard and mouse.
* At application launch, gamepads are assigned ControllerIDs *2-N* in an undefined order.
* If a gamepad is connected while the game is running, it is assigned the smallest unused ControllerID.
* If a gamepad is disconnected while the game is running, its ControllerID is marked as unused.

Because gamepads are assigned new ControllerIDs as they get connected, and their ControllerIDs are recycled when disconnected, a single gamepad might have multiple IDs over the course of a single gameplay session.  This makes ControllerIDs unsuitable for direct input tracking, as the same player may change ControllerIDs without even changing controllers.

As a developer, you will generally only use ControllerIDs when handling controller assignment, which is covered in the next section.

## Streams

To track player input, %Enterprise games dynamically assign controllers to one or more **input streams**.  %Input streams serve as "channels" for player input: each input context is bound to a specific stream, and only responds to input from that stream.  By working with streams instead of controllers, it is possible to respond to player input in a multiplayer game without knowing *which* controller each player is using.

Streams are identified with another integer `typedef`, Input::StreamID.  As with ControllerIDs, StreamID 0 represents a *null* stream; but unlike ControllerIDs, StreamIDs are not prescribed by the engine.  Instead, developers themselves choose the number and function of the streams.  For example, StreamID 1 might be used for Player One, StreamID 2 for Player Two, etc.

## Controller Assignment

By default, the keyboard and mouse (ControllerID 1) is assigned to StreamID 1, but no other assignments occur automatically.  To support gamepads in your game, you must assign them to an input stream.

The best place to do this is in a "ControllerWake" event handler.  *ControllerWake* events occur in three situations:

1. A controller was already connected at application launch.
2. A controller was just connected to the computer.
3. %Input was detected on an unassigned controller.

*ControllerWake* events contain the `ControllerID` of the controller that just woke up.  This can be passed into Input::BindController() to assign the controller to a stream.

In the following example, the most recently used/connected controller is automatically assigned to StreamID 1:

```cpp
using Enterprise::Events;
using Enterprise::Input;

bool handleControllerWake(Events::Event& e)
{
    Input::ControllerID awokenController = Events::Unpack<Input::ControllerID>(e);
    Input::BindController(awokenController, 1);

    return false;
}

void Init()
{
	Events::Subscribe(HN("ControllerWake"), handleControllerWake);
}
```

The above code is sufficient for most single-player games. However, multiplayer games require more robust stream management.  An example of an advanced approach is provided in [Example: \"Press to Join\" Controller Assignment](@ref Example_Press_to_Join_Controller_Assignment).

@note *ControllerWake* events do not have a default handler.  It is safe to return `true` in your event handler function to block *ControllerWake* events.

@see @ref Events

## Disconnected Controllers

When a gamepad is disconnected from the player's computer, it is automatically unassigned from its input stream, if it was assigned to one.  However, it is often useful to implement a custom response to the disconnection, such as automatically pausing the game or presenting an in-game pop-up.

To respond to disconnections, subscribe to "ControllerDisconnect" events.  *ControllerDisconnect* passes the `StreamID` that was just cut off, allowing you to identify which player is impacted.

```cpp
using Enterprise::Events;
using Enterprise::Input;

bool onControllerDisconnect(Events::Event& e)
{
    Input::StreamID disconnectedStream = Events::Unpack<Input::StreamID>(e);

    PauseGame();
    EP_TRACE("Controller disconnected for StreamID {}", disconnectedStream);
    
    return false;
}

void Init()
{
    Events::Subscribe(HN("ControllerDisconnect"), onControllerDisconnect);
}
```

Only controllers assigned to a stream generate *ControllerDisconnect* events.

@note Just as with *ControllerWake*, *ControllerDisconnect* is not handled by the engine's low-level systems.  It is safe to block the event.

@see @ref Events

@anchor Example_Press_to_Join_Controller_Assignment
## Example: "Press to Join" Controller Assignment

@note This example contains advanced input response code and discussion of the context stack.  Check out [Reading Input](@ref Reading_Input) before studying this section.

It is common for multiplayer games to provide each player with their own input stream.  However, robust controller assignment systems require more complex arrangements.

For example, consider a “Press \[button\] to join” system.  In %Enterprise, this can be implemented by dedicating a stream to handling unassigned controllers.  The following code sample shows how that works:

```cpp
using Enterprise::Events;
using Enterprise::Input;

Input::ControllerID lastWokenController = 0;
constexpr Input::StreamID watcherStream = 5;
Input::ContextHandle watcherContext;

bool hasPlayerJoined[4] = { 0 };

void Init()
{
    // Automatically assign every unassigned controller to watcherStream
    Events::Subscribe(HN("ControllerWake"),
                      [](Events::Event& e)
                      {
                          lastWokenController = Events::Unpack<Input::ControllerID>(e);
                          Input::BindController(lastWokenController, watcherStream);
                          return true;
                      });

    // Track when a player's controller is disconnected
    Events::Subscribe(HN("ControllerDisconnect"),
                      [](Events::Event& e)
                      {
                          Input::StreamID disconnectedStream = Events::Unpack<StreamID>(e);
                          if (disconnectedStream < watcherStream)
                          {
                              hasPlayerJoined[disconnectedStream - 1] = false;
                          }
                          return true;
                      });
    
    // Bind an input context for handling "press to join" controls
    watcherContext = Input::BindContext(HN("PressToJoin"), watcherStream);

    // Make the "Join" action assign the controller to the next player's input stream
    Input::BindAction(watcherContext, HN("Join"),
                      []()
                      {
                          for (int i = 0; i < 4; i++)
                          {
                              if (!hasPlayerJoined[i])
                              {
                                  Input::BindController(lastWokenController, i + 1);
                                  hasPlayerJoined[i] = true;

                                  // Tell the game a controller has been assigned to player 'i'

                                  break;
                              }
                          }
                      });
}
```

When a player presses the "join" button on an unassigned gamepad in the above example, the following sequence of events occurs:

1. A *ControllerWake* event is triggered for the gamepad.
2. The *ControllerWake* handler assigns the gamepad to `watcherStream`.
3. The context stack is serviced, and the "PressToJoin" context (which is watching `watcherStream`) checks the newly assigned gamepad for the "Join" button press.
4. The *Join* action is triggered, and in the callback, the gamepad is reassigned to one of the dedicated "player" input streams.

This technique allows `watcherStream` to technically watch for input from all unassigned controllers at once, even though it is only ever assigned one controller at a time.  However, it suffers from a notable drawback: if multiple unassigned controllers are being operated simultaneously, it is possible to completely miss a "join" button press on one or more of them.  This is because only the last gamepad assigned to `watcherStream` will be checked for a "join" button press when the context stack is serviced, and multiple gamepads can generate wake events in the same frame.  In other words, a gamepad can be assigned to `watcherStream`, then immediately be unassigned from it before the "join" button is checked.

All of that being said, the chances of this actually negatively impacting the player's experience is likely to be very low.  It is possible to develop more robust handling methods that will resolve the "wake collision" problem described above, should it be necessary in your game.

@anchor Reading_Input
# Reading Input

Once you have defined a context in an INI file and set up controller assignment, you are ready to start reading input.  There are four steps to it:

1. Load the context's configuration file using Input::LoadContextFile().
2. Add an instance of the context to the context stack using Input::BindContext().
3. Bind action callbacks using Input::BindAction().
4. Poll axis values using Input::GetAxis().

## Loading Contexts

The first step to using an input context is to load its definition into the **context registry**.  The context registry is a dictionary of context definitions: it describes how contexts are supposed to behave when they are bound to an input stream.  Context definitions are loaded using Input::LoadContextFile().

Input::LoadContextFile() works by parsing an INI file for sections in the *%Input* section group.  A context definition is added to the registry for every subsection, each subsection name becoming the name of a context.  Refer back to [Defining a Context](@ref Defining_a_Context) for a more detailed explanation of how contexts are defined in INI files.

LoadContextFile() is usually only invoked once per file, often during application launch.  In a lot of games, context definitions only need to be loaded once: after they’re in the registry, Input::BindContext() can use them until program termination.  However, it is sometimes useful to reload context definitions at runtime.  To do this, simply run Input::LoadContextFile() on the file again.

@remarks After the @ref Console is finished, reloading context definitions from their files will likely become a built-in command.

## Binding Contexts

Once the context definition is loaded into the context registry, you can bind it with Input::BindContext().  This adds a copy of the context to the **context stack**, which is a layered list of the game's currently active contexts. Throughout a game's lifetime, contexts are added to and removed from the stack, dynamically forming a chain-of-responsibility pattern for player input.

Every frame, the input system gathers raw input from the operating system and uses it to service each context in the stack.  During this process, control values are scaled and mapped to axes, and actions are triggered if their conditions are met.  However, contexts can “block” lower contexts from receiving input if they want to.  If Input::BindContext() is passed a non-zero `blockingLevel`, it creates a **blocking context**, which prevents lower contexts from receiving input.  Any controls blocked by a blocking context do not trigger actions or update axes in lower contexts.

A context can be at one of three blocking levels:

* *Not blocking* (default, `blockingLevel = 0`): This context does not block any input.
* *Blocks mapped controls* (`blockingLevel = 1`): The specific controls mapped by this context are blocked from use in lower contexts.
* *Blocks all input* (`blockingLevel = 2`): For all lower contexts, actions do not trigger, and axes evaluate to `0.0f`.

This arrangement allows contexts to override each other.  For example, if an in-game menu’s input context is blocking, it supplants normal gameplay controls, preventing the player from accidentally moving their character while operating the menu.  Effective use of blocking contexts can make complex input scenarios easier to program.

Bound contexts only respond to input from their associated input streams, assigned by Input::BindContext() using the `stream` parameter.  A few notes about this:

* A bound context will respond to input from whatever controller is currently associated with its input stream.  If you bind a different controller to the stream, the context will automatically start responding to the new controller.
* You cannot bind a context to more than one stream.  For multiplayer games, this means you need to bind contexts for each player individually.
* A blocking context only blocks controls for its associated stream.

BindContext() returns a handle to the created instance of the context.  With it, you can register action callbacks, poll axes, and remove the context from the context stack.  Contexts can be removed from the stack using Input::PopContext().

## Binding Actions

After binding the context, you can use it to respond to player input.  Actions are responded to through callbacks, which are registered using Input::BindAction():

```cpp
using Enterprise::Input;

Input::ContextHandle context;
void onJump(){ player.Jump(); }

void Init()
{
    context = Input::BindContext(HN("PlayerMovement"), 1);
    Input::BindAction(context, HN("Jump"), onJump);
}
```

The callback must be of type Input::ActionCallbackPtr, meaning it takes no parameters and returns `void`.  Lambdas can be used as action callbacks as well, provided that they are captureless:

```cpp
Input::BindAction(context, HN("Jump"), [](){ player.Jump(); });
```

## Reading Axes

Unlike actions, input axes are polled.  To read one, call Input::GetAxis().

```cpp
using Enterprise::Input;

Input::ContextHandle context;

void Init()
{
    context = Input::BindContext(HN("VehicleMovement"), 1);
}

void Update()
{
    float gasPedal = Input::GetAxis(context, HN("Accelerate"));
}
```

## Example

The following code snippet shows the entire workflow of loading, binding, and reading input from an input context:

```cpp
using Enterprise::Input;

Input::ContextHandle context;
void handleJump(){ player.Jump(); }

void Init()
{
    // 1. Load the context's configuration file
    Input::LoadContextFile("CONTENT/Input.ini");

    // 2. Bind an instance of the "PlayerMovement" context for StreamID 1
    context = Input::BindContext(HN("PlayerMovement"), 1);
    
    // 3. Bind the "Jump" action to handleJump()
    Input::BindAction(context, HN("Jump"), handleJump);
}

void Update()
{
    // 4. Poll axes
    player.pos.x += Input::GetAxis(context, HN("MoveX"));
    player.pox.y += Input::GetAxis(context, HN("MoveY"));
}
```
