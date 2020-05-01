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


#ifndef QPIPER_DBUS_LED_INTERFACE_HPP
#define QPIPER_DBUS_LED_INTERFACE_HPP

#include "dbus/DBusIndexableInterface.hpp"


struct Color
{
    quint32 red;
    quint32 green;
    quint32 blue;
};

Q_DECLARE_METATYPE(Color)
QDBusArgument& operator<<(QDBusArgument& arg, const Color& color);
const QDBusArgument& operator>>(const QDBusArgument& arg, Color& color);


class DBusLedInterface : public DBusIndexableInterface
{
    Q_OBJECT
    Q_PROPERTY(quint32 Mode READ getMode_ WRITE setMode_)
    Q_PROPERTY(Color Color READ getColor WRITE setColor)
    Q_PROPERTY(quint32 ColorDepth READ getColorDepth_)
    Q_PROPERTY(quint32 EffectDuration READ getEffectDuration WRITE setEffectDuration)
    Q_PROPERTY(quint32 Brightness READ getBrightness WRITE setBrightness)

public:
    enum Modes {
        Off = 0,
        Constant = 1,
        Cycles = 2,
        Breathing = 3
    };

    enum ColorDepths {
        ZeroBits = 0,
        EightBits = 1,
        OneBit = 2
    };

public:
    explicit DBusLedInterface(const QString& obj);

    static QString getModeDescription(const Modes mode);
    static QString getColorDepthDescription(const ColorDepths color);

    Modes getMode() const;
    void setMode(const Modes mode);

    Color getColor() const;
    void setColor(const Color color);

    ColorDepths getColorDepth() const;

    quint32 getEffectDuration() const;
    void setEffectDuration(const quint32 duration);

    quint32 getBrightness() const;
    void setBrightness(const quint32 brightness);
private:
    quint32 getMode_() const;
    void setMode_(const quint32 mode);

    quint32 getColorDepth_() const;
};


#endif // QPIPER_DBUS_LED_INTERFACE_HPP
