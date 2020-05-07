# Copyright (C) 2020 Mattéo Rossillol‑‑Laruelle <beatussum@protonmail.com>
#
# This program is free software:  you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published  by
# the Free Software Foundation, either version 3 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be  useful,
# but WITHOUT ANY WARRANTY;   without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program. If not, see <https://www.gnu.org/licenses/>.


cmake_minimum_required(VERSION 3.14)

project(QPiper
        VERSION 0.9.0
        DESCRIPTION "Qt application to configure gaming devices"
        HOMEPAGE_URL "https://github.com/beatussum/qpiper/"
        LANGUAGES CXX)

option(WITH_DOCS "Enable building of documentation" OFF)

unset(CMAKE_CXX_FLAGS_RELEASE CACHE)
macro(add_gcc_cxx_flags flags)
    if(CMAKE_COMPILER_IS_GNUCXX)
         set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flags}")
    endif()
endmacro()

# Use ccache if it is installed
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

add_gcc_cxx_flags(-fno-operator-names -funsigned-char -pipe)
add_gcc_cxx_flags(-W -pedantic)
add_gcc_cxx_flags(-Wold-style-cast -Wcomment -Wformat=2 -Wmain -Winline)
add_gcc_cxx_flags(-Wparentheses -Wunused -Wuninitialized -Wunknown-pragmas)
add_gcc_cxx_flags(-Wfloat-equal -Wcast-qual -Wcast-align -Wconversion)
add_gcc_cxx_flags(-Wsign-compare -Wunreachable-code -Wmissing-braces)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_gcc_cxx_flags(-O0)
    add_gcc_cxx_flags(-Wpadded)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(QT_NO_DEBUG_OUTPUT)
    add_gcc_cxx_flags(-O2)
endif()

add_subdirectory("${CMAKE_SOURCE_DIR}/src")
