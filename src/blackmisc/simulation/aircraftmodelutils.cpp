/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/aircraftmodelutils.h"
#include "blackmisc/simulation/fscommon/fscommonutil.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/verify.h"

#include <algorithm>

namespace BlackMisc
{
    namespace Simulation
    {
        bool CAircraftModelUtilities::mergeWithVPilotData(CAircraftModelList &modelToBeModified, const CAircraftModelList &vPilotModels, bool force)
        {
            if (vPilotModels.isEmpty() || modelToBeModified.isEmpty()) { return false; }
            for (CAircraftModel &simModel : modelToBeModified)
            {
                if (!force && simModel.hasValidAircraftAndAirlineDesignator()) { continue; }  // already done
                const CAircraftModel vPilotModel(vPilotModels.findFirstByModelStringOrDefault(simModel.getModelString()));
                if (!vPilotModel.hasValidDbKey())
                {
                    continue; // not found
                }
                simModel.updateMissingParts(vPilotModel, false);
            }
            return true;
        }

        QString CAircraftModelUtilities::createIcaoAirlineAircraftHtmlMatrix(const CAircraftModelList &models)
        {
            if (models.isEmpty()) { return {}; }
            static const QString emptyDesignator = "----";
            static const QString colorLiveryDesignator = "-C-";

            QMap<QString, QMap<QString, CAircraftModelList>> modelsByDesignator;

            // create an empty map of all airlines
            const QMap<QString, CAircraftModelList> emptyAirlineDesignatorMap;
            CAircraftModelList sortedByAircraft(models);
            sortedByAircraft.sortBy(&CAircraftModel::getAircraftIcaoCodeDesignator);

            for (const CAircraftModel &model : std::as_const(sortedByAircraft))
            {
                const QString aircraftIcao(model.hasAircraftDesignator() ? model.getAircraftIcaoCodeDesignator() : emptyDesignator);
                if (!modelsByDesignator.contains(aircraftIcao))
                {
                    modelsByDesignator.insert(aircraftIcao, emptyAirlineDesignatorMap);
                }
                QMap<QString, CAircraftModelList> &airlineModels = modelsByDesignator[aircraftIcao];
                const QString airlineIcao(
                    model.getLivery().isColorLivery() ? colorLiveryDesignator :
                    model.hasAirlineDesignator() ? model.getAirlineIcaoCodeDesignator() : emptyDesignator);
                if (airlineModels.contains(airlineIcao))
                {
                    airlineModels[airlineIcao].push_back(model);
                }
                else
                {
                    airlineModels.insert(airlineIcao, CAircraftModelList({model}));
                }
            }

            // to HTML
            QString html("<table>\n");
            QStringList airlineIcaos = models.getAirlineVDesignators().values();
            airlineIcaos.sort();
            airlineIcaos.push_front(colorLiveryDesignator);
            airlineIcaos.push_back(emptyDesignator);
            QStringList aircraftIcaos = modelsByDesignator.keys();
            aircraftIcaos.sort();

            // header
            html += "<thead><tr>\n"
                    "<th></th>";
            for (const QString &airline : std::as_const(airlineIcaos))
            {
                html += "<th>";
                html += airline;
                html += "</th>";
            }
            html += "\n</tr></thead>\n"
                    "<tbody>\n";

            // fill data
            for (const QString &aircraftIcao : std::as_const(aircraftIcaos))
            {
                html += "<tr>\n"
                        "  <th>";
                html += aircraftIcao;
                html += "</th>\n";

                const QMap<QString, CAircraftModelList> &airlineModels = modelsByDesignator[aircraftIcao];
                for (const QString &airline : std::as_const(airlineIcaos))
                {
                    if (airlineModels.contains(airline))
                    {
                        html += "  <td>";
                        const CAircraftModelList &models(airlineModels[airline]);
                        html += "<a>";
                        html += QString::number(models.size());
                        html += "</a><div class=\"mouseoverdisplay\">";
                        html += models.asHtmlSummary();
                        html += "</div>"
                                "</td>\n";
                    }
                    else
                    {
                        html += "  <td></td>\n";
                    }
                }
                html += "</tr>\n";
            }
            html += "</tbody>\n"
                    "</table>\n";
            return html;
        }

        QString CAircraftModelUtilities::createIcaoAirlineAircraftHtmlMatrixFile(const CAircraftModelList &models, const QString &tempDir)
        {
            Q_ASSERT_X(!tempDir.isEmpty(), Q_FUNC_INFO, "Need directory");
            if (models.isEmpty()) { return {}; }
            const QString html = createIcaoAirlineAircraftHtmlMatrix(models);
            if (html.isEmpty()) { return {}; }

            QDir dir(tempDir);
            BLACK_VERIFY_X(dir.exists(), Q_FUNC_INFO, "Directory does not exist");
            if (!dir.exists()) { return {}; }

            const QString htmlTemplate = CFileUtils::readFileToString(CSwiftDirectories::htmlTemplateFilePath());
            const QString fn("airlineAircraftMatrix.html");
            const bool ok = CFileUtils::writeStringToFile(htmlTemplate.arg(html), dir.absoluteFilePath(fn));
            return ok ? dir.absoluteFilePath(fn) : "";
        }

        CStatusMessageList CAircraftModelUtilities::validateModelFiles(const CSimulatorInfo &simulator, const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmpty, int stopAtFailedFiles, std::atomic_bool &wasStopped, const QString &simulatorDir)
        {
            // some generic tests
            CLogCategoryList cats = { CLogCategories::matching() };
            CStatusMessageList msgs;
            if (models.isEmpty())
            {
                msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, u"No models", true));
                return msgs;
            }

            const int noDb = models.size() - models.countWithValidDbKey();
            if (noDb > 0)
            {
                msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, QStringLiteral("%1 models without DB data, is this intended?").arg(noDb), true));
                const QString ms = models.findWithoutValidDbKey(5).getModelStringList().join(", ");
                msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, QStringLiteral("Some of the non DB models are: '%1'").arg(ms), true));
            }

            const int noExcluded = models.countByMode(CAircraftModel::Exclude);
            if (noExcluded > 0)
            {
                msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, QStringLiteral("%1 models marked as excluded, is this intended?").arg(noExcluded), true));
                const QString ms = models.findAllExcludedModels(5).getModelStringList().join(", ");
                msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, QStringLiteral("Some of the excluded models are: '%1'").arg(ms), true));
            }

            // specific checks for FSX/XPlane/FG
            CStatusMessageList specificTests;
            if (simulator.isMicrosoftOrPrepare3DSimulator() || models.isLikelyFsFamilyModelList())
            {
                const CStatusMessageList specificTests1 = FsCommon::CFsCommonUtil::validateAircraftConfigFiles(models, validModels, invalidModels, ignoreEmpty, stopAtFailedFiles, wasStopped);
                specificTests.push_back(specificTests1);

                if (simulator.isP3D())
                {
                    const CStatusMessageList specificTests2 = FsCommon::CFsCommonUtil::validateP3DSimObjectsPath(models, validModels, invalidModels, ignoreEmpty, stopAtFailedFiles, wasStopped, simulatorDir);
                    specificTests.push_back(specificTests2);
                }
                else if (simulator.isFSX())
                {
                    const CStatusMessageList specificTests2 = FsCommon::CFsCommonUtil::validateFSXSimObjectsPath(models, validModels, invalidModels, ignoreEmpty, stopAtFailedFiles, wasStopped, simulatorDir);
                    specificTests.push_back(specificTests2);
                }
            }
            else if (simulator.isXPlane() || models.isLikelyXPlaneModelList())
            {
                specificTests = models.validateFiles(validModels, invalidModels, ignoreEmpty, stopAtFailedFiles, wasStopped, simulatorDir);
            }
            else
            {
                specificTests = models.validateFiles(validModels, invalidModels, ignoreEmpty, stopAtFailedFiles, wasStopped, {});
            }

            msgs.push_back(specificTests);
            return msgs;
        }
    } // ns
} // ns
