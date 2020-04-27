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


/**
 * @class DBusException dbus/DBusAbstractInterface.hpp
 * @brief An exception in case of DBus error
 */
class DBusException final : public std::runtime_error
{
public:
    /**
     * @brief Construct a new DBusException object
     *
     * @param e the QDBusError object
     */
    DBusException(QDBusError e);

    /**
     * @brief Get some information about the exception
     *
     * @return The message
     */
    const char* what() const noexcept override;
private:
    QDBusError m_error_;
};

/**
 * @class DBusAbstractInterface dbus/DBusAbstractInterface.hpp
 * @brief A base class for all DBus interfaces
 *
 * This class provides some useful methods in order to avoid
 * repetitive code.
 */
class DBusAbstractInterface : public QDBusInterface
{
public:
    /**
     * @brief The name of the DBus service
     *
     * This constant can have different values depending on
     * whether **RATBAG_TEST** is set or not.
     */
    static const QString m_kServiceName;
public:
    /**
     * @brief Construct a new DBusAbstractInterface object
     *
     * @param interface the interface name without the root prefix: e.g.
     * _button_ not _org.example.button_
     * @param obj       the DBusAbstractInterface object
     */
    DBusAbstractInterface(const QString& interface, const QString& obj) noexcept
        : m_interface_(m_kServiceName + '.' + interface)
        , m_obj_(obj)
        , QDBusInterface(m_kServiceName, m_interface_,
                         m_obj_, QDBusConnection::systemBus())
        {}

    /**
     * @brief An alias to QDBusConnection::connect()
     *
     * @param  name     the name of the signal
     * @param  receiver the object receiving the signal
     * @param  slot     the name of the slot
     * @return true     if the connection has been successful
     * @return false    if the connection has not been successful
     */
    bool connect(const QString& name, QObject* receiver, const char* slot);

    /**
     * @brief Get the property value
     *
     * @tparam T        the type returned by the property
     * @param  property the name of the property
     * @return T        the type returned by the property
     */
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
