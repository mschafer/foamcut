#ifndef foamcut_kernel_dll_hpp
#define foamcut_kernel_dll_hpp

/* Cmake will define foamcut_kernel_EXPORTS on Windows when it
configures to build a shared library. If you are going to use
another build system on windows or create the visual studio
projects by hand you need to define it when
building a DLL on windows.
*/
// We are using the Visual Studio Compiler and building Shared libraries

#if defined (_WIN32) 
  // VS will warn about every boost and std template not having dll interface w/o these
  #pragma warning( disable: 4251 )
  #pragma warning( disable: 4275 )
  #if defined(foamcut_kernel_EXPORTS)
    #define  foamcut_kernel_API __declspec(dllexport)
  #else
    #define  foamcut_kernel_API __declspec(dllimport)
  #endif
#else
 #define foamcut_kernel_API
#endif

#endif
