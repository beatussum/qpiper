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

#include "dbus/DBusIndexableInterface.hpp"


class QDBusArgument;


class Macro
{
public:
    enum KeyEvent : quint32 {
        None,
        Release,
        Press,
        Wait
    };
public:
    KeyEvent event;
    quint32 value;
};

Q_DECLARE_METATYPE(Macro)
QDBusArgument& operator<<(QDBusArgument& arg, const Macro& macro);
const QDBusArgument& operator>>(const QDBusArgument& arg, Macro& macro);


class Mapping
{
public:
    enum class ActionType : quint32 {
        None,
        Button,
        Special,
        Macro = 4,
        Unknown = 1000
    };

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
public:
    ActionType getActionType() const { return m_type_; }

    quint32 getButton() const;
    void setButton(const quint32 button);

    SpecialButton getSpecial() const;
    void setSpecial(const SpecialButton special);

    QVector<Macro> getMacro() const;
    void setMacro(const QVector<Macro>& macro);

    friend QDBusArgument& operator<<(QDBusArgument& arg, const Mapping& mapping);
    friend const QDBusArgument& operator>>(const QDBusArgument& arg, Mapping& mapping);
private:
    void setActionType(const ActionType action) { m_type_ = action; }
private:
    ActionType m_type_;
    QDBusVariant m_var_;
};

Q_DECLARE_METATYPE(Mapping)
Q_DECLARE_METATYPE(Mapping::SpecialButton)


class DBusButtonInterface : public DBusIndexableInterface
{
    Q_OBJECT
    Q_PROPERTY(QVector<quint32> ActionTypes
               READ getSupportedActionTypes_)
    Q_PROPERTY(Mapping Mapping READ getMapping WRITE setMapping)

public:
    explicit DBusButtonInterface(const QString& obj);

    QVector<Mapping::ActionType> getSupportedActionTypes() const;

    Mapping getMapping() const;
    void setMapping(const Mapping& mapping);
private:
    QVector<quint32> getSupportedActionTypes_() const;
};


#endif // QPIPER_DBUS_BUTTON_INTERFACE_HPP
