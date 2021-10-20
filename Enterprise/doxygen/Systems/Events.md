@page Events Events (System)
@tableofcontents

Enterprise::Events is %Enterprise's event/messaging system.  It is one of the three "foundation" systems.  It provides the following:

* Classes for generically packaging data
* A global event dispatch mechanism
* Callback registration for event handling

%Events allows unrelated systems to easily communicate.  While %Enterprise is not event-driven *per se*, events are critical to the extensibility of the framework.  Without an events system, adding new game systems could quickly lead to dependency nightmares.

There are two aspects to %Events: *dispatching* events and *subscribing to* them.  New events are broadcast using Events::Dispatch().  To respond to them, systems register callback functions with Events::Subscribe().

@anchor The_Event_Class
# The Event Class

Events in %Enterprise are embodied by the Events::Event class.  This is a polymorphic class with a templated subclass, Events::DataEvent.  The basic Event class contains little more than a HashName identifier.  DataEvent, on the other hand, is templated, and contains an additional generic data member.

Callback functions registered with Events::Subscribe() take an Event object reference as a parameter.  This allows the callback to do two things:

* Differentiate between event types by comparing the event’s HashName.
* Extract data from the Event by casting it to the right kind of DataEvent.

To make it easier to extract data from events, developers are encouraged to use the helper function Events::Unpack().  Under the hood, this function casts the Event to the appropriate DataEvent type, then returns its value.

```cpp
bool exampleCallback(Events::Event& e)
{
    int value = Events::Unpack<int>(e);
}
```

A few notes on this:

* Events::Unpack() requires that you already know the type of data contained in the event.
* If you try to extract the wrong data type, an [assertion](@ref Assertions) will trigger.

Event types should only ever be associated with one kind of data payload.  If systems disagree on the type of data to unpack from an event type, then the game will crash.

# Dispatching Events

To dispatch an event, use Events::Dispatch().  There are three versions of it:

* **Events::Dispatch(HashName)**: Dispatches a basic Event.
* **Events::Dispatch(HashName, T)**: Dispatches a DataEvent.
* **Events::Dispatch(Event&)**: Dispatches a pre-made Event or DataEvent.

You will use the first two of these in most situations.  Simply provide the event name and, if you wish, a data payload, and the function will create the Event object and invoke subscribed callbacks.  In some cases, however, you may wish to allocate the Event object yourself.  In such situations, use Events::Dispatch(Event&) to dispatch your pre-made Event.

Here are some examples of each of these in use:

```cpp
using Enterprise::Events;

// 1. Simple event dispatch

Events::Dispatch(HN("Example Event"));

// 2. Dispatch with a data payload

int payload = 5;
Events::Dispatch(HN("Example DataEvent"), payload);

// 3. Pre-made event dispatch

Events::DataEvent<std::tuple<int, float, std::string>> e = 
{
    HN("Pre-made DataEvent"),
    { 1, 2.0f, "three" } 
};
Events::Dispatch(e);
```

# Subscribing to Events

To respond to events, you must create and register a callback function.  This is done with Events::Subscribe():

```cpp
Events::Subscribe(HN("EventType"), callbackName);
```

In order to be used as a callback, a function must be of type Events::EventCallbackPtr, meaning it must have the following signature:

```cpp
bool callbackName(Events::Event&);
```

Callback functions are very simple: they receive an Events::Event reference as a parameter and return a `bool`.  The parameter provides [information about the event](@ref The_Event_Class), and the return value indicates whether the event handler is *blocking*.

A blocking callback prevents lower-priority callbacks from being called.  Normally, callbacks are invoked in last in, first out order: that is, the most recently registered callback is invoked first, and the oldest registered callback is invoked last.  If a callback returns `true`, however, then the event will stop propagating, and lower-priority callbacks will not be invoked.  This allows the event handlers to form a chain-of-responsibility pattern for events.

For an example of event blocking, consider the close button ("X") on the game window.  Clicking it generates a "QuitRequested" event, which by default invokes Application::Quit(), terminating the program.  Developers can subvert this behavior by handling the "QuitRequested" event themselves and returning "true" in the custom callback.

## Lambdas as Event Handlers

It is possible to use lambdas as event callbacks, as long as they are captureless and have signatures matching Events::EventCallbackPtr.  This can consolidate your code considerably in some situations.

```cpp
using Enterprise::Events;

void Init()
{
    Events::Subscribe(HN("ExampleEvent"),
                      [](Events::Event& e)
                      {
                          // Handle event here
                          return false;
                      });
}
```

# Example: Kill Streak Tracking

Let's look at an example.  Say your game is about fighting off waves of robots, and you want to implement a kill-streak tracker.  An events-based solution might look like the following: 

1. Register a callback for "EnemyDefeated" events in your point system's Init() call.
```cpp
// PointsSystem.cpp
using Enterprise::Events;

bool onDefeatedEnemy(Events::Event& e); // callback

PointsSystem::Init()
{
    Events::Subscribe(HN("EnemyDefeated"), onDefeatedEnemy);
    // ...
}

// ...
```
2. In your game's health system, dispatch an "EnemyDefeated" event each time a robot is destroyed.
```cpp
// HealthSystem.cpp
using Enterprise::Events;

// ...

if (robot.health == 0)
{
    robot.kill();
    Events::Dispatch(HN("EnemyDefeated"));
}
```
3. Back in the points system, increment a kill streak counter when "EnemyDefeated" events occur back-to-back.
```cpp
// PointsSystem.cpp
using Enterprise::Events;
using Enterprise::Time;

static float lastDefeatTime = 0.0f;
static int currentStreak = 0;

// ...

bool onDefeatedEnemy(Events::Event& e)
{
    EP_ASSERT_SLOW(e.Type() == HN("EnemyDefeated"));

    float currentTime = Time::RunningTime();
    if (currentTime - lastDefeatTime < 1.0f)
    {
        currentStreak++;
    }
    else
    {
        currentStreak = 1;
    }

    lastDefeatTime = currentTime;
    return false;
}

// ...

```

Without introducing any hard dependencies, the health and points systems now work together to produce a relatively complex gameplay feature.  And once they are implemented, "EnemyDefeated" events can be used by any future system.  For example, a tutorial system could use them to track player progress -- and this would require no further edits to the health system.

It is easy to see how using events can make writing advanced gameplay code very simple.  Due to their far-reaching utility and low overhead, developers are encouraged to use events in all aspects of game development.
