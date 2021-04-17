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
#include "blackmisc/propertyindexref.h"
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
    class CPropertyIndexList;
    class CPropertyIndexVariantMap;

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
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! Property by index
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! Compare for index
            int comparePropertyByIndex(CPropertyIndexRef index, const Derived &compareValue) const;

            //! Is given variant equal to value of property index?
            bool equalsPropertyByIndex(const QVariant &compareValue, CPropertyIndexRef index) const;

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }

            template <typename T, std::enable_if_t<std::is_default_constructible_v<T>, int> = 0>
            QVariant myself() const { return QVariant::fromValue(*derived()); }
            template <typename T, std::enable_if_t<std::is_default_constructible_v<T>, int> = 0>
            void myself(const QVariant &variant) { *derived() = variant.value<T>(); }

            template <typename T, std::enable_if_t<! std::is_default_constructible_v<T>, int> = 0>
            QVariant myself() const { qFatal("isMyself should have been handled before reaching here"); return {}; }
            template <typename T, std::enable_if_t<! std::is_default_constructible_v<T>, int> = 0>
            void myself(const QVariant &) { qFatal("isMyself should have been handled before reaching here"); }

            template <typename T>
            QVariant basePropertyByIndex(const T *base, CPropertyIndexRef index) const { return base->propertyByIndex(index); }
            template <typename T>
            void baseSetPropertyByIndex(T *base, const QVariant &var, CPropertyIndexRef index) { base->setPropertyByIndex(index, var); }

            QVariant basePropertyByIndex(const void *, CPropertyIndexRef) const
            {
                //qFatal("%s", qPrintable("Property by index not found, index: " + index.toQString())); return {};
                qFatal("Property by index not found"); return {};
            }

            void baseSetPropertyByIndex(void *, const QVariant &, CPropertyIndexRef)
            {
                //qFatal("%s", qPrintable("Property by index not found (setter), index: " + index.toQString()));
                qFatal("Property by index not found");
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
        void Index<Derived>::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
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
        QVariant Index<Derived>::propertyByIndex(CPropertyIndexRef index) const
        {
            if (index.isMyself()) { return myself<Derived>(); }
            const auto i = index.frontCasted<ColumnIndex>(); // keep that "auto", otherwise I won's compile
            switch (i)
            {
            case IndexIcon: return CIcons::toVariant(derived()->toIcon());
            case IndexPixmap: return CIcons::toVariantPixmap(derived()->toIcon());
            case IndexString: return QVariant(derived()->toQString());
            default: return basePropertyByIndex(static_cast<const TIndexBaseOfT<Derived> *>(derived()), index);
            }
        }

        template <class Derived>
        bool Index<Derived>::equalsPropertyByIndex(const QVariant &compareValue, CPropertyIndexRef index) const
        {
            return derived()->propertyByIndex(index) == compareValue;
        }

        template<class Derived>
        int Index<Derived>::comparePropertyByIndex(CPropertyIndexRef index, const Derived &compareValue) const
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
