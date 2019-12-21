#pragma once

/* Console.h
	This is the platform-agnostic representation of the console.  It is defined in the Platform folder.
	The Application class creates a console when in Debug builds.  It is platform-agnostic to allow the client
	to work with it.
*/

namespace Enterprise {
	class Console 
	{
	public:
		Console();
		virtual ~Console();
	};
}