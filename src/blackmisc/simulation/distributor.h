/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_DISTRIBUTOR_H
#define BLACKMISC_SIMULATION_DISTRIBUTOR_H

#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/orderable.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscexport.h"

#include <QJsonObject>
#include <QMetaType>
#include <QMap>
#include <QSet>
#include <QString>

namespace BlackMisc
{
    namespace Simulation
    {
        //! Value object encapsulating information of software distributor.
        class BLACKMISC_EXPORT CDistributor :
            public CValueObject<CDistributor>,
            public Db::IDatastoreObjectWithStringKey,
            public IOrderable
        {
        public:
            //! Property indexes
            enum ColumnIndex
            {
                IndexDescription = CPropertyIndexRef::GlobalIndexCDistributor,
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
            const QString &getDescription() const { return m_description;}

            //! Get id and description
            const QString getIdAndDescription() const;

            //! Set description
            void setDescription(const QString &description) { m_description = description.trimmed(); }

            //! Has description
            bool hasDescription() const { return !m_description.isEmpty(); }

            //! Get alias1
            const QString &getAlias1() const { return m_alias1;}

            //! Get alias2
            const QString &getAlias2() const { return m_alias2;}

            //! Set alias1
            void setAlias1(const QString &alias) { m_alias1 = alias; }

            //! Set alias2
            void setAlias2(const QString &alias) { m_alias2 = alias; }

            //! Alias 1?
            bool hasAlias1() const { return !m_alias1.isEmpty(); }

            //! Alias 2?
            bool hasAlias2() const { return !m_alias2.isEmpty(); }

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
            QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc Mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(CPropertyIndexRef index, const CDistributor &compareValue) const;

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

            //! Hardcoded keys for standard models
            //! @{
            static const QString &standardFSX();
            static const QString &standardP3D();
            static const QString &standardFS9();
            static const QString &standardXPlane();
            static const QString &standardFlightGear();
            static const QSet<QString> &standardAllFsFamily();
            static const QSet<QString> &xplaneMostPopular();
            static const QString &xplaneBlueBell();
            static const QString &xplaneXcsl();
            //! @}

        private:
            QString m_description;                             //!< description
            QString m_alias1;                                  //!< alias name
            QString m_alias2;                                  //!< alias name
            BlackMisc::Simulation::CSimulatorInfo m_simulator; //!< simulator

            //! "Clean up" the keys, like X-CSL => XCSL
            static QString unifyKeyOrAlias(const QString &value);

            BLACK_METACLASS(
                CDistributor,
                BLACK_METAMEMBER(dbKey, 0, CaseInsensitiveComparison),
                BLACK_METAMEMBER(loadedFromDb),
                BLACK_METAMEMBER(timestampMSecsSinceEpoch),
                BLACK_METAMEMBER(order),
                BLACK_METAMEMBER(description),
                BLACK_METAMEMBER(alias1, 0, CaseInsensitiveComparison),
                BLACK_METAMEMBER(alias2, 0, CaseInsensitiveComparison),
                BLACK_METAMEMBER(simulator)
            );
        };

        //! Distributor/id map
        using DistributorIdMap = QMap<QString, CDistributor>;

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CDistributor)

#endif // guard
