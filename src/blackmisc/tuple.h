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

#ifdef Q_COMPILER_VARIADIC_TEMPLATES

    /*!
     * \brief   Works like std::tie but with special handling for any argument which are tuples.
     * \details Returns a tuple of references to its arguments which can be used in the same way as
     *          std::tie, except for arguments which are themselves tuples. Arguments which are tuples
     *          are copied into the result tuple by value. This enables nesting of calls to tie within
     *          other calls to tie, to workaround implementations which have a maximum tuple size.
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


#else // !Q_COMPILER_VARIADIC_TEMPLATES

    template <class T0>
    auto tie(T0 &&arg0)
        -> decltype(std::make_tuple(Private::tieHelper(arg0)))
    {
        return std::make_tuple(Private::tieHelper(arg0));
    }

    template <class T0, class T1>
    auto tie(T0 &&arg0, T1 &&arg1)
        -> decltype(std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1)))
    {
        return std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1));
    }

    template <class T0, class T1, class T2>
    auto tie(T0 &&arg0, T1 &&arg1, T2 &&arg2)
        -> decltype(std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1), Private::tieHelper(arg2)))
    {
        return std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1), Private::tieHelper(arg2));
    }

    template <class T0, class T1, class T2, class T3>
    auto tie(T0 &&arg0, T1 &&arg1, T2 &&arg2, T3 &&arg3)
        -> decltype(std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1), Private::tieHelper(arg2), Private::tieHelper(arg3)))
    {
        return std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1), Private::tieHelper(arg2), Private::tieHelper(arg3));
    }

    template <class T0, class T1, class T2, class T3, class T4>
    auto tie(T0 &&arg0, T1 &&arg1, T2 &&arg2, T3 &&arg3, T4 &&arg4)
        -> decltype(std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1), Private::tieHelper(arg2), Private::tieHelper(arg3), Private::tieHelper(arg4)))
    {
        return std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1), Private::tieHelper(arg2), Private::tieHelper(arg3), Private::tieHelper(arg4));
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    auto tie(T0 &&arg0, T1 &&arg1, T2 &&arg2, T3 &&arg3, T4 &&arg4, T5 &&arg5)
        -> decltype(std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1), Private::tieHelper(arg2), Private::tieHelper(arg3), Private::tieHelper(arg4),
                                    Private::tieHelper(arg5)))
    {
        return std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1), Private::tieHelper(arg2), Private::tieHelper(arg3), Private::tieHelper(arg4),
                               Private::tieHelper(arg5));
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    auto tie(T0 &&arg0, T1 &&arg1, T2 &&arg2, T3 &&arg3, T4 &&arg4, T5 &&arg5, T6 &&arg6)
        -> decltype(std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1), Private::tieHelper(arg2), Private::tieHelper(arg3), Private::tieHelper(arg4),
                                    Private::tieHelper(arg5), Private::tieHelper(arg6)))
    {
        return std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1), Private::tieHelper(arg2), Private::tieHelper(arg3), Private::tieHelper(arg4),
                               Private::tieHelper(arg5), Private::tieHelper(arg6));
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    auto tie(T0 &&arg0, T1 &&arg1, T2 &&arg2, T3 &&arg3, T4 &&arg4, T5 &&arg5, T6 &&arg6, T7 &&arg7)
        -> decltype(std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1), Private::tieHelper(arg2), Private::tieHelper(arg3), Private::tieHelper(arg4),
                                    Private::tieHelper(arg5), Private::tieHelper(arg6), Private::tieHelper(arg7)))
    {
        return std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1), Private::tieHelper(arg2), Private::tieHelper(arg3), Private::tieHelper(arg4),
                               Private::tieHelper(arg5), Private::tieHelper(arg6), Private::tieHelper(arg7));
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    auto tie(T0 &&arg0, T1 &&arg1, T2 &&arg2, T3 &&arg3, T4 &&arg4, T5 &&arg5, T6 &&arg6, T7 &&arg7, T8 &&arg8)
        -> decltype(std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1), Private::tieHelper(arg2), Private::tieHelper(arg3), Private::tieHelper(arg4),
                                    Private::tieHelper(arg5), Private::tieHelper(arg6), Private::tieHelper(arg7), Private::tieHelper(arg8)))
    {
        return std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1), Private::tieHelper(arg2), Private::tieHelper(arg3), Private::tieHelper(arg4),
                               Private::tieHelper(arg5), Private::tieHelper(arg6), Private::tieHelper(arg7), Private::tieHelper(arg8));
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    auto tie(T0 &&arg0, T1 &&arg1, T2 &&arg2, T3 &&arg3, T4 &&arg4, T5 &&arg5, T6 &&arg6, T7 &&arg7, T8 &&arg8, T9 &&arg9)
        -> decltype(std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1), Private::tieHelper(arg2), Private::tieHelper(arg3), Private::tieHelper(arg4),
                                    Private::tieHelper(arg5), Private::tieHelper(arg6), Private::tieHelper(arg7), Private::tieHelper(arg8), Private::tieHelper(arg9)))
    {
        return std::make_tuple(Private::tieHelper(arg0), Private::tieHelper(arg1), Private::tieHelper(arg2), Private::tieHelper(arg3), Private::tieHelper(arg4),
                               Private::tieHelper(arg5), Private::tieHelper(arg6), Private::tieHelper(arg7), Private::tieHelper(arg8), Private::tieHelper(arg9));
    }

    inline int compare(std::tuple<> a, std::tuple<> b)
    {
        return Private::TupleHelper<0>::compare(a, b);
    }

    template <class T0>
    int compare(std::tuple<T0> a, std::tuple<T0> b)
    {
        return Private::TupleHelper<1>::compare(a, b);
    }

    template <class T0, class T1>
    int compare(std::tuple<T0, T1> a, std::tuple<T0, T1> b)
    {
        return Private::TupleHelper<2>::compare(a, b);
    }

    template <class T0, class T1, class T2>
    int compare(std::tuple<T0, T1, T2> a, std::tuple<T0, T1, T2> b)
    {
        return Private::TupleHelper<3>::compare(a, b);
    }

    template <class T0, class T1, class T2, class T3>
    int compare(std::tuple<T0, T1, T2, T3> a, std::tuple<T0, T1, T2, T3> b)
    {
        return Private::TupleHelper<4>::compare(a, b);
    }

    template <class T0, class T1, class T2, class T3, class T4>
    int compare(std::tuple<T0, T1, T2, T3, T4> a, std::tuple<T0, T1, T2, T3, T4> b)
    {
        return Private::TupleHelper<5>::compare(a, b);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    int compare(std::tuple<T0, T1, T2, T3, T4, T5> a, std::tuple<T0, T1, T2, T3, T4, T5> b)
    {
        return Private::TupleHelper<6>::compare(a, b);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    int compare(std::tuple<T0, T1, T2, T3, T4, T5, T6> a, std::tuple<T0, T1, T2, T3, T4, T5, T6> b)
    {
        return Private::TupleHelper<7>::compare(a, b);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    int compare(std::tuple<T0, T1, T2, T3, T4, T5, T6, T7> a, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7> b)
    {
        return Private::TupleHelper<8>::compare(a, b);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    int compare(std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> a, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> b)
    {
        return Private::TupleHelper<9>::compare(a, b);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    int compare(std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> a, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> b)
    {
        return Private::TupleHelper<10>::compare(a, b);
    }

    inline QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<> tu)
    {
        return Private::TupleHelper<0>::marshall(arg, tu);
    }

    template <class T0>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0> tu)
    {
        return Private::TupleHelper<1>::marshall(arg, tu);
    }

    template <class T0, class T1>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0, T1> tu)
    {
        return Private::TupleHelper<2>::marshall(arg, tu);
    }

    template <class T0, class T1, class T2>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0, T1, T2> tu)
    {
        return Private::TupleHelper<3>::marshall(arg, tu);
    }

    template <class T0, class T1, class T2, class T3>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0, T1, T2, T3> tu)
    {
        return Private::TupleHelper<4>::marshall(arg, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0, T1, T2, T3, T4> tu)
    {
        return Private::TupleHelper<5>::marshall(arg, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0, T1, T2, T3, T4, T5> tu)
    {
        return Private::TupleHelper<6>::marshall(arg, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0, T1, T2, T3, T4, T5, T6> tu)
    {
        return Private::TupleHelper<7>::marshall(arg, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7> tu)
    {
        return Private::TupleHelper<8>::marshall(arg, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> tu)
    {
        return Private::TupleHelper<9>::marshall(arg, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> tu)
    {
        return Private::TupleHelper<10>::marshall(arg, tu);
    }

    inline const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<> tu)
    {
        return Private::TupleHelper<0>::unmarshall(arg, tu);
    }

    template <class T0>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0> tu)
    {
        return Private::TupleHelper<1>::unmarshall(arg, tu);
    }

    template <class T0, class T1>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0, T1> tu)
    {
        return Private::TupleHelper<2>::unmarshall(arg, tu);
    }

    template <class T0, class T1, class T2>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0, T1, T2> tu)
    {
        return Private::TupleHelper<3>::unmarshall(arg, tu);
    }

    template <class T0, class T1, class T2, class T3>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0, T1, T2, T3> tu)
    {
        return Private::TupleHelper<4>::unmarshall(arg, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0, T1, T2, T3, T4> tu)
    {
        return Private::TupleHelper<5>::unmarshall(arg, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0, T1, T2, T3, T4, T5> tu)
    {
        return Private::TupleHelper<6>::unmarshall(arg, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0, T1, T2, T3, T4, T5, T6> tu)
    {
        return Private::TupleHelper<7>::unmarshall(arg, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7> tu)
    {
        return Private::TupleHelper<8>::unmarshall(arg, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> tu)
    {
        return Private::TupleHelper<9>::unmarshall(arg, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> tu)
    {
        return Private::TupleHelper<10>::unmarshall(arg, tu);
    }

    inline QDebug operator <<(QDebug debug, std::tuple<> tu)
    {
        return Private::TupleHelper<0>::debug(debug, tu);
    }

    template <class T0>
    QDebug operator <<(QDebug debug, std::tuple<T0> tu)
    {
        return Private::TupleHelper<1>::debug(debug, tu);
    }

    template <class T0, class T1>
    QDebug operator <<(QDebug debug, std::tuple<T0, T1> tu)
    {
        return Private::TupleHelper<2>::debug(debug, tu);
    }

    template <class T0, class T1, class T2>
    QDebug operator <<(QDebug debug, std::tuple<T0, T1, T2> tu)
    {
        return Private::TupleHelper<3>::debug(debug, tu);
    }

    template <class T0, class T1, class T2, class T3>
    QDebug operator <<(QDebug debug, std::tuple<T0, T1, T2, T3> tu)
    {
        return Private::TupleHelper<4>::debug(debug, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4>
    QDebug operator <<(QDebug debug, std::tuple<T0, T1, T2, T3, T4> tu)
    {
        return Private::TupleHelper<5>::debug(debug, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    QDebug operator <<(QDebug debug, std::tuple<T0, T1, T2, T3, T4, T5> tu)
    {
        return Private::TupleHelper<6>::debug(debug, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    QDebug operator <<(QDebug debug, std::tuple<T0, T1, T2, T3, T4, T5, T6> tu)
    {
        return Private::TupleHelper<7>::debug(debug, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    QDebug operator <<(QDebug debug, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7> tu)
    {
        return Private::TupleHelper<8>::debug(debug, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    QDebug operator <<(QDebug debug, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> tu)
    {
        return Private::TupleHelper<9>::debug(debug, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    QDebug operator <<(QDebug debug, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> tu)
    {
        return Private::TupleHelper<10>::debug(debug, tu);
    }

    inline QNoDebug operator <<(QNoDebug noDebug, std::tuple<>)
    {
        return noDebug;
    }

    template <class T0>
    QNoDebug operator <<(QNoDebug noDebug, std::tuple<T0>)
    {
        return noDebug;
    }

    template <class T0, class T1>
    QNoDebug operator <<(QNoDebug noDebug, std::tuple<T0, T1>)
    {
        return noDebug;
    }

    template <class T0, class T1, class T2>
    QNoDebug operator <<(QNoDebug noDebug, std::tuple<T0, T1, T2>)
    {
        return noDebug;
    }

    template <class T0, class T1, class T2, class T3>
    QNoDebug operator <<(QNoDebug noDebug, std::tuple<T0, T1, T2, T3>)
    {
        return noDebug;
    }

    template <class T0, class T1, class T2, class T3, class T4>
    QNoDebug operator <<(QNoDebug noDebug, std::tuple<T0, T1, T2, T3, T4>)
    {
        return noDebug;
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    QNoDebug operator <<(QNoDebug noDebug, std::tuple<T0, T1, T2, T3, T4, T5>)
    {
        return noDebug;
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    QNoDebug operator <<(QNoDebug noDebug, std::tuple<T0, T1, T2, T3, T4, T5, T6>)
    {
        return noDebug;
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    QNoDebug operator <<(QNoDebug noDebug, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7>)
    {
        return noDebug;
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    QNoDebug operator <<(QNoDebug noDebug, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8>)
    {
        return noDebug;
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    QNoDebug operator <<(QNoDebug noDebug, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>)
    {
        return noDebug;
    }

    inline void deserializeJson(const QJsonObject &json, const QStringList &members, std::tuple<> tu)
    {
        Private::TupleHelper<0>::deserializeJson(json, members, tu);
    }

    template <class T0>
    void deserializeJson(const QJsonObject &json, const QStringList &members, std::tuple<T0> tu)
    {
        Private::TupleHelper<1>::deserializeJson(json, members, tu);
    }

    template <class T0, class T1>
    void deserializeJson(const QJsonObject &json, const QStringList &members, std::tuple<T0, T1> tu)
    {
        Private::TupleHelper<2>::deserializeJson(json, members, tu);
    }

    template <class T0, class T1, class T2>
    void deserializeJson(const QJsonObject &json, const QStringList &members, std::tuple<T0, T1, T2> tu)
    {
        Private::TupleHelper<3>::deserializeJson(json, members, tu);
    }

    template <class T0, class T1, class T2, class T3>
    void deserializeJson(const QJsonObject &json, const QStringList &members, std::tuple<T0, T1, T2, T3> tu)
    {
        Private::TupleHelper<4>::deserializeJson(json, members, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4>
    void deserializeJson(const QJsonObject &json, const QStringList &members, std::tuple<T0, T1, T2, T3, T4> tu)
    {
        Private::TupleHelper<5>::deserializeJson(json, members, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    void deserializeJson(const QJsonObject &json, const QStringList &members, std::tuple<T0, T1, T2, T3, T4, T5> tu)
    {
        Private::TupleHelper<6>::deserializeJson(json, members, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    void deserializeJson(const QJsonObject &json, const QStringList &members, std::tuple<T0, T1, T2, T3, T4, T5, T6> tu)
    {
        Private::TupleHelper<7>::deserializeJson(json, members, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    void deserializeJson(const QJsonObject &json, const QStringList &members, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7> tu)
    {
        Private::TupleHelper<8>::deserializeJson(json, members, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    void deserializeJson(const QJsonObject &json, const QStringList &members, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> tu)
    {
        Private::TupleHelper<9>::deserializeJson(json, members, tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    void deserializeJson(const QJsonObject &json, const QStringList &members, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> tu)
    {
        Private::TupleHelper<10>::deserializeJson(json, members, tu);
    }

    inline uint qHash(std::tuple<> tu)
    {
        return Private::TupleHelper<0>::hash(tu);
    }

    template <class T0>
    uint qHash(std::tuple<T0> tu)
    {
        return Private::TupleHelper<1>::hash(tu);
    }

    template <class T0, class T1>
    uint qHash(std::tuple<T0, T1> tu)
    {
        return Private::TupleHelper<2>::hash(tu);
    }

    template <class T0, class T1, class T2>
    uint qHash(std::tuple<T0, T1, T2> tu)
    {
        return Private::TupleHelper<3>::hash(tu);
    }

    template <class T0, class T1, class T2, class T3>
    uint qHash(std::tuple<T0, T1, T2, T3> tu)
    {
        return Private::TupleHelper<4>::hash(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4>
    uint qHash(std::tuple<T0, T1, T2, T3, T4> tu)
    {
        return Private::TupleHelper<5>::hash(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    uint qHash(std::tuple<T0, T1, T2, T3, T4, T5> tu)
    {
        return Private::TupleHelper<6>::hash(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    uint qHash(std::tuple<T0, T1, T2, T3, T4, T5, T6> tu)
    {
        return Private::TupleHelper<7>::hash(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    uint qHash(std::tuple<T0, T1, T2, T3, T4, T5, T6, T7> tu)
    {
        return Private::TupleHelper<8>::hash(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    uint qHash(std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> tu)
    {
        return Private::TupleHelper<9>::hash(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    uint qHash(std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> tu)
    {
        return Private::TupleHelper<9>::hash(tu);
    }

    inline QJsonObject serializeJson(const QStringList &members, std::tuple<> tu)
    {
        QJsonObject json;
        Private::TupleHelper<0>::serializeJson(json, members, tu);
        return json;
    }

    template <class T0>
    QJsonObject serializeJson(const QStringList &members, std::tuple<T0> tu)
    {
        QJsonObject json;
        Private::TupleHelper<1>::serializeJson(json, members, tu);
        return json;
    }

    template <class T0, class T1>
    QJsonObject serializeJson(const QStringList &members, std::tuple<T0, T1> tu)
    {
        QJsonObject json;
        Private::TupleHelper<2>::serializeJson(json, members, tu);
        return json;
    }

    template <class T0, class T1, class T2>
    QJsonObject serializeJson(const QStringList &members, std::tuple<T0, T1, T2> tu)
    {
        QJsonObject json;
        Private::TupleHelper<3>::serializeJson(json, members, tu);
        return json;
    }

    template <class T0, class T1, class T2, class T3>
    QJsonObject serializeJson(const QStringList &members, std::tuple<T0, T1, T2, T3> tu)
    {
        QJsonObject json;
        Private::TupleHelper<4>::serializeJson(json, members, tu);
        return json;
    }

    template <class T0, class T1, class T2, class T3, class T4>
    QJsonObject serializeJson(const QStringList &members, std::tuple<T0, T1, T2, T3, T4> tu)
    {
        QJsonObject json;
        Private::TupleHelper<5>::serializeJson(json, members, tu);
        return json;
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    QJsonObject serializeJson(const QStringList &members, std::tuple<T0, T1, T2, T3, T4, T5> tu)
    {
        QJsonObject json;
        Private::TupleHelper<6>::serializeJson(json, members, tu);
        return json;
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    QJsonObject serializeJson(const QStringList &members, std::tuple<T0, T1, T2, T3, T4, T5, T6> tu)
    {
        QJsonObject json;
        Private::TupleHelper<7>::serializeJson(json, members, tu);
        return json;
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    QJsonObject serializeJson(const QStringList &members, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7> tu)
    {
        QJsonObject json;
        Private::TupleHelper<8>::serializeJson(json, members, tu);
        return json;
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    QJsonObject serializeJson(const QStringList &members, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> tu)
    {
        QJsonObject json;
        Private::TupleHelper<9>::serializeJson(json, members, tu);
        return json;
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    QJsonObject serializeJson(const QStringList &members, std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> tu)
    {
        QJsonObject json;
        Private::TupleHelper<10>::serializeJson(json, members, tu);
        return json;
    }

#endif // Q_COMPILER_VARIADIC_TEMPLATES

} // namespace BlackMisc

#endif // guard
