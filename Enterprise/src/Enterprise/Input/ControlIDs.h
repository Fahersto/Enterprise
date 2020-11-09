#pragma once

namespace Enterprise
{

enum class ControlID
{
	// Gamepads

	GP_Dpad_Up,
	GP_Dpad_Down,
	GP_Dpad_Left,
	GP_Dpad_Right,
	GP_Menu,
	GP_Options,
	GP_LStick_Click,
	GP_RStick_Click,
	GP_LShoulder,
	GP_RShoulder,
	GP_FaceButton_Down,
	GP_FaceButton_Right,
	GP_FaceButton_Left,
	GP_FaceButton_Up,

	_EndOfGPButtons,

	GP_LTrigger,
	GP_RTrigger,
	GP_LStick_X,
	GP_LStick_Y,
	GP_LStick_NormalX,
	GP_LStick_NormalY,
	GP_RStick_X,
	GP_RStick_Y,
	GP_RStick_NormalX,
	GP_RStick_NormalY,

	_EndOfGPAxes,


	// Keyboard

	KB_Backspace,
	KB_Tab,
	//KB_Clr, // Is this a thing on Mac keyboards?
	KB_Enter,
	//KB_Shift,
	//KB_Ctrl,
	//KB_Alt,
	KB_PauseBreak,
	KB_CapsLock,
	KB_Esc,
	KB_Space,
	KB_PageUp,
	KB_PageDown,
	KB_End,
	KB_Home,
	KB_Left,
	KB_Up,
	KB_Right,
	KB_Down,
	KB_PrintScreen,
	KB_Insert,
	KB_Delete,

	KB_0,
	KB_1,
	KB_2,
	KB_3,
	KB_4,
	KB_5,
	KB_6,
	KB_7,
	KB_8,
	KB_9,
	KB_A,
	KB_B,
	KB_C,
	KB_D,
	KB_E,
	KB_F,
	KB_G,
	KB_H,
	KB_I,
	KB_J,
	KB_K,
	KB_L,
	KB_M,
	KB_N,
	KB_O,
	KB_P,
	KB_Q,
	KB_R,
	KB_S,
	KB_T,
	KB_U,
	KB_V,
	KB_W,
	KB_X,
	KB_Y,
	KB_Z,

	KB_Super_Left,
	KB_Super_Right,
	//KB_Menu,

	KB_Numpad_0,
	KB_Numpad_1,
	KB_Numpad_2,
	KB_Numpad_3,
	KB_Numpad_4,
	KB_Numpad_5,
	KB_Numpad_6,
	KB_Numpad_7,
	KB_Numpad_8,
	KB_Numpad_9,
	KB_Numpad_Muliply,
	KB_Numpad_Add,
	//KB_Numpad_Separator, //Enter key?
	KB_Numpad_Subtract,
	KB_Numpad_Decimal,
	KB_Numpad_Divide,
	KB_F1,
	KB_F2,
	KB_F3,
	KB_F4,
	KB_F5,
	KB_F6,
	KB_F7,
	KB_F8,
	KB_F9,
	KB_F10,
	KB_F11,
	KB_F12, // Are the other function keys common on mac?

	KB_NumLock,
	KB_ScrollLock,

	KB_Shift_Left,
	KB_Shift_Right,
	KB_Ctrl_Left,
	KB_Ctrl_Right,
	KB_Menu_Left,
	KB_Menu_Right,


	// Mouse
	Mouse_Button_1, // Left
	Mouse_Button_2, // Right
	Mouse_Button_3, // Middle
	Mouse_Button_4,
	Mouse_Button_5,

	_EndOfKBMouseButtons,

	Mouse_Wheel_Y,
	Mouse_Wheel_X,

	Mouse_Pointer_X,
	Mouse_Pointer_Y,
	Mouse_Delta_X,
	Mouse_Delta_Y,
	
	_EndOfIDs
};

}
