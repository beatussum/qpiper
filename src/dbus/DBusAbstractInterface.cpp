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


#include "dbus/DBusAbstractInterface.hpp"

#include <QtCore/QProcessEnvironment>

const char* DBusException::what() const noexcept
{
    return QString("%1: %2")
        .arg(m_error_.name())
        .arg(m_error_.message())
        .toLocal8Bit().data();
}

const QString DBusAbstractInterface::m_kServiceName = getServiceName_();

QString DBusAbstractInterface::getServiceName_()
{
    QProcessEnvironment env;

    return env.value("RATBAG_TEST").isEmpty()
        ? "org.freedesktop.ratbag1"
        : "org.freedesktop.ratbag_devel1";
}

bool DBusAbstractInterface::connect(const QString& name, QObject* receiver, const char* slot)
{
    return QDBusConnection::systemBus().connect(m_kServiceName, m_obj_, m_interface_,
                                                name, receiver, slot);
}
