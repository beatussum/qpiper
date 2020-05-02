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

#include <QtCore/QLatin1String>


#define nqDebug() qDebug().nospace()
#define nqInfo() qInfo().nospace()

#define qStrL(str) QStringLiteral(str)
#define qByteL(ba) QByteArrayLiteral(ba)


extern const bool isColoredOutput;

QLatin1String operator "" _qls(const char* value, const std::size_t size);
bool qPiperGetEnv(const char* name, const QByteArray value);

namespace AnsiColor
{
    inline const QLatin1String rst = "\u001B[0m"_qls;

    namespace fg
    {
        inline const QLatin1String b = "\u001B[1m"_qls;
        inline const QLatin1String bred = "\u001B[1;31m"_qls;
        inline const QLatin1String bgreen = "\u001B[1;32m"_qls;
    }

    namespace bg
    {
        inline const QLatin1String bryellow = "\u001B[103m"_qls;
        inline const QLatin1String brblue = "\u001B[104m"_qls;
        inline const QLatin1String brcyan = "\u001B[106m"_qls;
    }
}

void qPiperMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);


#endif // QPIPER_CORE_HPP
