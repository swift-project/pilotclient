// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PROPERTYINDEX_H
#define SWIFT_MISC_PROPERTYINDEX_H

#include <initializer_list>
#include <type_traits>

#include <QList>
#include <QMetaType>
#include <QString>

#include "misc/metaclass.h"
#include "misc/mixin/mixincompare.h"
#include "misc/mixin/mixindatastream.h"
#include "misc/mixin/mixindbus.h"
#include "misc/mixin/mixinhash.h"
#include "misc/mixin/mixinjson.h"
#include "misc/mixin/mixinmetatype.h"
#include "misc/mixin/mixinstring.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/typetraits.h"

namespace swift::misc
{
    /*!
     * Property index. The index can be nested, that's why it is a sequence
     * (e.g. PropertyIndexPilot, PropertyIndexRealname).
     */
    class SWIFT_MISC_EXPORT CPropertyIndex :
        public mixin::MetaType<CPropertyIndex>,
        public mixin::HashByMetaClass<CPropertyIndex>,
        public mixin::DBusByMetaClass<CPropertyIndex>,
        public mixin::DataStreamByMetaClass<CPropertyIndex>,
        public mixin::JsonOperators<CPropertyIndex>,
        public mixin::EqualsByMetaClass<CPropertyIndex>,
        public mixin::LessThanByMetaClass<CPropertyIndex>,
        public mixin::CompareByMetaClass<CPropertyIndex>,
        public mixin::String<CPropertyIndex>
    {
        // In the first trial I have used CSequence<int> as base class. This has created too much circular dependencies of the headers
        // CIndexVariantMap is used in CValueObject, CPropertyIndex in CIndexVariantMap

    public:
        //! Default constructor.
        CPropertyIndex() = default;

        //! Non nested index
        CPropertyIndex(int singleProperty);

        //! Initializer list constructor
        CPropertyIndex(std::initializer_list<int> il);

        //! Construct from a list of indexes.
        CPropertyIndex(const QList<int> &indexes);

        //! From string
        CPropertyIndex(const QString &indexes);

        //! Return a simplified non-owning reference
        operator CPropertyIndexRef() const;

        //! Copy with first element removed
        Q_REQUIRED_RESULT CPropertyIndex copyFrontRemoved() const;

        //! Is nested index?
        bool isNested() const;

        //! Myself index, used with nesting
        bool isMyself() const;

        //! Empty?
        bool isEmpty() const;

        //! Index list
        QList<int> indexList() const;

        //! Shift existing indexes to right and insert given index at front
        void prepend(int newLeftIndex);

        //! Contains index?
        bool contains(int index) const;

        //! Compare with index given by enum
        template <class EnumType>
        bool contains(EnumType ev) const
        {
            static_assert(std::is_enum_v<EnumType>, "Argument must be an enum");
            return this->contains(static_cast<int>(ev));
        }

        //! Front to integer
        int frontToInt() const;

        //! Starts with given index?
        bool startsWith(int index) const;

        //! First element casted to given type, usually the PropertIndex enum
        template <class CastType>
        CastType frontCasted() const
        {
            static_assert(std::is_enum_v<CastType> || std::is_integral_v<CastType>, "CastType must be an enum or integer");
            return static_cast<CastType>(frontToInt());
        }

        //! Compare with index given by enum
        template <class EnumType>
        bool startsWithPropertyIndexEnum(EnumType ev) const
        {
            static_assert(std::is_enum_v<EnumType>, "Argument must be an enum");
            return this->startsWith(static_cast<int>(ev));
        }

        //! Return a predicate function which can compare two objects based on this index
        auto comparator() const
        {
            return [index = *this](const auto &a, const auto &b) {
                using T = std::decay_t<decltype(a)>;
                if constexpr (THasComparePropertyByIndex<T>::value)
                {
                    return a.comparePropertyByIndex(index, b);
                }
                else if constexpr (THasPropertyByIndex<T>::value)
                {
                    return compare(a.propertyByIndex(index), b.propertyByIndex(index));
                }
                else
                {
                    qFatal("Not implemented");
                    return 0;
                }
            };
        }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::CValueObject::toJson
        QJsonObject toJson() const;

        //! \copydoc swift::misc::CValueObject::convertFromJson
        void convertFromJson(const QJsonObject &json);

        //! an empty property index
        static const CPropertyIndex &empty()
        {
            static const CPropertyIndex pi;
            return pi;
        }

    protected:
        //! Parse indexes from string
        void parseFromString(const QString &indexes);

    private:
        QList<int> m_indexes;

        SWIFT_METACLASS(
            CPropertyIndex,
            SWIFT_METAMEMBER(indexes));
    };
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::CPropertyIndex)

#endif // guard
