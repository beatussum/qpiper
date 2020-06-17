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
/*           debug operators           */
/*#####################################*/

QDebug operator<<(QDebug debug, const Macro macro)
{
    debug << "the macro of type" << enumToString(macro.event)
          << "having the value" << macro.value;

    return debug;
}

QDebug operator<<(QDebug debug, const QVector<Macro>& macros) {
    using namespace AnsiColor;

    debug.noquote();

    for (const auto i : macros) {
        debug.nospace() << "\t\t";

        if (isColoredOutput) {
            debug << fg::bgreen << '-' << rst << fg::b;
        } else {
            debug << '-';
        }

        debug.space() << i << '\n';
    }

    return debug;
}

QDebug operator<<(QDebug debug, const Mapping mapping)
{
    debug << "mapping of type" << enumToString(mapping.m_type_);

    return debug;
}

QDebug operator<<(QDebug debug, const QVector<Mapping::ActionType>& actions)
{
    debug.nospace().noquote();

    {
        const int size = actions.size();
        for (int i = 0; i < size; ++i) {
            debug << enumToString(actions.at(i))
                  << getVecSeparator(size, i);
        }
    }

    return debug;
}


/*#####################################*/
/*    marshallers and demarshallers    */
/*#####################################*/

QDBusArgument& operator<<(QDBusArgument& arg, const Macro macro)
{
    qqDebug() << "marshalling" << macro;

    arg.beginStructure();
    arg << static_cast<quint32>(macro.event) << macro.value;
    arg.endStructure();

    return arg;
}

const QDBusArgument& operator>>(const QDBusArgument& arg, Macro& macro)
{
    qqDebug() << "demarshalling" << macro;

    arg.beginStructure();
    macro.event = static_cast<Macro::KeyEvent>(qdbus_cast<quint32>(arg));
    arg >> macro.value;
    arg.endStructure();

    return arg;
}

QDBusArgument& operator<<(QDBusArgument& arg, const Mapping& mapping)
{
    qqDebug() << "marshalling" << mapping;

    arg.beginStructure();
    arg << static_cast<quint32>(mapping.m_type_) << mapping.m_var_;
    arg.endStructure();

    return arg;
}

const QDBusArgument& operator>>(const QDBusArgument& arg, Mapping& mapping)
{
    qqDebug() << "demarshalling" << mapping;

    arg.beginStructure();
    mapping.m_type_ = static_cast<Mapping::ActionType>(qdbus_cast<quint32>(arg));
    arg >> mapping.m_var_;
    arg.endStructure();

    return arg;
}


/*#####################################*/
/*               Mapping               */
/*#####################################*/

template<class T>
T Mapping::getAndCheckVariant_(const ActionType shouldBe) const
{
    qqDebug() << "checking whether the" << m_button_->toString()
              << "has a valid variant";

    const auto currentAction = getActionType();

    if (currentAction != shouldBe) {
        throw BadActionType(currentAction, shouldBe);
    } else {
        return qvariant_cast<T>(m_var_.variant());
    }
}

void Mapping::setActionType_(const ActionType action)
{
    qqInfo() << "setting the action type of" << m_button_->toString()
             << "to" << enumToString(action);

    const auto& tmp = m_button_->getSupportedActionTypes();

    if (!tmp.contains(action)) {
        throw BadActionType(action);
    } else {
        m_type_ = action;
    }
}

quint32 Mapping::getButton() const
{
    qqDebug() << "getting the value of the variant of"
              << m_button_->toString() << "as a button";

    const auto ret = getAndCheckVariant_<quint32>(ActionType::Button);
    return ret;
}

void Mapping::setButton(const quint32 button)
{
    qqInfo() << "setting the value of the variant of"
             << m_button_->toString() << "as a button:"
             << button;

    setActionType_(ActionType::Button);
    m_var_ = QDBusVariant(button);
}

Mapping::SpecialButton Mapping::getSpecial() const
{
    qqDebug() << "getting the value of the variant of"
              << m_button_->toString() << "as a special button";

    const auto ret = getAndCheckVariant_<SpecialButton>(ActionType::Special);
    return ret;
}

void Mapping::setSpecial(const SpecialButton special)
{
    qqInfo() << "setting the value of the variant of"
             << m_button_->toString() << "as a special button:"
             << enumToString(special);

    setActionType_(ActionType::Special);
    m_var_ = QDBusVariant(static_cast<quint32>(special));
}

QVector<Macro> Mapping::getMacros() const
{
    qqDebug() << "getting the value of the variant of"
              << m_button_->toString() << "as macros";

    const auto currentAction = getActionType();

    if (currentAction != ActionType::Macro) {
        throw BadActionType(currentAction, ActionType::Macro);
    } else {
        const auto& ret = qdbus_cast<QVector<Macro>>(m_var_.variant());
        return ret;
    }
}

void Mapping::setMacros(const QVector<class Macro>& macros)
{
    qqInfo() << "setting the value of the variant of"
             << m_button_->toString() << "as macros";
    qInfo() << "\n\n\tthis button is mapped to the macros:\n" << macros;

    setActionType_(ActionType::Macro);
    m_var_.setVariant(QVariant::fromValue(macros));
}


/*#####################################*/
/*         DBusButtonInterface         */
/*#####################################*/

DBusButtonInterface::DBusButtonInterface(const QDBusObjectPath& obj)
    : IDBusIndexableInterface("Button", obj)
{
    qDBusRegisterMetaType<Macro>();
    qDBusRegisterMetaType<QVector<Macro>>();
    qDBusRegisterMetaType<Mapping>();

    for (const auto i : getSupportedActionTypes_())
        m_supportedActionTypes_.append(static_cast<Mapping::ActionType>(i));

    qInfo() << "the mouse supports the button action types:"
            << m_supportedActionTypes_;
}

QVector<quint32> DBusButtonInterface::getSupportedActionTypes_() const
{
    return getPropertyAndCheck<QVector<quint32>>("ActionTypes");
}

Mapping DBusButtonInterface::getMapping() const
{
    auto ret = getPropertyAndCheck<Mapping>("Mapping");

    qqDebug() << "linking" << toString() << "to the `Mapping` instance";
    ret.m_button_ = shared_from_this();

    return ret;
}

void DBusButtonInterface::setMapping(const Mapping& mapping)
{
    setPropertyAndCheck("Mapping", mapping);
}

void DBusButtonInterface::disable()
{
    qqInfo() << "disabling" << toString();
    callAndCheck("Disable");
}
