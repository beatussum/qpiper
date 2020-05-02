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

void qPiperMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    using namespace AnsiColor;

    std::string fileName = context.file;
    fileName = fileName.substr(fileName.find("src"));
    switch (type) {
        case QtDebugMsg:
            // prints "%1DEBUG%2 (%3%4%5:%6%7%8): %9%10%11\n"
            std::cerr << fg::b << bg::brcyan << "DEBUG" << rst << "\u00A0("
                      << fg::bred << std::move(fileName) << rst << ':'
                      << fg::bgreen << context.line << rst << "): "
                      << fg::b << msg.toLocal8Bit().data() << rst << '\n';
            break;
        case QtWarningMsg:
            // prints "%1%2!! WARNING !!%3 (%4%5%6): %7%8%9\n"
            std::cerr << fg::bred << bg::bryellow << "!!\u00A0WARNING\u00A0!!"
                      << rst << "\u00A0(" << fg::bgreen << context.function
                      << rst << "): " << fg::b << msg.toLocal8Bit().data()
                      << rst << '\n';
            break;
        default:
            // prints "%1%2INFO%3: %4%5%6\n"
            std::cout << fg::b << bg::brblue << "INFO" << rst << ": " << fg::b
                      << msg.toLocal8Bit().data() << rst << '\n';
            break;
    }
}
