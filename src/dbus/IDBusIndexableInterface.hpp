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


#ifndef QPIPER_DBUS_INDEXABLE_INTERFACE_HPP
#define QPIPER_DBUS_INDEXABLE_INTERFACE_HPP

#include "dbus/IDBusInterface.hpp"


/**
 * @class IDBusIndexableInterface dbus/IDBusIndexableInterface.hpp
 * @brief A child of `IDBusInterface` for indexable interfaces
 */
class IDBusIndexableInterface : protected IDBusInterface
{
    Q_OBJECT
    Q_PROPERTY(quint32 Index READ getIndex)

public:
    quint8 getIndex() const;

    explicit operator QString() const;

    explicit IDBusIndexableInterface(const QString& inter, const QDBusObjectPath& obj);

    virtual ~IDBusIndexableInterface() = 0;
private:
    const QString m_interface_;
};


#endif // QPIPER_DBUS_INDEXABLE_INTERFACE_HPP
