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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR})

set(QPIPER_UI)
set(QPIPER_HPP
    core/core.hpp
    dbus/IDBusInterface.hpp
    dbus/IDBusIndexableInterface.hpp
    dbus/DBusLedInterface.hpp
    dbus/DBusButtonInterface.hpp)
set(QPIPER_CPP
    main.cpp
    core/core.cpp
    dbus/IDBusInterface.cpp
    dbus/IDBusIndexableInterface.cpp
    dbus/DBusLedInterface.cpp
    dbus/DBusButtonInterface.cpp)

set(DOCS_DIR "${CMAKE_BINARY_DIR}/docs" CACHE PATH
    "The path where the documention is built")
set(DOCS_NAMESPACE "fr.beatussum.qpiper")

find_package(Qt5 REQUIRED
             COMPONENTS DBus Gui Widgets)
find_package(Doxygen REQUIRED)

set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTOUIC TRUE)

add_executable(qpiper ${QPIPER_UI} ${QPIPER_HPP} ${QPIPER_CPP})
target_link_libraries(qpiper Qt5::DBus Qt5::Gui Qt5::Widgets)

if(DOXYGEN_FOUND AND WITH_DOCS)
    set(DOXYGEN_OUTPUT_DIRECTORY ${DOCS_DIR})
    set(DOXYGEN_ALLOW_UNICODE_NAMES YES)
    set(DOXYGEN_REPEAT_BRIEF NO)
    set(DOXYGEN_INLINE_INHERITED_MEMB YES)
    set(DOXYGEN_HIDE_UNDOC_CLASSES YES)
    set(DOXYGEN_HIDE_IN_BODY_DOCS YES)
    set(DOXYGEN_WARN_NO_PARAMDOC YES)
    set(DOXYGEN_WARN_LOGFILE "${DOCS_DIR}/log")
    set(DOXYGEN_EXCLUDE_PATTERNS "${DOXYGEN_EXCLUDE_PATTERNS} *.in")

    doxygen_add_docs(docs
                     ${QPIPER_HPP}
                     COMMENT "Generating documentation…")
endif()
