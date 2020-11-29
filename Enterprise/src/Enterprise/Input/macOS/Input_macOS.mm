#include "EP_PCH.h"
#if defined(__APPLE__) && defined(__MACH__)

#include "Enterprise/Input/Input.h"
#include "Enterprise/Input/InputEvents.h"

using Enterprise::Input;
using Enterprise::Events;

static NSMutableArray *gamepads; // The list of connected GCControllers.  Ordered oldest to newest.
static std::vector<size_t> activeGamepadIDs; // The list of active GamepadIDs.  Ordered oldest to newest.
static std::vector<bool> isGamepadIDActive; // Stores GamepadID active status by index.

void Input::PlatformInit()
{
	// Init gamepads array
	gamepads = [[NSMutableArray alloc] init];

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
	std::vector<size_t>::iterator GPIDit = activeGamepadIDs.begin();
	for (GCController *gamepad in gamepads)
	{
		// Buttons

		Input::gpBuffer[*GPIDit].buttons[currentBuffer] =
			gamepad.extendedGamepad.dpad.up.pressed * BIT(uint16_t(ControlID::GP_Dpad_Up))
			| gamepad.extendedGamepad.dpad.down.pressed * BIT(uint16_t(ControlID::GP_Dpad_Down))
			| gamepad.extendedGamepad.dpad.left.pressed * BIT(uint16_t(ControlID::GP_Dpad_Left))
			| gamepad.extendedGamepad.dpad.right.pressed * BIT(uint16_t(ControlID::GP_Dpad_Right))
			| gamepad.extendedGamepad.buttonMenu.pressed * BIT(uint16_t(ControlID::GP_Menu))
			// TODO: Handle gamepads without the options button.
			| gamepad.extendedGamepad.buttonOptions.pressed * BIT(uint16_t(ControlID::GP_Options))
			// TODO: Handle gamepads without clickable thumbsticks.
			| gamepad.extendedGamepad.leftThumbstickButton.pressed * BIT(uint16_t(ControlID::GP_LStick_Click))
			| gamepad.extendedGamepad.rightThumbstickButton.pressed * BIT(uint16_t(ControlID::GP_RStick_Click))
			| gamepad.extendedGamepad.leftShoulder.pressed * BIT(uint16_t(ControlID::GP_LShoulder))
			| gamepad.extendedGamepad.rightShoulder.pressed * BIT(uint16_t(ControlID::GP_RShoulder))
			| gamepad.extendedGamepad.buttonA.pressed * BIT(uint16_t(ControlID::GP_FaceButton_Down))
			| gamepad.extendedGamepad.buttonB.pressed * BIT(uint16_t(ControlID::GP_FaceButton_Right))
			| gamepad.extendedGamepad.buttonX.pressed * BIT(uint16_t(ControlID::GP_FaceButton_Left))
			| gamepad.extendedGamepad.buttonY.pressed * BIT(uint16_t(ControlID::GP_FaceButton_Up));

		// Triggers

		// The triggers are already normalized by the Game Controller framework.
		Input::gpBuffer[*GPIDit].axes[currentBuffer]
			[size_t(ControlID::GP_LTrigger) - size_t(ControlID::_EndOfGPButtons) - 1] = gamepad.extendedGamepad.leftTrigger.value;
		Input::gpBuffer[*GPIDit].axes[currentBuffer]
			[size_t(ControlID::GP_RTrigger) - size_t(ControlID::_EndOfGPButtons) - 1] = gamepad.extendedGamepad.rightTrigger.value;

		// Sticks
		{
			// While the Game Controller framework does normalize the thumbsticks, they are normalized per axis.
			// We have to renormalize them in 2D to ensure that the overall magnitude is never greater than 1.
			// Dead zones are already handled in 2D by the framework.

			// Left Stick

			float thumbX = gamepad.extendedGamepad.leftThumbstick.xAxis.value;
			float thumbY = gamepad.extendedGamepad.leftThumbstick.yAxis.value;
			float magnitude = sqrt(thumbX * thumbX + thumbY * thumbY);

			if (magnitude > 0.0)
			{
				// Get the unit vector for the stick
				thumbX /= magnitude;
				thumbY /= magnitude;

				// Clamp the 2D magnitude to normalled range
				if (magnitude > 1.0) magnitude = 1.0f;


				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_LStick_NormalX) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbX;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_LStick_NormalY) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbY;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_LStick_X) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbX * magnitude;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_LStick_Y) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbY * magnitude;
			}
			else
			{
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_LStick_NormalX) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_LStick_NormalY) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_LStick_X) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_LStick_Y) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
			}

			// Right Stick

			thumbX = gamepad.extendedGamepad.rightThumbstick.xAxis.value;
			thumbY = gamepad.extendedGamepad.rightThumbstick.yAxis.value;
			magnitude = sqrt(thumbX * thumbX + thumbY * thumbY);

			if (magnitude > 0.0)
			{
				// Get the unit vector for the stick
				thumbX /= magnitude;
				thumbY /= magnitude;

				// Clamp the 2D magnitude to normalled range
				if (magnitude > 1.0) magnitude = 1.0f;


				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_RStick_NormalX) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbX;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_RStick_NormalY) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbY;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_RStick_X) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbX * magnitude;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_RStick_Y) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbY * magnitude;
			}
			else
			{
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_RStick_NormalX) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_RStick_NormalY) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_RStick_X) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_RStick_Y) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
			}
		}

		// Step the GPID iterator for the next connected gamepad
		++GPIDit;
	}
}

#endif // macOS
