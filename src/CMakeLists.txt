
cmake_minimum_required(VERSION 2.8)

PROJECT(CleftLip)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

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
