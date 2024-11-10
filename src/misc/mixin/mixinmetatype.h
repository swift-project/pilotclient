// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_MIXIN_MIXINMETATYPE_H
#define SWIFT_MISC_MIXIN_MIXINMETATYPE_H

#include "misc/inheritancetraits.h"
#include "misc/propertyindexref.h"
#include "misc/variant.h"
#include <QMetaType>
#include <QtGlobal>
#include <QString>

namespace swift::misc
{
    namespace mixin
    {
        /*!
         * CRTP class template from which a derived class can inherit common methods dealing with the metatype of the class.
         *
         * \see SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE
         */
        template <class Derived>
        class MetaType
        {
        public:
            //! Register metadata
            static void registerMetadata();

            //! Returns the Qt meta type ID of this object
            //! \remark for CVariant this returns the id of CVariant, not of the encapsulated object. valueVariant.userType()` returns metatype of the contained object
            int getMetaTypeId() const;

            //! Class name
            QString getClassName() const;

            //! Returns true if this object is an instance of the class with the given meta type ID, or one of its subclasses.
            bool isA(int metaTypeId) const;

        private:
            const Derived *derived() const;
            Derived *derived();

            template <typename Base2>
            static bool baseIsA(const Base2 *base, int metaTypeId);
            static bool baseIsA(const void *, int);
        };

        template <class Derived>
        void MetaType<Derived>::registerMetadata()
        {
            private_ns::MetaTypeHelper<Derived>::maybeRegisterMetaType();
        }

        template <class Derived>
        int MetaType<Derived>::getMetaTypeId() const
        {
            return private_ns::MetaTypeHelper<Derived>::maybeGetMetaTypeId();
        }

        template <class Derived>
        QString MetaType<Derived>::getClassName() const
        {
            return QMetaType::typeName(getMetaTypeId());
        }

        template <class Derived>
        bool MetaType<Derived>::isA(int metaTypeId) const
        {
            if (metaTypeId == QMetaType::UnknownType) { return false; }
            if (metaTypeId == getMetaTypeId()) { return true; }
            return baseIsA(static_cast<const TMetaBaseOfT<Derived> *>(derived()), metaTypeId);
        }

        template <class Derived>
        const Derived *MetaType<Derived>::derived() const
        {
            return static_cast<const Derived *>(this);
        }

        template <class Derived>
        Derived *MetaType<Derived>::derived()
        {
            return static_cast<Derived *>(this);
        }

        template <class Derived>
        template <typename Base2>
        bool MetaType<Derived>::baseIsA(const Base2 *base, int metaTypeId)
        {
            return base->isA(metaTypeId);
        }

        template <class Derived>
        bool MetaType<Derived>::baseIsA(const void *, int)
        {
            return false;
        }

        // *INDENT-OFF*
        /*!
         * When a derived class and a base class both inherit from mixin::MetaType,
         * the derived class uses this macro to disambiguate the inherited members.
         */
#define SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(DERIVED)             \
    using ::swift::misc::mixin::MetaType<DERIVED>::registerMetadata; \
    using ::swift::misc::mixin::MetaType<DERIVED>::getMetaTypeId;    \
    using ::swift::misc::mixin::MetaType<DERIVED>::getClassName;     \
    using ::swift::misc::mixin::MetaType<DERIVED>::isA;
        // *INDENT-ON*

    } // Mixin
} // namespace

#endif
