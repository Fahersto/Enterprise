#include "EP_PCH.h"
#include "HashNames.h"
#include "Core.h"

static std::unordered_map<HashName, std::string> HashNameTable;

HashName HN(std::string stringname)
{
	HashName hash = SpookyHash::Hash64(stringname.c_str(), stringname.size(), 0);

	#ifndef EP_CONFIG_DIST
	std::unordered_map<HashName, std::string>::iterator it = HashNameTable.find(hash);
	if (it == HashNameTable.end())
	{
		HashNameTable[hash] = stringname;
	}
	else
	{
		// TODO: Display the string name in the assertion failure, once EP_ASSERTF is fixed.
		EP_ASSERTF(stringname == it->second, "Hash collision detected!");
	}
	#endif

	return hash;
}
