/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "vpilotmodelruleset.h"
#include "blackmisc/predicates.h"

using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {

            CVPilotModelRuleSet::CVPilotModelRuleSet(const CCollection<CVPilotModelRule> &other) :
                CCollection<CVPilotModelRule>(other)
            { }

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
                return this->findBy([ = ](const CVPilotModelRule & rule)
                {
                    return rule.getModelName().startsWith(mn, Qt::CaseInsensitive);
                });
            }

            CAircraftMappingList CVPilotModelRuleSet::toMappings() const
            {
                CAircraftMappingList mappings;
                for (const CVPilotModelRule &rule : (*this))
                {
                    mappings.push_back(rule.toMapping());
                }
                return mappings;
            }

            QStringList CVPilotModelRuleSet::toUpper(const QStringList &stringList)
            {
                QStringList upper;
                for (const QString &s : stringList)
                {
                    upper.append(s.toUpper());
                }
                return upper;
            }

            QStringList CVPilotModelRuleSet::getSortedModelNames() const
            {
                QStringList ms;
                for (const CVPilotModelRule &rule : (*this))
                {
                    ms.append(rule.getModelName());
                }
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

                QSet<QString> removeSet(knownModels.toSet().intersect(remove.toSet()));
                int c = 0;
                for (const QString &model : removeSet)
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

                QSet<QString> removeSet(knownModels.toSet().subtract(keep.toSet()));
                int c = 0;
                for (const QString &model : removeSet)
                {
                    c += this->removeIf(&CVPilotModelRule::getModelName, model);
                }
                return c;
            }

        } // namespace
    } // namespace
} // namespace
