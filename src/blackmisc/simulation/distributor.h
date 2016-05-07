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
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/orderable.h"
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
            public BlackMisc::IDatastoreObjectWithStringKey,
            public BlackMisc::IOrderable
        {
        public:
            //! Property indexes
            enum ColumnIndex
            {
                IndexDescription = CPropertyIndex::GlobalIndexCDistributor,
                IndexAlias1,
                IndexAlias2,
                IndexSimulator
            };

            //! Default constructor.
            CDistributor();

            //! Constructor
            CDistributor(const QString &key);

            //! Constructor
            CDistributor(const QString &id, const QString &description, const QString &alias1, const QString &alias2, const BlackMisc::Simulation::CSimulatorInfo &simulator = BlackMisc::Simulation::CSimulatorInfo());

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

            //! Simulator
            const BlackMisc::Simulation::CSimulatorInfo &getSimulator() const { return m_simulator; }

            //! Set simulator
            void setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) { m_simulator = simulator; }

            //! Matches key or alias
            bool matchesKeyOrAlias(const QString &keyOrAlias) const;

            //! Matches key or alias
            bool matchesKeyOrAlias(const CDistributor &distributor) const;

            //! Matches simulator
            bool matchesSimulator(const CSimulatorInfo &simulator) const;

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
            QString m_description;                             //!< description
            QString m_alias1;                                  //!< alias name
            QString m_alias2;                                  //!< alias name
            BlackMisc::Simulation::CSimulatorInfo m_simulator; //!< simulator

            BLACK_METACLASS(
                CDistributor,
                BLACK_METAMEMBER(dbKey, 0, CaseInsensitiveComparison),
                BLACK_METAMEMBER(timestampMSecsSinceEpoch),
                BLACK_METAMEMBER(order),
                BLACK_METAMEMBER(description),
                BLACK_METAMEMBER(alias1, 0, CaseInsensitiveComparison),
                BLACK_METAMEMBER(alias2, 0, CaseInsensitiveComparison),
                BLACK_METAMEMBER(simulator)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CDistributor)

#endif // guard
