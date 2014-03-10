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
            static_assert(Private::HasEnabledTupleConversion<T>::type::value,   \
                "Missing BLACK_ENABLE_TUPLE_CONVERSION macro in " #T);          \
            static auto toTuple(const T &o) -> decltype(std::tie MEMBERS)       \
            {                                                                   \
                return std::tie MEMBERS;                                        \
            }                                                                   \
            static auto toTuple(T &o) -> decltype(std::tie MEMBERS)             \
            {                                                                   \
                return std::tie MEMBERS;                                        \
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
            static_assert(Private::HasEnabledTupleConversion<T<U>>::type::value,\
                "Missing BLACK_ENABLE_TUPLE_CONVERSION macro in " #T);          \
            static auto toTuple(const T<U> &o) -> decltype(std::tie MEMBERS)    \
            {                                                                   \
                return std::tie MEMBERS;                                        \
            }                                                                   \
            static auto toTuple(T<U> &o) -> decltype(std::tie MEMBERS)          \
            {                                                                   \
                return std::tie MEMBERS;                                        \
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
        //! @}
    };

    // Needed so that our qHash overload doesn't hide the qHash overloads in the global namespace.
    // This will be safe as long as no global qHash has the same signature as ours.
    // Alternative would be to qualify all our invokations of the global qHash as ::qHash.
    using ::qHash;

#ifdef Q_COMPILER_VARIADIC_TEMPLATES

    /*!
     * \brief   Lexicographical tuple comparison function which is CValueObject-aware.
     * \details Tuple members which are CValueObjects are compared using the compare() friend function of CValueObject;
     *          other tuple members are compared using operator< and operator>.
     * \ingroup Tuples
     */
    template <class... Ts>
    int compare(std::tuple<const Ts &...> a, std::tuple<const Ts &...> b)
    {
        return Private::TupleHelper<sizeof...(Ts)>::compare(a, b);
    }

    /*!
     * \brief   Marshall the elements of a tuple into a QDBusArgument.
     * \ingroup Tuples
     */
    template <class... Ts>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<const Ts &...> tu)
    {
        return Private::TupleHelper<sizeof...(Ts)>::marshall(arg, tu);
    }

    /*!
     * \brief   Unmarshall a QDBusArgument into the elements of a tuple.
     * \ingroup Tuples
     */
    template <class... Ts>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<Ts &...> tu)
    {
        return Private::TupleHelper<sizeof...(Ts)>::unmarshall(arg, tu);
    }

    /*!
     * \brief   Generate a hash value from the elements of a tuple.
     * \ingroup Tuples
     */
    template <class... Ts>
    uint qHash(std::tuple<const Ts &...> tu)
    {
        return Private::TupleHelper<sizeof...(Ts)>::hash(tu);
    }

#else // !Q_COMPILER_VARIADIC_TEMPLATES

    inline int compare(std::tuple<>, std::tuple<>)
    {
        return 0;
    }

    template <class T0>
    int compare(std::tuple<T0 &> a, std::tuple<T0 &> b)
    {
        return Private::compareHelper<0>(a, b);
    }

    template <class T0, class T1>
    int compare(std::tuple<T0 &, T1 &> a, std::tuple<T0 &, T1 &> b)
    {
        int result;
        if ((result = Private::compareHelper<0>(a, b))) { return result; }
        return Private::compareHelper<1>(a, b);
    }

    template <class T0, class T1, class T2>
    int compare(std::tuple<T0 &, T1 &, T2 &> a, std::tuple<T0 &, T1 &, T2 &> b)
    {
        int result;
        if ((result = Private::compareHelper<0>(a, b))) { return result; }
        if ((result = Private::compareHelper<1>(a, b))) { return result; }
        return Private::compareHelper<2>(a, b);
    }

    template <class T0, class T1, class T2, class T3>
    int compare(std::tuple<T0 &, T1 &, T2 &, T3 &> a, std::tuple<T0 &, T1 &, T2 &, T3 &> b)
    {
        int result;
        if ((result = Private::compareHelper<0>(a, b))) { return result; }
        if ((result = Private::compareHelper<1>(a, b))) { return result; }
        if ((result = Private::compareHelper<2>(a, b))) { return result; }
        return Private::compareHelper<3>(a, b);
    }

    template <class T0, class T1, class T2, class T3, class T4>
    int compare(std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &> a, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &> b)
    {
        int result;
        if ((result = Private::compareHelper<0>(a, b))) { return result; }
        if ((result = Private::compareHelper<1>(a, b))) { return result; }
        if ((result = Private::compareHelper<2>(a, b))) { return result; }
        if ((result = Private::compareHelper<3>(a, b))) { return result; }
        return Private::compareHelper<4>(a, b);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    int compare(std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &> a, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &> b)
    {
        int result;
        if ((result = Private::compareHelper<0>(a, b))) { return result; }
        if ((result = Private::compareHelper<1>(a, b))) { return result; }
        if ((result = Private::compareHelper<2>(a, b))) { return result; }
        if ((result = Private::compareHelper<3>(a, b))) { return result; }
        if ((result = Private::compareHelper<4>(a, b))) { return result; }
        return Private::compareHelper<5>(a, b);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    int compare(std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &> a, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &> b)
    {
        int result;
        if ((result = Private::compareHelper<0>(a, b))) { return result; }
        if ((result = Private::compareHelper<1>(a, b))) { return result; }
        if ((result = Private::compareHelper<2>(a, b))) { return result; }
        if ((result = Private::compareHelper<3>(a, b))) { return result; }
        if ((result = Private::compareHelper<4>(a, b))) { return result; }
        if ((result = Private::compareHelper<5>(a, b))) { return result; }
        return Private::compareHelper<6>(a, b);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    int compare(std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &, T7 &> a, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &, T7 &> b)
    {
        int result;
        if ((result = Private::compareHelper<0>(a, b))) { return result; }
        if ((result = Private::compareHelper<1>(a, b))) { return result; }
        if ((result = Private::compareHelper<2>(a, b))) { return result; }
        if ((result = Private::compareHelper<3>(a, b))) { return result; }
        if ((result = Private::compareHelper<4>(a, b))) { return result; }
        if ((result = Private::compareHelper<5>(a, b))) { return result; }
        if ((result = Private::compareHelper<6>(a, b))) { return result; }
        return Private::compareHelper<7>(a, b);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    int compare(std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &, T7 &, T8 &> a, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &, T7 &, T8 &> b)
    {
        int result;
        if ((result = Private::compareHelper<0>(a, b))) { return result; }
        if ((result = Private::compareHelper<1>(a, b))) { return result; }
        if ((result = Private::compareHelper<2>(a, b))) { return result; }
        if ((result = Private::compareHelper<3>(a, b))) { return result; }
        if ((result = Private::compareHelper<4>(a, b))) { return result; }
        if ((result = Private::compareHelper<5>(a, b))) { return result; }
        if ((result = Private::compareHelper<6>(a, b))) { return result; }
        if ((result = Private::compareHelper<7>(a, b))) { return result; }
        return Private::compareHelper<8>(a, b);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    int compare(std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &, T7 &, T8 &, T9 &> a, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &, T7 &, T8 &, T9 &> b)
    {
        int result;
        if ((result = Private::compareHelper<0>(a, b))) { return result; }
        if ((result = Private::compareHelper<1>(a, b))) { return result; }
        if ((result = Private::compareHelper<2>(a, b))) { return result; }
        if ((result = Private::compareHelper<3>(a, b))) { return result; }
        if ((result = Private::compareHelper<4>(a, b))) { return result; }
        if ((result = Private::compareHelper<5>(a, b))) { return result; }
        if ((result = Private::compareHelper<6>(a, b))) { return result; }
        if ((result = Private::compareHelper<7>(a, b))) { return result; }
        if ((result = Private::compareHelper<8>(a, b))) { return result; }
        return Private::compareHelper<9>(a, b);
    }

    inline QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<>)
    {
        return arg;
    }

    template <class T0>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0 &> tu)
    {
        return arg << std::get<0>(tu);
    }

    template <class T0, class T1>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0 &, T1 &> tu)
    {
        return arg << std::get<0>(tu) << std::get<1>(tu);
    }

    template <class T0, class T1, class T2>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0 &, T1 &, T2 &> tu)
    {
        return arg << std::get<0>(tu) << std::get<1>(tu) << std::get<2>(tu);
    }

    template <class T0, class T1, class T2, class T3>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0 &, T1 &, T2 &, T3 &> tu)
    {
        return arg << std::get<0>(tu) << std::get<1>(tu) << std::get<2>(tu) << std::get<3>(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &> tu)
    {
        return arg << std::get<0>(tu) << std::get<1>(tu) << std::get<2>(tu) << std::get<3>(tu) << std::get<4>(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &> tu)
    {
        return arg << std::get<0>(tu) << std::get<1>(tu) << std::get<2>(tu) << std::get<3>(tu) << std::get<4>(tu)
                   << std::get<5>(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &> tu)
    {
        return arg << std::get<0>(tu) << std::get<1>(tu) << std::get<2>(tu) << std::get<3>(tu) << std::get<4>(tu)
                   << std::get<5>(tu) << std::get<6>(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &, T7 &> tu)
    {
        return arg << std::get<0>(tu) << std::get<1>(tu) << std::get<2>(tu) << std::get<3>(tu) << std::get<4>(tu)
                   << std::get<5>(tu) << std::get<6>(tu) << std::get<7>(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &, T7 &, T8 &> tu)
    {
        return arg << std::get<0>(tu) << std::get<1>(tu) << std::get<2>(tu) << std::get<3>(tu) << std::get<4>(tu)
                   << std::get<5>(tu) << std::get<6>(tu) << std::get<7>(tu) << std::get<8>(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &, T7 &, T8 &, T9 &> tu)
    {
        return arg << std::get<0>(tu) << std::get<1>(tu) << std::get<2>(tu) << std::get<3>(tu) << std::get<4>(tu)
                   << std::get<5>(tu) << std::get<6>(tu) << std::get<7>(tu) << std::get<8>(tu) << std::get<9>(tu);
    }

    inline const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<>)
    {
        return arg;
    }

    template <class T0>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0 &> tu)
    {
        return arg >> std::get<0>(tu);
    }

    template <class T0, class T1>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0 &, T1 &> tu)
    {
        return arg >> std::get<0>(tu) >> std::get<1>(tu);
    }

    template <class T0, class T1, class T2>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0 &, T1 &, T2 &> tu)
    {
        return arg >> std::get<0>(tu) >> std::get<1>(tu) >> std::get<2>(tu);
    }

    template <class T0, class T1, class T2, class T3>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0 &, T1 &, T2 &, T3 &> tu)
    {
        return arg >> std::get<0>(tu) >> std::get<1>(tu) >> std::get<2>(tu) >> std::get<3>(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &> tu)
    {
        return arg >> std::get<0>(tu) >> std::get<1>(tu) >> std::get<2>(tu) >> std::get<3>(tu) >> std::get<4>(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &> tu)
    {
        return arg >> std::get<0>(tu) >> std::get<1>(tu) >> std::get<2>(tu) >> std::get<3>(tu) >> std::get<4>(tu)
                   >> std::get<5>(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &> tu)
    {
        return arg >> std::get<0>(tu) >> std::get<1>(tu) >> std::get<2>(tu) >> std::get<3>(tu) >> std::get<4>(tu)
                   >> std::get<5>(tu) >> std::get<6>(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &, T7 &> tu)
    {
        return arg >> std::get<0>(tu) >> std::get<1>(tu) >> std::get<2>(tu) >> std::get<3>(tu) >> std::get<4>(tu)
                   >> std::get<5>(tu) >> std::get<6>(tu) >> std::get<7>(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &, T7 &, T8 &> tu)
    {
        return arg >> std::get<0>(tu) >> std::get<1>(tu) >> std::get<2>(tu) >> std::get<3>(tu) >> std::get<4>(tu)
                   >> std::get<5>(tu) >> std::get<6>(tu) >> std::get<7>(tu) >> std::get<8>(tu);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &, T7 &, T8 &, T9 &> tu)
    {
        return arg >> std::get<0>(tu) >> std::get<1>(tu) >> std::get<2>(tu) >> std::get<3>(tu) >> std::get<4>(tu)
                   >> std::get<5>(tu) >> std::get<6>(tu) >> std::get<7>(tu) >> std::get<8>(tu) >> std::get<9>(tu);
    }

    inline uint qHash(std::tuple<>)
    {
        return 0;
    }

    template <class T0>
    uint qHash(std::tuple<T0 &> tu)
    {
        return qHash(std::get<0>(tu));
    }

    template <class T0, class T1>
    uint qHash(std::tuple<T0 &, T1 &> tu)
    {
        return qHash(std::get<0>(tu)) ^ qHash(std::get<1>(tu));
    }

    template <class T0, class T1, class T2>
    uint qHash(std::tuple<T0 &, T1 &, T2 &> tu)
    {
        return qHash(std::get<0>(tu)) ^ qHash(std::get<1>(tu)) ^ qHash(std::get<2>(tu));
    }

    template <class T0, class T1, class T2, class T3>
    uint qHash(std::tuple<T0 &, T1 &, T2 &, T3 &> tu)
    {
        return qHash(std::get<0>(tu)) ^ qHash(std::get<1>(tu)) ^ qHash(std::get<2>(tu)) ^ qHash(std::get<3>(tu));
    }

    template <class T0, class T1, class T2, class T3, class T4>
    uint qHash(std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &> tu)
    {
        return qHash(std::get<0>(tu)) ^ qHash(std::get<1>(tu)) ^ qHash(std::get<2>(tu)) ^ qHash(std::get<3>(tu)) ^ qHash(std::get<4>(tu));
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    uint qHash(std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &> tu)
    {
        return qHash(std::get<0>(tu)) ^ qHash(std::get<1>(tu)) ^ qHash(std::get<2>(tu)) ^ qHash(std::get<3>(tu)) ^ qHash(std::get<4>(tu))
             ^ qHash(std::get<5>(tu));
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    uint qHash(std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &> tu)
    {
        return qHash(std::get<0>(tu)) ^ qHash(std::get<1>(tu)) ^ qHash(std::get<2>(tu)) ^ qHash(std::get<3>(tu)) ^ qHash(std::get<4>(tu))
             ^ qHash(std::get<5>(tu)) ^ qHash(std::get<6>(tu));
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    uint qHash(std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &, T7 &> tu)
    {
        return qHash(std::get<0>(tu)) ^ qHash(std::get<1>(tu)) ^ qHash(std::get<2>(tu)) ^ qHash(std::get<3>(tu)) ^ qHash(std::get<4>(tu))
             ^ qHash(std::get<5>(tu)) ^ qHash(std::get<6>(tu)) ^ qHash(std::get<7>(tu));
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    uint qHash(std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &, T7 &, T8 &> tu)
    {
        return qHash(std::get<0>(tu)) ^ qHash(std::get<1>(tu)) ^ qHash(std::get<2>(tu)) ^ qHash(std::get<3>(tu)) ^ qHash(std::get<4>(tu))
             ^ qHash(std::get<5>(tu)) ^ qHash(std::get<6>(tu)) ^ qHash(std::get<7>(tu)) ^ qHash(std::get<8>(tu));
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    uint qHash(std::tuple<T0 &, T1 &, T2 &, T3 &, T4 &, T5 &, T6 &, T7 &, T8 &, T9 &> tu)
    {
        return qHash(std::get<0>(tu)) ^ qHash(std::get<1>(tu)) ^ qHash(std::get<2>(tu)) ^ qHash(std::get<3>(tu)) ^ qHash(std::get<4>(tu))
             ^ qHash(std::get<5>(tu)) ^ qHash(std::get<6>(tu)) ^ qHash(std::get<7>(tu)) ^ qHash(std::get<8>(tu)) ^ qHash(std::get<9>(tu));
    }

#endif // Q_COMPILER_VARIADIC_TEMPLATES

} // namespace BlackMisc

#endif // guard
