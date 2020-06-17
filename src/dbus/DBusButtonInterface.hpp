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


#ifndef QPIPER_DBUS_BUTTON_INTERFACE_HPP
#define QPIPER_DBUS_BUTTON_INTERFACE_HPP

#include "dbus/IDBusIndexableInterface.hpp"


class QDBusArgument;
class DBusButtonInterface;


struct Macro final
{
    Q_GADGET

public:
    enum KeyEvent : quint8 {
        Release = 1,
        Press,
        Wait
    };
    Q_ENUM(KeyEvent)
public:
    KeyEvent event;
    quint32 value;
};
Q_DECLARE_METATYPE(Macro)


class Mapping final
{
    Q_GADGET
    ENABLE_QVARIANT(Mapping)

    friend class DBusButtonInterface;
    friend QDebug operator<<(QDebug, const Mapping);
    friend QDBusArgument& operator<<(QDBusArgument&, const Mapping&);
    friend const QDBusArgument& operator>>(const QDBusArgument&, Mapping&);
public:
    enum class ActionType : quint16 {
        Button = 1,
        Special,
        Macro = 4,
        Unknown = 1000
    };
    Q_ENUM(ActionType)

    enum class SpecialButton : quint32 {
        Unknown = (1 << 30),
        DoubleClick,
        WheelLeft,
        WheelRight,
        WheelUp,
        WheelDown,
        RatchetSwitch,
        ResolutionCycleUp,
        ResolutionCycleDown,
        ResolutionUp,
        ResolutionDown,
        ResolutionAlternate,
        ResolutionDefault,
        ProfileCycleUp,
        ProfileCycleDown,
        ProfileUp,
        ProfileDown,
        SecondMode,
        BatteryLevel
    };
    Q_ENUM(SpecialButton)
private:
    template<class T>
    T getAndCheckVariant_(const ActionType shouldBe) const;

    void setActionType_(const ActionType);
public:
    ActionType getActionType() const noexcept { return m_type_; }

    quint32 getButton() const;
    void setButton(const quint32);

    SpecialButton getSpecial() const;
    void setSpecial(const SpecialButton);

    QVector<Macro> getMacros() const;
    void setMacros(const QVector<Macro>&);
private:
    ActionType m_type_;
    QDBusVariant m_var_;
    Shared<const DBusButtonInterface> m_button_;
};
Q_DECLARE_METATYPE(Mapping)
Q_DECLARE_METATYPE(Mapping::SpecialButton)


class DBusButtonInterface final
    : public IDBusIndexableInterface
    , public std::enable_shared_from_this<DBusButtonInterface>
{
    Q_OBJECT
    Q_PROPERTY(QVector<quint32> ActionTypes READ getSupportedActionTypes_)
    Q_PROPERTY(Mapping Mapping READ getMapping WRITE setMapping)

private:
    QVector<quint32> getSupportedActionTypes_() const;
public:
    explicit DBusButtonInterface(const QDBusObjectPath&);

    Mapping getMapping() const;
    void setMapping(const Mapping&);

    QVector<Mapping::ActionType> getSupportedActionTypes() const noexcept
        { return m_supportedActionTypes_; }
public slots:
    void disable();
private:
    QVector<Mapping::ActionType> m_supportedActionTypes_;
};


class BadActionType final : public std::runtime_error
{
public:
    explicit BadActionType(const Mapping::ActionType is, const QString& msg = QString())
        : std::runtime_error(("The action type " % enumToString(is)
                              % " is not compatible with the current device"
                              % (msg.isNull()
                                 ? qStrL("")
                                 : qStrL(": ") % msg))
                             .toLatin1())
    {}

    explicit BadActionType(const Mapping::ActionType is, const Mapping::ActionType shouldBe)
        : BadActionType(is, "it should be " % enumToString(shouldBe))
    {}
};


QDebug operator<<(QDebug, const Macro);
QDebug operator<<(QDebug, const QVector<Macro>&);
QDebug operator<<(QDebug, const QVector<Mapping::ActionType>&);

QDBusArgument& operator<<(QDBusArgument&, const Macro);
const QDBusArgument& operator>>(const QDBusArgument&, Macro&);


#endif // QPIPER_DBUS_BUTTON_INTERFACE_HPP
