#!/bin/bash

if [ $# -ge 3 ]; then
  if [ "$1" == "center_in_rect" ]; then
    AREA="-D_CENTER_IN_RECT"
  elif [ "$1" == "triangle_in_rect" ]; then
    AREA="-D_TRIANGLE_IN_RECT"
  elif [ "$1" == "center_in_diam" ]; then
    AREA="-D_CENTER_IN_DIAM"  
  elif [ "$1" == "triangle_in_diam" ]; then
    AREA="-D_TRIANGLE_IN_DIAM"
  else
    echo "Usage: ./mycmake.sh <area-method> <incircle-method> <default-method> [show-or-not]"
    exit
  fi

  if [ "$2" == "center_in_circle" ]; then
    CIRCLE="-D_CENTER_IN_CIRCLE"
  elif [ "$2" == "triangle_in_circle" ]; then
    CIRCLE="-D_TRIANGLE_IN_CIRCLE"
  else
    echo "Usage: ./mycmake.sh <area-method> <incircle-method> <default-method> [show-or-not]"
    exit
  fi

  if [ "$3" == "default_fixed_area" ]; then
    DEFAULT="-D_DEFAULT_FIXED_AREA"
  elif [ "$3" == "default_average_area" ]; then
    DEFAULT="-D_DEFAULT_AVERAGE_AREA"
  else
    echo "Usage: ./mycmake.sh <area-method> <incircle-method> <default-method> [show-or-not]"
    exit
  fi

  if [ $# -ge 4 ]; then
    if [ "$4" == "show" ]; then
      SHOW="-D_SHOW"
    else
      echo "Usage: ./mycmake.sh <area-method> <incircle-method> <default-method> [show-or-not]"
      exit
    fi
  fi
  
  rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake Makefile
  cmake --no-warn-unused-cli ${AREA}=1 ${CIRCLE}=1 ${DEFAULT}=1 ${SHOW}=1 ../src
else
  echo "Usage: ./mycmake.sh <area-method> <incircle-method> <default-method> [show-or-not]"
  exit
fi
