#include "EP_PCH.h"
#if defined(__APPLE__) && defined(__MACH__)

#include "Enterprise/Input/Input.h"
#include "Enterprise/Input/InputEvents.h"

using Enterprise::Input;
using Enterprise::Events;

static NSMutableArray<GCController *> *gamepads; // The list of connected GCControllers.  Ordered oldest to newest.
static std::vector<size_t> activeGamepadIDs; // The list of active GamepadIDs.  Ordered oldest to newest.
static std::vector<bool> isGamepadIDActive; // Stores GamepadID active status by index.

void Input::PlatformInit()
{
	// New gamepad connections
	[[NSNotificationCenter defaultCenter] addObserverForName:GCControllerDidConnectNotification
													  object:nil
													   queue:[NSOperationQueue currentQueue]
												  usingBlock:^(NSNotification* Notification)
	 {
		// We only support the extended gamepad profile.  TODO: Should we support the others?
		if ([Notification.object extendedGamepad] != nil)
		{
			// Identify the next available GamePadID.
			auto GPIDIt = std::find(isGamepadIDActive.begin(), isGamepadIDActive.end(), false);
			size_t newGPID = GPIDIt - isGamepadIDActive.begin();

			// Store the GCController reference and its GamepadID.
			[gamepads addObject:Notification.object];
			activeGamepadIDs.push_back(newGPID);

			// Mark the GamepadID as taken, allocating engine resources if needed.
			if (GPIDIt == isGamepadIDActive.end())
			{
				isGamepadIDActive.push_back(true);
				Input::gpBuffer.emplace_back();
			}
			else
			{
				isGamepadIDActive[newGPID] = true;
				(*GPIDIt) = true;
			}

			// Notify the game code that the controller was connected.
			Events::Dispatch(EventTypes::ControllerWake, newGPID + 1);

			EP_INFO("Gamepad connected.  ControllerID: {}", newGPID + 1);
		}
	 }];

	// Gamepad disconnections
	[[NSNotificationCenter defaultCenter] addObserverForName:GCControllerDidDisconnectNotification
													  object:nil
													   queue:[NSOperationQueue currentQueue]
												  usingBlock:^(NSNotification* Notification)
	{
		NSUInteger disconnectedReferenceIndex = [gamepads indexOfObject:Notification.object];
		if (disconnectedReferenceIndex != NSNotFound)
		{
			// Get the GamepadID of the disconnected controller.
			size_t disconnectedGPID = activeGamepadIDs[disconnectedReferenceIndex];

			// Release the GamepadID in the engine.
			Input::gpBuffer[disconnectedGPID] = GamePadBuffer(); // Clear buffer for future connections
			PlayerID disconnectedPlayerID = Input::UnassignController(disconnectedGPID + 1);

			// Notify the game code of the disconnection.
			if (disconnectedPlayerID != EP_PLAYERID_NULL)
			{
				Events::Dispatch(EventTypes::ControllerDisconnect, disconnectedPlayerID);
				EP_INFO("Gamepad disconnected.  PlayerID: {}", disconnectedPlayerID);
			}
			else
			{
				// ControllerDisconnect events only fire for gamepads assigned to a player.
				EP_INFO("Gamepad disconnected.  PlayerID: unassigned");
			}

			// Delete the gamepad reference and clear its ID.
			[gamepads removeObjectAtIndex:disconnectedReferenceIndex];
			activeGamepadIDs.erase(activeGamepadIDs.begin() + disconnectedReferenceIndex);
			isGamepadIDActive[disconnectedGPID] = false;
		}
		else //if notification.object is an extended gamepad
		{
			EP_ERROR("A gamepad was disconnected that Enterprise was unaware of.");
			EP_ASSERT_NOENTRY();
		}
	}];

	// Initial gamepad registration
	for (GCController* connectedCont in [GCController controllers])
	{
		// We only support the extended gamepad profile. TODO: Should we support the others?
		if (connectedCont.extendedGamepad != nil)
		{
			activeGamepadIDs.push_back(gamepads.count);
			[gamepads addObject:connectedCont];

			isGamepadIDActive.push_back(true);
			Input::gpBuffer.emplace_back();
		}
	}
}

// Somehow, when a PlayerID is assigned a Controller ID, that has to cause the associated controller object's
// PlayerIndex property to be set.  Likewise, we need to unset it when the controller is unassigned.
// Perhaps an event would do?

void Input::GetRawInput()
{
}

#endif // macOS
