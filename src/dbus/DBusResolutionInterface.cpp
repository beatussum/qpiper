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


#include "dbus/DBusResolutionInterface.hpp"

#include <QtCore/QtDebug>
#include <QtDBus/QDBusMetaType>


/*#####################################*/
/*           debug operators           */
/*#####################################*/

QDebug operator<<(QDebug debug, const Axes axes)
{
    debug.nospace().noquote() << static_cast<QString>(axes);

    return debug.space();
}

QDebug operator<<(QDebug debug, const Resolution res)
{
    debug.nospace().noquote() << static_cast<QString>(res);

    return debug.space();
}

QDebug operator<<(QDebug debug, const QVector<Resolution> res)
{
    debug.nospace();

    const auto size = res.size();
    for (int i = 0; i < size; ++i) {
        debug << res.at(i).m_axis_;

        switch (size - i) {
            case 2:
                debug << " and ";
                break;
            case 1:
                break;
            default:
                debug << ", ";
                break;
        }
    }

    return debug;
}


/*#####################################*/
/*    marshallers and demarshallers    */
/*#####################################*/

// Resolution with the format (x, y) not tested
QDBusArgument& operator<<(QDBusArgument& arg, const Axes axes)
{
    qDebug() << "marshalling" << axes;

    arg.beginStructure();
    arg << static_cast<quint32>(axes.x) << static_cast<quint32>(axes.y);
    arg.endStructure();

    qDebug() << axes << "marshalled";
    return arg;
}

// Resolution with the format (x, y) not tested
const QDBusArgument& operator>>(const QDBusArgument& arg, Axes& axes)
{
    qDebug() << "demarshalling" << axes;

    arg.beginStructure();
    axes.x = static_cast<quint16>(qdbus_cast<quint32>(arg));
    axes.y = static_cast<quint16>(qdbus_cast<quint32>(arg));
    arg.endStructure();

    qDebug() << axes << "demarshalled";
    return arg;
}


/*######################################*/
/*                 Axes                 */
/*######################################*/

Axes::operator QString() const
{
    return '(' % QString::number(x) % ", " % QString::number(y) % ')';
}


/*######################################*/
/*              Resolution              */
/*######################################*/

QString Resolution::translate() const
{
    switch (m_type_) {
        case Null:
            return QObject::tr("null");
        case Axis:
            return QObject::tr("%1\u00A0dpi").arg(m_axis_);
        case Axes:
            return QObject::tr("%1\u00A0dpi (for the x-axis) and "
                               "%2\u00A0dpi (for the y-axis)")
                   .arg(m_axes_.x).arg(m_axes_.y);
    }

    Q_UNREACHABLE();
}

Resolution::operator QString() const
{
    switch (m_type_) {
        case Axis:
            return '(' % QString::number(m_axis_) % ')';
        case Axes:
            return static_cast<QString>(m_axes_);
        case Null:
            return typeToString(Null);
    }

    Q_UNREACHABLE();
}

std::pair<Resolution, Resolution> Resolution::tie_() const
{
    return std::make_pair(m_axes_.x, m_axes_.y);
}

bool operator==(const Resolution a, const Resolution b)
{
    bool ret = false;

    if (a.m_type_ == b.m_type_) {
        switch (a.m_type_) {
            case Resolution::Axis:
                ret = (a.m_axis_ == b.m_axis_);
                break;
            case Resolution::Axes:
                ret = (a.m_axes_ == b.m_axes_);
                break;
            case Resolution::Null:
                ret = true;
                break;
        }
    }

    qDebug() << a << (ret ? "==" : "!=") << b;
    return ret;
}


/*#####################################*/
/*       DBusResolutionInterface       */
/*#####################################*/

void DBusResolutionInterface::checkResolution(const Resolution res) const
{
    nqDebug() << "checking whether resolution no.\u00A0"
              << getIndex() << " is valid";

    bool a = (res.m_type_ == m_type_);

    if (a) {
        switch (res.m_type_) {
            case Resolution::Axis:
                a = m_supportedResolutions_.contains(res);
                break;
            case Resolution::Axes:
                {
                    const auto [x, y] = res.tie_();
                    a = (m_supportedResolutions_.contains(x)
                          && m_supportedResolutions_.contains(y));
                }
                break;
            case Resolution::Null:
                break;
        }
    } else {
        throw BadResolution(res, res.m_type_, m_type_);
    }

    if (!a)
        throw BadResolution(res);

    nqDebug() << "resolution no.\u00A0" << getIndex() << " is valid";
}

QVector<quint32> DBusResolutionInterface::getSupportedResolutions_() const
{
    return getPropertyAndCheck<QVector<quint32>>("Resolutions");
}

DBusResolutionInterface::DBusResolutionInterface(const QDBusObjectPath& obj)
    : IDBusIndexableInterface("Resolution", obj)
    , m_type_(getResolution(true).m_type_)
{
    qDBusRegisterMetaType<Axes>();

    for (const auto i : getSupportedResolutions_())
        m_supportedResolutions_.append(static_cast<quint16>(i));

    qInfo() << "the mouse supports resolutions:" << m_supportedResolutions_;
    qInfo() << "the mouse supports resolutions with the form"
            << Resolution::typeToString(m_type_);
}

QDBusVariant DBusResolutionInterface::getResolution_() const
{
    return getPropertyAndCheck<QDBusVariant>("Resolution");
}

Resolution DBusResolutionInterface::getResolution(const bool assumed) const
{
    nqDebug() << "getting the value of resolution no.\u00A0"
              << getIndex();
    Resolution ret;

    switch (const auto& var = getResolution_().variant(); var.type()) {
        case QVariant::UInt:
            ret = static_cast<quint16>(var.toUInt());
            break;
        // Resolution with the format (x, y) not tested
        case QVariant::UserType:
            ret = qdbus_cast<Axes>(var);
            break;
        default:
            Q_UNREACHABLE();
            break;
    }

    if (!assumed) {
        checkResolution(ret);
    } else {
        qDebug() << "this value is assumed and therefore no check will be "
                    "performed because `assumed = true`";
    }

    nqDebug() << "the value of resolution no.\u00A0"
              << getIndex() << " get: let " << ret;
    return ret;
}

void DBusResolutionInterface::setResolution_(const QDBusVariant& res)
{
    setPropertyAndCheck("Resolution", res);
}

void DBusResolutionInterface::setResolution(const Resolution res)
{
    nqDebug() << "setting resolution no.\u00A0" << getIndex()
              << " to " << res;
    checkResolution(res);
    QDBusVariant var;

    switch (res.m_type_) {
        case Resolution::Axis:
            var.setVariant(QVariant::fromValue(QDBusVariant(static_cast<quint32>(res.m_axis_))));
            break;
        // Resolution with the format (x, y) not tested
        case Resolution::Axes:
            var.setVariant(QVariant::fromValue(QDBusVariant(QVariant::fromValue(res.m_axes_))));
            break;
        default:
            Q_UNREACHABLE();
            break;
    }

    setResolution_(var);
    nqDebug() << "resolution no.\u00A0" << getIndex()
              << " set to " << res;
}

bool DBusResolutionInterface::isDefault() const
{
    return getPropertyAndCheck<bool>("IsDefault");
}

void DBusResolutionInterface::setDefault()
{
    nqDebug() << "setting resolution no.\u00A0" << getIndex()
              << " by default";
    callAndCheck("SetDefault");
    nqDebug() << "resolution no.\u00A0" << getIndex()
              << " set by default";
}

bool DBusResolutionInterface::isActive() const
{
    return getPropertyAndCheck<bool>("IsActive");
}

void DBusResolutionInterface::setActive()
{
    nqDebug() << "activating resolution no.\u00A0" << getIndex();
    callAndCheck("SetActive");
    nqDebug() << "resolution no.\u00A0" << getIndex() << " activated";
}
