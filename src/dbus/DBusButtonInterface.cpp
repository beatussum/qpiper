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


#include "dbus/DBusButtonInterface.hpp"

#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusMetaType>


QDBusArgument& operator<<(QDBusArgument& arg, const Macro& macro)
{
    arg.beginStructure();
    arg << static_cast<quint32>(macro.event) << macro.value;
    arg.endStructure();

    return arg;
}

const QDBusArgument& operator>>(const QDBusArgument& arg, Macro& macro)
{
    arg.beginStructure();
    macro.event = static_cast<Macro::KeyEvent>(qdbus_cast<quint32>(arg));
    arg >> macro.value;
    arg.endStructure();

    return arg;
}


quint32 Mapping::getButton() const
{
    if (getActionType() == ActionType::Button)
        return qvariant_cast<quint32>(m_var_.variant());
}

void Mapping::setButton(const quint32 button)
{
    setActionType(ActionType::Button);
    m_var_ = QDBusVariant(button);
}

Mapping::SpecialButton Mapping::getSpecial() const
{
    if (getActionType() == ActionType::Special)
        return qvariant_cast<SpecialButton>(m_var_.variant());
}

void Mapping::setSpecial(const SpecialButton special)
{
    setActionType(ActionType::Special);
    m_var_ = QDBusVariant(static_cast<quint32>(special));
}

QVector<Macro> Mapping::getMacro() const
{
    if (getActionType() == ActionType::Macro)
        return qdbus_cast<QVector<Macro>>(m_var_.variant());
}

void Mapping::setMacro(const QVector<class Macro>& macro)
{
    setActionType(ActionType::Macro);
    m_var_.setVariant(QVariant::fromValue(macro));
}

QDBusArgument& operator<<(QDBusArgument& arg, const Mapping& mapping)
{
    arg.beginStructure();
    arg << static_cast<quint32>(mapping.m_type_) << mapping.m_var_;
    arg.endStructure();

    return arg;
}

const QDBusArgument& operator>>(const QDBusArgument& arg, Mapping& mapping)
{
    arg.beginStructure();
    mapping.m_type_ = static_cast<Mapping::ActionType>(qdbus_cast<quint32>(arg));
    arg >> mapping.m_var_;

    arg.endStructure();

    return arg;
}


DBusButtonInterface::DBusButtonInterface(const QString& obj)
    : DBusIndexableInterface("Button", obj)
{
    qDBusRegisterMetaType<Macro>();
    qDBusRegisterMetaType<QVector<Macro>>();
    qDBusRegisterMetaType<Mapping>();
}

QVector<quint32> DBusButtonInterface::getSupportedActionTypes_() const
{
    return getPropertyAndCheck<QVector<quint32>>("Mapping");
}

QVector<Mapping::ActionType> DBusButtonInterface::getSupportedActionTypes() const
{
    QVector<Mapping::ActionType> ret;

    const QVector<quint32>& vec = getSupportedActionTypes_();
    for (const auto i : vec)
        ret.append(static_cast<Mapping::ActionType>(i));

    return ret;
}

Mapping DBusButtonInterface::getMapping() const
{
    return getPropertyAndCheck<Mapping>("Mapping");
}

void DBusButtonInterface::setMapping(const Mapping& mapping)
{
    setPropertyAndCheck("Mapping", mapping);
}
