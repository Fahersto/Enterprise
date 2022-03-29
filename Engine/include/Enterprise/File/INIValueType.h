#pragma once
#include <sstream>
#include <variant>
#include "Enterprise/Core.h"

/// An enumeration of the data types which can be serialized to an INI file.
enum class INIDataType
{
	Char,
	Short, UShort,
	Int, UInt,
	Long, ULong,
	LongLong, ULongLong,
	Float, Double, LDouble,
	Bool,
	String,
	Ptr
};

/// A union type which can store any data type serializable to INI files.
/// Values of this type can safely be used as if they were the underlying type.
struct inivalue_t
{
	/// The underlying data store.
	std::variant<
		char, 
		short, unsigned short,
		int, unsigned int,
		long, unsigned long,
		long long, unsigned long long,
		float, double, long double,
		bool,
		std::string, const char*,
		const void*, nullptr_t> value;

	static_assert(std::is_same_v<std::variant_alternative_t<0, decltype(inivalue_t::value)>, char>);
	static_assert(std::is_same_v<std::variant_alternative_t<1, decltype(inivalue_t::value)>, short>);
	static_assert(std::is_same_v<std::variant_alternative_t<2, decltype(inivalue_t::value)>, unsigned short>);
	static_assert(std::is_same_v<std::variant_alternative_t<3, decltype(inivalue_t::value)>, int>);
	static_assert(std::is_same_v<std::variant_alternative_t<4, decltype(inivalue_t::value)>, unsigned int>);
	static_assert(std::is_same_v<std::variant_alternative_t<5, decltype(inivalue_t::value)>, long>);
	static_assert(std::is_same_v<std::variant_alternative_t<6, decltype(inivalue_t::value)>, unsigned long>);
	static_assert(std::is_same_v<std::variant_alternative_t<7, decltype(inivalue_t::value)>, long long>);
	static_assert(std::is_same_v<std::variant_alternative_t<8, decltype(inivalue_t::value)>, unsigned long long>);
	static_assert(std::is_same_v<std::variant_alternative_t<9, decltype(inivalue_t::value)>, float>);
	static_assert(std::is_same_v<std::variant_alternative_t<10, decltype(inivalue_t::value)>, double>);
	static_assert(std::is_same_v<std::variant_alternative_t<11, decltype(inivalue_t::value)>, long double>);
	static_assert(std::is_same_v<std::variant_alternative_t<12, decltype(inivalue_t::value)>, bool>);
	static_assert(std::is_same_v<std::variant_alternative_t<13, decltype(inivalue_t::value)>, std::string>);
	static_assert(std::is_same_v<std::variant_alternative_t<14, decltype(inivalue_t::value)>, const char*>);
	static_assert(std::is_same_v<std::variant_alternative_t<15, decltype(inivalue_t::value)>, const void*>);
	static_assert(std::is_same_v<std::variant_alternative_t<16, decltype(inivalue_t::value)>, nullptr_t>);

	/// Get the HashName of this value, if it is a string type.
	/// @return If @c value contains a string type, the associated HashName.  Otherwise, HN_NULL.
	inline HashName Hash()
	{
		if (value.index() == 13) { return HN(std::get<13>(value)); }
		else if (value.index() == 14) { return HN(std::get<14>(value)); }
		else { EP_WARN("inivalue_t::Hash(): Cannot hash a non-string type!"); return HN_NULL; }
	}

	inivalue_t() = default;
	inivalue_t(char v) : value(v) {}
	inivalue_t(short v) : value(v) {}
	inivalue_t(unsigned short v) : value(v) {}
	inivalue_t(int v) : value(v) {}
	inivalue_t(unsigned int v) : value(v) {}
	inivalue_t(long v) : value(v) {}
	inivalue_t(unsigned long v) : value(v) {}
	inivalue_t(long long v) : value(v) {}
	inivalue_t(unsigned long long v) : value(v) {}
	inivalue_t(float v) : value(v) {}
	inivalue_t(double v) : value(v) {}
	inivalue_t(long double v) : value(v) {}
	inivalue_t(bool v) : value(v) {}
	inivalue_t(std::string v) : value(v) {}
	inivalue_t(const char* v) : value(v) {}
	inivalue_t(const void* v) : value(v) {}
	inivalue_t(nullptr_t v) : value(v) {}

	inline operator char() const
	{
		switch (value.index())
		{
		case 0:  return (char)std::get<0>(value);  break; // char
		case 1:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'short' to 'char'!  "
				"Value: {}", std::get<1>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (char)std::get<1>(value);
			break; // short
		case 2:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'unsigned short' to 'char'!  "
				"Value: {}", std::get<2>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (char)std::get<2>(value);
			break; // unsigned short
		case 3:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'int' to 'char'!  "
				"Value: {}", std::get<3>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (char)std::get<3>(value);
			break; // int
		case 4:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'unsigned int' to 'char'!  "
				"Value: {}", std::get<4>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (char)std::get<4>(value);
			break; // unsigned int
		case 5:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'long' to 'char'!  "
				"Value: {}", std::get<5>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (char)std::get<5>(value);
			break; // long
		case 6:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'unsigned long' to 'char'!  "
				"Value: {}", std::get<6>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (char)std::get<6>(value);
			break; // unsigned long
		case 7:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'long long' to 'char'!  "
				"Value: {}", std::get<7>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (char)std::get<7>(value);
			break; // long long
		case 8:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'unsigned long long' to 'char'!  "
				"Value: {}", std::get<8>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (char)std::get<8>(value);
			break; // unsigned long long
		case 9:  return (char)std::get<9>(value);  break; // float
		case 10: return (char)std::get<10>(value); break; // double
		case 11: return (char)std::get<11>(value); break; // long double
		case 12: return (char)std::get<12>(value); break; // bool
		case 13: EP_WARN("inivalue_t: Cannot convert from 'std::string' to 'char'!"); return 0; break; // std::string
		case 14: EP_WARN("inivalue_t: Cannot convert from 'const char*' to 'char'!"); return 0; break; // c string
		case 15: EP_WARN("inivalue_t: Cannot convert from 'const void*' to 'char'!"); return 0; break; // ptr
		case 16: return 0; break; // nullptr
		default: EP_ASSERT_NOENTRY();
		}
		return 0;
	}

	inline operator short() const
	{
		switch (value.index())
		{
		case 0: return (short)std::get<0>(value); break; // char
		case 1: return (short)std::get<1>(value); break; // short
		case 2: return (short)std::get<2>(value); break; // unsigned short
		case 3:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'int' to 'short'!  "
				"Value: {}", std::get<3>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (short)std::get<3>(value);
			break; // int
		case 4:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'unsigned int' to 'short'!  Value: "
				"{}", std::get<4>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (short)std::get<4>(value);
			break; // unsigned int
		case 5:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'long' to 'short'!  "
				"Value: {}", std::get<5>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (short)std::get<5>(value);
			break; // long
		case 6:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'unsigned long' to 'short'!  "
				"Value: {}", std::get<6>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (short)std::get<6>(value);
			break; // unsigned long
		case 7:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'long long' to 'short'!  "
				"Value: {}", std::get<7>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (short)std::get<7>(value);
			break; // long long
		case 8:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'unsigned long long' to 'short'!  "
				"Value: {}", std::get<8>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (short)std::get<8>(value);
			break; // unsigned long long
		case 9: return (short)std::get<9>(value); break; // float
		case 10: return (short)std::get<10>(value); break; // double
		case 11: return (short)std::get<11>(value); break; // long double
		case 12: return (short)std::get<12>(value); break; // bool
		case 13: EP_WARN("inivalue_t: Cannot convert from 'std::string' to 'short'!");
			return 0; break; // std::string
		case 14: EP_WARN("inivalue_t: Cannot convert from 'const char*' to 'short'!");
			return 0; break; // c string
		case 15: EP_WARN("inivalue_t: Cannot convert from 'const void*' to 'short'!");
			return 0; break; // ptr
		case 16: return 0; break; // nullptr
		default: EP_ASSERT_NOENTRY();
		}
		return 0;
	}

	inline operator unsigned short() const
	{
		switch (value.index())
		{
		case 0: return (unsigned short)std::get<0>(value); break; // char
		case 1: return (unsigned short)std::get<1>(value); break; // short
		case 2: return (unsigned short)std::get<2>(value); break; // unsigned short
		case 3:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'int' to 'unsigned short'!  "
				"Value: {}", std::get<3>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (unsigned short)std::get<3>(value);
			break; // int
		case 4:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'unsigned int' to 'unsigned short'!  "
				"Value: {}", std::get<4>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (unsigned short)std::get<4>(value);
			break; // unsigned int
		case 5:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'long' to 'unsigned short'!  "
				"Value: {}", std::get<5>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (unsigned short)std::get<5>(value);
			break; // long
		case 6:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'unsigned long' to 'unsigned short'!  "
				"Value: {}", std::get<6>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (unsigned short)std::get<6>(value);
			break; // unsigned long
		case 7:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'long long' to 'unsigned short'!  "
				"Value: {}", std::get<7>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (unsigned short)std::get<7>(value);
			break; // long long
		case 8:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'unsigned long long' to 'unsigned short'!  "
				"Value: {}", std::get<8>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (unsigned short)std::get<8>(value);
			break; // unsigned long long
		case 9: return (unsigned short)std::get<9>(value); break; // float
		case 10: return (unsigned short)std::get<10>(value); break; // double
		case 11: return (unsigned short)std::get<11>(value); break; // long double
		case 12: return (unsigned short)std::get<12>(value); break; // bool
		case 13: EP_WARN("inivalue_t: Cannot convert from 'std::string' to 'unsigned short'!");
			return 0; break; // std::string
		case 14: EP_WARN("inivalue_t: Cannot convert from 'const char*' to 'unsigned short'!");
			return 0; break; // c string
		case 15: EP_WARN("inivalue_t: Cannot convert from 'const void*' to 'unsigned short'!");
			return 0; break; // ptr
		case 16: return 0; break; // nullptr
		default: EP_ASSERT_NOENTRY();
		}
		return 0;
	}

	inline operator int() const
	{
		switch (value.index())
		{
		case 0: return (int)std::get<0>(value); break; // char
		case 1: return (int)std::get<1>(value); break; // short
		case 2: return (int)std::get<2>(value); break; // unsigned short
		case 3: return (int)std::get<3>(value); break; // int
		case 4: return (int)std::get<4>(value); break; // unsigned int
		case 5:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'long' to 'int'!  "
				"Value: {}", std::get<5>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (int)std::get<5>(value);
			break; // long
		case 6:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'unsigned long' to 'int'!  "
				"Value: {}", std::get<6>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (int)std::get<6>(value);
			break; // unsigned long
		case 7:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'long long' to 'int'!  "
				"Value: {}", std::get<7>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (int)std::get<7>(value);
			break; // long long
		case 8:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'unsigned long long' to 'int'!  "
				"Value: {}", std::get<8>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (int)std::get<8>(value);
			break; // unsigned long long
		case 9: return (int)std::get<9>(value); break; // float
		case 10: return (int)std::get<10>(value); break; // double
		case 11: return (int)std::get<11>(value); break; // long double
		case 12: return (int)std::get<12>(value); break; // bool
		case 13: EP_WARN("inivalue_t: Cannot convert from 'std::string' to 'int'!");
			return 0; break; // std::string
		case 14: EP_WARN("inivalue_t: Cannot convert from 'const char*' to 'int'!");
			return 0; break; // c string
		case 15: EP_WARN("inivalue_t: Cannot convert from 'const void*' to 'int'!");
			return 0; break; // ptr
		case 16: return 0; break; // nullptr
		default: EP_ASSERT_NOENTRY();
		}
		return 0;
	}

	inline operator unsigned int() const
	{
		switch (value.index())
		{
		case 0: return (unsigned int)std::get<0>(value); break; // char
		case 1: return (unsigned int)std::get<1>(value); break; // short
		case 2: return (unsigned int)std::get<2>(value); break; // unsigned short
		case 3: return (unsigned int)std::get<3>(value); break; // int
		case 4: return (unsigned int)std::get<4>(value); break; // unsigned int
		case 5:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'long' to 'unsigned int'!  "
				"Value: {}", std::get<5>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (unsigned int)std::get<5>(value);
			break; // long
		case 6:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'unsigned long' to 'unsigned int'!  "
				"Value: {}", std::get<6>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (unsigned int)std::get<6>(value);
			break; // unsigned long
		case 7:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'long long' to 'unsigned int'!  "
				"Value: {}", std::get<7>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (unsigned int)std::get<7>(value);
			break; // long long
		case 8:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'unsigned long long' to 'unsigned int'!  "
				"Value: {}", std::get<8>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (unsigned int)std::get<8>(value);
			break; // unsigned long long
		case 9: return (unsigned int)std::get<9>(value); break; // float
		case 10: return (unsigned int)std::get<10>(value); break; // double
		case 11: return (unsigned int)std::get<11>(value); break; // long double
		case 12: return (unsigned int)std::get<12>(value); break; // bool
		case 13: EP_WARN("inivalue_t: Cannot convert from 'std::string' to 'unsigned int'!");
			return 0; break; // std::string
		case 14: EP_WARN("inivalue_t: Cannot convert from 'const char*' to 'unsigned int'!");
			return 0; break; // c string
		case 15: EP_WARN("inivalue_t: Cannot convert from 'const void*' to 'unsigned int'!");
			return 0; break; // ptr
		case 16: return 0; break; // nullptr
		default: EP_ASSERT_NOENTRY();
		}
		return 0;
	}

	inline operator long() const
	{
		switch (value.index())
		{
		case 0: return (long)std::get<0>(value); break; // char
		case 1: return (long)std::get<1>(value); break; // short
		case 2: return (long)std::get<2>(value); break; // unsigned short
		case 3: return (long)std::get<3>(value); break; // int
		case 4: return (long)std::get<4>(value); break; // unsigned int
		case 5: return (long)std::get<5>(value); break; // long
		case 6: return (long)std::get<6>(value); break; // unsigned long
		case 7:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'long long' to 'long'!  "
				"Value: {}", std::get<7>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (long)std::get<7>(value);
			break; // long long
		case 8:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'unsigned long long' to 'long'!  "
				"Value: {}", std::get<8>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (long)std::get<8>(value);
			break; // unsigned long long
		case 9: return (long)std::get<9>(value); break; // float
		case 10: return (long)std::get<10>(value); break; // double
		case 11: return (long)std::get<11>(value); break; // long double
		case 12: return (long)std::get<12>(value); break; // bool
		case 13: EP_WARN("inivalue_t: Cannot convert from 'std::string' to 'long'!");
			return 0; break; // std::string
		case 14: EP_WARN("inivalue_t: Cannot convert from 'const char*' to 'long'!");
			return 0; break; // c string
		case 15: EP_WARN("inivalue_t: Cannot convert from 'const void*' to 'long'!");
			return 0; break; // ptr
		case 16: return 0; break; // nullptr
		default: EP_ASSERT_NOENTRY();
		}
		return 0;
	}

	inline operator unsigned long() const
	{
		switch (value.index())
		{
		case 0: return (unsigned long)std::get<0>(value); break; // char
		case 1: return (unsigned long)std::get<1>(value); break; // short
		case 2: return (unsigned long)std::get<2>(value); break; // unsigned short
		case 3: return (unsigned long)std::get<3>(value); break; // int
		case 4: return (unsigned long)std::get<4>(value); break; // unsigned int
		case 5: return (unsigned long)std::get<5>(value); break; // long
		case 6: return (unsigned long)std::get<6>(value); break; // unsigned long
		case 7:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'long long' to 'unsigned long'!  "
				"Value: {}", std::get<7>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (unsigned long)std::get<7>(value);
			break; // long long
		case 8:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'unsigned long long' to 'unsigned long'!  "
				"Value: {}", std::get<8>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (unsigned long)std::get<8>(value);
			break; // unsigned long long
		case 9: return (unsigned long)std::get<9>(value); break; // float
		case 10: return (unsigned long)std::get<10>(value); break; // double
		case 11: return (unsigned long)std::get<11>(value); break; // long double
		case 12: return (unsigned long)std::get<12>(value); break; // bool
		case 13: EP_WARN("inivalue_t: Cannot convert from 'std::string' to 'unsigned long'!");
			return 0; break; // std::string
		case 14: EP_WARN("inivalue_t: Cannot convert from 'const char*' to 'unsigned long'!");
			return 0; break; // c string
		case 15: EP_WARN("inivalue_t: Cannot convert from 'const void*' to 'unsigned long'!");
			return 0; break; // ptr
		case 16: return 0; break; // nullptr
		default: EP_ASSERT_NOENTRY();
		}
		return 0;
	}

	inline operator long long() const
	{
		switch (value.index())
		{
		case 0: return (long long)std::get<0>(value); break; // char
		case 1: return (long long)std::get<1>(value); break; // short
		case 2: return (long long)std::get<2>(value); break; // unsigned short
		case 3: return (long long)std::get<3>(value); break; // int
		case 4: return (long long)std::get<4>(value); break; // unsigned int
		case 5: return (long long)std::get<5>(value); break; // long
		case 6: return (long long)std::get<6>(value); break; // unsigned long
		case 7: return (long long)std::get<7>(value); break; // long long
		case 8: return (long long)std::get<8>(value); break; // unsigned long long
		case 9: return (long long)std::get<9>(value); break; // float
		case 10: return (long long)std::get<10>(value); break; // double
		case 11: return (long long)std::get<11>(value); break; // long double
		case 12: return (long long)std::get<12>(value); break; // bool
		case 13: EP_WARN("inivalue_t: Cannot convert from 'std::string' to 'long long'!");
			return 0; break; // std::string
		case 14: EP_WARN("inivalue_t: Cannot convert from 'const char*' to 'long long'!");
			return 0; break; // c string
		case 15: EP_WARN("inivalue_t: Cannot convert from 'const void*' to 'long long'!");
			return 0; break; // ptr
		case 16: return 0; break; // nullptr
		default: EP_ASSERT_NOENTRY();
		}
		return 0;
	}

	inline operator unsigned long long() const
	{
		switch (value.index())
		{
		case 0: return (unsigned long long)std::get<0>(value); break; // char
		case 1: return (unsigned long long)std::get<1>(value); break; // short
		case 2: return (unsigned long long)std::get<2>(value); break; // unsigned short
		case 3: return (unsigned long long)std::get<3>(value); break; // int
		case 4: return (unsigned long long)std::get<4>(value); break; // unsigned int
		case 5: return (unsigned long long)std::get<5>(value); break; // long
		case 6: return (unsigned long long)std::get<6>(value); break; // unsigned long
		case 7: return (unsigned long long)std::get<7>(value); break; // long long
		case 8: return (unsigned long long)std::get<8>(value); break; // unsigned long long
		case 9: return (unsigned long long)std::get<9>(value); break; // float
		case 10: return (unsigned long long)std::get<10>(value); break; // double
		case 11: return (unsigned long long)std::get<11>(value); break; // long double
		case 12: return (unsigned long long)std::get<12>(value); break; // bool
		case 13: EP_WARN("inivalue_t: Cannot convert from 'std::string' to 'unsigned long long'!");
			return 0; break; // std::string
		case 14: EP_WARN("inivalue_t: Cannot convert from 'const char*' to 'unsigned long long'!");
			return 0; break; // c string
		case 15: EP_WARN("inivalue_t: Cannot convert from 'const void*' to 'unsigned long long'!");
			return 0; break; // ptr
		case 16: return 0; break; // nullptr
		default: EP_ASSERT_NOENTRY();
		}
		return 0;
	}

	inline operator float() const
	{
		switch (value.index())
		{
		case 0: return (float)std::get<0>(value); break; // char
		case 1: return (float)std::get<1>(value); break; // short
		case 2: return (float)std::get<2>(value); break; // unsigned short
		case 3: return (float)std::get<3>(value); break; // int
		case 4: return (float)std::get<4>(value); break; // unsigned int
		case 5: return (float)std::get<5>(value); break; // long
		case 6: return (float)std::get<6>(value); break; // unsigned long
		case 7: return (float)std::get<7>(value); break; // long long
		case 8: return (float)std::get<8>(value); break; // unsigned long long
		case 9: return (float)std::get<9>(value); break; // float
		case 10:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'double' to 'float'!  "
				"Value: {}", std::get<10>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (float)std::get<10>(value);
			break; // double
		case 11:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'long double' to 'float'!  "
				"Value: {}", std::get<11>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (float)std::get<11>(value);
			break; // long double
		case 12: return (float)std::get<12>(value); break; // bool
		case 13: EP_WARN("inivalue_t: Cannot convert from 'std::string' to 'float'!");
			return 0; break; // std::string
		case 14: EP_WARN("inivalue_t: Cannot convert from 'const char*' to 'float'!");
			return 0; break; // c string
		case 15: EP_WARN("inivalue_t: Cannot convert from 'const void*' to 'float'!");
			return 0; break; // ptr
		case 16: return 0; break; // nullptr
		default: EP_ASSERT_NOENTRY();
		}
		return 0;
	}

	inline operator double() const
	{
		switch (value.index())
		{
		case 0: return (double)std::get<0>(value); break; // char
		case 1: return (double)std::get<1>(value); break; // short
		case 2: return (double)std::get<2>(value); break; // unsigned short
		case 3: return (double)std::get<3>(value); break; // int
		case 4: return (double)std::get<4>(value); break; // unsigned int
		case 5: return (double)std::get<5>(value); break; // long
		case 6: return (double)std::get<6>(value); break; // unsigned long
		case 7: return (double)std::get<7>(value); break; // long long
		case 8: return (double)std::get<8>(value); break; // unsigned long long
		case 9: return (double)std::get<9>(value); break; // float
		case 10: return (double)std::get<10>(value); break; // double
		case 11:
			EP_WARN("inivalue_t: Potential narrowing conversion from type 'long double' to 'double'!  "
				"Value: {}", std::get<11>(value));
#ifdef EP_GUARDS_INIVALUET_NARROWING
			EP_DEBUGBREAK();
#endif
			return (double)std::get<11>(value);
			break; // long double
		case 12: return (double)std::get<12>(value); break; // bool
		case 13: EP_WARN("inivalue_t: Cannot convert from 'std::string' to 'double'!");
			return 0; break; // std::string
		case 14: EP_WARN("inivalue_t: Cannot convert from 'const char*' to 'double'!");
			return 0; break; // c string
		case 15: EP_WARN("inivalue_t: Cannot convert from 'const void*' to 'double'!");
			return 0; break; // ptr
		case 16: return 0; break; // nullptr
		default: EP_ASSERT_NOENTRY();
		}
		return 0;
	}

	inline operator long double() const
	{
		switch (value.index())
		{
		case 0: return (long double)std::get<0>(value); break; // char
		case 1: return (long double)std::get<1>(value); break; // short
		case 2: return (long double)std::get<2>(value); break; // unsigned short
		case 3: return (long double)std::get<3>(value); break; // int
		case 4: return (long double)std::get<4>(value); break; // unsigned int
		case 5: return (long double)std::get<5>(value); break; // long
		case 6: return (long double)std::get<6>(value); break; // unsigned long
		case 7: return (long double)std::get<7>(value); break; // long long
		case 8: return (long double)std::get<8>(value); break; // unsigned long long
		case 9: return (long double)std::get<9>(value); break; // float
		case 10: return (long double)std::get<10>(value); break; // double
		case 11: return (long double)std::get<11>(value); break; // long double
		case 12: return (long double)std::get<12>(value); break; // bool
		case 13: EP_WARN("inivalue_t: Cannot convert from 'std::string' to 'long double'!");
			return 0; break; // std::string
		case 14: EP_WARN("inivalue_t: Cannot convert from 'const char*' to 'long double'!");
			return 0; break; // c string
		case 15: EP_WARN("inivalue_t: Cannot convert from 'const void*' to 'long double'!");
			return 0; break; // ptr
		case 16: return 0; break; // nullptr
		default: EP_ASSERT_NOENTRY();
		}
		return 0;
	}

	inline operator bool() const
	{
		switch (value.index())
		{
		case 0: return (bool)std::get<0>(value); break; // char
		case 1: return (bool)std::get<1>(value); break; // short
		case 2: return (bool)std::get<2>(value); break; // unsigned short
		case 3: return (bool)std::get<3>(value); break; // int
		case 4: return (bool)std::get<4>(value); break; // unsigned int
		case 5: return (bool)std::get<5>(value); break; // long
		case 6: return (bool)std::get<6>(value); break; // unsigned long
		case 7: return (bool)std::get<7>(value); break; // long long
		case 8: return (bool)std::get<8>(value); break; // unsigned long long
		case 9: return (bool)std::get<9>(value); break; // float
		case 10: return (bool)std::get<10>(value); break; // double
		case 11: return (bool)std::get<11>(value); break; // long double
		case 12: return (bool)std::get<12>(value); break; // bool
		case 13: EP_WARN("inivalue_t: Cannot convert from 'std::string' to 'bool'!");
			return false; break; // std::string
		case 14: EP_WARN("inivalue_t: Cannot convert from 'const char*' to 'bool'!");
			return false; break; // c string
		case 15: EP_WARN("inivalue_t: Cannot convert from 'const void*' to 'bool'!");
			return false; break; // ptr
		case 16: return false; break; // nullptr
		default: EP_ASSERT_NOENTRY();
		}
		return 0;
	}

	inline operator std::string() const
	{
		std::stringstream ss;

		switch (value.index())
		{
		case 0: return std::string(1, std::get<0>(value)); break; // char
		case 1:  ss << std::get<1>(value); break; // short
		case 2:  ss << std::get<2>(value); break; // unsigned short
		case 3:  ss << std::get<3>(value); break; // int
		case 4:  ss << std::get<4>(value); break; // unsigned int
		case 5:  ss << std::get<5>(value); break; // long
		case 6:  ss << std::get<6>(value); break; // unsigned long
		case 7:  ss << std::get<7>(value); break; // long long
		case 8:  ss << std::get<8>(value); break; // unsigned long long
		case 9:  ss << std::get<9>(value); break; // float
		case 10: ss << std::get<10>(value); break; // double
		case 11: ss << std::get<11>(value); break; // long double
		case 12: ss << std::get<12>(value); break; // bool
		case 13: return std::get<13>(value); break; // std::string
		case 14: return std::string(std::get<14>(value)); break; // c string
		case 15: ss << std::get<15>(value); break; // ptr
		case 16: ss << std::get<16>(value); break; // nullptr
		default: EP_ASSERT_NOENTRY();
		}

		return ss.str();
	}

	inline operator const char* () const
	{
		if (value.index() == 14)
			return std::get<const char*>(value);
		else if (value.index() == 13)
			return std::get<std::string>(value).c_str();
		else
		{
			EP_WARN("inivalue_t: Cannot convert non-string types to 'const char*'!");
			return nullptr;
		}
	}

	inline operator const void* () const
	{
		if (value.index() == 8)
			return (const void*)std::get<unsigned long long>(value);
		if (value.index() == 15)
			return std::get<const void*>(value);
		else
		{
			EP_WARN("inivalue_t: Cannot convert non-pointer types to 'const void*'!");
			return nullptr;
		}
	}
	
	inline operator nullptr_t() const { return nullptr; }
};

inline std::ostream& operator<<(std::ostream& os, const inivalue_t& v)
{
	switch (v.value.index())
	{
	case 0: os << std::get<0>(v.value); break;
	case 1: os << std::get<1>(v.value); break;
	case 2: os << std::get<2>(v.value); break;
	case 3: os << std::get<3>(v.value); break;
	case 4: os << std::get<4>(v.value); break;
	case 5: os << std::get<5>(v.value); break;
	case 6: os << std::get<6>(v.value); break;
	case 7: os << std::get<7>(v.value); break;
	case 8: os << std::get<8>(v.value); break;
	case 9: os << std::get<9>(v.value); break;
	case 10: os << std::get<10>(v.value); break;
	case 11: os << std::get<11>(v.value); break;
	case 12: os << (std::get<bool>(v.value) ? "true" : "false"); break;
	case 13: os << std::get<13>(v.value); break;
	case 14: os << std::get<14>(v.value); break;
	case 15: os << std::get<15>(v.value); break;
	case 16: os << std::get<16>(v.value); break;
	}
	return os;
}
