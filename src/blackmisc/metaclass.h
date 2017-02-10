/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_METACLASS_H
#define BLACKMISC_METACLASS_H

#include "blackmisc/metaclassprivate.h"
#include "blackmisc/invoke.h"

/*!
 * \defgroup MetaClass Metaclass system
 * Compile-time reflection toolkit for iterating over
 * members of value classes.
 */

/*!
 * Macro to define a nested metaclass that describes the attributes of its
 * enclosing class. Use in the private section of the class.
 *
 * \tparam CLASS The name of the class containing the member.
 * \note A semicolon is needed at the end.
 * \ingroup MetaClass
 */
#define BLACK_METACLASS(CLASS, ...)                                         \
    friend struct BlackMisc::Private::CMetaClassAccessor;                   \
    struct MetaClass : public BlackMisc::CMetaClass                         \
    {                                                                       \
        using Class = CLASS;                                                \
        BLACK_NO_EXPORT_CONSTEXPR static auto getMemberList()               \
        {                                                                   \
            return makeMetaMemberList(__VA_ARGS__);                         \
        }                                                                   \
    }

/*!
 * Macro to define an element within a metaclass.
 *
 * Additional arguments can be supplied in the variadic part, which will be
 * forwarded to CMetaClass::makeMetaMember.
 *
 * \tparam MEMBER The name of the member without m_ part.
 * \see BLACK_METACLASS
 * \see BLACK_METAMEMBER_NAMED
 * \see BlackMisc::CMetaClass::makeMetaMember
 * \ingroup MetaClass
 */
#define BLACK_METAMEMBER(MEMBER, ...)                                       \
    makeMetaMember(                                                         \
        &Class::m_##MEMBER, #MEMBER BLACK_TRAILING_VA_ARGS(__VA_ARGS__)     \
    )

/*!
 * Same as BLACK_METAMEMBER but the second parameter is a string literal
 * containing the JSON name of the member.
 *
 * \ingroup MetaClass
 */
#define BLACK_METAMEMBER_NAMED(MEMBER, NAME, ...)                           \
    makeMetaMember(                                                         \
        &Class::m_##MEMBER, NAME BLACK_TRAILING_VA_ARGS(__VA_ARGS__)        \
    )

namespace BlackMisc
{

    class CVariant;

    /*!
     * Type wrapper for passing MetaFlag to CMetaClassIntrospector::with and CMetaClassIntrospector::without.
     * \ingroup MetaClass
     */
    template <quint64 F>
    struct MetaFlags : public std::integral_constant<quint64, F>
    {
        //! Implicit conversion to std::false_type (if F is zero) or std::true_type (if F is non-zero).
        constexpr operator std::integral_constant<bool, static_cast<bool>(F)>() const { return {}; }
    };

    /*!
     * Compile-time union of MetaFlags.
     * \ingroup MetaClass
     */
    template <quint64 A, quint64 B>
    constexpr MetaFlags<A | B> operator |(MetaFlags<A>, MetaFlags<B>) { return {}; }

    /*!
     * Compile-time intersection of MetaFlags.
     * \ingroup MetaClass
     */
    template <quint64 A, quint64 B>
    constexpr MetaFlags<A & B> operator &(MetaFlags<A>, MetaFlags<B>) { return {}; }

    /*!
     * Literal aggregate type representing attributes of one member of a value class.
     * \ingroup MetaClass
     */
    template <typename M, quint64 Flags>
    struct CMetaMember
    {
        //! Pointer to the member.
        const M m_ptr;

        //! Member name.
        const char *const m_name;

        //! Property index of the member.
        //! \deprecated Reserved for future use.
        const int m_index;

        //! Any flags applying to the member.
        const MetaFlags<Flags> m_flags;

        //! True if m_flags contains all flags.
        template <typename Flags2>
        constexpr bool has(Flags2 flags) const { return (m_flags & flags) == flags; }

        //! Invoke the member on an instance of the value class.
        template <typename T, typename... Ts>
        decltype(auto) in(T &&object, Ts &&... args) const
        {
            return Private::invoke(m_ptr, std::forward<T>(object), std::forward<Ts>(args)...);
        }

        //! Return name as QLatin1String.
        Q_DECL_CONSTEXPR auto latin1Name() const { return QLatin1String(m_name); }
    };

    /*!
     * Literal aggregate type representing attributes of the members of a value class.
     * \ingroup MetaClass
     */
    template <typename... Members>
    struct CMetaMemberList
    {
        //! Tuple of CMetaMember.
        const Private::tuple<Members...> m_members;

        //! Number of members.
        static constexpr size_t c_size = sizeof...(Members);

        //! Convenience method returning the member at index I.
        template <size_t I>
        constexpr auto at(std::integral_constant<size_t, I> = {}) const BLACK_TRAILING_RETURN(Private::get<I>(m_members))
        {
            return Private::get<I>(m_members);
        }
    };

    /*!
     * Metadata flags attached to members of a meta class.
     * \ingroup MetaClass
     */
    enum MetaFlag
    {
        DisabledForComparison = 1 << 0,     //!< Element will be ignored by compare() and comparison operators
        DisabledForMarshalling = 1 << 1,    //!< Element will be ignored during DBus marshalling
        DisabledForDebugging = 1 << 2,      //!< Element will be ignored when streaming to QDebug
        DisabledForHashing = 1 << 3,        //!< Element will be ignored by qHash()
        DisabledForJson = 1 << 4,           //!< Element will be ignored during JSON serialization
        CaseInsensitiveComparison = 1 << 5  //!< Element will be compared case insensitively (must be a QString)
    };

    /*!
     * Base class for meta classes.
     * Just static protected members to be used by derived meta classes.
     * \ingroup MetaClass
     */
    class CMetaClass
    {
    protected:
        //! Flags wrapped as compile-time constants.
        //! @{
        constexpr static MetaFlags<MetaFlag::DisabledForComparison> DisabledForComparison {};
        constexpr static MetaFlags<MetaFlag::DisabledForMarshalling> DisabledForMarshalling {};
        constexpr static MetaFlags<MetaFlag::DisabledForDebugging> DisabledForDebugging {};
        constexpr static MetaFlags<MetaFlag::DisabledForHashing> DisabledForHashing {};
        constexpr static MetaFlags<MetaFlag::DisabledForJson> DisabledForJson {};
        constexpr static MetaFlags<MetaFlag::CaseInsensitiveComparison> CaseInsensitiveComparison {};
        //! @}

        //! Return a CMetaMemberList of type deduced from the types of the meta members.
        //! Usually not used directly, but via the macros.
        template <typename... Members>
        constexpr static CMetaMemberList<Members...> makeMetaMemberList(Members... members)
        {
            return { Private::tuple<Members...>(members...) };
        }

        //! Return a CMetaMethod of type deduced from the type of the member.
        //! Usually not used directly, but via the macros.
        template <typename M, quint64 Flags = 0>
        constexpr static CMetaMember<M, Flags> makeMetaMember(M ptrToMember, const char *name = nullptr, int index = 0, MetaFlags<Flags> flags = {})
        {
            static_assert(std::is_member_object_pointer<M>::value, "M must be a pointer to member object");
            return { ptrToMember, name, index, flags };
        }
    };

    /*!
     * Implementation of an introspector for the metaclass of T.
     * Obtain an instance of this class via BlackMisc::introspect.
     * \ingroup MetaClass
     */
    template <typename T, typename MetaClass, size_t... Is>
    class CMetaClassIntrospector
    {
    public:
        //! Return a CMetaClassIntrospector<T> covering only those members which have the given flags.
        //! \see BlackMisc::MetaFlags
        template <typename Flags>
        static auto with(Flags) { return filter(MaskSequence<(members().at(index<Is>()).has(Flags()))...>()); }

        //! Return a CMetaClassIntrospector<T> covering only those members which do not have the given flags.
        //! \see BlackMisc::MetaFlags
        template <typename Flags>
        static auto without(Flags) { return filter(MaskSequence<(! members().at(index<Is>()).has(Flags()))...>()); }

        //! For each metamember in metaclass, pass metamember as argument to visitor function.
        template <typename F>
        static void forEachMember(F &&visitor)
        {
            // parameter pack swallow idiom
            static_cast<void>(std::initializer_list<int>
            {
                (static_cast<void>(std::forward<F>(visitor)(members().at(index<Is>()))), 0)...
            });
        }

    private:
        template <bool... Mask>
        using MaskSequence = Private::MaskSequence<Private::index_sequence<Is...>, Mask...>;

        template <size_t... Js>
        static auto filter(Private::index_sequence<Js...>) { return CMetaClassIntrospector<T, MetaClass, Js...>(); }

        template <size_t I>
        using index = std::integral_constant<size_t, I>;

        constexpr static auto members() BLACK_TRAILING_RETURN(MetaClass::getMemberList()) { return MetaClass::getMemberList(); }
    };

    namespace Private
    {
        //! \private Friend class of all value classes, so it can access the private nested class.
        struct CMetaClassAccessor
        {
            template <typename T, size_t... Is>
            static auto getIntrospector(index_sequence<Is...>)
            {
                return CMetaClassIntrospector<T, typename T::MetaClass, Is...>();
            }

            template <typename T>
            static auto getIntrospector()
            {
                return getIntrospector<T>(Private::make_index_sequence<T::MetaClass::getMemberList().c_size>());
            }
        };
    }

    /*!
     * Obtain the CMetaClassIntrospector for the metaclass of T.
     * \return BlackMisc::CMetaClassIntrospector
     * \ingroup MetaClass
     */
    template <typename T>
    auto introspect()
    {
        return Private::CMetaClassAccessor::getIntrospector<T>();
    }

} // namespace

#endif
