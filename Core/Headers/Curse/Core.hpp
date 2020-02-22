/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

#ifndef CURSE_CORE_CORE_HPP
#define CURSE_CORE_CORE_HPP

/*
* Current version of Curse.
*/
#define CURSE_VERSION_MAJOR 0
#define CURSE_VERSION_MINOR 1
#define CURSE_VERSION_PATCH 0

#define CURSE_VERSION Curse::Version(CURSE_VERSION_MAJOR, CURSE_VERSION_MINOR, CURSE_VERSION_PATCH)

/*
* For checking platform, example:
*   #if CURSE_PLATFORM == CURSE_PLATFORM_LINUX
*   ...
*   auto name = CURSE_PLATFORM_NAME;
*
*/
#define CURSE_PLATFORM_LINUX 1
#define CURSE_PLATFORM_WINDOWS 2
#define CURSE_PLATFORM_LINUX_NAME "Linux"
#define CURSE_PLATFORM_WINDOWS_NAME "Windows"

/*
* For checking architecture, example:
*   #if CURSE_ARCH == CURSE_ARCH_X86_64
*   ...
*   auto name = CURSE_ARCH_NAME;
* ...
* #endif
*/
#define CURSE_ARCH_X86_32 1
#define CURSE_ARCH_X86_64 2
#define CURSE_ARCH_X86_32_NAME "x86-32"
#define CURSE_ARCH_X86_64_NAME "x86-64"

/*
* For checking if build is in debug or release mode, example:
*   #if CURSE_BUILD == CURSE_BUILD_DEBUG
*   ...
*   auto name = CURSE_BUILD_NAME;
*/
#define CURSE_BUILD_DEBUG 1
#define CURSE_BUILD_RELEASE 2
#define CURSE_BUILD_DEBUG_NAME "Debug"
#define CURSE_BUILD_RELEASE_NAME "Release"

/*
* DLL import or export, required for Windows, example:
*   class CURSE_API MyClass {};
*/
#define CURSE_API

/*
* Disable anymous structure warnings,
* by encapsulating with CURSE_ANONYMOUS_STRUCTURE_BEGIN and CURSE_ANONYMOUS_STRUCTURE_END.
*/
#define CURSE_ANONYMOUS_STRUCTURE_BEGIN
#define CURSE_ANONYMOUS_STRUCTURE_END

/*
* Disable padded structure warnings,
* by encapsulating with CURSE_PADDED_STRUCT_BEGIN and CURSE_PADDED_STRUCT_END.
*/
#define CURSE_PADDED_STRUCT_BEGIN
#define CURSE_PADDED_STRUCT_END

/*
* Disable unscoped enum warnings,
* by encapsulating with CURSE_UNSCOPED_ENUM_BEGIN and CURSE_UNSCOPED_ENUM_END.
*/
#define CURSE_UNSCOPED_ENUM_BEGIN
#define CURSE_UNSCOPED_ENUM_END

// Windows
#if defined( _WIN32 ) || defined( __WIN32__ ) || defined( _WIN64 ) || defined( __WIN64__ )
    #define CURSE_PLATFORM CURSE_PLATFORM_WINDOWS
    #define CURSE_PLATFORM_NAME CURSE_PLATFORM_WINDOWS_NAME

    #if defined(_M_ARM)
        #error Not supporting ARM architecture.
    #endif

    #if defined(_M_IX86)
        #define CURSE_ARCH CURSE_ARCH_X86_32
        #define CURSE_ARCH_NAME CURSE_ARCH_X86_32_NAME
    #elif defined(_M_AMD64)
        #define CURSE_ARCH CURSE_ARCH_X86_64
        #define CURSE_ARCH_NAME CURSE_ARCH_X86_64_NAME
    #else
        #error Unkown architecture.
    #endif

// Linux
#elif defined( linux ) || defined( __linux )
    #define CURSE_PLATFORM CURSE_PLATFORM_LINUX
    #define CURSE_PLATFORM_NAME CURSE_PLATFORM_LINUX_NAME

    #if defined(__arm__)
        #error Not supporting ARM architecture.
    #endif

    #if defined(__amd64__) || defined(__amd64)
        #if defined(__x86_64__) || defined(__x86_64)
            #define CURSE_ARCH CURSE_ARCH_X86_64
            #define CURSE_ARCH_NAME CURSE_ARCH_X86_64_NAME
        #else
            #define CURSE_ARCH CURSE_ARCH_X86_32
            #define CURSE_ARCH_NAME CURSE_ARCH_X86_32_NAME
        #endif
    #else
        #error Unkown architecture.
    #endif

    #undef CURSE_ANONYMOUS_STRUCTURE_BEGIN
    #undef CURSE_ANONYMOUS_STRUCTURE_END
    #define CURSE_ANONYMOUS_STRUCTURE_BEGIN _Pragma("GCC diagnostic push") \
                                            _Pragma("GCC diagnostic ignored \"-Wpedantic\"")
    #define CURSE_ANONYMOUS_STRUCTURE_END _Pragma("GCC diagnostic pop")
#else
    #error Unkown platform.
#endif

// Visual C++
#ifdef _MSC_VER
    #pragma warning(disable : 4201) // nonstandard extension used : nameless struct/union

    #undef CURSE_UNSCOPED_ENUM_BEGIN
    #undef CURSE_UNSCOPED_ENUM_END
    #define CURSE_UNSCOPED_ENUM_BEGIN __pragma(warning(disable : 26812))
    #define CURSE_UNSCOPED_ENUM_END __pragma(warning(default : 26812))

    #undef CURSE_PADDED_STRUCT_BEGIN
    #undef CURSE_PADDED_STRUCT_END
    #define CURSE_PADDED_STRUCT_BEGIN __pragma(warning(disable : 4324))
    #define CURSE_PADDED_STRUCT_END __pragma(warning(default : 4324))

#endif

// Build type
#if defined( NDEBUG ) || !defined( _DEBUG )
    #define CURSE_BUILD CURSE_BUILD_RELEASE
    #define CURSE_BUILD_NAME CURSE_BUILD_RELEASE_NAME
#else
    #define CURSE_BUILD CURSE_BUILD_DEBUG
    #define CURSE_BUILD_NAME CURSE_BUILD_DEBUG_NAME
#endif

// Export API
#if !defined(CURSE_STATIC)
    #ifdef _MSC_VER
        #pragma warning(disable : 4251) // identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
        #pragma warning(disable : 4100) // identifier' : unreferenced formal parameter
    #endif

    // Define as export or import, if FLARE_EXPORTS is defined.
    #if CURSE_PLATFORM == CURSE_PLATFORM_WINDOWS
        #undef CURSE_API
        #if defined(CURSE_EXPORT)
            #define CURSE_API __declspec(dllexport)
        #else
            #define CURSE_API __declspec(dllimport)
        #endif
    #endif
#endif

#endif // CURSE_CORE_HPP
