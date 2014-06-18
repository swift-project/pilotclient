/* Copyright (C) 2014 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file

#ifndef BLACKMISC_TUPLE_H
#define BLACKMISC_TUPLE_H

#include "tuple_private.h"

/*!
 * \defgroup Tuples Tuple conversion of object data members
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
 *                (the letter o followed by dot). Can also use any types or functions inherited from TupleConverterBase.
 * \par     Example
 *              If class Things::MyThing has data members m_first, m_second, and m_third:
 *              \code
 *              BLACK_DEFINE_TUPLE_CONVERSION(Things::MyThing, (o.m_first, o.m_second, o.m_third))
 *              \endcode
 *              To disable m_third from participating in hash value generation:
 *              \code
 *              BLACK_DEFINE_TUPLE_CONVERSION(Things::MyThing, (o.m_first, o.m_second, attr(o.m_third, flags<DisabledForHash>())))
 *              \endcode
 * \see     BLACK_DECLARE_TUPLE_CONVERSION_TEMPLATE If T is a template, use this instead.
 * \hideinitializer
 * \ingroup Tuples
 */
#define BLACK_DECLARE_TUPLE_CONVERSION(T, MEMBERS)                              \
    namespace BlackMisc                                                         \
    {                                                                           \
        template <> class TupleConverter<T> : TupleConverterBase                \
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
            static auto toMetaTuple(const T &o) -> decltype(BlackMisc::tieMeta MEMBERS) \
            {                                                                   \
                auto tu = BlackMisc::tieMeta MEMBERS;                           \
                return tu;                                                      \
            }                                                                   \
            static auto toMetaTuple(T &o) -> decltype(BlackMisc::tieMeta MEMBERS) \
            {                                                                   \
                auto tu = BlackMisc::tieMeta MEMBERS;                           \
                return tu;                                                      \
            }                                                                   \
            static const Parser &parser()                                       \
            {                                                                   \
                static const Parser p(#MEMBERS);                                \
                return p;                                                       \
            }                                                                   \
            static const QStringList &jsonMembers()                             \
            {                                                                   \
                return parser().m_names;                                        \
            }                                                                   \
        public:                                                                 \
            static auto constToTuple(const T &o) -> decltype(BlackMisc::tie MEMBERS) \
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
        template <class... U> class TupleConverter<T<U...>> : TupleConverterBase \
        {                                                                       \
            friend class T<U...>;                                               \
            static_assert(Private::HasEnabledTupleConversion<T<U...>>::value,   \
                          "Missing BLACK_ENABLE_TUPLE_CONVERSION macro in " #T); \
            static auto toTuple(const T<U...> &o) -> decltype(BlackMisc::tie MEMBERS) \
            {                                                                   \
                return BlackMisc::tie MEMBERS;                                  \
            }                                                                   \
            static auto toTuple(T<U...> &o) -> decltype(BlackMisc::tie MEMBERS) \
            {                                                                   \
                return BlackMisc::tie MEMBERS;                                  \
            }                                                                   \
            static auto toMetaTuple(const T<U...> &o) -> decltype(BlackMisc::tieMeta MEMBERS) \
            {                                                                   \
                auto tu = BlackMisc::tieMeta MEMBERS;                           \
                return tu;                                                      \
            }                                                                   \
            static auto toMetaTuple(T<U...> &o) -> decltype(BlackMisc::tieMeta MEMBERS) \
            {                                                                   \
                auto tu = BlackMisc::tieMeta MEMBERS;                           \
                return tu;                                                      \
            }                                                                   \
            static const Parser &parser()                                       \
            {                                                                   \
                static const Parser p(#MEMBERS);                                \
                return p;                                                       \
            }                                                                   \
            static const QStringList &jsonMembers()                             \
            {                                                                   \
                return parser().m_names;                                        \
            }                                                                   \
        public:                                                                 \
            static auto constToTuple(const T<U...> &o) -> decltype(BlackMisc::tie MEMBERS) \
            {                                                                   \
                return BlackMisc::tie MEMBERS;                                  \
            }                                                                   \
        };                                                                      \
    }

namespace BlackMisc
{

    /*!
     * \brief   Base class for TupleConverter<T>.
     * \details Defines common types and functions which can be used inside the BLACK_DECLARE_TUPLE_CONVERSION() macro.
     * \ingroup Tuples
     */
    class TupleConverterBase
    {
    public:
        //! \brief   Metadata flags attached to tuple elements.
        enum Flags
        {
            DisabledForComparison = 1 << 0,     //!< Element will be ignored by compare(), but not by operators
            DisabledForMarshalling = 1 << 1,    //!< Element will be ignored during DBus marshalling
            DisabledForDebugging = 1 << 2,      //!< Element will be ignored when streaming to QDebug
            DisabledForHashing = 1 << 3,        //!< Element will be ignored by qHash()
            DisabledForJson = 1 << 4            //!< Element will be ignored during JSON serialization
        };

    protected:
        //! \brief   A shorthand alias for passing flags as a compile-time constant.
        template <quint64 F = 0>
        using flags = std::integral_constant<quint64, F>;

        //! \brief   Create a tuple element with default metadata.
        template <class T>
        static Private::Attribute<T> attr(T &obj)
        {
            return { obj };
        }

        //! \brief   Create a tuple element with attached metadata.
        template <class T, quint64 F>
        static Private::Attribute<T, F> attr(T &obj, std::integral_constant<quint64, F>)
        {
            return { obj };
        }

        //! \brief   Helper class which parses the stringified macro argument.
        struct Parser
        {
            Parser(QString); //!< Constructor.
            QStringList m_raw; //!< The raw macro argument, split by top-level commas.
            QStringList m_names; //!< The names of the tuple members, stripped of any o.m_ prefix.
        };
    };

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
    template <class T> class TupleConverter : private TupleConverterBase
    {
        // BLACK_DECLARE_TUPLE_CONVERSION generates an explicit specialization of TupleConverter,
        // so this unspecialized template will only be used if the macro is missing. It is also
        // a good place to put Doxygen comments to document the API of the macro-generated specializations.

        static_assert(std::is_void<T>::value, // always false; is_void<> trick is just to make the condition dependent on the template parameter T
                      "Missing BLACK_DECLARE_TUPLE_CONVERSION macro for T");

    public:
        /*!
         * \name    Static Private Member Functions
         * \brief   Returns a tuple of references to object's data members listed in BLACK_DECLARE_TUPLE_CONVERSION().
         *          Can be used like <CODE> std::tie </CODE>.
         */
        //! @{
        static std::tuple<> toTuple(const T &object);
        static std::tuple<> toTuple(T &object);
        static std::tuple<> constToTuple(const T &object);
        //! @}

        /*!
         * \name    Static Private Member Functions
         * \brief   Returns a tuple of structs, each of which contains a reference to one of object's data members and its attched metadata.
         */
        //! @{
        static std::tuple<> toMetaTuple(const T &object);
        static std::tuple<> toMetaTuple(T &object);
        //! @}

        /*!
         * \name    Static Private Member Functions
         * \brief   Returns an object with information extracted from the stringified macro argument.
         */
        static const Parser &parser();

        /*!
         * \name    Static Private Member Functions
         * \brief   Returns a list of the names of the tuple members.
         */
        static const QStringList &jsonMembers();
    };

    // Needed so that our qHash overload doesn't hide the qHash overloads in the global namespace.
    // This will be safe as long as no global qHash has the same signature as ours.
    // Alternative would be to qualify all our invokations of the global qHash as ::qHash.
    using ::qHash;

    /*!
     * \brief   Works like std::tie, returning a tuple of references to just the values, with metadata removed.
     * \ingroup Tuples
     */
    template <class... Ts>
    auto tie(Ts &&... args) -> decltype(std::make_tuple(Private::tieHelper(args)...))
    {
        return std::make_tuple(Private::tieHelper(args)...);
    }

    /*!
     * \brief   Works like std::tie, returning a tuple of objects, each of which contains metadata plus a reference to a value.
     * \ingroup Tuples
     */
    template <class... Ts>
    auto tieMeta(Ts &&... args) -> decltype(std::make_tuple(Private::tieMetaHelper(args)...))
    {
        return std::make_tuple(Private::tieMetaHelper(args)...);
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
        auto valuesA = Private::stripMeta(a, Private::make_index_sequence<sizeof...(Ts)>());
        auto valuesB = Private::stripMeta(b, Private::make_index_sequence<sizeof...(Ts)>());
        auto metaTu = Private::recoverMeta(a, Private::make_index_sequence<sizeof...(Ts)>());
        return Private::TupleHelper::compare(valuesA, valuesB, Private::skipFlaggedIndices<TupleConverterBase::DisabledForComparison>(metaTu));
    }

    /*!
     * \brief   Marshall the elements of a tuple into a QDBusArgument.
     * \ingroup Tuples
     */
    template <class... Ts>
    QDBusArgument &operator <<(QDBusArgument &arg, std::tuple<Ts...> tu)
    {
        auto valueTu = Private::stripMeta(tu, Private::make_index_sequence<sizeof...(Ts)>());
        auto metaTu = Private::recoverMeta(tu, Private::make_index_sequence<sizeof...(Ts)>());
        return Private::TupleHelper::marshall(arg, valueTu, Private::skipFlaggedIndices<TupleConverterBase::DisabledForMarshalling>(metaTu));
    }

    /*!
     * \brief   Unmarshall a QDBusArgument into the elements of a tuple.
     * \ingroup Tuples
     */
    template <class... Ts>
    const QDBusArgument &operator >>(const QDBusArgument &arg, std::tuple<Ts...> tu)
    {
        auto valueTu = Private::stripMeta(tu, Private::make_index_sequence<sizeof...(Ts)>());
        auto metaTu = Private::recoverMeta(tu, Private::make_index_sequence<sizeof...(Ts)>());
        return Private::TupleHelper::unmarshall(arg, valueTu, Private::skipFlaggedIndices<TupleConverterBase::DisabledForMarshalling>(metaTu));
    }

    /*!
     * \brief   Stream a tuple to qDebug.
     * \ingroup Tuples
     */
    template <class... Ts>
    QDebug operator <<(QDebug debug, std::tuple<Ts &...> tu)
    {
        auto valueTu = Private::stripMeta(tu, Private::make_index_sequence<sizeof...(Ts)>());
        auto metaTu = Private::recoverMeta(tu, Private::make_index_sequence<sizeof...(Ts)>());
        return Private::TupleHelper::debug(debug, valueTu, Private::skipFlaggedIndices<TupleConverterBase::DisabledForDebugging>(metaTu));
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
        auto valueTu = Private::stripMeta(tu, Private::make_index_sequence<sizeof...(Ts)>());
        auto metaTu = Private::recoverMeta(tu, Private::make_index_sequence<sizeof...(Ts)>());
        return Private::TupleHelper::hash(valueTu, Private::skipFlaggedIndices<TupleConverterBase::DisabledForHashing>(metaTu));
    }

    /*!
     * \brief   Convert to a JSON object
     * \ingroup Tuples
     */
    template <class... Ts>
    QJsonObject serializeJson(const QStringList &members, std::tuple<Ts...> tu)
    {
        QJsonObject json;
        auto valueTu = Private::stripMeta(tu, Private::make_index_sequence<sizeof...(Ts)>());
        auto metaTu = Private::recoverMeta(tu, Private::make_index_sequence<sizeof...(Ts)>());
        Private::TupleHelper::serializeJson(json, members, valueTu, Private::skipFlaggedIndices<TupleConverterBase::DisabledForJson>(metaTu));
        return json;
    }

    /*!
     * \brief   Convert from JSON to object
     * \ingroup Tuples
     */
    template <class... Ts>
    void deserializeJson(const QJsonObject &json, const QStringList &members, std::tuple<Ts...> tu)
    {
        auto valueTu = Private::stripMeta(tu, Private::make_index_sequence<sizeof...(Ts)>());
        auto metaTu = Private::recoverMeta(tu, Private::make_index_sequence<sizeof...(Ts)>());
        Private::TupleHelper::deserializeJson(json, members, valueTu, Private::skipFlaggedIndices<TupleConverterBase::DisabledForJson>(metaTu));
    }

} // namespace BlackMisc

#endif // guard
