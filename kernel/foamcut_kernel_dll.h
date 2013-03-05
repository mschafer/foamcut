/*
 * (C) Copyright 2013 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */
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
