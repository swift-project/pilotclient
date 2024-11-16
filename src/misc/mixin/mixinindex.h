// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_MIXIN_MIXININDEX_H
#define SWIFT_MISC_MIXIN_MIXININDEX_H

#include <type_traits>

#include <QDBusArgument>
#include <QList>
#include <QMap>
#include <QMetaType>
#include <QString>
#include <QVariant>
#include <QtDebug>
#include <QtGlobal>

#include "misc/icon.h"
#include "misc/inheritancetraits.h"
#include "misc/predicates.h"
#include "misc/propertyindexref.h"

namespace swift::misc
{
    class CPropertyIndexList;
    class CPropertyIndexVariantMap;

    namespace mixin
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
            const Derived *derived() const;
            Derived *derived();

            template <typename T>
            QVariant myself() const;
            template <typename T>
            void myself(const QVariant &variant);

            template <typename T>
            QVariant basePropertyByIndex(const T *base, CPropertyIndexRef index) const;
            template <typename T>
            void baseSetPropertyByIndex(T *base, const QVariant &var, CPropertyIndexRef index);

            QVariant basePropertyByIndex(const void *, CPropertyIndexRef) const;
            void baseSetPropertyByIndex(void *, const QVariant &, CPropertyIndexRef);
        };

        template <class Derived>
        const Derived *Index<Derived>::derived() const
        {
            return static_cast<const Derived *>(this);
        }

        template <class Derived>
        Derived *Index<Derived>::derived()
        {
            return static_cast<Derived *>(this);
        }

        template <class Derived>
        template <typename T>
        QVariant Index<Derived>::myself() const
        {
            if constexpr (std::is_default_constructible_v<T>) { return QVariant::fromValue(*derived()); }
            else
            {
                qFatal("isMyself should have been handled before reaching here");
                return {};
            }
        }

        template <class Derived>
        template <typename T>
        void Index<Derived>::myself(const QVariant &variant)
        {
            if constexpr (std::is_default_constructible_v<T>) { *derived() = variant.value<T>(); }
            else { qFatal("isMyself should have been handled before reaching here"); }
        }

        template <class Derived>
        template <typename T>
        QVariant Index<Derived>::basePropertyByIndex(const T *base, CPropertyIndexRef index) const
        {
            return base->propertyByIndex(index);
        }

        template <class Derived>
        template <typename T>
        void Index<Derived>::baseSetPropertyByIndex(T *base, const QVariant &var, CPropertyIndexRef index)
        {
            base->setPropertyByIndex(index, var);
        }

        template <class Derived>
        QVariant Index<Derived>::basePropertyByIndex(const void *, CPropertyIndexRef) const
        {
            // qFatal("%s", qPrintable("Property by index not found, index: " + index.toQString())); return {};
            qFatal("Property by index not found");
            return {};
        }

        template <class Derived>
        void Index<Derived>::baseSetPropertyByIndex(void *, const QVariant &, CPropertyIndexRef)
        {
            // qFatal("%s", qPrintable("Property by index not found (setter), index: " + index.toQString()));
            qFatal("Property by index not found");
        }

        /*!
         * When a derived class and a base class both inherit from mixin::Index,
         * the derived class uses this macro to disambiguate the inherited members.
         */
        // *INDENT-OFF*
#define SWIFT_MISC_DECLARE_USING_MIXIN_INDEX(DERIVED)                   \
    using ::swift::misc::mixin::Index<DERIVED>::apply;                  \
    using ::swift::misc::mixin::Index<DERIVED>::setPropertyByIndex;     \
    using ::swift::misc::mixin::Index<DERIVED>::propertyByIndex;        \
    using ::swift::misc::mixin::Index<DERIVED>::comparePropertyByIndex; \
    using ::swift::misc::mixin::Index<DERIVED>::equalsPropertyByIndex;
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

        template <class Derived>
        int Index<Derived>::comparePropertyByIndex(CPropertyIndexRef index, const Derived &compareValue) const
        {
            if (this == &compareValue) { return 0; }
            if (index.isMyself())
            {
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
    } // namespace mixin
} // namespace swift::misc

#endif // guard
