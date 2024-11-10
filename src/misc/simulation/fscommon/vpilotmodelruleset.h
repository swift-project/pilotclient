// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_FSCOMMON_VPILOTMODELRULELIST_H
#define SWIFT_MISC_SIMULATION_FSCOMMON_VPILOTMODELRULELIST_H

#include <QMetaType>
#include <QString>
#include <QStringList>

#include "misc/swiftmiscexport.h"
#include "misc/collection.h"
#include "misc/mixin/mixincompare.h"
#include "misc/sequence.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/fscommon/vpilotmodelrule.h"

BLACK_DECLARE_COLLECTION_MIXINS(swift::misc::simulation::fscommon, CVPilotModelRule, CVPilotModelRuleSet)

namespace swift::misc::simulation::fscommon
{
    //! Value object reading a set of vPilot rules
    //! \deprecated vPilot handling will be most likely removed in the future
    class SWIFT_MISC_EXPORT CVPilotModelRuleSet :
        public CCollection<CVPilotModelRule>,
        public mixin::MetaType<CVPilotModelRuleSet>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CVPilotModelRuleSet)
        using CCollection::CCollection;

        //! Default constructor.
        CVPilotModelRuleSet() = default;

        //! Construct from a base class object.
        CVPilotModelRuleSet(const CCollection<CVPilotModelRule> &other);

        //! Find by model string
        CVPilotModelRuleSet findByModelName(const QString &modelName) const;

        //! Find first by model string
        CVPilotModelRule findFirstByModelName(const QString &modelName) const;

        //! Find models starting with
        CVPilotModelRuleSet findModelsStartingWith(const QString &modelName) const;

        //! Model strings
        QStringList getSortedModelNames() const;

        //! List of distributors
        QStringList getSortedDistributors() const;

        //! Removed given models
        int removeModels(const QStringList &modelsToBeRemoved);

        //! Keep given models (if in list)
        int keepModels(const QStringList &modelsToBeKept);

        //! To aircraft models
        //! \note slow operation, can take a while
        simulation::CAircraftModelList toAircraftModels() const;

    private:
        //! Convert values to upper case
        static QStringList toUpper(const QStringList &stringList);
    };
} // namespace

Q_DECLARE_METATYPE(swift::misc::simulation::fscommon::CVPilotModelRuleSet)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::simulation::fscommon::CVPilotModelRule>)

#endif // guard
