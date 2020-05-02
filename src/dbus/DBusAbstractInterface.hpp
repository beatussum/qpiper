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


#ifndef QPIPER_DBUS_ABSTRACT_INTERFACE_HPP
#define QPIPER_DBUS_ABSTRACT_INTERFACE_HPP

#include "core/core.hpp"

#include <QtCore/QStringBuilder>
#include <QtDBus/QDBusInterface>


/**
 * @class DBusCallException dbus/DBusAbstractInterface.hpp
 * @brief An exception in case of a DBus error when calling a method
 */
class DBusCallException final : public std::runtime_error
{
public:
    explicit DBusCallException(const QDBusError& error)
        : std::runtime_error((error.name() % qStrL(": ")
                              % error.message() % qStrL("."))
                             .toLatin1())
    {}
};


/**
 * @class DBusPropertyException dbus/DBusAbstractInterface.hpp
 * @brief An exception in case of property error
 */
class DBusPropertyException final : public std::runtime_error
{
public:
    /**
     * @brief Construct a new DBusPropertyException object with a name and
     * a possible custom suffix
     *
     * @param name   the name of the property
     */
    explicit DBusPropertyException(const QString& name)
        : std::runtime_error((qStrL("Unable to get or set the property ")
                             % name % '.').toLatin1())
    {}

    /**
     * @brief Construct a new DBusPropertyException object with a name and
     * a description
     *
     * @param name the name of the property
     * @param desc a description about the exception
     */
    explicit DBusPropertyException(const char* name, const QString& desc)
        : DBusPropertyException(name % qStrL(": ") % desc)
    {}

    /**
     * @brief Check if the value of a property is within a given range
     *
     * @param name  the name of the property
     * @param value the value of the property
     * @param lim   maximum
     *
     * @throws DBusPropertyException if the value is higher than value
     */
    static void checkInRange(const char* name, quint32 value, quint32 lim);
};


/**
 * @class DBusAbstractInterface dbus/DBusAbstractInterface.hpp
 * @brief A base class for all DBus interfaces
 *
 * This class provides some useful methods in order to avoid
 * repetitive code.
 *
 * @warning Before using any interface, please first set the name of the service
 * via DBusAbstractInterface::setServiceName(const QString& name).
 */
class DBusAbstractInterface : public QDBusInterface
{
public:
    /**
     * @brief Construct a new DBusAbstractInterface object
     *
     * @param inter the interface name without the root prefix: e.g.
     *              _button_ not _org.example.button_
     * @param obj   the DBusAbstractInterface object
     */
    explicit DBusAbstractInterface(const QString& inter, const QString& obj);

    virtual ~DBusAbstractInterface() = 0;

    /**
     * @brief Set the name of the service
     *
     * @param name the name of the service
     */
    static void setServiceName(const QString& name) noexcept;

    /**
     * @brief Call a void method
     *
     * @param method the name of the method
     *
     * @throws DBusCallException if an error has occured
     */
    void callAndCheck(const QString& method);

    /**
     * @brief get the value of a DBus property
     *
     * @tparam T    the type of the property
     * @param  name the name of the property
     *
     * @return a valid QVariant
     * @throws DBusPropertyException if the QVariant is invalid
     */
    template<class T>
    T getPropertyAndCheck(const char* name) const;

    /**
     * @brief set a DBus property
     *
     * @tparam T     the type of the property
     * @param  name  the name of the property
     * @param  value the desired value
     *
     * @throws DBusPropertyException if the QVariant is invalid
     */
    template<class T>
    void setPropertyAndCheck(const char* name, T value);
private:
    static QString m_serviceName_;
};

template<class T>
T DBusAbstractInterface::getPropertyAndCheck(const char* name) const
{
    QVariant ret = property(name);

    if (!ret.isValid()) {
        throw DBusPropertyException(name, "the QVariant is not valid");
    } else {
        return qvariant_cast<T>(ret);
    }
}

template<class T>
void DBusAbstractInterface::setPropertyAndCheck(const char* name, T value)
{
    if (!setProperty(name, QVariant::fromValue(value)))
        throw DBusPropertyException(name);
}


#endif // QPIPER_DBUS_ABSTRACT_INTERFACE_HPP
