/*
 * Copyright (C) 2020 Mattéo Rossillol‑‑Laruelle <beatussum@protonmail.com>
 *
 * This program is free software:  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published  by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be  useful,
 * but WITHOUT ANY WARRANTY;   without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef QPIPER_CORE_HPP
#define QPIPER_CORE_HPP

#include <QtGlobal>


#define nqDebug() qDebug().nospace()
#define nqInfo() qInfo().nospace()

extern const bool isColoredOutput;

namespace AnsiColor
{
    constexpr char rst[] = "\u001B[0m";

    namespace fg
    {
        constexpr char b[] = "\u001B[1m";
        constexpr char bred[] = "\u001B[1;31m";
        constexpr char bgreen[] = "\u001B[1;32m";
    }

    namespace bg
    {
        constexpr char bryellow[] = "\u001B[103m";
        constexpr char brblue[] = "\u001B[104m";
        constexpr char brcyan[] = "\u001B[106m";
    }
}

void qPiperMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);


#endif // QPIPER_CORE_HPP
