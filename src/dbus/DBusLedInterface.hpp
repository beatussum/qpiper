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

#include "dbus/IDBusIndexableInterface.hpp"


class QDBusArgument;


struct Color final
{
    quint32 red;
    quint32 green;
    quint32 blue;
};
Q_DECLARE_METATYPE(Color)


class DBusLedInterface final : public IDBusIndexableInterface
{
    Q_OBJECT
    Q_PROPERTY(quint32 Mode READ getMode_ WRITE setMode_)
    Q_PROPERTY(Color Color READ getColor WRITE setColor)
    Q_PROPERTY(quint32 ColorDepth READ getColorDepth_)
    Q_PROPERTY(quint32 EffectDuration READ getEffectDuration WRITE setEffectDuration)
    Q_PROPERTY(quint32 Brightness READ getBrightness WRITE setBrightness)

public:
    enum class Mode : quint8 {
        Off = 0,
        Constant = 1,
        Cycles = 2,
        Breathing = 3
    };
    Q_ENUM(Mode)

    enum class ColorDepth : quint8 {
        ZeroBits = 0,
        EightBits = 1,
        OneBit = 2
    };

private:
    quint8 getMode_() const;
    void setMode_(const quint8 mode);

    quint8 getColorDepth_() const;

    quint8 getBitsNumber_() const;
public:
    explicit DBusLedInterface(const QDBusObjectPath& obj);

    static QString getModeDescription(const Mode mode);
    static QString getColorDepthDescription(const ColorDepth depth);

    Mode getMode() const;
    void setMode(const Mode mode);

    Color getColor() const;
    void setColor(const Color color);

    ColorDepth getColorDepth() const;

    quint16 getEffectDuration() const;
    void setEffectDuration(const quint16 duration);

    quint8 getBrightness() const;
    void setBrightness(const quint8 brightness);
private:
    const quint32 m_colorMax_;
};


QDebug operator<<(QDebug debug, const DBusLedInterface::Mode mode);
QDebug operator<<(QDebug debug, const Color& color);

QDBusArgument& operator<<(QDBusArgument& arg, const Color& color);
const QDBusArgument& operator>>(const QDBusArgument& arg, Color& color);


#endif // QPIPER_DBUS_LED_INTERFACE_HPP
