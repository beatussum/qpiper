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

/**
 * @file core/core.hpp
 * @brief Some core utils
 */


#ifndef QPIPER_CORE_HPP
#define QPIPER_CORE_HPP

#include <memory>

#include <QtCore/QLatin1String>
#include <QtCore/QMetaEnum>


#define nqDebug() qDebug().nospace()
#define qqDebug() qDebug().noquote()
#define nqInfo() qInfo().nospace()
#define qqInfo() qInfo().noquote()

#define qStrL(str) QStringLiteral(str)
#define qByteL(ba) QByteArrayLiteral(ba)

/**
 * @brief Allow cast from `QVariant` with a `private` default
 * constructor
 *
 * @param T the type in question
 */
#define ENABLE_QVARIANT(T)                                         \
friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<T, true>; \
friend struct QtPrivate::QVariantValueHelper<T>;                   \
                                                                   \
private:                                                           \
T() = default;

template<class T>
using Shared = std::shared_ptr<T>;

/**
 * @brief Plural of `std::is_same::value`
 */
template <typename T, typename... U>
constexpr bool are_same = std::conjunction_v<std::is_same<T, U>...>;

/**
 * @struct is_comparable core/core.hpp
 * @brief The type used as a compile-time boolean with `true` value if \p T
 * is comparable
 */
template<typename T, typename = void>
struct is_comparable : std::false_type {};

template<typename T>
struct is_comparable<T,
        typename std::enable_if<
            true,
            decltype(std::declval<T>() > std::declval<T>(), static_cast<void>(0))
        >::type>
    : std::true_type
{};


/**
 * @brief `true` if the output should be colored
 */
extern const bool isColoredOutput;

constexpr quint16 operator "" _us(const quint64 value)
    { return static_cast<quint16>(value); }

QLatin1String operator "" _qls(const char* value, const std::size_t size);

/**
 * @brief Check the value of an environment variable
 *
 * @param name  the name of the environment variable without its prefix:
 *              e.g. _VAR_ not _QPIPER_VAR_
 * @param value the value to be tested
 *
 * @return true if the value of \p name is equal to \p value
 */
bool qPiperGetEnv(const char* name, const QByteArray value);

/**
 * @brief Contain some ANSI escape code
 */
namespace AnsiColor
{
    /**
     * @brief Reset
     */
    inline const QLatin1String rst = "\u001B[0m"_qls;

    /**
     * @brief About the foreground
     */
    namespace fg
    {
        /**
         * @brief Bold
         */
        inline const QLatin1String b = "\u001B[1m"_qls;
        /**
         * @brief Bold red
         */
        inline const QLatin1String bred = "\u001B[1;31m"_qls;
        /**
         * @brief Bold green
         */
        inline const QLatin1String bgreen = "\u001B[1;32m"_qls;
    }

    /**
     * @brief About the background
     */
    namespace bg
    {
        /**
         * @brief Bright yellow
         */
        inline const QLatin1String bryellow = "\u001B[103m"_qls;
        /**
         * @brief Bright blue
         */
        inline const QLatin1String brblue = "\u001B[104m"_qls;
        /**
         * @brief Bright cyan
         */
        inline const QLatin1String brcyan = "\u001B[106m"_qls;
    }
}

void qPiperMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

/**
 * @brief Get the name of an enumeration constant
 *
 * @tparam T     an enumeration type
 * @param  value the value of the constant
 *
 * @return the name of the constant
 */
template<typename T>
constexpr const char* enumToString(const T value)
{
    return QMetaEnum::fromType<T>().valueToKey(static_cast<int>(value));
}


#endif // QPIPER_CORE_HPP
