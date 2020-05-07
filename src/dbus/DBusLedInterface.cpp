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
    arg.beginStructure();
    arg << color.red << color.green << color.blue;
    arg.endStructure();

    qDebug() << "marshalling" << color;

    return arg;
}

const QDBusArgument& operator>>(const QDBusArgument& arg, Color& color)
{
    arg.beginStructure();
    arg >> color.red >> color.green >> color.blue;
    arg.endStructure();

    qDebug() << "demarshalling" << color;

    return arg;
}


/*######################################*/
/*           DBusLedInterface           */
/*######################################*/

quint8 DBusLedInterface::getBitsNumber_() const
{
    switch (getColorDepth()) {
        case ColorDepths::ZeroBits:
            return 0;
        case ColorDepths::EightBits:
            return 8;
        case ColorDepths::OneBit:
            return 1;
    }

    Q_UNREACHABLE();
}

DBusLedInterface::DBusLedInterface(const QString& obj)
    : DBusIndexableInterface("Led", obj)
    , m_colorMax_(static_cast<quint32>(qPow(2, getBitsNumber_() * 8)) - 1)
{
    nqInfo() << "colors are " << getBitsNumber_()
             << "-bits color-coded: let [0;"
             << m_colorMax_ << ']';

    qDBusRegisterMetaType<Color>();
}

QString DBusLedInterface::getModeDescription(const Modes mode)
{
    switch (mode) {
        case Modes::Off:
            return QObject::tr("LED is off.");
        case Modes::Constant:
            return QObject::tr("LED is on with constant brightness.");
        case Modes::Cycles:
            return QObject::tr("LED cycles through a set of colors. This "
                               "mode ignores the `Color` values.");
        case Modes::Breathing:
            return QObject::tr("LED uses a breathing-style animation.");
    }

    Q_UNREACHABLE();
}

QString DBusLedInterface::getColorDepthDescription(const ColorDepths depth)
{
    switch (depth) {
        case ColorDepths::ZeroBits:
            return QObject::tr("Zero bits per color: e.g. monochrome.");
        case ColorDepths::EightBits:
            return QObject::tr("Eight bits per color.");
        case ColorDepths::OneBit:
            return QObject::tr("One bit per color.");
    }

    Q_UNREACHABLE();
}

quint8 DBusLedInterface::getMode_() const
{
     return getPropertyAndCheck<quint8>("Mode");
}

void DBusLedInterface::setMode_(const quint8 mode)
{
    setPropertyAndCheck("Mode", mode);
}

DBusLedInterface::Modes DBusLedInterface::getMode() const
{
     return static_cast<Modes>(getMode_());
}

void DBusLedInterface::setMode(const Modes mode)
{
    setMode_(static_cast<quint8>(mode));
}

Color DBusLedInterface::getColor() const
{
    auto ret = getPropertyAndCheck<Color>("Color");
    DBusException::checkInRange("Brightness", m_colorMax_,
                                ret.red, ret.green, ret.blue);

    return ret;
}

void DBusLedInterface::setColor(const Color color)
{
    DBusException::checkInRange("Brightness", m_colorMax_,
                                color.red, color.green, color.blue);
    setPropertyAndCheck("Color", color);
}

quint8 DBusLedInterface::getColorDepth_() const
{
     return getPropertyAndCheck<quint8>("ColorDepth");
}

DBusLedInterface::ColorDepths DBusLedInterface::getColorDepth() const
{
     return static_cast<ColorDepths>(getColorDepth_());
}

quint16 DBusLedInterface::getEffectDuration() const
{
    auto ret = getPropertyAndCheck<quint16>("EffectDuration");
    DBusException::checkInRange("Brightness", 10'000_us, ret);

    return ret;
}

void DBusLedInterface::setEffectDuration(const quint16 effect)
{
    DBusException::checkInRange("Brightness", 10'000_us, effect);
    setPropertyAndCheck("EffectDuration", effect);
}

quint8 DBusLedInterface::getBrightness() const
{
    return getPropertyAndCheck<quint8>("Brightness");
}

void DBusLedInterface::setBrightness(const quint8 brightness)
{
    setPropertyAndCheck("Brightness", brightness);
}
