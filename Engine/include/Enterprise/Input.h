#pragma once
#include <forward_list>
#include <glm/glm.hpp>
#include "Enterprise/Core.h"
#include "Enterprise/Events.h"
#include "Enterprise/Input/ControlIDs.h"

namespace Enterprise
{

/// The Enterprise input system.
/// @see @ref Input
class Input
{
public:
	/// The maximum number of layers allowed in the input context stack.
	static constexpr int MaxInputContextLayers = 10;

	/// Identifier for a connected input controller.
	/// @note `ControllerID(0)` is considered "null".
	typedef unsigned int ControllerID;
	/// Identifier for an input stream.
	/// @note `StreamID(0)` is considered "null".
	typedef unsigned int StreamID;

	/// Bind a controller to an input stream.
	/// @param controller The controller to bind.
	/// @param stream The input stream to bind to.
	/// @note ControllerID 1 (keyboard and mouse) is automatically bound to StreamID 1 at launch.
	EP_API static void BindController(ControllerID controller, StreamID stream);


	/// Load input context definitions from an INI file.
	/// @param path Virtual path to the INI file to load.
	/// @param fallbackPath Virtual path to the fallback INI file.
	EP_API static void LoadContextFile(std::string path, std::string fallbackPath = "");

	/// Handle to a bound input context.
	typedef void* ContextHandle;

	/// Add a new input context to the context stack.
	/// @param contextName The HashName of the context to bind.
	/// @param stream The input stream to associate with this context.
	/// @param blockingLevel The blocking behavior of this context.
	/// 0=No blocking 1=Block only bound controls 2=Block all input
	/// @param layer The layer to insert the new context into.
	/// @return The handle of the newly bound context.
	EP_API static ContextHandle BindContext(HashName contextName,
									 StreamID stream,
									 int blockingLevel = 0,
									 unsigned int layer = 0);

	/// Pop an active input context from the context stack.
	/// @param context Handle of the context.
	EP_API static void PopContext(ContextHandle context);


	/// A pointer to an input action callback function.
	typedef void(*ActionCallbackPtr)();

	/// Bind an input action to a callback.
	/// @param context The handle of the context containing the action. 
	/// @param actionName The HashName of the action.
	/// @param callback A pointer to the callback function.
	EP_API static void BindAction(ContextHandle context,
						   HashName actionName,
						   ActionCallbackPtr callback);

	/// Get the current value of an axis from an input context.
	/// @param context The context handle.
	/// @param axisName The HashName of the axis.
	/// @return The current value of the axis.
	EP_API static float GetAxis(ContextHandle context, HashName axisName);

	/// Get the current position of the mouse cursor.
	/// @return The current position of the cursor in pixel coordinates.
	/// @remarks Pixel space is relative to the lower left corner of the window or screen, where that corner is (0, 0)
	/// and the upper-right corner is (Window::GetWidth(), Window::GetHeight()).
	EP_API static inline glm::vec2 GetMousePos() { return cursorPos; };

private:
	friend class Application;

	// Raw input stuff

	struct KBMouseBuffer 
	{
		static_assert((size_t(ControlID::_EndOfKBMouseButtons) - size_t(ControlID::_EndOfGPAxes) - 1) <= 128,
					  "There are more KBMouseButtons than will fit in two 64-bit bit fields.");
		static constexpr size_t numOfMouseAxes = size_t(ControlID::_EndOfIDs) - size_t(ControlID::_EndOfKBMouseButtons) - 1;

		bool isAssigned = false; // can only be assigned to StreamID(1), so we don't track the assigned stream.

		uint64_t keys[2][2] = { 0 }; // bit fields.  Dim 1 = buffer, Dim 2 = low/high order word
		float axes[2][numOfMouseAxes] = { 0 };

		uint64_t keys_blockstatus[2] = { 0 }; // bit field
		bool axes_blockstatus[numOfMouseAxes] = { 0 };

	};
	struct GamePadBuffer 
	{
		static_assert(size_t(ControlID::_EndOfGPButtons) <= 16,
					  "There are more GPButtons than will fit in a 16-bit bit field.");
		static constexpr size_t numOfGamepadAxes = size_t(ControlID::_EndOfGPAxes) - size_t(ControlID::_EndOfGPButtons) - 1;

		StreamID assignedStream = 0;

		uint16_t buttons[2] = { 0 }; // Bit field
		float axes[2][numOfGamepadAxes] = { 0 };

		uint16_t buttons_blockstatus = { 0 }; // Bit field
		bool axes_blockstatus[numOfGamepadAxes] = { 0 };
	};

	static KBMouseBuffer kbmBuffer;
	static glm::vec2 cursorPos;
	static std::vector<GamePadBuffer> gpBuffer;
	static bool currentBuffer; // used as index to double-buffered raw input

	// Context stuff

	struct ActionMapping
	{
		HashName name;
		ControlID control;
		float threshold;
		bool isDownAction;
	};
	struct AxisMapping
	{
		HashName name;
		ControlID control;
		float scale;
	};
	struct Context
	{
		std::vector<ActionMapping> actions;		// set by LoadContextFile()
		std::vector<AxisMapping> axes;			// set by LoadContextFile()

		int blockingLevel = 0;					// set by BindContext()
		StreamID stream = 0;					// set by BindContext()
	};

	static std::map<HashName, Context> contextRegistry;
	static std::forward_list<Context> contextStack[MaxInputContextLayers];
	static std::map<ContextHandle, int> layerOfContext; // The layer number of a given context.
	static std::map<ContextHandle, std::map<HashName, float>> axisValues[2];


	static bool HandlePlatformEvents(Events::Event& e);
	static void GetRawInput();
	static void CheckForControllerWake();
	static StreamID UnbindController(ControllerID controller);
	static void ProcessContext(Context& context);

	static void Init();
	static void PlatformInit();
	static void Update();
};

}
