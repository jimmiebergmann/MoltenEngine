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

#ifndef MOLTEN_CORE_CORE_HPP
#define MOLTEN_CORE_CORE_HPP

/*
* Current version of Molten.
*/
#define MOLTEN_VERSION_MAJOR 0
#define MOLTEN_VERSION_MINOR 1
#define MOLTEN_VERSION_PATCH 0

#define MOLTEN_VERSION Molten::Version(MOLTEN_VERSION_MAJOR, MOLTEN_VERSION_MINOR, MOLTEN_VERSION_PATCH)

/*
* For checking platform, example:
*   #if MOLTEN_PLATFORM == CMOLTEN_PLATFORM_LINUX
*   ...
*   auto name = MOLTEN_PLATFORM_NAME;
*
*/
#define MOLTEN_PLATFORM_LINUX 1
#define MOLTEN_PLATFORM_WINDOWS 2
#define MOLTEN_PLATFORM_LINUX_NAME "Linux"
#define MOLTEN_PLATFORM_WINDOWS_NAME "Windows"

/*
* For checking architecture, example:
*   #if MOLTEN_ARCH == MOLTEN_ARCH_X86_64
*   ...
*   auto name = MOLTEN_ARCH_NAME;
* ...
* #endif
*/
#define MOLTEN_ARCH_X86_32 1
#define MOLTEN_ARCH_X86_64 2
#define MOLTEN_ARCH_X86_32_NAME "x86-32"
#define MOLTEN_ARCH_X86_64_NAME "x86-64"

/*
* For checking if build is in debug or release mode, example:
*   #if MOLTEN_BUILD == MOLTEN_BUILD_DEBUG
*   ...
*   auto name = MOLTEN_BUILD_NAME;
*/
#define MOLTEN_BUILD_DEBUG 1
#define MOLTEN_BUILD_RELEASE 2
#define MOLTEN_BUILD_DEBUG_NAME "Debug"
#define MOLTEN_BUILD_RELEASE_NAME "Release"

/* For getting base DPI of platform. */
#define MOLTEN_PLATFORM_BASE_DPI_LINUX 96
#define MOLTEN_PLATFORM_BASE_DPI_WINDOWS 96

/*
* DLL import or export, required for Windows, example:
*   class MOLTEN_API MyClass {};
*/
#define MOLTEN_API

/*
* Disable anymous structure warnings,
* by encapsulating with MOLTEN_ANONYMOUS_STRUCTURE_BEGIN and MOLTEN_ANONYMOUS_STRUCTURE_END.
*/
#define MOLTEN_ANONYMOUS_STRUCTURE_BEGIN
#define MOLTEN_ANONYMOUS_STRUCTURE_END

/*
* Disable padded structure warnings,
* by encapsulating with MOLTEN_PADDED_STRUCT_BEGIN and MOLTEN_PADDED_STRUCT_END.
*/
#define MOLTEN_PADDED_STRUCT_BEGIN
#define MOLTEN_PADDED_STRUCT_END

/*
* Disable unscoped enum warnings,
* by encapsulating with MOLTEN_UNSCOPED_ENUM_BEGIN and MOLTEN_UNSCOPED_ENUM_END.
*/
#define MOLTEN_UNSCOPED_ENUM_BEGIN
#define MOLTEN_UNSCOPED_ENUM_END

// Windows
#if defined( _WIN32 ) || defined( __WIN32__ ) || defined( _WIN64 ) || defined( __WIN64__ )
    #define MOLTEN_PLATFORM MOLTEN_PLATFORM_WINDOWS
    #define MOLTEN_PLATFORM_NAME MOLTEN_PLATFORM_WINDOWS_NAME

    #if defined(_M_ARM)
        #error Not supporting ARM architecture.
    #endif

    #if defined(_M_IX86)
        #define MOLTEN_ARCH MOLTEN_ARCH_X86_32
        #define MOLTEN_ARCH_NAME MOLTEN_ARCH_X86_32_NAME
    #elif defined(_M_AMD64)
        #define MOLTEN_ARCH MOLTEN_ARCH_X86_64
        #define MOLTEN_ARCH_NAME MOLTEN_ARCH_X86_64_NAME
    #else
        #error Unkown architecture.
    #endif

    #define MOLTEN_PLATFORM_BASE_DPI MOLTEN_PLATFORM_BASE_DPI_WINDOWS

// Linux
#elif defined( linux ) || defined( __linux )
    #define MOLTEN_PLATFORM MOLTEN_PLATFORM_LINUX
    #define MOLTEN_PLATFORM_NAME MOLTEN_PLATFORM_LINUX_NAME

    #if defined(__arm__)
        #error Not supporting ARM architecture.
    #endif

    #if defined(__amd64__) || defined(__amd64)
        #if defined(__x86_64__) || defined(__x86_64)
            #define MOLTEN_ARCH MOLTEN_ARCH_X86_64
            #define MOLTEN_ARCH_NAME MOLTEN_ARCH_X86_64_NAME
        #else
            #define MOLTEN_ARCH MOLTEN_ARCH_X86_32
            #define MOLTEN_ARCH_NAME MOLTEN_ARCH_X86_32_NAME
        #endif
    #else
        #error Unkown architecture.
    #endif

    #undef MOLTEN_ANONYMOUS_STRUCTURE_BEGIN
    #undef MOLTEN_ANONYMOUS_STRUCTURE_END
    #define MOLTEN_ANONYMOUS_STRUCTURE_BEGIN _Pragma("GCC diagnostic push") \
                                            _Pragma("GCC diagnostic ignored \"-Wpedantic\"")
    #define MOLTEN_ANONYMOUS_STRUCTURE_END _Pragma("GCC diagnostic pop")

    #define MOLTEN_PLATFORM_BASE_DPI MOLTEN_PLATFORM_BASE_DPI_LINUX

#else
    #error Unkown platform.
#endif

// Visual C++
#ifdef _MSC_VER
    #pragma warning(disable : 4201) // nonstandard extension used : nameless struct/union

    #undef MOLTEN_UNSCOPED_ENUM_BEGIN
    #undef MOLTEN_UNSCOPED_ENUM_END
    #define MOLTEN_UNSCOPED_ENUM_BEGIN __pragma(warning(disable : 26812))
    #define MOLTEN_UNSCOPED_ENUM_END __pragma(warning(default : 26812))

    #undef MOLTEN_PADDED_STRUCT_BEGIN
    #undef MOLTEN_PADDED_STRUCT_END
    #define MOLTEN_PADDED_STRUCT_BEGIN __pragma(warning(disable : 4324))
    #define MOLTEN_PADDED_STRUCT_END __pragma(warning(default : 4324))

#endif

// Build type
#if defined( NDEBUG ) || !defined( _DEBUG )
    #define MOLTEN_BUILD MOLTEN_BUILD_RELEASE
    #define MOLTEN_BUILD_NAME MOLTEN_BUILD_RELEASE_NAME
#else
    #define MOLTEN_BUILD MOLTEN_BUILD_DEBUG
    #define MOLTEN_BUILD_NAME MOLTEN_BUILD_DEBUG_NAME
#endif

// Debug assert
#if MOLTEN_BUILD == MOLTEN_BUILD_DEBUG
    #if _MSC_VER
        #define MOLTEN_DEBUG_ASSERT(condition, message) \
            if(!(condition)) { ((_CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", message) != 1 ) || (_CrtDbgBreak(),  0));}
    #else 
        #define MOLTEN_DEBUG_ASSERT(condition, message)
    #endif        
#else
    #define MOLTEN_DEBUG_ASSERT(condition, message)
#endif

// Export API
#if !defined(MOLTEN_STATIC)
    #ifdef _MSC_VER
        #pragma warning(disable : 4251) // identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
        #pragma warning(disable : 4100) // identifier' : unreferenced formal parameter
    #endif

    // Define as export or import, if FLARE_EXPORTS is defined.
    #if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
        #undef MOLTEN_API
        #if defined(MOLTEN_EXPORT)
            #define MOLTEN_API __declspec(dllexport)
        #else
            #define MOLTEN_API __declspec(dllimport)
        #endif
    #endif
#endif

#endif
