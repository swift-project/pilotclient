/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_MIXIN_MIXININDEX_H
#define BLACKMISC_MIXIN_MIXININDEX_H

#include "blackmisc/icon.h"
#include "blackmisc/inheritancetraits.h"
#include "blackmisc/predicates.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/propertyindexlist.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/variant.h"

#include <QDBusArgument>
#include <QList>
#include <QMap>
#include <QMetaType>
#include <QString>
#include <QVariant>
#include <QtDebug>
#include <QtGlobal>
#include <type_traits>

namespace BlackMisc
{
    namespace Mixin
    {
        /*!
         * CRTP class template from which a derived class can inherit property indexing functions.
         *
         * This is only a placeholder for future support of implementing property indexing through the tuple system.
         * At the moment, it just implements the default properties: String, Icon, and Pixmap.
         */
        template <class Derived>
        class Index
        {
        public:
            //! Base class enums
            enum ColumnIndex
            {
                IndexPixmap = 10, // manually set to avoid circular dependencies
                IndexIcon,
                IndexString
            };

            //! Update by variant map
            //! \return number of values changed, with skipEqualValues equal values will not be changed
            CPropertyIndexList apply(const CPropertyIndexVariantMap &indexMap, bool skipEqualValues = false); // impl in propertyindexvariantmap.h

            //! Set property by index
            void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

            //! Property by index
            CVariant propertyByIndex(const CPropertyIndex &index) const;

            //! Compare for index
            int comparePropertyByIndex(const CPropertyIndex &index, const Derived &compareValue) const;

            //! Is given variant equal to value of property index?
            bool equalsPropertyByIndex(const CVariant &compareValue, const CPropertyIndex &index) const;

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }

            template <typename T, std::enable_if_t<std::is_default_constructible<T>::value, int> = 0>
            CVariant myself() const { return CVariant::from(*derived()); }
            template <typename T, std::enable_if_t<std::is_default_constructible<T>::value, int> = 0>
            void myself(const CVariant &variant) { *derived() = variant.to<T>(); }

            template <typename T, std::enable_if_t<! std::is_default_constructible<T>::value, int> = 0>
            CVariant myself() const { qFatal("isMyself should have been handled before reaching here"); return {}; }
            template <typename T, std::enable_if_t<! std::is_default_constructible<T>::value, int> = 0>
            void myself(const CVariant &) { qFatal("isMyself should have been handled before reaching here"); }

            template <typename T>
            CVariant basePropertyByIndex(const T *base, const CPropertyIndex &index) const { return base->propertyByIndex(index); }
            template <typename T>
            void baseSetPropertyByIndex(T *base, const CVariant &var, const CPropertyIndex &index) { base->setPropertyByIndex(index, var); }

            CVariant basePropertyByIndex(const void *, const CPropertyIndex &index) const
            {
                qFatal("%s", qPrintable("Property by index not found, index: " + index.toQString())); return {};
            }

            void baseSetPropertyByIndex(void *, const CVariant &, const CPropertyIndex &index)
            {
                qFatal("%s", qPrintable("Property by index not found (setter), index: " + index.toQString()));
            }
        };

        /*!
         * When a derived class and a base class both inherit from Mixin::Index,
         * the derived class uses this macro to disambiguate the inherited members.
         */
        // *INDENT-OFF*
#       define BLACKMISC_DECLARE_USING_MIXIN_INDEX(DERIVED)                     \
            using ::BlackMisc::Mixin::Index<DERIVED>::apply;                    \
            using ::BlackMisc::Mixin::Index<DERIVED>::setPropertyByIndex;       \
            using ::BlackMisc::Mixin::Index<DERIVED>::propertyByIndex;          \
            using ::BlackMisc::Mixin::Index<DERIVED>::comparePropertyByIndex;   \
            using ::BlackMisc::Mixin::Index<DERIVED>::equalsPropertyByIndex;
        // *INDENT-ON*

        template <class Derived>
        void Index<Derived>::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself())
            {
                myself<Derived>(variant);
            }
            else
            {
                baseSetPropertyByIndex(static_cast<TIndexBaseOfT<Derived> *>(derived()), variant, index);
            }
        }

        template <class Derived>
        CVariant Index<Derived>::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return myself<Derived>(); }
            const auto i = index.frontCasted<ColumnIndex>(); // keep that "auto", otherwise I won's compile
            switch (i)
            {
            case IndexIcon: return CVariant::from(CIcon(derived()->toIcon()));
            case IndexPixmap: return CVariant::from(CIcon(derived()->toIcon()).toPixmap());
            case IndexString: return CVariant(derived()->toQString());
            default: return basePropertyByIndex(static_cast<const TIndexBaseOfT<Derived> *>(derived()), index);
            }
        }

        template <class Derived>
        bool Index<Derived>::equalsPropertyByIndex(const CVariant &compareValue, const CPropertyIndex &index) const
        {
            return derived()->propertyByIndex(index) == compareValue;
        }

        template<class Derived>
        int Index<Derived>::comparePropertyByIndex(const CPropertyIndex &index, const Derived &compareValue) const
        {
            if (this == &compareValue) { return 0; }
            if (index.isMyself()) {
                // slow, only last resort
                return derived()->toQString().compare(compareValue.toQString());
            }

            const auto i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIcon:
            case IndexPixmap:
            case IndexString:
            default:
                break; // also covers
            }

            // slow, only last resort
            return derived()->toQString().compare(compareValue.toQString());
        }
    } // ns
} // ns

#endif // guard
