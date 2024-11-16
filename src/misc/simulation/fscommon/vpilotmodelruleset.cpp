// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/fscommon/vpilotmodelruleset.h"

#include <tuple>

#include <QList>
#include <QSet>
#include <QStringList>
#include <Qt>

#include "misc/range.h"
#include "misc/simulation/aircraftmodel.h"

SWIFT_DEFINE_COLLECTION_MIXINS(swift::misc::simulation::fscommon, CVPilotModelRule, CVPilotModelRuleSet)

namespace swift::misc::simulation::fscommon
{
    CVPilotModelRuleSet::CVPilotModelRuleSet(const CCollection<CVPilotModelRule> &other)
        : CCollection<CVPilotModelRule>(other)
    {}

    CVPilotModelRuleSet CVPilotModelRuleSet::findByModelName(const QString &modelName) const
    {
        return this->findBy(&CVPilotModelRule::getModelName, modelName.trimmed().toUpper());
    }

    CVPilotModelRule CVPilotModelRuleSet::findFirstByModelName(const QString &modelName) const
    {
        return this->findFirstByOrDefault(&CVPilotModelRule::getModelName, modelName.trimmed().toUpper());
    }

    CVPilotModelRuleSet CVPilotModelRuleSet::findModelsStartingWith(const QString &modelName) const
    {
        QString mn(modelName.trimmed().toUpper());
        return this->findBy(
            [=](const CVPilotModelRule &rule) { return rule.getModelName().startsWith(mn, Qt::CaseInsensitive); });
    }

    QStringList CVPilotModelRuleSet::toUpper(const QStringList &stringList)
    {
        QStringList upper;
        upper.reserve(stringList.size());
        for (const QString &s : stringList) { upper.append(s.toUpper()); }
        return upper;
    }

    QStringList CVPilotModelRuleSet::getSortedModelNames() const
    {
        QStringList ms;
        ms.reserve(size());
        for (const CVPilotModelRule &rule : (*this)) { ms.append(rule.getModelName()); }
        ms.sort(Qt::CaseInsensitive);
        return ms;
    }

    QStringList CVPilotModelRuleSet::getSortedDistributors() const
    {
        QStringList distributors;
        for (const CVPilotModelRule &rule : (*this))
        {
            QString d(rule.getDistributor());
            if (distributors.contains(d)) { continue; }
            distributors.append(d);
        }
        distributors.sort(Qt::CaseInsensitive);
        return distributors;
    }

    int CVPilotModelRuleSet::removeModels(const QStringList &modelsToBeRemoved)
    {
        QStringList knownModels(getSortedModelNames());
        if (knownModels.isEmpty()) { return 0; }

        QStringList remove(toUpper(modelsToBeRemoved));
        remove.sort();

        QSet<QString> removeSet(knownModels.begin(), knownModels.end());
        removeSet &= QSet<QString>(remove.begin(), remove.end());
        int c = 0;
        for (const QString &model : std::as_const(removeSet))
        {
            c += this->removeIf(&CVPilotModelRule::getModelName, model);
        }
        return c;
    }

    int CVPilotModelRuleSet::keepModels(const QStringList &modelsToBeKept)
    {
        QStringList knownModels(getSortedModelNames());
        if (knownModels.isEmpty()) { return 0; }

        QStringList keep(toUpper(modelsToBeKept));
        keep.sort();

        QSet<QString> removeSet(knownModels.begin(), knownModels.end());
        removeSet.subtract(QSet<QString>(keep.begin(), keep.end()));
        int c = 0;
        for (const QString &model : removeSet) { c += this->removeIf(&CVPilotModelRule::getModelName, model); }
        return c;
    }

    CAircraftModelList CVPilotModelRuleSet::toAircraftModels() const
    {
        QStringList modelNames;
        CAircraftModelList models;
        for (const CVPilotModelRule &rule : *this)
        {
            QString m(rule.getModelName());
            if (m.isEmpty()) { continue; }
            if (modelNames.contains(m, Qt::CaseInsensitive))
            {
                CAircraftModel model(rule.toAircraftModel());
                for (CAircraftModel &exisitingModel : models)
                {
                    if (!exisitingModel.matchesModelString(m, Qt::CaseInsensitive)) { continue; }
                    exisitingModel.updateMissingParts(model);
                    break;
                }
            }
            else
            {
                models.push_back(rule.toAircraftModel());
                modelNames.append(m);
            }
        }
        return models;
    }
} // namespace swift::misc::simulation::fscommon
