@page Assertions %Enterprise Assertions
@tableofcontents

%Enterprise offers a suite of assertion macros useful for game development.  This page covers their purpose and use cases.

# Static Assertions

Static assertions are the only type of assertion for which %Enterprise does not offer a custom macro.  Instead, developers are encouraged to use C++11's [static_assert()](https://en.cppreference.com/w/cpp/language/static_assert) for compile-time checks.  Thanks to its portability and clear error messages, it is considered suitable for use in %Enterprise.

Visit [cppreference.com](https://en.cppreference.com/w/cpp/language/static_assert) for more information about static_assert().  Note that because %Enterprise is built in C++17, you may invoke static_assert() with or without the `message` parameter.

# Non-Static Assertions

At the time of this writing, %Enterprise provides a whopping 20 different kinds of asserts.  Thankfully, they boil down to just three types: *assert*, *verify*, and *control flow*.  By learning a few terms, you will be able to quickly identify which macro to use for any situation.

## Assert vs. Verify

Most assertions in %Enterprise begin with "EP_ASSERT" or "EP_VERIFY".  These macros differ based on how they handle expression evaluation in distribution builds:

* Expressions in *assert* macros are completely removed from **Dist** builds.  They will evaluate in **Debug** and **Dev** builds only.
* Expressions in *verify* macros will evaluate in all builds, including **Dist** builds, but validity is only tested in **Debug** and **Dev** builds.

*Verify* macros allow you to wrap critical functions in assertions without breaking anything in **Dist** builds. Use *verify* to check the return values of important function calls and *assert* to test expressions that can safely be deleted.

@see EP_ASSERT()
@see EP_VERIFY()

### Custom Error Messages

You can pass formatted strings to your assertions to be used as custom error messages by using a macro with an "f" suffix, such as EP_ASSERTF() or EP_VERIFYF().  This allows your assertions to provide additional context if they fail.

@note Formatted strings in assertion errors follow the same rules as formatted strings in console logging.  For more information on this, check out @ref Console.

@see @ref Console
@see EP_ASSERTF()
@see EP_VERIFYF()

### EQ and NEQ

When building in **Dist** configuration, you might encounter compiler warnings when using EP_VERIFY() to check for equality or inequality.  For example:

```cpp
EP_VERIFY(someFunction() == someValue);
```

The code above will generate an "equality comparison result unused" warning in your compiler.

As it happens, this warning is actually correct.  While the above code snippet may appear to be an appropriate use of EP_VERIFY(), remember that everything in the parentheses is left in your **Dist** build code.  After the C preprocessor has expanded the code above, the compiler sees the following in **Dist** builds:

```cpp
// preprocessor output
someFunction() == someValue;
```

To avoid this situation, use EP_VERIFY_EQ() to check for equality and EP_VERIFY_NEQ() to check for inequality.  While these macros serve the exact same function as EP_VERIFY(), they are designed to remove the right-hand side of the comparison, preventing compiler warnings.

@see EP_VERIFY_EQ()
@see EP_VERIFYF_EQ()
@see EP_VERIFY_NEQ()
@see EP_VERIFYF_NEQ()

### Slow Assertions

All the assertion macros mentioned so far also have a "slow" version.  *Slow* assertions are identical to their non-slow counterparts, except that in addition to being disabled in **Dist** builds, they are also disabled in **Dev** builds.

Use *slow* assertions whenever the assertion would occur on frequently executed code paths, such as Update() calls.  This will prevent the assertion from seriously impacting the performance of **Dev** builds, while still offering protection in **Debug** builds.

@see EP_ASSERT_SLOW()
@see EP_ASSERTF_SLOW()
@see EP_VERIFY_SLOW()
@see EP_VERIFYF_SLOW()
@see EP_VERIFY_EQ_SLOW()
@see EP_VERIFYF_EQ_SLOW()
@see EP_VERIFY_NEQ_SLOW()
@see EP_VERIFYF_NEQ_SLOW()

## Control Flow Assertions

In addition to *assert* and *verify* macros, %Enterprise offers macros useful for detecting unexpected control flow: EP_ASSERT_NOENTRY() and EP_ASSERT_NOREENTRY().

If EP_ASSERT_NOENTRY() is ever reached during program execution, an assertion failure will occur.  This assertion type should be placed in code paths your program should never reach, such as invalid `switch` cases.  EP_ASSERT_NOREENTRY(), on the other hand, will only trigger after it's been invoked two times.  Use it in scenarios where you must ensure code executes only once, such as initialization code.

@see EP_ASSERT_NOENTRY()
@see EP_ASSERT_NOREENTRY()

## Assertion Code Blocks

Some assertion scenarios require additional code to set up.  In situations where you need to prepare data for use in an assertion, use EP_ASSERT_CODE() or EP_ASSERT_CODE_SLOW() to wrap your setup code.  These wrappers will strip the code block out of builds where your assertion is disabled.

@note Both macros wrap your code block in a do-while structure, allowing them to be safely inlined.

@warning Be careful to ensure that none of the code in an assertion code block has any side-effects when deleted.  If the code in one of these blocks has any impact on the program behavior, hard-to-spot issues may occur in your distribution builds.

@see EP_ASSERT_CODE()
@see EP_ASSERT_CODE_SLOW()
