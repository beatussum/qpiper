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
#include "dbus/IDBusIndexableInterface.hpp"

#include <QtCore/QtDebug>


quint8 IDBusIndexableInterface::getIndex() const
{
    return static_cast<quint8>(getPropertyAndCheck<quint32>("Index"));
}

IDBusIndexableInterface::operator QString() const
{
    return m_interface_ % " no.\u00A0" % QString::number(getIndex());
}

IDBusIndexableInterface::IDBusIndexableInterface(const QString& inter, const QDBusObjectPath& obj)
    : IDBusInterface(inter, obj)
    , m_interface_(inter)
{
    qqInfo() << "creating an instance of"
             << static_cast<QString>(*this);
}

IDBusIndexableInterface::~IDBusIndexableInterface() {}
