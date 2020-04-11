# Install script for directory: /media/gal/Windows8_OS/Linux/CG/src/freeglut

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/media/gal/Windows8_OS/Linux/CG/bin")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfreeglut.so.3.11.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfreeglut.so.3"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfreeglut.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/media/gal/Windows8_OS/Linux/CG/src/freeglut/lib/libfreeglut.so.3.11.0"
    "/media/gal/Windows8_OS/Linux/CG/src/freeglut/lib/libfreeglut.so.3"
    "/media/gal/Windows8_OS/Linux/CG/src/freeglut/lib/libfreeglut.so"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfreeglut.so.3.11.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfreeglut.so.3"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfreeglut.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/media/gal/Windows8_OS/Linux/CG/src/freeglut/lib/libfreeglut.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/GL" TYPE FILE FILES
    "/media/gal/Windows8_OS/Linux/CG/src/freeglut/include/GL/freeglut.h"
    "/media/gal/Windows8_OS/Linux/CG/src/freeglut/include/GL/freeglut_ucall.h"
    "/media/gal/Windows8_OS/Linux/CG/src/freeglut/include/GL/freeglut_ext.h"
    "/media/gal/Windows8_OS/Linux/CG/src/freeglut/include/GL/freeglut_std.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE RENAME "freeglut.pc" FILES "/media/gal/Windows8_OS/Linux/CG/src/freeglut/freeglut.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/FreeGLUT/FreeGLUTTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/FreeGLUT/FreeGLUTTargets.cmake"
         "/media/gal/Windows8_OS/Linux/CG/src/freeglut/CMakeFiles/Export/lib/cmake/FreeGLUT/FreeGLUTTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/FreeGLUT/FreeGLUTTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/FreeGLUT/FreeGLUTTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/FreeGLUT" TYPE FILE FILES "/media/gal/Windows8_OS/Linux/CG/src/freeglut/CMakeFiles/Export/lib/cmake/FreeGLUT/FreeGLUTTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/FreeGLUT" TYPE FILE FILES "/media/gal/Windows8_OS/Linux/CG/src/freeglut/CMakeFiles/Export/lib/cmake/FreeGLUT/FreeGLUTTargets-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/FreeGLUT" TYPE FILE FILES
    "/media/gal/Windows8_OS/Linux/CG/src/freeglut/FreeGLUT/FreeGLUTConfig.cmake"
    "/media/gal/Windows8_OS/Linux/CG/src/freeglut/FreeGLUT/FreeGLUTConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/media/gal/Windows8_OS/Linux/CG/src/freeglut/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
