# Install script for directory: /Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Library/Developer/CommandLineTools/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/voronota-js" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/voronota-js")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/voronota-js")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-voromqa"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-only-global-voromqa"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-membrane-voromqa"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-ifeatures-voromqa"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-fast-iface-voromqa"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-fast-iface-cadscore"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-fast-iface-cadscore-matrix"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-fast-iface-data-graph"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-fast-iface-contacts"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-voroif-gnn"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-ligand-cadscore"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-pdb-utensil-renumber-by-sequence"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-pdb-utensil-detect-inter-structure-contacts"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-pdb-utensil-split-to-models"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-pdb-utensil-filter-atoms"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-pdb-utensil-rename-chains"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-pdb-utensil-print-sequence-from-structure"
    "/Users/makarbetlei/Documents/Internship/voronota_1.29.4198/expansion_js/voronota-js-pdb-utensil-download-structure"
    )
endif()

