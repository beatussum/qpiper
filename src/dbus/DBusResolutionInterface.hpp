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
    QString toString() const;
public:
    quint16 x;
    quint16 y;
};
Q_DECLARE_METATYPE(Axes)

constexpr bool operator==(const Axes a, const Axes b) noexcept
    { return (a.x == b.x) && (a.y == b.y); }


class Resolution final
{
    friend class DBusResolutionInterface;
    friend QDebug operator<<(QDebug, const Resolution);
    friend QDebug operator<<(QDebug, const QVector<Resolution>&);
    friend bool operator==(const Resolution, const Resolution);
public:
    enum Type : quint8 { Null, Axis, Axes };
private:
    explicit operator std::pair<Resolution, Resolution>() const;
public:
    static constexpr const char* typeToString(const Type) noexcept;
public:
    constexpr Resolution(const quint16 xy) noexcept
        : m_type_(Axis), m_axis_(xy)
    {}

    constexpr Resolution(const struct Axes xy) noexcept
        : m_type_(Axes), m_axes_(xy)
    {}

    constexpr Resolution(std::nullptr_t = nullptr) noexcept
        : m_type_(Null), m_null_(true)
    {}

    QString translate() const;
    QString toString() const;
private:
    Type m_type_;

    union {
        struct Axes m_axes_;
        quint16 m_axis_;
        bool m_null_;
    };
};

constexpr const char* Resolution::typeToString(const Type type) noexcept
{
    switch (type) {
        case Resolution::Axis:
            return "(xy)";
        case Resolution::Axes:
            return "(x, y)";
        case Resolution::Null:
            return "(null)";
    }

    Q_UNREACHABLE();
}


class BadResolution final : public std::runtime_error
{
public:
    explicit BadResolution(const Resolution nsupported, const QString msg = QString())
        : std::runtime_error(("The resolution "
                              % nsupported.toString()
                              % " is not compatible with the current device"
                              % (msg.isNull()
                                 ? qStrL("")
                                 : qStrL(": ") % msg))
                             .toLatin1())
    {}

    explicit BadResolution(const Resolution nsupported, const Resolution::Type is, const Resolution::Type shouldBe)
        : BadResolution(nsupported, qStrL("the resolution is of type ")
                                    % Resolution::typeToString(is)
                                    % " but should be of type "
                                    % Resolution::typeToString(shouldBe))
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
    void checkResolution(const Resolution) const;
    QVector<quint32> getSupportedResolutions_() const;

    QDBusVariant getResolution_() const;
    void setResolution_(const QDBusVariant&);
public:
    explicit DBusResolutionInterface(const QDBusObjectPath&);

    Resolution getResolution(const bool assumed = false) const;
    void setResolution(const Resolution);

    QVector<Resolution> getSupportedResolutions() const noexcept
        { return m_supportedResolutions_; }

    Resolution::Type getType() const noexcept { return m_type_; }

    bool isActive() const;
    bool isDefault() const;
public slots:
    void setActive();
    void setDefault();
private:
    QVector<Resolution> m_supportedResolutions_;
    const Resolution::Type m_type_;
};


QDebug operator<<(QDebug, const Axes);

QDBusArgument& operator<<(QDBusArgument&, const Axes);
const QDBusArgument& operator>>(const QDBusArgument&, Axes&);


#endif // QPIPER_DBUS_RESOLUTION_INTERFACE_HPP
