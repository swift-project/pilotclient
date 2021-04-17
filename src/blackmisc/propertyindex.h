/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PROPERTYINDEX_H
#define BLACKMISC_PROPERTYINDEX_H

#include "blackmisc/propertyindexref.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/mixin/mixindbus.h"
#include "blackmisc/mixin/mixindatastream.h"
#include "blackmisc/mixin/mixinhash.h"
#include "blackmisc/mixin/mixinjson.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/mixin/mixinstring.h"
#include "blackmisc/typetraits.h"
#include "blackmisc/mixin/mixinmetatype.h"

#include <QList>
#include <QMetaType>
#include <QString>
#include <initializer_list>
#include <type_traits>

namespace BlackMisc
{
    class CPropertyIndex;

    namespace Private
    {
        //! \private
        template <class T, class X>
        int compareByProperty(const T &a, const T &b, const CPropertyIndex &index, std::true_type, X)
        {
            return a.comparePropertyByIndex(index, b);
        }
        //! \private
        template <class T>
        int compareByProperty(const T &a, const T &b, const CPropertyIndex &index, std::false_type, std::true_type)
        {
            return compare(a.propertyByIndex(index), b.propertyByIndex(index));
        }
        //! \private
        template <class T>
        int compareByProperty(const T &, const T &, const CPropertyIndex &, std::false_type, std::false_type)
        {
            qFatal("Not implemented");
            return 0;
        }
    }

    /*!
     * Property index. The index can be nested, that's why it is a sequence
     * (e.g. PropertyIndexPilot, PropertyIndexRealname).
     */
    class BLACKMISC_EXPORT CPropertyIndex :
        public Mixin::MetaType<CPropertyIndex>,
        public Mixin::HashByMetaClass<CPropertyIndex>,
        public Mixin::DBusByMetaClass<CPropertyIndex>,
        public Mixin::DataStreamByMetaClass<CPropertyIndex>,
        public Mixin::JsonOperators<CPropertyIndex>,
        public Mixin::EqualsByMetaClass<CPropertyIndex>,
        public Mixin::LessThanByMetaClass<CPropertyIndex>,
        public Mixin::CompareByMetaClass<CPropertyIndex>,
        public Mixin::String<CPropertyIndex>
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

        //! Construct from a vector of indexes.
        CPropertyIndex(const QVector<int> &indexes);

        //! Construct from a list of indexes.
        CPropertyIndex(const QList<int> &indexes);

        //! From string
        CPropertyIndex(const QString &indexes);

        //! Return a simplified non-owning reference
        operator CPropertyIndexRef() const;

        //! Copy with first element removed
        CPropertyIndex copyFrontRemoved() const;

        //! Is nested index?
        bool isNested() const;

        //! Myself index, used with nesting
        bool isMyself() const;

        //! Empty?
        bool isEmpty() const;

        //! Index vector
        QVector<int> indexVector() const;

        //! Index list
        QList<int> indexList() const;

        //! Shift existing indexes to right and insert given index at front
        void prepend(int newLeftIndex);

        //! Contains index?
        bool contains(int index) const;

        //! Compare with index given by enum
        template<class EnumType> bool contains(EnumType ev) const
        {
            static_assert(std::is_enum_v<EnumType>, "Argument must be an enum");
            return this->contains(static_cast<int>(ev));
        }

        //! Front to integer
        int frontToInt() const;

        //! Starts with given index?
        bool startsWith(int index) const;

        //! First element casted to given type, usually the PropertIndex enum
        template<class CastType> CastType frontCasted() const
        {
            static_assert(std::is_enum_v<CastType> || std::is_integral_v<CastType>, "CastType must be an enum or integer");
            return static_cast<CastType>(frontToInt());
        }

        //! Compare with index given by enum
        template<class EnumType> bool startsWithPropertyIndexEnum(EnumType ev) const
        {
            static_assert(std::is_enum_v<EnumType>, "Argument must be an enum");
            return this->startsWith(static_cast<int>(ev));
        }

        //! Return a predicate function which can compare two objects based on this index
        auto comparator() const
        {
            return [index = *this](const auto & a, const auto & b)
            {
                using T = std::decay_t<decltype(a)>;
                return Private::compareByProperty(a, b, index, THasComparePropertyByIndex<T>(), THasPropertyByIndex<T>());
            };
        }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::CValueObject::toJson
        QJsonObject toJson() const;

        //! \copydoc BlackMisc::CValueObject::convertFromJson
        void convertFromJson(const QJsonObject &json);

        //! an empty property index
        static const CPropertyIndex &empty() { static const CPropertyIndex pi; return pi; }

    protected:
        //! Parse indexes from string
        void parseFromString(const QString &indexes);

    private:
        QVector<int> m_indexes;

        BLACK_METACLASS(
            CPropertyIndex,
            BLACK_METAMEMBER(indexes)
        );
    };
} //namespace

Q_DECLARE_METATYPE(BlackMisc::CPropertyIndex)

#endif //guard
