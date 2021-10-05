/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_MIXIN_MIXINMETATYPE_H
#define BLACKMISC_MIXIN_MIXINMETATYPE_H

#include "blackmisc/inheritancetraits.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/variant.h"
#include <QMetaType>
#include <QtGlobal>
#include <QString>

namespace BlackMisc
{
    namespace Mixin
    {
        /*!
         * CRTP class template from which a derived class can inherit common methods dealing with the metatype of the class.
         *
         * \see BLACKMISC_DECLARE_USING_MIXIN_METATYPE
         */
        template <class Derived>
        class MetaType
        {
        public:
            //! Register metadata
            static void registerMetadata()
            {
                Private::MetaTypeHelper<Derived>::maybeRegisterMetaType();
            }

            //! Returns the Qt meta type ID of this object
            //! \remark for CVariant this returns the id of CVariant, not of the encapsulated object. valueVariant.userType()` returns metatype of the contained object
            int getMetaTypeId() const
            {
                return Private::MetaTypeHelper<Derived>::maybeGetMetaTypeId();
            }

            //! Class name
            QString getClassName() const
            {
                return QMetaType::typeName(getMetaTypeId());
            }

            //! Returns true if this object is an instance of the class with the given meta type ID, or one of its subclasses.
            bool isA(int metaTypeId) const
            {
                if (metaTypeId == QMetaType::UnknownType) { return false; }
                if (metaTypeId == getMetaTypeId()) { return true; }
                return baseIsA(static_cast<const TMetaBaseOfT<Derived> *>(derived()), metaTypeId);
            }

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }

            template <typename Base2> static bool baseIsA(const Base2 *base, int metaTypeId) { return base->isA(metaTypeId); }
            static bool baseIsA(const void *, int) { return false; }
        };

        // *INDENT-OFF*
        /*!
         * When a derived class and a base class both inherit from Mixin::MetaType,
         * the derived class uses this macro to disambiguate the inherited members.
         */
#       define BLACKMISC_DECLARE_USING_MIXIN_METATYPE(DERIVED)                  \
            using ::BlackMisc::Mixin::MetaType<DERIVED>::registerMetadata;      \
            using ::BlackMisc::Mixin::MetaType<DERIVED>::getMetaTypeId;         \
            using ::BlackMisc::Mixin::MetaType<DERIVED>::getClassName;          \
            using ::BlackMisc::Mixin::MetaType<DERIVED>::isA;
        // *INDENT-ON*

    } // Mixin
} // namespace

#endif
