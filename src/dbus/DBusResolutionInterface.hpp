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


#ifndef QPIPER_DBUS_RESOLUTION_INTERFACE_HPP
#define QPIPER_DBUS_RESOLUTION_INTERFACE_HPP

#include "dbus/IDBusIndexableInterface.hpp"


class QDBusArgument;


struct Axes final
{
public:
    explicit operator QString() const;
public:
    quint16 x;
    quint16 y;
};
Q_DECLARE_METATYPE(Axes)

inline bool operator==(const Axes a, const Axes b)
    { return (a.x == b.x) && (a.y == b.y); }

inline bool operator==(const quint16 a, const Axes b)
    { return (a == b.x) || (a == b.y); }


class Resolution final
{
    Q_GADGET

    friend class DBusResolutionInterface;
    friend QDebug operator<<(QDebug debug, const Resolution res);
    friend QDebug operator<<(QDebug debug, const QVector<Resolution> res);
    friend bool operator==(const Resolution, const Resolution);
private:
    enum Type : quint8 { Null, Axis, Axes };
public:
    constexpr Resolution(const quint16 xy)
        : m_type_(Axis), m_axis_(xy)
    {}

    constexpr Resolution(const struct Axes xy)
        : m_type_(Axes), m_axes_(xy)
    {}

    constexpr Resolution(std::nullptr_t = nullptr)
        : m_type_(Null), m_null_(true)
    {}

    bool isNull() const { return m_null_; }
    QString translate() const;

    explicit operator QString() const;
private:
    Type m_type_;

    union {
        struct Axes m_axes_;
        quint16 m_axis_;
        bool m_null_;
    };
};


class BadResolution final : public std::runtime_error
{
public:
    BadResolution(const Resolution nsupported)
        : std::runtime_error(("The resolution ("
                              % static_cast<QString>(nsupported)
                              % ") is not compatible with the current device")
                             .toLatin1())
    {}
};


class DBusResolutionInterface final : private IDBusIndexableInterface
{
    Q_OBJECT
    Q_PROPERTY(bool IsActive READ isActive)
    Q_PROPERTY(bool IsDefault READ isDefault)
    Q_PROPERTY(QDBusVariant Resolution READ getResolution_ WRITE setResolution_)
    Q_PROPERTY(QVector<quint32> Resolutions READ getSupportedResolutions_)

private:
    QDBusVariant getResolution_() const;
    void setResolution_(const QDBusVariant& res);

    QVector<quint32> getSupportedResolutions_() const;
    void checkResolution(const Resolution) const;
public:
    explicit DBusResolutionInterface(const QDBusObjectPath& obj);

    Resolution getResolution() const;
    void setResolution(const Resolution res);

    QVector<Resolution> getSupportedResolutions() const
        { return m_supportedResolutions_; }

    bool isActive() const;
    bool isDefault() const;
public slots:
    void setActive();
    void setDefault();
private:
    QVector<Resolution> m_supportedResolutions_;
    Resolution::Type m_type_;
};


QDebug operator<<(QDebug debug, const Axes axes);

QDBusArgument& operator<<(QDBusArgument& arg, const Axes axes);
const QDBusArgument& operator>>(const QDBusArgument& arg, Axes& axes);


#endif // QPIPER_DBUS_RESOLUTION_INTERFACE_HPP