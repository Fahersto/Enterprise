@page Assertions %Enterprise Assertions
@tableofcontents

%Enterprise offers a suite of assertion macros useful for game development.  This page covers their use.

# Non-Static Assertions

There are three categories of %Enterprise assertions: *assert*, *verify*, and *control flow*.  There are multiple versions of each kind of assertion to cover a wide variety of situations.  By learning a few key terms, you will be able to quickly identify the correct macro for any specific use case.

## Assert vs. Verify

Most assertions in %Enterprise are from the *assert* or *verify* families, which begin with `EP_ASSERT` or `EP_VERIFY`.  In **Debug** and **Dev** builds, these macros are identical: they test a condition, then halt execution if the test fails.  In **Release** builds, however, the macros expand differently:

* *Assert* macros are deleted from **Release** builds entirely.  The code wrapped by them only executes in **Debug** and **Dev** builds.
* In **Release** builds, *verify* macros expand into just the test expression.  While the expression is no longer tested for validity, it is still compiled and runs in the shipping game.

*Verify* is particularly useful for testing the return values of important functions.  However, it is a best practice to use *assert* macros when possible, and to only use *verify* when the test expression may have side effects.  Incorrect use of *verify* can lead to performance loss in the shipping game.

@warning Using *assert* on an expression with side effects can lead to hard-to-identify bugs that only occur in **Release** builds.  Be especially careful to use *verify* when the expression may have side effects.

@see EP_ASSERT()
@see EP_VERIFY()

### EQ and NEQ

When building in **Release** configuration, you may encounter compiler warnings when using EP_VERIFY() to check for equality or inequality.  For example, `EP_VERIFY(someFunction() == someValue)` will generate an "equality comparison result unused" warning.

While this may seem unexpected, these warnings are actually correct.  This is because *all* of the code wrapped by EP_VERIFY() is left behind in **Release** builds.  With the assertion mechanism missing, equality checks are left unused, so the warning appears.

```cpp
[SomeFile.cpp]
EP_VERIFY(someFunction() == someValue);

[Release build preprocessor output]
someFunction() == someValue; // warning: unused equality comparison!
```

To avoid this situation, use EP_VERIFY_EQ() to check for equality and EP_VERIFY_NEQ() to check for inequality.  These macros are functionally identical to EP_VERIFY(), but they strip the right-hand side of the comparison in **Release** builds, preventing compiler warnings.

```cpp
[SomeFile.cpp]
EP_VERIFY_EQ(someFunction(), someValue);

[Release build preprocessor output]
someFunction(); // all good!
```

@warning Be careful that the right-hand side of the comparison has no side effects.  Bugs that arise from the removal of assertion code in **Release** builds can be hard to identify.

@see EP_VERIFY_EQ()
@see EP_VERIFY_NEQ()

@anchor Slow_Assertions
### Slow Assertions

Adding assertions to frequently executed code paths, such as Update() and Draw(), can negatively affect game performance during development.  For these situations, %Enterprise offers "slow" versions of each assertion macro.

Slow assertions are identical to their non-slow counterparts, except that they are also disabled in **Dev** builds.  This allows you to catch problems quickly while adding new features in **Debug**, but still test the game at representative speeds in **Dev** builds.

@see EP_ASSERT_SLOW()
@see EP_VERIFY_SLOW()
@see EP_VERIFY_EQ_SLOW()
@see EP_VERIFY_NEQ_SLOW()

### Custom Error Messages

If you use an assertion macro with an "f" suffix, such as EP_ASSERTF() or EP_VERIFYF(), you can pass formatted strings to be used as custom error messages.  This allows you to display or log important contextual information about an assertion failure.

@note These macros accept formatted strings in the same style as the console logger.  For more information on console logging, check out the documentation for the [developer's console](@ref Console).

@see EP_ASSERTF()
@see EP_ASSERTF_SLOW()
@see EP_VERIFYF()
@see EP_VERIFYF_SLOW()
@see EP_VERIFYF_EQ()
@see EP_VERIFYF_EQ_SLOW()
@see EP_VERIFYF_NEQ()
@see EP_VERIFYF_NEQ_SLOW()

## Control Flow Assertions

In addition to *assert* and *verify* macros, %Enterprise offers *control flow* assertion macros: EP_ASSERT_NOENTRY() and EP_ASSERT_NOREENTRY().  These can help you catch unexpected execution paths.

* If EP_ASSERT_NOENTRY() is ever invoked during program execution, an assertion failure will occur.  Place this assertion type in execution paths your program should never reach, such as invalid `switch` cases.
* EP_ASSERT_NOREENTRY() will trigger after it's been invoked more than once.  Use this macro to ensure a section of code only executes once.

@see EP_ASSERT_NOENTRY()
@see EP_ASSERT_NOREENTRY()

## Assertion Code Blocks

Some assertion scenarios require additional code to set up.  In situations where you need to prepare data for use in an assertion, use EP_ASSERT_CODE() or EP_ASSERT_CODE_SLOW() to wrap your setup code.  These wrappers will strip the code block out of builds where your assertion is disabled.

@note Both macros wrap your code block in a do-while structure, allowing them to be safely inlined.

@warning Be careful that none of the code in an assertion code block has side effects.  If the code in an assertion code block impacts program behavior, hard-to-spot issues may occur when the block is removed from your **Release** builds.

@see EP_ASSERT_CODE()
@see EP_ASSERT_CODE_SLOW()

# Static Assertions

%Enterprise does not offer a custom macro for compile-time assertions.  Instead, developers are encouraged to use C++11's [static_assert()](https://en.cppreference.com/w/cpp/language/static_assert).  Thanks to its portability and clear error messages, it is considered suitable for use in %Enterprise.

@note Because %Enterprise is built in C++17, static_assert() can be used with or without the `message` parameter.

@see [static_assert declaration (cppreference.com)](https://en.cppreference.com/w/cpp/language/static_assert)
