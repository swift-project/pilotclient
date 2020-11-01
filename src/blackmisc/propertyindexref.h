/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PROPERTYINDEXREF_H
#define BLACKMISC_PROPERTYINDEXREF_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/typetraits.h"
#include <QVector>

namespace BlackMisc
{
    class CPropertyIndexRef;

    namespace Private
    {
        //! \private
        template <class T, class X>
        int compareByProperty(const T &a, const T &b, const CPropertyIndexRef &index, std::true_type, X)
        {
            return a.comparePropertyByIndex(index, b);
        }
        //! \private
        template <class T>
        int compareByProperty(const T &a, const T &b, const CPropertyIndexRef &index, std::false_type, std::true_type)
        {
            return compare(a.propertyByIndex(index), b.propertyByIndex(index));
        }
        //! \private
        template <class T>
        int compareByProperty(const T &, const T &, const CPropertyIndexRef &, std::false_type, std::false_type)
        {
            qFatal("Not implemented");
            return 0;
        }
    }

    /*!
     * Non-owning reference to a CPropertyIndex with a subset of its features.
     */
    class BLACKMISC_EXPORT CPropertyIndexRef
    {
    public:
        //! Construct from a single index.
        CPropertyIndexRef(int index);

        //! Construct from the data of a CPropertyIndex.
        explicit CPropertyIndexRef(const QVector<int> &indexes);

        //! Forbid accidental constructor from an rvalue.
        explicit CPropertyIndexRef(QVector<int> &&) = delete;

        //! Copy with first element removed
        CPropertyIndexRef copyFrontRemoved() const;

        //! Is nested index?
        bool isNested() const;

        //! Myself index, used with nesting
        bool isMyself() const;

        //! Empty?
        bool isEmpty() const;

        //! Front to integer
        int frontToInt() const;

        //! Starts with given index?
        bool startsWith(int index) const;

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString toQString(bool i18n = false) const;

        //! First element casted to given type, usually the PropertIndex enum
        template<class CastType> CastType frontCasted() const
        {
            static_assert(std::is_enum<CastType>::value || std::is_integral<CastType>::value, "CastType must be an enum or integer");
            return static_cast<CastType>(frontToInt());
        }

        //! Compare with index given by enum
        template<class EnumType> bool startsWithPropertyIndexEnum(EnumType ev) const
        {
            static_assert(std::is_enum<EnumType>::value, "Argument must be an enum");
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

        //! an empty property index
        static CPropertyIndexRef empty() { return -1; }

    private:
        const int *m_begin = nullptr;
        int m_sizeOrIndex = -1;
    };
}

#endif
