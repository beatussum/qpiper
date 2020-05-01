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

#include <QDBusMetaType>


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

    return arg;
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

QString DBusLedInterface::getColorDepthDescription(const ColorDepths color)
{
    switch (color) {
        case ZeroBits:
            return QObject::tr("Zero bits per color: e.g. monochrome.");
        case EightBits:
            return QObject::tr("Eight bits per color.");
        case OneBit:
            return QObject::tr("One bit per color.");
    }
}

DBusLedInterface::DBusLedInterface(const QString& obj)
    : DBusIndexableInterface("Led", obj)
{
    qDBusRegisterMetaType<Color>();
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
     return getPropertyAndCheck<Color>("Color");
}

void DBusLedInterface::setColor(const Color color)
{
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

    if (ret >= 10'000) {
        throw DBusPropertyException("EffectDuration",
                                    "Not in the range [0;10'000]");
    } else {
        return ret;
    }
}

void DBusLedInterface::setEffectDuration(const quint32 effect)
{
    if (effect >= 10'000) {
        throw DBusPropertyException("EffectDuration",
                                    "Not in the range [0;10'000]");
    } else {
        setPropertyAndCheck<quint32>("EffectDuration", effect);
    }
}

quint32 DBusLedInterface::getBrightness() const
{
    return getPropertyAndCheck<quint32>("Brightness");
}

void DBusLedInterface::setBrightness(const quint32 brightness)
{
    setPropertyAndCheck<quint32>("Brightness", brightness);
}
