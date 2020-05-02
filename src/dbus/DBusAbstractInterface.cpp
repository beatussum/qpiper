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

#include <QtCore/QtDebug>
#include <QtDBus/QDBusReply>


void DBusPropertyException::checkInRange(const char* name, quint32 value, quint32 lim)
{
    if (value > lim)
        throw DBusPropertyException(name, QString::number(value)
                                          % qStrL(" not in the range [0;")
                                          % QString::number(lim)
                                          % ']');
}


QString DBusAbstractInterface::m_serviceName_;

DBusAbstractInterface::DBusAbstractInterface(const QString& inter, const QString& obj)
    : QDBusInterface(m_serviceName_, obj,
                     m_serviceName_ % '.' % inter,
                     QDBusConnection::systemBus())
{
    qInfo() << "creating a new interface" << interface() << "for the object"
            << path();
}

DBusAbstractInterface::~DBusAbstractInterface() {}

void DBusAbstractInterface::setServiceName(const QString& name) noexcept
{
    qInfo() << "service name set to" << name;
    m_serviceName_ = name;
}

void DBusAbstractInterface::callAndCheck(const QString& method)
{
    const QDBusReply<void> r = call(method);

    if (!r.isValid())
        throw DBusCallException(r.error());
}
