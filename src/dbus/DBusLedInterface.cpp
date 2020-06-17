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


#include "dbus/DBusLedInterface.hpp"

#include <QtCore/QtDebug>
#include <QtCore/QtMath>
#include <QtDBus/QDBusMetaType>


/*#####################################*/
/*           debug operators           */
/*#####################################*/

QDebug operator<<(QDebug debug, const DBusLedInterface::Mode mode)
{
    debug.noquote() << enumToString(mode) << "mode";

    return debug;
}

QDebug operator<<(QDebug debug, const Color& color)
{
    debug.nospace() << "rgb(" << color.red
                    << ", " << color.green
                    << ", " << color.blue
                    << ')';

    return debug;
}


/*#####################################*/
/*    marshallers and demarshallers    */
/*#####################################*/

QDBusArgument& operator<<(QDBusArgument& arg, const Color& color)
{
    qDebug() << "marshalling" << color;

    arg.beginStructure();
    arg << color.red << color.green << color.blue;
    arg.endStructure();

    return arg;
}

const QDBusArgument& operator>>(const QDBusArgument& arg, Color& color)
{
    qDebug() << "demarshalling" << color;

    arg.beginStructure();
    arg >> color.red >> color.green >> color.blue;
    arg.endStructure();

    return arg;
}


/*######################################*/
/*           DBusLedInterface           */
/*######################################*/

quint8 DBusLedInterface::getBitsNumber_() const
{
    switch (getColorDepth()) {
        case ColorDepth::ZeroBits:
            return 0;
        case ColorDepth::EightBits:
            return 8;
        case ColorDepth::OneBit:
            return 1;
    }

    Q_UNREACHABLE();
}

QString DBusLedInterface::getModeDescription(const Mode mode)
{
    switch (mode) {
        case Mode::Off:
            return QObject::tr("LED is off");
        case Mode::Constant:
            return QObject::tr("LED is on with constant brightness");
        case Mode::Cycles:
            return QObject::tr("LED cycles through a set of colors. This "
                               "mode ignores the `Color` values");
        case Mode::Breathing:
            return QObject::tr("LED uses a breathing-style animation");
    }

    Q_UNREACHABLE();
}

QString DBusLedInterface::getColorDepthDescription(const ColorDepth depth)
{
    switch (depth) {
        case ColorDepth::ZeroBits:
            return QObject::tr("Zero bits per color: e.g. monochrome");
        case ColorDepth::EightBits:
            return QObject::tr("Eight bits per color");
        case ColorDepth::OneBit:
            return QObject::tr("One bit per color");
    }

    Q_UNREACHABLE();
}

DBusLedInterface::DBusLedInterface(const QDBusObjectPath& obj)
    : IDBusIndexableInterface("Led", obj)
    , m_colorMax_(static_cast<quint32>(qPow(2, getBitsNumber_()) - 1))
{
    qDBusRegisterMetaType<Color>();

    nqInfo() << "colors are " << getBitsNumber_()
             << "-bits color-coded: let [0;"
             << m_colorMax_ << ']';
    qInfo() << "the led is in" << getMode();
}

quint8 DBusLedInterface::getMode_() const
{
     return getPropertyAndCheck<quint8>("Mode");
}

DBusLedInterface::Mode DBusLedInterface::getMode() const
{
     return static_cast<Mode>(getMode_());
}

void DBusLedInterface::setMode_(const quint8 mode)
{
    setPropertyAndCheck("Mode", mode);
}

void DBusLedInterface::setMode(const Mode mode)
{
    qqInfo() << "setting" << toString() << "to" << mode;

    setMode_(static_cast<quint8>(mode));
}

Color DBusLedInterface::getColor() const
{
    auto ret = getPropertyAndCheck<Color>("Color");
    DBusException::checkInRange("Color", m_colorMax_,
                                ret.red, ret.green, ret.blue);

    return ret;
}

void DBusLedInterface::setColor(const Color color)
{
    qqInfo() << "setting the color profile of" << toString()
             << "to" << color;

    DBusException::checkInRange("Color", m_colorMax_,
                                color.red, color.green, color.blue);
    setPropertyAndCheck("Color", color);
}

quint8 DBusLedInterface::getColorDepth_() const
{
     return getPropertyAndCheck<quint8>("ColorDepth");
}

DBusLedInterface::ColorDepth DBusLedInterface::getColorDepth() const
{
     return static_cast<ColorDepth>(getColorDepth_());
}

quint16 DBusLedInterface::getEffectDuration() const
{
    auto ret = getPropertyAndCheck<quint16>("EffectDuration");
    DBusException::checkInRange("EffectDuration", 10'000_us, ret);

    return ret;
}

void DBusLedInterface::setEffectDuration(const quint16 effect)
{
    qqInfo() << "setting the effect duration of" << toString()
             << "to" << effect;

    DBusException::checkInRange("EffectDuration", 10'000_us, effect);
    setPropertyAndCheck("EffectDuration", effect);
}

quint8 DBusLedInterface::getBrightness() const
{
    return getPropertyAndCheck<quint8>("Brightness");
}

void DBusLedInterface::setBrightness(const quint8 brightness)
{
    qqInfo() << "setting the brightness of" << toString()
             << "to" << brightness;

    setPropertyAndCheck("Brightness", brightness);
}
