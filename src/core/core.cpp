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


#include "core/core.hpp"

#include <QString>
#include <iostream>


const bool isColoredOutput = (qgetenv("QPIPER_COLOR") != "none");

void qPiperMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    using namespace AnsiColor;

    QString fileName = context.file;
    fileName = fileName.mid(fileName.indexOf("src"));
    QString tmp;
    switch (type) {
        case QtDebugMsg:
            tmp = isColoredOutput
                  ? QString("%1%2DEBUG%3\u00A0(%4{fileName}%3:%5{line}%3): %1{msg}%3\n")
                    .arg(fg::b).arg(bg::brcyan).arg(rst).arg(fg::bred)
                    .arg(fg::bgreen)
                  : QString("DEBUG\u00A0({fileName}:{line}): {msg}\n");

            std::cerr << tmp.replace("{fileName}", std::move(fileName))
                         .replace("{line}", QString::number(context.line))
                         .replace("{msg}", msg).toStdString();
            break;
        case QtWarningMsg:
            tmp = isColoredOutput
                  ? QString("%1%2!!\u00A0WARNING\u00A0!!%3\u00A0(%4{function}%3): %5{msg}%3\n")
                    .arg(fg::bred).arg(bg::bryellow).arg(rst).arg(fg::bred)
                    .arg(fg::b)
                  : QString("!!\u00A0WARNING\u00A0!!\u00A0({function}): {msg}\n");

            std::cerr << tmp.replace("{function}", context.function)
                         .replace("{msg}", msg).toStdString();
            break;
        default:
            tmp = isColoredOutput
                  ? QString("%1%2INFO%3: %1{msg}%3\n")
                    .arg(fg::b).arg(bg::brblue).arg(rst)
                  : QString("INFO: {msg}\n");

            std::cout << tmp.replace("{msg}", msg).toStdString();
            break;
    }
}
