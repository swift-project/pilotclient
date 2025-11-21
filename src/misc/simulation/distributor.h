// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_DISTRIBUTOR_H
#define SWIFT_MISC_SIMULATION_DISTRIBUTOR_H

#include <QJsonObject>
#include <QMap>
#include <QMetaType>
#include <QSet>
#include <QString>

#include "misc/db/datastore.h"
#include "misc/metaclass.h"
#include "misc/orderable.h"
#include "misc/propertyindexref.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/statusmessagelist.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation, CDistributor)

namespace swift::misc::simulation
{
    //! Value object encapsulating information of software distributor.
    class SWIFT_MISC_EXPORT CDistributor :
        public CValueObject<CDistributor>,
        public db::IDatastoreObjectWithStringKey,
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
        CDistributor() = default;

        //! Constructor
        CDistributor(const QString &key);

        //! Constructor
        CDistributor(
            const QString &id, const QString &description, const QString &alias1, const QString &alias2,
            const swift::misc::simulation::CSimulatorInfo &simulator = swift::misc::simulation::CSimulatorInfo());

        //! Get description
        const QString &getDescription() const { return m_description; }

        //! Get id and description
        QString getIdAndDescription() const;

        //! Set description
        void setDescription(const QString &description) { m_description = description.trimmed(); }

        //! Has description
        bool hasDescription() const { return !m_description.isEmpty(); }

        //! Get alias1
        const QString &getAlias1() const { return m_alias1; }

        //! Get alias2
        const QString &getAlias2() const { return m_alias2; }

        //! Set alias1
        void setAlias1(const QString &alias) { m_alias1 = alias; }

        //! Set alias2
        void setAlias2(const QString &alias) { m_alias2 = alias; }

        //! Alias 1?
        bool hasAlias1() const { return !m_alias1.isEmpty(); }

        //! Alias 2?
        bool hasAlias2() const { return !m_alias2.isEmpty(); }

        //! Simulator
        const swift::misc::simulation::CSimulatorInfo &getSimulator() const { return m_simulator; }

        //! Set simulator
        void setSimulator(const swift::misc::simulation::CSimulatorInfo &simulator) { m_simulator = simulator; }

        //! Matches key or alias
        bool matchesKeyOrAlias(const QString &keyOrAlias) const;

        //! Matches key or alias
        bool matchesKeyOrAlias(const CDistributor &distributor) const;

        //! Matches simulator
        bool matchesSimulator(const CSimulatorInfo &simulator) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CDistributor &compareValue) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Complete data?
        bool hasCompleteData() const;

        //! Validate data
        swift::misc::CStatusMessageList validate() const;

        //! Update missing parts
        void updateMissingParts(const CDistributor &otherDistributor);

        //! Object from JSON
        static CDistributor fromDatabaseJson(const QJsonObject &json, const QString &prefix = QString());

        //! @{
        //! Hardcoded keys for standard models
        static const QString &standardFSX();
        static const QString &standardP3D();
        static const QString &standardFS9();
        static const QString &standardXPlane();
        static const QString &standardFlightGear();
        static const QSet<QString> &standardAllFsFamily();
        static const QSet<QString> &xplaneMostPopular();
        static const QString &xplaneBlueBell();
        static const QString &xplaneXcsl();
        static const QString &standardMsfs();
        static const QString &standardMsfs2024();
        //! @}

    private:
        QString m_description; //!< description
        QString m_alias1; //!< alias name
        QString m_alias2; //!< alias name
        swift::misc::simulation::CSimulatorInfo m_simulator; //!< simulator

        //! "Clean up" the keys, like X-CSL => XCSL
        static QString unifyKeyOrAlias(const QString &value);

        SWIFT_METACLASS(
            CDistributor,
            SWIFT_METAMEMBER(dbKey, 0, CaseInsensitiveComparison),
            SWIFT_METAMEMBER(loadedFromDb),
            SWIFT_METAMEMBER(timestampMSecsSinceEpoch),
            SWIFT_METAMEMBER(order),
            SWIFT_METAMEMBER(description),
            SWIFT_METAMEMBER(alias1, 0, CaseInsensitiveComparison),
            SWIFT_METAMEMBER(alias2, 0, CaseInsensitiveComparison),
            SWIFT_METAMEMBER(simulator));
    };

    //! Distributor/id map
    using DistributorIdMap = QMap<QString, CDistributor>;

} // namespace swift::misc::simulation

Q_DECLARE_METATYPE(swift::misc::simulation::CDistributor)

#endif // SWIFT_MISC_SIMULATION_DISTRIBUTOR_H
