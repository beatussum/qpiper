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


#ifndef QPIPER_DBUSABSTRACTINTERFACE_HPP
#define QPIPER_DBUSABSTRACTINTERFACE_HPP

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

class DBusException final : public std::runtime_error
{
public:
    DBusException(QDBusError e);

    const char* what() const noexcept override;
private:
    QDBusError m_error_;
};

class DBusAbstractInterface : public QDBusInterface
{
public:
    static const QString m_kServiceName;
public:
    DBusAbstractInterface(const QString& interface, const QString& obj) noexcept
        : m_interface_(m_kServiceName + '.' + interface)
        , m_obj_(obj)
        , QDBusInterface(m_kServiceName, m_interface_,
                         m_obj_, QDBusConnection::systemBus())
        {}

    bool connect(const QString& name, QObject* receiver, const char* slot);

    template<class T>
    T getProperty(const QString& property);
private:
    static QString getServiceName_();
private:
    QString m_interface_;
    QString m_obj_;
};

template<class T>
T DBusAbstractInterface::getProperty(const QString& property)
{
    QDBusReply<T> reply = call(property);

    if (!reply.isValid())
        throw DBusException(reply.error());

    return static_cast<T>(reply);
}

#endif // QPIPER_DBUSABSTRACTINTERFACE_HPP
