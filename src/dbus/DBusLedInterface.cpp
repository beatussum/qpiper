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


QDBusArgument& operator<<(QDBusArgument& arg, const Color& color)
{
    arg.beginStructure();
    arg << color.red << color.green << color.blue;
    arg.endStructure();

    return arg;
}

const QDBusArgument& operator>>(const QDBusArgument& arg, Color& color)
{
    arg.beginStructure();
    arg >> color.red >> color.green >> color.blue;
    arg.endStructure();
    nqDebug() << "rgb(" << color.red
              << ", " << color.green
              << ", " << color.blue << ')';

    return arg;
}


DBusLedInterface::DBusLedInterface(const QString& obj)
    : DBusIndexableInterface("Led", obj)
    , m_colorMax_(static_cast<quint32>(qPow(2, getBitsNumber() * 8)) - 1)
{
    nqInfo() << "colors are " << getBitsNumber()
             << "-bits color-coded: let [0;"
             << m_colorMax_ << ']';

    qDBusRegisterMetaType<Color>();
}

QString DBusLedInterface::getModeDescription(const Modes mode)
{
    switch (mode) {
        case Off:
            return QObject::tr("LED is off.");
        case Constant:
            return QObject::tr("LED is on with constant brightness.");
        case Cycles:
            return QObject::tr("LED cycles through a set of colors. This "
                               "mode ignores the `Color` values.");
        case Breathing:
            return QObject::tr("LED uses a breathing-style animation.");
    }
}

QString DBusLedInterface::getColorDepthDescription(const ColorDepths depth)
{
    switch (depth) {
        case ZeroBits:
            return QObject::tr("Zero bits per color: e.g. monochrome.");
        case EightBits:
            return QObject::tr("Eight bits per color.");
        case OneBit:
            return QObject::tr("One bit per color.");
    }
}

quint32 DBusLedInterface::getBitsNumber() const
{
    switch (getColorDepth()) {
        case ZeroBits:
            return 0;
        case EightBits:
            return 8;
        case OneBit:
            return 1;
    }
}

DBusLedInterface::Modes DBusLedInterface::getMode() const
{
     return static_cast<Modes>(getMode_());
}

void DBusLedInterface::setMode(const Modes mode)
{
    setMode_(static_cast<quint32>(mode));
}

quint32 DBusLedInterface::getMode_() const
{
     return getPropertyAndCheck<quint32>("Mode");
}

void DBusLedInterface::setMode_(const quint32 mode)
{
    setPropertyAndCheck("Mode", mode);
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

DBusLedInterface::ColorDepths DBusLedInterface::getColorDepth() const
{
     return static_cast<ColorDepths>(getColorDepth_());
}

quint32 DBusLedInterface::getColorDepth_() const
{
     return getPropertyAndCheck<quint32>("ColorDepth");
}

quint32 DBusLedInterface::getEffectDuration() const
{
    auto ret = getPropertyAndCheck<quint32>("EffectDuration");
    DBusException::checkInRange("Brightness", 10'000u, ret);

    return ret;
}

void DBusLedInterface::setEffectDuration(const quint32 effect)
{
    DBusException::checkInRange("Brightness", 10'000u, effect);
    setPropertyAndCheck<quint32>("EffectDuration", effect);
}

quint32 DBusLedInterface::getBrightness() const
{
    auto ret = getPropertyAndCheck<quint32>("Brightness");
    DBusException::checkInRange("Brightness", 255u, ret);

    return ret;
}

void DBusLedInterface::setBrightness(const quint32 brightness)
{
    DBusException::checkInRange("Brightness", 255u, brightness);
    setPropertyAndCheck<quint32>("Brightness", brightness);
}
