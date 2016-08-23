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
            return CMetaClass::makeMetaMemberList(__VA_ARGS__);             \
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
    CMetaClass::makeMetaMember(                                             \
        &Class::m_##MEMBER, #MEMBER BLACK_TRAILING_VA_ARGS(__VA_ARGS__)     \
    )

/*!
 * Same as BLACK_METAMEMBER but the second parameter is a string literal
 * containing the JSON name of the member.
 *
 * \ingroup MetaClass
 */
#define BLACK_METAMEMBER_NAMED(MEMBER, NAME, ...)                           \
    CMetaClass::makeMetaMember(                                             \
        &Class::m_##MEMBER, NAME BLACK_TRAILING_VA_ARGS(__VA_ARGS__)        \
    )

namespace BlackMisc
{

    class CVariant;

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
     * Type wrapper for passing MetaFlag to CMetaClassIntrospector::with and CMetaClassIntrospector::without.
     * \ingroup MetaClass
     */
    template <quint64 F>
    using MetaFlags = std::integral_constant<quint64, F>;

    /*!
     * Literal aggregate type representing attributes of one member of a value class.
     * \ingroup MetaClass
     */
    template <typename M>
    struct CMetaMember
    {
        //! Pointer to the member.
        const M m_ptr;

        //! Member name.
        const char *const m_name;

        //! Property index of the member.
        //! \todo Not used yet.
        const int m_index;

        //! Any flags applying to the member.
        const quint64 m_flags;

        //! True if m_flags contains Flags.
        template <typename Flags>
        constexpr bool has(Flags) const { return (m_flags & Flags::value) == Flags::value; }

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
     * Base class for meta classes.
     * Just static protected members to be used by derived meta classes.
     * \ingroup MetaClass
     */
    class CMetaClass
    {
    protected:
        //! Return a CMetaMemberList of type deduced from the types of the meta members.
        //! Usually not used directly, but via the macros.
        template <typename... Members>
        constexpr static CMetaMemberList<Members...> makeMetaMemberList(Members... members)
        {
            return { Private::tuple<Members...>(members...) };
        }

        //! Return a CMetaMethod of type deduced from the type of the member.
        //! Usually not used directly, but via the macros.
        template <typename M>
        constexpr static CMetaMember<M> makeMetaMember(M ptrToMember, const char *name = nullptr, int index = 0, quint64 flags = 0)
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

        //! Return a tuple containing references to all members of object.
        //! @{
        static auto toTuple(T &object) { return std::tie((members().at(index<Is>()).in(object))...); }
        static auto toTuple(const T &object) { return std::tie((members().at(index<Is>()).in(object))...); }
        //! @}

        //! For each member in object, pass member as argument to visitor function.
        //! @{
        template <typename F>
        static void forEachMember(T &object, F &&visitor)
        {
            forEachImpl([ & ](auto &&member, auto) { std::forward<F>(visitor)(member.in(object)); });
        }
        template <typename F>
        static void forEachMember(const T &object, F &&visitor)
        {
            forEachImpl([ & ](auto &&member, auto) { std::forward<F>(visitor)(member.in(object)); });
        }
        //! @}

        //! For each member in object pair, pass member pair as arguments to visitor function.
        //! @{
        template <typename F>
        static void forEachMemberPair(T &left, T &right, F &&visitor)
        {
            forEachImpl([ & ](auto &&member, auto flags) { std::forward<F>(visitor)(member.in(left), member.in(right), flags); });
        }
        template <typename F>
        static void forEachMemberPair(const T &left, T &right, F &&visitor)
        {
            forEachImpl([ & ](auto &&member, auto flags) { std::forward<F>(visitor)(member.in(left), member.in(right), flags); });
        }
        template <typename F>
        static void forEachMemberPair(T &left, const T &right, F &&visitor)
        {
            forEachImpl([ & ](auto &&member, auto flags) { std::forward<F>(visitor)(member.in(left), member.in(right), flags); });
        }
        template <typename F>
        static void forEachMemberPair(const T &left, const T &right, F &&visitor)
        {
            forEachImpl([ & ](auto &&member, auto flags) { std::forward<F>(visitor)(member.in(left), member.in(right), flags); });
        }
        //! @}

        //! For each member in object, pass member and its name as arguments to visitor function.
        //! @{
        template <typename F>
        static void forEachMemberName(T &object, F &&visitor)
        {
            forEachImpl([ & ](auto &&member, auto) { std::forward<F>(visitor)(member.in(object), member.latin1Name()); });
        }
        template <typename F>
        static void forEachMemberName(const T &object, F &&visitor)
        {
            forEachImpl([ & ](auto &&member, auto) { std::forward<F>(visitor)(member.in(object), member.latin1Name()); });
        }
        //! @}

    private:
        template <bool... Mask>
        using MaskSequence = Private::MaskSequence<Private::index_sequence<Is...>, Mask...>;

        template <size_t... Js>
        static auto filter(Private::index_sequence<Js...>) { return CMetaClassIntrospector<T, MetaClass, Js...>(); }

        template <size_t I>
        using index = std::integral_constant<size_t, I>;

        constexpr static auto members() BLACK_TRAILING_RETURN(MetaClass::getMemberList()) { return MetaClass::getMemberList(); }

        template <typename F>
        static void forEachImpl(F &&visitor)
        {
            // parameter pack swallow idiom
            static_cast<void>(std::initializer_list<int>
            {
                (static_cast<void>(std::forward<F>(visitor)(members().at(index<Is>()), MetaFlags<members().at(index<Is>()).m_flags>())), 0)...
            });
        }
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
