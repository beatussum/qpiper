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

#include "core/core.hpp"
#include "dbus/DBusIndexableInterface.hpp"


class QDBusArgument;
class DBusButtonInterface;


class Macro final
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

    // to allow construction from QVariant
    friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<Mapping, true>;
    friend struct QtPrivate::QVariantValueHelper<Mapping>;

    friend class DBusButtonInterface;
    friend QDBusArgument& operator<<(QDBusArgument& arg, const Mapping& mapping);
    friend const QDBusArgument& operator>>(const QDBusArgument& arg, Mapping& mapping);
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
    Mapping() = default;

    template<class T>
    T getAndCheckVariant_(const ActionType check) const;

    void setActionType_(const ActionType action);
public:
    ActionType getActionType() const { return m_type_; }

    quint32 getButton() const;
    void setButton(const quint32 button);

    SpecialButton getSpecial() const;
    void setSpecial(const SpecialButton special);

    QVector<Macro> getMacros() const;
    void setMacros(const QVector<Macro>& macros);
private:
    ActionType m_type_;
    QDBusVariant m_var_;
    Shared<const DBusButtonInterface> m_button_;
};
Q_DECLARE_METATYPE(Mapping)
Q_DECLARE_METATYPE(Mapping::SpecialButton)


class DBusButtonInterface final
        : public DBusIndexableInterface
        , public std::enable_shared_from_this<DBusButtonInterface>
{
    Q_OBJECT
    Q_PROPERTY(QVector<quint32> ActionTypes
               READ getSupportedActionTypes_)
    Q_PROPERTY(Mapping Mapping READ getMapping WRITE setMapping)

    // force to be only a shared pointer
    template<typename T, typename... Params>
    friend Shared<T> std::make_shared(Params&&... params);
    template<typename T>
    friend class __gnu_cxx::new_allocator;
private:
    explicit DBusButtonInterface(const QString& obj);

    QVector<quint32> getSupportedActionTypes_() const;
public:
    Mapping getMapping() const;
    void setMapping(const Mapping& mapping);

    QVector<Mapping::ActionType> getSupportedActionTypes() const
        { return m_supportedActionTypes_; }
private:
    QVector<Mapping::ActionType> m_supportedActionTypes_;
};


QString actionsToStr(const QVector<Mapping::ActionType>& actions);

class BadActionType final : public std::runtime_error
{
public:
    BadActionType(const char *const is, const QString& suffix)
        : std::runtime_error((qStrL("The action type is `") % is
                              % "` " % suffix).toLatin1())
    {}

    BadActionType(const char *const is, const char *const shouldBe)
        : BadActionType(is, qStrL("but should be `") % shouldBe % '`')
    {}

    BadActionType(const char *const is, const QVector<Mapping::ActionType>& actions)
        : BadActionType(is, ", which is not compatible with the current device:"
                            " only " % actionsToStr(actions) % " are allowed")
    {}
};


QDebug operator<<(QDebug debug, const Macro macro);
QDebug operator<<(QDebug debug, const QVector<Macro>& macros);
QDebug operator<<(QDebug debug, const Mapping::ActionType action);
QDebug operator<<(QDebug debug, const QVector<Mapping::ActionType>& macros);

QDBusArgument& operator<<(QDBusArgument& arg, const Macro macro);
const QDBusArgument& operator>>(const QDBusArgument& arg, Macro& macro);


#endif // QPIPER_DBUS_BUTTON_INTERFACE_HPP
