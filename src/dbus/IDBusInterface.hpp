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
 * @class DBusException dbus/IDBusInterface.hpp
 * @brief An exception in case of property error
 */
class DBusException final : public std::runtime_error
{
public:
    /**
     * @brief Construct a new `DBusException` object with a name and
     * a description
     *
     * @param name the name of the property
     * @param desc a description about the exception
     */
    explicit DBusException(const char *const name, const QString& desc)
        : std::runtime_error((qStrL("Unable to get or set the property ")
                              % name % ": " % desc).toLatin1())
    {}

    /**
     * @brief Construct a new `DBusException` object with the name of a
     * property and a `QDBusError`
     *
     * @param name  the name of the property
     * @param error the DBus error
     */
    explicit DBusException(const char *const name, const QDBusError& error)
        : DBusException(name, "\n\t" % error.name() % ": " % error.message())
    {}

    /**
     * @brief Check if the value of a property is within a given range
     *
     * @param name   the name of the property
     * @param lim    maximum
     * @param params a list of parameters to be tested
     *
     * @throw DBusException if at least one of \p params is higher
     * than \p lim
     */
    template<typename T, typename... Params>
    static void checkInRange(const char *const name, T lim, T first, Params... params);
};

template<typename T, typename... Params>
void DBusException::checkInRange(const char *const name, T lim, T first, Params... params)
{
    static_assert(are_same<T, T, Params...> && is_comparable<T>::value,
                  "the variadic parameters must be of the same "
                  "type and comparable");

    if (first > lim) {
        throw DBusException(name, QString::number(first)
                                  % qStrL(" not in the range [0;")
                                  % QString::number(lim) % ']');
    } else if constexpr (sizeof...(params) > 0) {
        DBusException::checkInRange(name, lim, params...);
    }
}


/**
 * @class IDBusInterface dbus/IDBusInterface.hpp
 * @brief A base class for all DBus interfaces
 *
 * This class provides some useful methods in order to avoid repetitive
 * code.
 *
 * @warning Before using any interface, please first set the name of the
 * service via `IDBusInterface::setServiceName(const QString& name)`.
 */
class IDBusInterface : protected QDBusInterface
{
public:
    /**
     * @brief Set the name of the service
     *
     * @param name the name of the service
     */
    static void setServiceName(const QString& name) noexcept;
protected:
    /**
     * @brief Construct a new `IDBusInterface` object
     *
     * @param inter the interface name without the root prefix: e.g.
     *              _button_ not _org.example.button_
     * @param obj   the DBus object
     */
    explicit IDBusInterface(const QString& inter, const QDBusObjectPath& obj);

    virtual ~IDBusInterface() = 0;

    /**
     * @brief Call a DBus `void` method
     *
     * @param method the name of the method
     *
     * @throw DBusException if an error has occured
     */
    void callAndCheck(const char *const method);

    /**
     * @brief get the value of a DBus property
     *
     * @tparam T    the type of the property
     * @param  name the name of the property
     *
     * @return a valid `QVariant`
     * @throw DBusException if an error has occured
     */
    template<class T>
    T getPropertyAndCheck(const char *const name) const;

    /**
     * @brief set a DBus property
     *
     * @tparam T     the type of the property
     * @param  name  the name of the property
     * @param  value the desired value usable by `QVariant::fromValue(const T &value)`
     *
     * @throw DBusException if an error has occured
     */
    template<class T>
    void setPropertyAndCheck(const char *const name, T value);
private:
    static QString m_serviceName_;
};

template<class T>
T IDBusInterface::getPropertyAndCheck(const char *const name) const
{
    const QVariant& ret = property(name);

    if (const QDBusError& error = lastError(); error.isValid()) {
        throw DBusException(name, error);
    } else {
        return qvariant_cast<T>(ret);
    }
}

template<class T>
void IDBusInterface::setPropertyAndCheck(const char *const name, T value)
{
    if (!setProperty(name, QVariant::fromValue(value)))
        throw DBusException(name, lastError());
}


#endif // QPIPER_DBUS_ABSTRACT_INTERFACE_HPP
