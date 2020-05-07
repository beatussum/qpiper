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

#include <QtCore/QtDebug>
#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusMetaType>


/*#####################################*/
/*                utils                */
/*#####################################*/

QString actionsToStr(const QVector<Mapping::ActionType>& actions)
{
    QString ret;
    const auto size = actions.size();

    for (int i = 0; i < size; ++i) {
        ret = ret + '`' + enumToString(actions.at(i)) + '`';

        switch (size - i) {
            case 2:
                ret += " and ";
                break;
            case 1:
                break;
            default:
                ret += ", ";
                break;
        }
    }

    return ret;
}


/*#####################################*/
/*           debug operators           */
/*#####################################*/

QDebug operator<<(QDebug debug, const Macro macro)
{
    debug.nospace() << "the macro of type `"
                    << enumToString(macro.event)
                    << "` having the value "
                    << macro.value;

    return debug;
}

QDebug operator<<(QDebug debug, const QVector<Macro>& macros) {
    using namespace AnsiColor;

    debug.noquote().nospace() << '\n';

    for (const auto i : macros) {
        debug << "\t\t";

        if (isColoredOutput) {
            debug << fg::bgreen << '-' << rst << fg::b;
        } else {
            debug << '-';
        }

        debug << ' ' << i << '\n';
    }

    return debug;
}

QDebug operator<<(QDebug debug, const Mapping::ActionType action)
{
    debug.nospace() << "the mapping is of the `"
                    << enumToString(action)
                    << "` type";

    return debug;
}

QDebug operator<<(QDebug debug, const QVector<Mapping::ActionType>& vec)
{
    debug.noquote().nospace() << "the mouse supports: "
                              << actionsToStr(vec);

    return debug;
}


/*#####################################*/
/*    marshallers and demarshallers    */
/*#####################################*/

QDBusArgument& operator<<(QDBusArgument& arg, const Macro macro)
{
    arg.beginStructure();
    arg << static_cast<quint32>(macro.event) << macro.value;
    arg.endStructure();

    qDebug() << "marshalling" << macro;

    return arg;
}

const QDBusArgument& operator>>(const QDBusArgument& arg, Macro& macro)
{
    arg.beginStructure();
    macro.event = static_cast<Macro::KeyEvent>(qdbus_cast<quint32>(arg));
    arg >> macro.value;
    arg.endStructure();

    qDebug() << "demarshalling" << macro;

    return arg;
}

QDBusArgument& operator<<(QDBusArgument& arg, const Mapping& mapping)
{
    arg.beginStructure();
    arg << static_cast<quint32>(mapping.m_type_) << mapping.m_var_;
    arg.endStructure();

    qDebug() << mapping.m_type_;

    return arg;
}

const QDBusArgument& operator>>(const QDBusArgument& arg, Mapping& mapping)
{
    arg.beginStructure();
    mapping.m_type_ = static_cast<Mapping::ActionType>(qdbus_cast<quint32>(arg));
    arg >> mapping.m_var_;
    arg.endStructure();

    qDebug() << mapping.m_type_;

    return arg;
}


/*#####################################*/
/*               Mapping               */
/*#####################################*/

template<class T>
T Mapping::getAndCheckVariant_(const ActionType check) const
{
    const auto currentAction = getActionType();

    if (currentAction != check) {
        throw BadActionType(enumToString(currentAction),
                            enumToString(check));
    } else {
        return qvariant_cast<T>(m_var_.variant());
    }
}

void Mapping::setActionType_(const ActionType action)
{
    const auto& tmp = m_button_->getSupportedActionTypes();

    if (!tmp.contains(action)) {
        throw BadActionType(enumToString(action),
                            tmp);
    } else {
        qDebug() << "setting action type to" << enumToString(action);
        m_type_ = action;
    }
}

quint32 Mapping::getButton() const
{
    const auto ret = getAndCheckVariant_<quint32>(ActionType::Button);
    nqInfo() << "this button is mapped to the button n°\u00A0" << ret;

    return ret;
}

void Mapping::setButton(const quint32 button)
{
    setActionType_(ActionType::Button);
    m_var_ = QDBusVariant(button);
}

Mapping::SpecialButton Mapping::getSpecial() const
{
    const auto ret = getAndCheckVariant_<SpecialButton>(ActionType::Special);
    nqInfo() << "this button is mapped to the special button `"
             << enumToString(ret) << '`';

    return ret;
}

void Mapping::setSpecial(const SpecialButton special)
{
    setActionType_(ActionType::Special);
    m_var_ = QDBusVariant(static_cast<quint32>(special));
}

QVector<Macro> Mapping::getMacros() const
{
    const auto currentAction = getActionType();

    if (currentAction != ActionType::Macro) {
        throw BadActionType(enumToString(currentAction),
                            enumToString(ActionType::Macro));
    } else {
        const auto& ret = qdbus_cast<QVector<Macro>>(m_var_.variant());
        qInfo() << "\n\n\tthis button is mapped to the macros:" << ret;
        return ret;
    }
}

void Mapping::setMacros(const QVector<class Macro>& macros)
{
    setActionType_(ActionType::Macro);
    m_var_.setVariant(QVariant::fromValue(macros));
}


/*#####################################*/
/*         DBusButtonInterface         */
/*#####################################*/

DBusButtonInterface::DBusButtonInterface(const QString& obj)
    : IDBusIndexableInterface("Button", obj)
{
    qDBusRegisterMetaType<Macro>();
    qDBusRegisterMetaType<QVector<Macro>>();
    qDBusRegisterMetaType<Mapping>();

    for (const auto i : getSupportedActionTypes_())
        m_supportedActionTypes_.append(static_cast<Mapping::ActionType>(i));

    qInfo() << m_supportedActionTypes_;
}

QVector<quint32> DBusButtonInterface::getSupportedActionTypes_() const
{
    return getPropertyAndCheck<QVector<quint32>>("ActionTypes");
}

Mapping DBusButtonInterface::getMapping() const
{
    auto ret = getPropertyAndCheck<Mapping>("Mapping");

    qDebug() << "linking this button to the `Mapping` instance";
    ret.m_button_ = shared_from_this();

    return ret;
}

void DBusButtonInterface::setMapping(const Mapping& mapping)
{
    setPropertyAndCheck("Mapping", mapping);
}

void DBusButtonInterface::disable()
{
    callAndCheck("Disable");
    nqDebug() << "button n°\u00A0" << getIndex() << " disabled";
}
