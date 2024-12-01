// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_METACLASS_H
#define SWIFT_MISC_METACLASS_H

#include <functional>
#include <type_traits>

#include <QHash>
#include <QLatin1String>
#include <QString>

#include "misc/invoke.h"
#include "misc/tuple.h"

/*!
 * \defgroup MetaClass Metaclass system
 * Compile-time reflection toolkit for iterating over
 * members of value classes.
 */

//! \cond PRIVATE

// Work around MinGW problem with combination of constexpr and extern template
#if defined(Q_OS_WIN) && defined(Q_CC_GNU)
#    define SWIFT_NO_EXPORT_CONSTEXPR constexpr inline __attribute__((always_inline))
#else
#    define SWIFT_NO_EXPORT_CONSTEXPR constexpr
#endif

// MSVC, GCC, Clang all have non-standard extensions for skipping trailing
// commas in variadic macros, but the MSVC extension differs from the others.
#ifdef Q_CC_MSVC
#    define SWIFT_TRAILING_VA_ARGS(...) , __VA_ARGS__
#else
#    define SWIFT_TRAILING_VA_ARGS(...) , ##__VA_ARGS__
#endif

//! \endcond

// *INDENT-OFF*
/*!
 * Macro to define a nested metaclass that describes the attributes of its
 * enclosing class. Use in the private section of the class.
 *
 * \tparam CLASS The name of the class containing the member.
 * \note A semicolon is needed at the end.
 * \ingroup MetaClass
 */
#define SWIFT_METACLASS(CLASS, ...)                                                                                    \
    friend struct swift::misc::private_ns::CMetaClassAccessor;                                                         \
    struct MetaClass : public swift::misc::CMetaClass                                                                  \
    {                                                                                                                  \
        using Class = CLASS;                                                                                           \
        SWIFT_NO_EXPORT_CONSTEXPR static auto getMemberList() { return makeMetaMemberList(__VA_ARGS__); }              \
    }

/*!
 * Macro to define an element within a metaclass.
 *
 * Additional arguments can be supplied in the variadic part, which will be
 * forwarded to CMetaClass::makeMetaMember.
 *
 * \tparam MEMBER The name of the member without m_ part.
 * \see SWIFT_METACLASS
 * \see SWIFT_METAMEMBER_NAMED
 * \see swift::misc::CMetaClass::makeMetaMember
 * \ingroup MetaClass
 */
#define SWIFT_METAMEMBER(MEMBER, ...) makeMetaMember(&Class::m_##MEMBER, #MEMBER SWIFT_TRAILING_VA_ARGS(__VA_ARGS__))

/*!
 * Same as SWIFT_METAMEMBER but the second parameter is a string literal
 * containing the JSON name of the member.
 *
 * \ingroup MetaClass
 */
#define SWIFT_METAMEMBER_NAMED(MEMBER, NAME, ...)                                                                      \
    makeMetaMember(&Class::m_##MEMBER, NAME SWIFT_TRAILING_VA_ARGS(__VA_ARGS__))
// *INDENT-ON*

//! std::string qHash
inline size_t qHash(const std::string &key, uint seed) { return qHash(QString::fromStdString(key), seed); }

//! std::string qHash
inline size_t qHash(const std::string &key) { return qHash(QString::fromStdString(key)); }

namespace swift::misc
{
    class CVariant;

    /*!
     * Simple literal type containing a single QLatin1String.
     *
     * Just useful for encapsulating a QLatin1String in a way that inhibits implicit conversion to QString
     * to avoid ambiguities in overload resolution.
     */
    struct CExplicitLatin1String
    {
        //! Embedded string.
        const QLatin1String m_latin1;

        //! Implicit constructor.
        constexpr CExplicitLatin1String(QLatin1String s) : m_latin1(s) {}

        //! Implicit conversion.
        constexpr operator QLatin1String() const { return m_latin1; }
    };

    /*!
     * Type wrapper for passing MetaFlag to CMetaMember::has.
     * \ingroup MetaClass
     */
    template <quint64 F>
    struct MetaFlags : public std::integral_constant<quint64, F>
    {
        //! Implicit conversion to std::false_type (if F is zero) or std::true_type (if F is non-zero).
        constexpr operator std::bool_constant<static_cast<bool>(F)>() const { return {}; }
    };

    /*!
     * Compile-time union of MetaFlags.
     * \ingroup MetaClass
     */
    template <quint64 A, quint64 B>
    constexpr MetaFlags<A | B> operator|(MetaFlags<A>, MetaFlags<B>)
    {
        return {};
    }

    /*!
     * Compile-time intersection of MetaFlags.
     * \ingroup MetaClass
     */
    template <quint64 A, quint64 B>
    constexpr MetaFlags<A & B> operator&(MetaFlags<A>, MetaFlags<B>)
    {
        return {};
    }

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
        static constexpr bool has(Flags2 flags)
        {
            return (MetaFlags<Flags>() & flags) == flags;
        }

        //! Invoke the member on an instance of the value class.
        template <typename T, typename... Ts>
        decltype(auto) in(T &&object, Ts &&...args) const
        {
            return std::invoke(m_ptr, std::forward<T>(object), std::forward<Ts>(args)...);
        }

        //! Return name as QLatin1String.
        constexpr auto latin1Name() const { return QLatin1String(m_name); }
    };

    /*!
     * Literal aggregate type representing attributes of the members of a value class.
     * \ingroup MetaClass
     */
    template <typename... Members>
    struct CMetaMemberList
    {
        //! Tuple of CMetaMember.
        const private_ns::tuple<Members...> m_members;

        //! Number of members.
        static constexpr size_t c_size = sizeof...(Members);
    };

    /*!
     * Metadata flags attached to members of a meta class.
     * \ingroup MetaClass
     */
    enum MetaFlag
    {
        DisabledForComparison = 1 << 0, //!< Element will be ignored by compare() and comparison operators
        DisabledForMarshalling = 1 << 1, //!< Element will be ignored during DBus and QDataStream marshalling
        DisabledForDebugging = 1 << 2, //!< Element will be ignored when streaming to QDebug
        DisabledForHashing = 1 << 3, //!< Element will be ignored by qHash()
        DisabledForJson = 1 << 4, //!< Element will be ignored during JSON serialization
        CaseInsensitiveComparison = 1 << 5, //!< Element will be compared case insensitively (must be a QString)
        LosslessMarshalling = 1 << 6, //!< Element marshalling will preserve data at the expense of size
        RequiredForJson = 1 << 7 //!< Element is required when parsing from JSON
    };

    /*!
     * Base class for meta classes.
     * Just static protected members to be used by derived meta classes.
     * \ingroup MetaClass
     */
    class CMetaClass
    {
    protected:
        //! @{
        //! Flags wrapped as compile-time constants.
        constexpr static MetaFlags<MetaFlag::DisabledForComparison> DisabledForComparison {};
        constexpr static MetaFlags<MetaFlag::DisabledForMarshalling> DisabledForMarshalling {};
        constexpr static MetaFlags<MetaFlag::DisabledForDebugging> DisabledForDebugging {};
        constexpr static MetaFlags<MetaFlag::DisabledForHashing> DisabledForHashing {};
        constexpr static MetaFlags<MetaFlag::DisabledForJson> DisabledForJson {};
        constexpr static MetaFlags<MetaFlag::CaseInsensitiveComparison> CaseInsensitiveComparison {};
        constexpr static MetaFlags<MetaFlag::LosslessMarshalling> LosslessMarshalling {};
        constexpr static MetaFlags<MetaFlag::RequiredForJson> RequiredForJson {};
        //! @}

        //! Return a CMetaMemberList of type deduced from the types of the meta members.
        //! Usually not used directly, but via the macros.
        template <typename... Members>
        constexpr static CMetaMemberList<Members...> makeMetaMemberList(Members... members)
        {
            return { { { { members }... } } };
        }

        //! Return a CMetaMethod of type deduced from the type of the member.
        //! Usually not used directly, but via the macros.
        template <typename M, quint64 Flags = 0>
        constexpr static CMetaMember<M, Flags> makeMetaMember(M ptrToMember, const char *name = nullptr, int index = 0,
                                                              MetaFlags<Flags> flags = {})
        {
            static_assert(std::is_member_object_pointer_v<M>, "M must be a pointer to member object");
            return { ptrToMember, name, index, flags };
        }
    };

    /*!
     * An introspector for a metaclass.
     * Obtain an instance of this class via swift::misc::introspect.
     * \ingroup MetaClass
     */
    template <typename MetaClass>
    class CMetaClassIntrospector
    {
    public:
        //! For each metamember in metaclass, pass metamember as argument to visitor function.
        template <typename F>
        static void forEachMember(F &&visitor)
        {
            MetaClass::getMemberList().m_members.for_each(std::forward<F>(visitor));
        }
    };

    namespace private_ns
    {
        //! \private Friend class of all value classes, so it can access the private nested class.
        struct CMetaClassAccessor
        {
            template <typename T>
            constexpr static auto getIntrospector()
            {
                return CMetaClassIntrospector<typename T::MetaClass>();
            }
        };
    } // namespace private_ns

    /*!
     * Obtain the CMetaClassIntrospector for the metaclass of T.
     * \return swift::misc::CMetaClassIntrospector
     * \ingroup MetaClass
     */
    template <typename T>
    constexpr auto introspect()
    {
        return private_ns::CMetaClassAccessor::getIntrospector<T>();
    }

} // namespace swift::misc

#endif // SWIFT_MISC_METACLASS_H
