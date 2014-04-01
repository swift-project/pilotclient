#ifndef BLACKMISC_DBUS_H
#define BLACKMISC_DBUS_H

#include <QDBusArgument>
#include <type_traits>

/*!
 * Non-member non-friend operator for streaming enums to QDBusArgument.
 *
 * \param argument
 * \param enumType
 * \return
 * \remarks Currently outside namespace for OSX build, see https://dev.vatsim-germany.org/issues/184
 */
template <class ENUM> typename std::enable_if<std::is_enum<ENUM>::value, QDBusArgument>::type const &
operator>>(const QDBusArgument &argument, ENUM &enumType)
{
    uint e;
    argument >> e;
    enumType = static_cast<ENUM>(e);
    return argument;
}
#endif // guard
