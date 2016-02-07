/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_DISTRIBUTOR_H
#define BLACKMISC_SIMULATION_DISTRIBUTOR_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/datastore.h"
#include "blackmisc/statusmessagelist.h"
#include <QJsonObject>

namespace BlackMisc
{
    namespace Simulation
    {
        //! Value object encapsulating information of software distributor.
        class BLACKMISC_EXPORT CDistributor :
            public BlackMisc::CValueObject<CDistributor>,
            public BlackMisc::IDatastoreObjectWithStringKey
        {
        public:
            //! Property indexes
            enum ColumnIndex
            {
                IndexDescription = CPropertyIndex::GlobalIndexCDistributor,
                IndexAlias1,
                IndexAlias2
            };

            //! Default constructor.
            CDistributor();

            //! Constructor
            CDistributor(const QString &key);

            //! Constructor
            CDistributor(const QString &id, const QString &description, const QString &alias1, const QString &alias2);

            //! Get description
            const QString &getDescription() const { return this->m_description;}

            //! Set description
            void setDescription(const QString &description) { this->m_description = description.trimmed(); }

            //! Has description
            bool hasDescription() const { return !this->m_description.isEmpty(); }

            //! Get alias1
            const QString &getAlias1() const { return this->m_alias1;}

            //! Get alias2
            const QString &getAlias2() const { return this->m_alias2;}

            //! Set alias1
            void setAlias1(const QString &alias) { this->m_alias1 = alias.trimmed().toUpper(); }

            //! Set alias2
            void setAlias2(const QString &alias) { this->m_alias2 = alias.trimmed().toUpper(); }

            //! Alias 1?
            bool hasAlias1() const { return !this->m_alias1.isEmpty(); }

            //! Alias 2?
            bool hasAlias2() const { return !this->m_alias2.isEmpty(); }

            //! Matches key or alias
            bool matchesKeyOrAlias(const QString &keyOrAlias) const;

            //! Matches key or alias
            bool matchesKeyOrAlias(const CDistributor &distributor) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! Compare for index
            int comparePropertyByIndex(const CDistributor &compareValue, const CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Complete data?
            bool hasCompleteData() const;

            //! Validate data
            BlackMisc::CStatusMessageList validate() const;

            //! Update missing parts
            void updateMissingParts(const CDistributor &otherDistributor);

            //! Object from JSON
            static CDistributor fromDatabaseJson(const QJsonObject &json, const QString &prefix = QString());

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CDistributor)
            QString m_description; //!< description
            QString m_alias1;      //!< alias name
            QString m_alias2;      //!< alias name
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CDistributor)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Simulation::CDistributor, (
                                   attr(o.m_dbKey, flags <CaseInsensitiveComparison> ()),
                                   attr(o.m_timestampMSecsSinceEpoch),
                                   attr(o.m_description),
                                   attr(o.m_alias1, flags <CaseInsensitiveComparison> ()),
                                   attr(o.m_alias2, flags <CaseInsensitiveComparison> ())
                               ))

#endif // guard
