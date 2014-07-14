cmake_minimum_required(VERSION 2.8)

PROJECT(CleftLip)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

if (DEFINED _CENTER_IN_RECT)
  add_definitions(-DCENTER_IN_RECT)
elseif (DEFINED _TRIANGLE_IN_RECT)
  add_definitions(-DTRIANGLE_IN_RECT)
elseif (DEFINED _CENTER_IN_DIAM)
  add_definitions(-DCENTER_IN_DIAM)
elseif (DEFINED _TRIANGLE_IN_DIAM)
  add_definitions(-DTRIANGLE_IN_DIAM)
endif ()

if (DEFINED _CENTER_IN_CIRCLE)
  add_definitions(-DCENTER_IN_CIRCLE)
elseif (DEFINED _TRIANGLE_IN_CIRCLE)
  add_definitions(-DTRIANGLE_IN_CIRCLE)
endif ()

if (DEFINED _DEFAULT_FIXED_AREA)
  add_definitions(-DDEFAULT_FIXED_AREA)
elseif (DEFINED _DEFAULT_AVERAGE_AREA)
  add_definitions(-DDEFAULT_AVERAGE_AREA)
endif ()

if (DEFINED _SHOW)
  add_definitions(-DSHOW)
endif ()

add_executable(NostrilDetect MACOSX_BUNDLE
               NostrilDetect.cxx Utility.cxx Detector.cxx)
add_executable(ShowLandmarks MACOSX_BUNDLE
               ShowLandmarks.cxx Utility.cxx)

if(VTK_LIBRARIES)
  target_link_libraries(NostrilDetect ${VTK_LIBRARIES})
  target_link_libraries(ShowLandmarks ${VTK_LIBRARIES})
else()
  target_link_libraries(NostrilDetect vtkHybrid)
  target_link_libraries(ShowLandmarks vtkHybrid)
endif()
