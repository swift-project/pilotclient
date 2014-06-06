/* Copyright (C) 2014 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_TUPLE_H
#define BLACKMISC_TUPLE_H

#include "tuple_private.h"

/*!
 * \defgroup Tuples Tuples Simplified handling of class members (CValueObject) by std::tuple
 */

/*!
 * \brief   Macro to make a class available to TupleConverter.
 * \details Put this macro anywhere in the private section of a class declaration to make it usable in TupleConverter.
 * \param   T The name of the class.
 * \hideinitializer
 * \ingroup Tuples
 */
#define BLACK_ENABLE_TUPLE_CONVERSION(T)                    \
    public: typedef std::true_type EnabledTupleConversion;  \
    private: friend class BlackMisc::TupleConverter<T>;

/*!
 * \brief   Macro to make a class known to TupleConverter.
 * \details Put this macro outside of any namespace, in the same header as T, to make it usable in TupleConverter.
 * \param   T The fully qualified name of the class.
 * \param   MEMBERS A parenthesized, comma-separated list of the data members of T, with each member prefixed by "o."
 *                (the letter o followed by dot).
 * \par     Example
 *              If class Things::MyThing has data members m_first, m_second, and m_third:
 *              \code
 *              BLACK_DEFINE_TUPLE_CONVERSION(Things::MyThing, (o.m_first, o.m_second, o.m_third))
 *              \endcode
 * \see     BLACK_DECLARE_TUPLE_CONVERSION_TEMPLATE If T is a template, use this instead.
 * \hideinitializer
 * \ingroup Tuples
 */
#define BLACK_DECLARE_TUPLE_CONVERSION(T, MEMBERS)                              \
    namespace BlackMisc                                                         \
    {                                                                           \
        template <> class TupleConverter<T>                                     \
        {                                                                       \
            friend class T;                                                     \
            static_assert(Private::HasEnabledTupleConversion<T>::value,         \
                          "Missing BLACK_ENABLE_TUPLE_CONVERSION macro in " #T); \
            static auto toTuple(const T &o) -> decltype(BlackMisc::tie MEMBERS) \
            {                                                                   \
                return BlackMisc::tie MEMBERS;                                  \
            }                                                                   \
            static auto toTuple(T &o) -> decltype(BlackMisc::tie MEMBERS)       \
            {                                                                   \
                return BlackMisc::tie MEMBERS;                                  \
            }                                                                   \
            static const QStringList &jsonMembers()                             \
            {                                                                   \
                static QStringList members = QString(#MEMBERS).replace("tie(","").replace("(","").replace(")","").replace(" ","").replace("o.","").split(","); \
                return members;                                                 \
            }                                                                   \
        public:                                                                 \
            static auto constToTuple(const T &o) -> decltype(BlackMisc::tie MEMBERS)    \
            {                                                                   \
                return BlackMisc::tie MEMBERS;                                  \
            }                                                                   \
        };                                                                      \
    }

/*!
 * \brief   Same as BLACK_DECLARE_TUPLE_CONVERSION(), but T can be a class template.
 * \hideinitializer
 * \ingroup Tuples
 */
#define BLACK_DECLARE_TUPLE_CONVERSION_TEMPLATE(T, MEMBERS)                     \
    namespace BlackMisc                                                         \
    {                                                                           \
        template <class U> class TupleConverter<T<U>>                           \
        {                                                                       \
            friend class T<U>;                                                  \
            static_assert(Private::HasEnabledTupleConversion<T<U>>::value,      \
                          "Missing BLACK_ENABLE_TUPLE_CONVERSION macro in " #T);          \
            static auto toTuple(const T<U> &o) -> decltype(BlackMisc::tie MEMBERS)  \
            {                                                                   \
                return BlackMisc::tie MEMBERS;                                  \
            }                                                                   \
            static auto toTuple(T<U> &o) -> decltype(BlackMisc::tie MEMBERS)    \
            {                                                                   \
                return BlackMisc::tie MEMBERS;                                  \
            }                                                                   \
            static const QStringList &jsonMembers()                             \
            {                                                                   \
                static QStringList members = QString(#MEMBERS).replace("tie(","").replace("(","").replace(")","").replace(" ","").replace("o.","").split(","); \
                return members;                                                 \
            }                                                                   \
        public:                                                                 \
            static auto constToTuple(const T<U> &o) -> decltype(BlackMisc::tie MEMBERS) \
            {                                                                   \
                return BlackMisc::tie MEMBERS;                                  \
            }                                                                   \
        };                                                                      \
    }

namespace BlackMisc
{

    /*!
     * \brief   Class template for converting class objects to tuples
     * \details If a class T uses the BLACK_ENABLE_TUPLE_CONVERSION() and BLACK_DECLARE_TUPLE_CONVERSION() macros, and <CODE>object</CODE>
     *          is an instance of T, then <CODE> TupleConverter<T>::toTuple(object) </CODE> will return a std::tuple representing object.
     *          This tuple can then be passed to a generic function like one of those listed under See Also:
     * \see     BlackMisc::compare(), BlackMisc::qHash(), BlackMisc::operator<<(), BlackMisc::operator>>(), std::operator==().
     * \remarks The toTuple() function is a private member of TupleConverter, and the class T is declared to be its friend.
     * \nosubgrouping
     * \ingroup Tuples
     */
    template <class T> class TupleConverter
    {
        // BLACK_DECLARE_TUPLE_CONVERSION generates an explicit specialization of TupleConverter,
        // so this unspecialized template will only be used if the macro is missing. It is also
        // a good place to put Doxygen comments to document the API of the macro-generated specializations.
    public:
        /*!
         * \name    Static Private Member Functions
         * \brief   Returns a tuple of references to object's data members as they were declared in BLACK_DECLARE_TUPLE_CONVERSION().
         *          Can be used like <CODE> std::tie </CODE>.
         */
        //! @{
        static std::tuple<> toTuple(const T &object)
        {
            static_assert(std::is_void<T>::value, // always false; is_void<> trick is just to make the condition dependent on the template parameter T
                          "Missing BLACK_DECLARE_TUPLE_CONVERSION macro for T");
            Q_UNUSED(object);
            return std::tuple<>();
        }
        static std::tuple<> toTuple(T &object)
        {
            static_assert(std::is_void<T>::value, // always false; is_void<> trick is just to make the condition dependent on the template parameter T
                          "Missing BLACK_DECLARE_TUPLE_CONVERSION macro for T");
            Q_UNUSED(object);
            return std::tuple<>();
        }
        static std::tuple<> constToTuple(const T &object)
        {
            return toTuple(object);
        }
        static const QStringList jsonMembers()
        {
            static_assert(std::is_void<T>::value, // always false; is_void<> trick is just to make the condition dependent on the template parameter T
                          "Missing BLACK_DECLARE_TUPLE_CONVERSION macro for T");
            static QStringList members;
            return members;
        }
        //! @}
    };

    // Needed so that our qHash overload doesn't hide the qHash overloads in the global namespace.
    // This will be safe as long as no global qHash has the same signature as ours.
    // Alternative would be to qualify all our invokations of the global qHash as ::qHash.
    using ::qHash;

    /*!
     * \brief   Works like std::tie, and allows us to hook in our own customizations.
     */
    template <class... Ts>
    auto tie(Ts &&... args) -> decltype(std::make_tuple(Private::tieHelper(args)...))
    {
        return std::make_tuple(Private::tieHelper(args)...);
    }

    /*!
     * \brief   Lexicographical tuple comparison function which is CValueObject-aware.
     * \details Tuple members which are CValueObjects are compared using the compare() friend function of CValueObject;
     *          other tuple members are compared using operator< and operator>.
     * \ingroup Tuples
     */
    template <class... Ts>
    int compare(std::tuple<Ts...> a, std::tuple<Ts...> b)
    {
        return Private::TupleHelper<sizeof...(Ts)>::compare(a, b);
    }

    /*!
     * \brief   Marshall the elements of a tuple into a QDBusArgument.
     * \ingroup Tuples
     */
    template <class... Ts>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<Ts...> tu)
    {
        return Private::TupleHelper<sizeof...(Ts)>::marshall(arg, tu);
    }

    /*!
     * \brief   Unmarshall a QDBusArgument into the elements of a tuple.
     * \ingroup Tuples
     */
    template <class... Ts>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<Ts...> tu)
    {
        return Private::TupleHelper<sizeof...(Ts)>::unmarshall(arg, tu);
    }

    /*!
     * \brief   Stream a tuple to qDebug.
     * \ingroup Tuples
     */
    template <class... Ts>
    QDebug operator <<(QDebug debug, std::tuple<Ts &...> tu)
    {
        return Private::TupleHelper<sizeof...(Ts)>::debug(debug, tu);
    }

    /*!
     * \brief   Stream a tuple to qDebug.
     * \ingroup Tuples
     */
    template <class... Ts>
    QNoDebug operator <<(QNoDebug noDebug, std::tuple<Ts &...> tu)
    {
        Q_UNUSED(tu);
        return noDebug;
    }

    /*!
     * \brief   Generate a hash value from the elements of a tuple.
     * \ingroup Tuples
     */
    template <class... Ts>
    uint qHash(std::tuple<Ts...> tu)
    {
        return Private::TupleHelper<sizeof...(Ts)>::hash(tu);
    }

    /*!
     * \brief   Convert to a JSON object
     * \ingroup Tuples
     */
    template <class... Ts>
    QJsonObject serializeJson(const QStringList &members, std::tuple<Ts...> tu)
    {
        QJsonObject json;
        Private::TupleHelper<sizeof...(Ts)>::serializeJson(json, members, tu);
        return json;
    }

    /*!
     * Convert from JSON to object
     * \ingroup Tuples
     */
    template <class... Ts>
    void deserializeJson(const QJsonObject &json, const QStringList &members, std::tuple<Ts...> tu)
    {
        Private::TupleHelper<sizeof...(Ts)>::deserializeJson(json, members, tu);
    }

} // namespace BlackMisc

#endif // guard
