/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/compare.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/icon.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/icons.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/verify.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QString>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Db;

namespace BlackMisc
{
    namespace Simulation
    {
        void CAircraftModel::registerMetadata()
        {
            CValueObject<CAircraftModel>::registerMetadata();
            qRegisterMetaType<ModelType>();
        }

        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type) :
            m_modelString(model.trimmed().toUpper()), m_modelType(type)
        {}

        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type, const CAircraftIcaoCode &icao, const CLivery &livery) :
            m_aircraftIcao(icao), m_livery(livery), m_modelString(model.trimmed().toUpper()), m_modelType(type)
        {}

        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type, const QString &description, const CAircraftIcaoCode &icao, const Aviation::CLivery &livery) :
            m_aircraftIcao(icao), m_livery(livery), m_modelString(model.trimmed().toUpper()), m_description(description.trimmed()), m_modelType(type)
        {}

        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type, const CSimulatorInfo &simulator, const QString &name, const QString &description, const CAircraftIcaoCode &icao, const CLivery &livery) :
            m_aircraftIcao(icao), m_livery(livery), m_simulator(simulator), m_modelString(model.trimmed().toUpper()), m_name(name.trimmed()), m_description(description.trimmed()), m_modelType(type)
        { }

        QString CAircraftModel::convertToQString(bool i18n) const
        {
            QString s = this->m_modelString;
            if (!s.isEmpty()) { s += " type: "; }
            s += this->getModelTypeAsString();
            s += ' ';
            s += this->getAircraftIcaoCode().toQString(i18n);
            s += ' ';
            s += this->m_livery.toQString(i18n);
            if (!this->m_fileName.isEmpty())
            {
                s += ' ';
                s += m_fileName;
            }
            return s;
        }

        QJsonObject CAircraftModel::toDatabaseJson() const
        {
            QJsonObject obj;

            // filename not in DB
            obj.insert("id", this->getDbKeyAsJsonValue());
            obj.insert("name", this->getName());
            obj.insert("modelstring", QJsonValue(this->m_modelString));
            obj.insert("description", QJsonValue(this->m_description));
            obj.insert("mode", QJsonValue(getModelModeAsString().left(1).toUpper()));

            // sims
            const CSimulatorInfo sim(getSimulator());
            QString flag = CDatastoreUtility::boolToDbYN(sim.fsx());
            obj.insert("simfsx", QJsonValue(flag));
            flag = CDatastoreUtility::boolToDbYN(sim.p3d());
            obj.insert("simp3d", QJsonValue(flag));
            flag = CDatastoreUtility::boolToDbYN(sim.fs9());
            obj.insert("simfs9", QJsonValue(flag));
            flag = CDatastoreUtility::boolToDbYN(sim.xplane());
            obj.insert("simxplane", QJsonValue(flag));

            // foreign keys
            obj.insert("iddistributor", this->getDistributor().getDbKeyAsJsonValue());
            obj.insert("idaircrafticao", this->getAircraftIcaoCode().getDbKeyAsJsonValue());
            obj.insert("idlivery", this->getLivery().getDbKeyAsJsonValue());
            obj.insert("idairlineicao", this->getLivery().getAirlineIcaoCode().getDbKeyAsJsonValue()); // not really needed if livery is complete

            return obj;
        }

        QString CAircraftModel::toDatabaseJsonString(QJsonDocument::JsonFormat format) const
        {
            return QJsonDocument(this->toDatabaseJson()).toJson(format);
        }

        QJsonObject CAircraftModel::toMemoizedJson(MemoHelper::CMemoizer &helper) const
        {
            QJsonObject json;
            auto meta = introspect<CAircraftModel>().without(MetaFlags<DisabledForJson>());
            meta.forEachMemberName(*this, [ & ](const auto & member, CExplicitLatin1String name)
            {
                auto &&maybeMemo = helper.maybeMemoize(member);
                json << std::make_pair(name.toJsonKey(), std::cref(maybeMemo));
            });
            return json;
        }

        void CAircraftModel::convertFromMemoizedJson(const QJsonObject &json, const MemoHelper::CUnmemoizer &helper)
        {
            auto meta = introspect<CAircraftModel>().without(MetaFlags<DisabledForJson>());
            meta.forEachMemberName(*this, [ & ](auto & member, CExplicitLatin1String name)
            {
                auto it = json.find(name);
                if (it != json.end()) { it.value() >> helper.maybeUnmemoize(member).get(); }
            });
        }

        QString CAircraftModel::asHtmlSummary() const
        {
            const QString html = "Model: %1<br>Aircraft ICAO: %2<br>Livery: %3";
            return html.arg(this->getModelStringAndDbKey(), this->getAircraftIcaoCode().asHtmlSummary(), this->getLivery().asHtmlSummary());
        }

        bool CAircraftModel::canInitializeFromFsd() const
        {
            const bool nw = this->getModelType() == CAircraftModel::TypeQueriedFromNetwork ||
                            this->getModelType() == CAircraftModel::TypeFSInnData ||
                            this->getModelType() == CAircraftModel::TypeUnknown;
            return nw;
        }

        QString CAircraftModel::getModelStringAndDbKey() const
        {
            if (this->hasValidDbKey())
            {
                if (this->hasModelString())
                {
                    return QString(this->getModelString()).append(" ").append(this->getDbKeyAsStringInParentheses());
                }
                else
                {
                    return this->getDbKeyAsString();
                }
            }
            else
            {
                return this->getModelString();
            }
        }

        CVariant CAircraftModel::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }
            if (IOrderable::canHandleIndex(index)) { return IOrderable::propertyByIndex(index);}

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModelString:
                return CVariant(this->m_modelString);
            case IndexHasQueriedModelString:
                return CVariant::fromValue(this->hasQueriedModelString());
            case IndexModelType:
                return CVariant::fromValue(this->m_modelType);
            case IndexModelTypeAsString:
                return CVariant(this->getModelTypeAsString());
            case IndexModelMode:
                return CVariant::fromValue(this->m_modelMode);
            case IndexModelModeAsString:
                return CVariant::fromValue(this->getModelModeAsString());
            case IndexModelModeAsIcon:
                return CVariant::fromValue(this->getModelModeAsIcon());
            case IndexDistributor:
                return m_distributor.propertyByIndex(index.copyFrontRemoved());
            case IndexSimulatorInfo:
                return m_simulator.propertyByIndex(index.copyFrontRemoved());
            case IndexSimulatorInfoAsString:
                return CVariant(m_simulator.toQString());
            case IndexDescription:
                return CVariant(this->m_description);
            case IndexName:
                return CVariant(this->m_name);
            case IndexFileName:
                return CVariant(this->m_fileName);
            case IndexIconPath:
                return CVariant(this->m_iconPath);
            case IndexAircraftIcaoCode:
                return m_aircraftIcao.propertyByIndex(index.copyFrontRemoved());
            case IndexLivery:
                return m_livery.propertyByIndex(index.copyFrontRemoved());
            case IndexCallsign:
                return m_callsign.propertyByIndex(index.copyFrontRemoved());
            case IndexMembersDbStatus:
                return getMembersDbStatus();
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CAircraftModel::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAircraftModel>(); return; }
            if (IOrderable::canHandleIndex(index)) { IOrderable::setPropertyByIndex(index, variant); return; }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { IDatastoreObjectWithIntegerKey::setPropertyByIndex(index, variant); return; }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModelString:
                this->m_modelString = variant.toQString();
                break;
            case IndexAircraftIcaoCode:
                this->m_aircraftIcao.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexLivery:
                this->m_livery.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexDistributor:
                this->m_distributor.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexDescription:
                this->m_description = variant.toQString();
                break;
            case IndexSimulatorInfo:
                this->m_simulator.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexName:
                this->m_name = variant.toQString();
                break;
            case IndexCallsign:
                this->m_callsign.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexFileName:
                this->m_fileName = variant.toQString();
                break;
            case IndexIconPath:
                this->m_iconPath = variant.toQString();
                break;
            case IndexModelType:
                this->m_modelType = variant.value<ModelType>();
                break;
            case IndexModelMode:
                if (variant.type() == QMetaType::QString)
                {
                    this->setModelModeAsString(variant.toQString());
                }
                else
                {
                    this->m_modelMode = variant.value<ModelMode>();
                }
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        int CAircraftModel::comparePropertyByIndex(const CPropertyIndex &index, const CAircraftModel &compareValue) const
        {
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::comparePropertyByIndex(index, compareValue);}
            if (IOrderable::canHandleIndex(index)) { return IOrderable::comparePropertyByIndex(index, compareValue);}
            if (index.isMyself()) { return this->m_modelString.compare(compareValue.getModelString(), Qt::CaseInsensitive); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModelString:
                return this->m_modelString.compare(compareValue.getModelString(), Qt::CaseInsensitive);
            case IndexAircraftIcaoCode:
                return this->m_aircraftIcao.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getAircraftIcaoCode());
            case IndexLivery:
                return this->m_livery.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getLivery());
            case IndexDistributor:
                return this->m_distributor.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getDistributor());
            case IndexDescription:
                return this->m_description.compare(compareValue.getDescription(), Qt::CaseInsensitive);
            case IndexSimulatorInfoAsString:
            case IndexSimulatorInfo:
                return this->m_simulator.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getSimulator());
            case IndexName:
                return this->m_name.compare(compareValue.getName(), Qt::CaseInsensitive);
            case IndexCallsign:
                return this->m_callsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign());
            case IndexFileName:
                return this->m_fileName.compare(compareValue.getFileName(), Qt::CaseInsensitive);
            case IndexIconPath:
                return this->m_iconPath.compare(compareValue.getIconPath(), Qt::CaseInsensitive);
            case IndexModelType:
                return Compare::compare(this->m_modelType, compareValue.getModelType());
            case IndexModelMode:
            case IndexModelModeAsString:
            case IndexModelModeAsIcon:
                return Compare::compare(this->m_modelMode, compareValue.getModelMode());
            case IndexMembersDbStatus:
                return getMembersDbStatus().compare(compareValue.getMembersDbStatus());
            default:
                break;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "No comparison");
            return 0;
        }

        bool CAircraftModel::setAircraftIcaoCode(const CAircraftIcaoCode &aircraftIcaoCode)
        {
            if (this->m_aircraftIcao == aircraftIcaoCode) { return false; }
            this->m_aircraftIcao = aircraftIcaoCode;
            return true;
        }

        void CAircraftModel::setAircraftIcaoDesignator(const QString &designator)
        {
            this->m_aircraftIcao.setDesignator(designator);
        }

        void CAircraftModel::setAircraftIcaoCodes(const CAircraftIcaoCode &aircraftIcaoCode, const CAirlineIcaoCode &airlineIcaoCode)
        {
            m_aircraftIcao = aircraftIcaoCode;
            m_livery.setAirlineIcaoCode(airlineIcaoCode);
        }

        bool CAircraftModel::hasValidAircraftAndAirlineDesignator() const
        {
            return this->hasKnownAircraftDesignator() && this->m_livery.hasValidAirlineDesignator();
        }

        bool CAircraftModel::hasAircraftDesignator() const
        {
            return this->m_aircraftIcao.hasDesignator();
        }

        bool CAircraftModel::hasKnownAircraftDesignator() const
        {
            return this->m_aircraftIcao.hasKnownDesignator();
        }

        bool CAircraftModel::hasAirlineDesignator() const
        {
            return this->m_livery.hasValidAirlineDesignator();
        }

        bool CAircraftModel::hasAircraftAndAirlineDesignator() const
        {
            return this->hasAircraftDesignator() && this->hasAirlineDesignator();
        }

        bool CAircraftModel::isMilitary() const
        {
            return this->getAircraftIcaoCode().isMilitary() ||
                   this->getLivery().isMilitary();
        }

        bool CAircraftModel::setDistributorOrder(int order)
        {
            if (order < 0) { return false; }
            if (!this->m_distributor.isLoadedFromDb()) { return false; }
            this->m_distributor.setOrder(order);
            return true;
        }

        bool CAircraftModel::setDistributorOrder(const CDistributorList &distributors)
        {
            if (distributors.isEmpty()) { return false; }
            bool found = false;
            const int noDistributorOrder = distributors.size();
            if (this->hasDbDistributor())
            {
                const CDistributor d = distributors.findByKeyOrAlias(this->m_distributor.getDbKey());
                if (d.hasValidDbKey())
                {
                    this->m_distributor.setOrder(d.getOrder());
                    found = true;
                }
                else
                {
                    this->m_distributor.setOrder(noDistributorOrder);
                }
            }
            else
            {
                this->m_distributor.setOrder(noDistributorOrder);
            }
            return found;
        }

        bool CAircraftModel::hasDbDistributor() const
        {
            return this->m_distributor.isLoadedFromDb();
        }

        bool CAircraftModel::hasDistributor() const
        {
            return this->m_distributor.hasValidDbKey(); // key is valid, but not guaranteed from DB
        }

        bool CAircraftModel::matchesDbDistributor(const CDistributor &distributor) const
        {
            if (!distributor.isLoadedFromDb()) { return false; }
            if (!this->hasDbDistributor()) { return false; }
            return this->m_distributor.getDbKey() == distributor.getDbKey();
        }

        bool CAircraftModel::matchesAnyDbDistributor(const CDistributorList &distributors) const
        {
            if (distributors.isEmpty()) { return false; }
            if (!this->hasDbDistributor()) { return false; }
            return distributors.matchesAnyKeyOrAlias(this->m_distributor.getDbKey());
        }

        bool CAircraftModel::matchesMode(ModelModeFilter mode) const
        {
            if (mode == All) { return true; }
            return (mode & this->m_modelMode) > 0;
        }

        const CIcon &CAircraftModel::getModelModeAsIcon() const
        {
            switch (this->getModelMode())
            {
            case Include:
                return CIconList::iconByIndex(CIcons::ModelInclude);
            case Exclude:
                return CIconList::iconByIndex(CIcons::ModelExclude);
            case Undefined:
                return CIconList::iconByIndex(CIcons::StandardIconUnknown16);
            default:
                Q_ASSERT_X(false, Q_FUNC_INFO, "wrong mode");
                break;
            }
            return CIconList::iconByIndex(CIcons::ModelInclude);
        }

        void CAircraftModel::setModelModeAsString(const QString &mode)
        {
            this->setModelMode(CAircraftModel::modelModeFromString(mode));
        }

        bool CAircraftModel::matchesSimulator(const CSimulatorInfo &simulator) const
        {
            return (static_cast<int>(simulator.getSimulator()) & static_cast<int>(this->getSimulator().getSimulator())) > 0;
        }

        bool CAircraftModel::matchesSimulatorFlag(CSimulatorInfo::Simulator simulator) const
        {
            return (static_cast<int>(simulator) & static_cast<int>(this->getSimulator().getSimulator())) > 0;
        }

        CPixmap CAircraftModel::loadIcon(CStatusMessage &success) const
        {
            static const CStatusMessage noIcon(this, CStatusMessage::SeverityInfo, "no icon");
            if (this->m_iconPath.isEmpty()) { success = noIcon; return CPixmap(); }

            // load from file
            const CPixmap pm(CPixmap::loadFromFile(this->m_iconPath, success));
            return pm;
        }

        QString CAircraftModel::getSwiftLiveryString() const
        {
            const QString cc(this->getLivery().getCombinedCode());
            if (cc.isEmpty() && !this->hasModelString()) { return ""; }
            if (cc.isEmpty()) { return this->getModelString(); }
            if (!this->hasModelString()) { return cc; }
            return cc + " [" + this->getModelString() + "]";
        }

        void CAircraftModel::updateMissingParts(const CAircraftModel &otherModel, bool dbModelPriority)
        {
            if (dbModelPriority && !this->hasValidDbKey() && otherModel.hasValidDbKey())
            {
                // we have no DB data, but the other one has
                // so we change roles. We take the DB object as base, and update our parts
                CAircraftModel copy(otherModel);
                copy.updateMissingParts(*this);
                *this = copy;
                return;
            }

            this->updateLocalFileNames(otherModel);
            if (this->m_callsign.isEmpty())       { this->setCallsign(otherModel.getCallsign()); }
            if (this->m_modelString.isEmpty())    { this->setModelString(otherModel.getModelString()); }
            if (this->m_name.isEmpty())           { this->setName(otherModel.getName()); }
            if (this->m_modelType == TypeUnknown) { this->m_modelType = otherModel.getModelType(); }
            if (this->m_modelMode == Undefined)   { this->m_modelType = otherModel.getModelType(); }
            if (this->m_description.isEmpty() || this->m_description.startsWith(CAircraftModel::autoGenerated(), Qt::CaseInsensitive)) { this->setDescription(otherModel.getDescription()); }
            if (this->m_simulator.isUnspecified())
            {
                this->setSimulator(otherModel.getSimulator());
            }
            else
            {
                this->m_simulator.add(otherModel.getSimulator());
            }

            this->m_livery.updateMissingParts(otherModel.getLivery());
            this->m_aircraftIcao.updateMissingParts(otherModel.getAircraftIcaoCode());
            this->m_distributor.updateMissingParts(otherModel.getDistributor());
        }

        bool CAircraftModel::hasQueriedModelString() const
        {
            return this->m_modelType == TypeQueriedFromNetwork && this->hasModelString();
        }

        bool CAircraftModel::hasManuallySetString() const
        {
            return this->m_modelType == TypeManuallySet && this->hasModelString();
        }

        bool CAircraftModel::hasDescription(bool ignoreAutoGenerated) const
        {
            if (this->m_description.isEmpty()) { return false; }
            if (!ignoreAutoGenerated) { return true; }
            return (!this->getDescription().startsWith(autoGenerated(), Qt::CaseInsensitive));
        }

        bool CAircraftModel::hasValidSimulator() const
        {
            return m_simulator.isAnySimulator();
        }

        QString CAircraftModel::getMembersDbStatus() const
        {
            QString s(hasValidDbKey() ? "M" : "m");
            s = s.append(getDistributor().hasValidDbKey() ? 'D' : 'd');
            s = s.append(getAircraftIcaoCode().hasValidDbKey() ? 'A' : 'a');
            if (getLivery().isLoadedFromDb() && getLivery().isColorLivery())
            {
                s = s.append("C-");
            }
            else
            {
                s = s.append(getLivery().isLoadedFromDb() ? 'L' : 'l');
                s = s.append(getLivery().getAirlineIcaoCode().hasValidDbKey() ? 'A' : 'a');
            }
            return s;
        }

        void CAircraftModel::normalizeFileNameForDb()
        {
            this->m_fileName = CAircraftModel::normalizeFileNameForDb(this->m_fileName);
        }

        void CAircraftModel::updateLocalFileNames(const CAircraftModel &model)
        {
            if (this->getModelType() == CAircraftModel::TypeOwnSimulatorModel)
            {
                // this is local model, ignore
                return;
            }

            if (model.getModelType() == CAircraftModel::TypeOwnSimulatorModel)
            {
                // other local, priority
                this->setFileName(model.getFileName());
                this->setIconPath(model.getIconPath());
                return;
            }

            // both not local, override empty values
            if (this->m_fileName.isEmpty()) { this->setFileName(model.getFileName()); }
            if (this->m_iconPath.isEmpty()) { this->setIconPath(model.getIconPath()); }
        }

        bool CAircraftModel::matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            return this->m_modelString.length() == modelString.length() &&
                   this->m_modelString.startsWith(modelString, sensitivity);
        }

        int CAircraftModel::calculateScore(const CAircraftModel &compareModel) const
        {
            int score = this->getAircraftIcaoCode().calculateScore(compareModel.getAircraftIcaoCode());
            score += this->getLivery().calculateScore(compareModel.getLivery());
            return 0.5 * score;
        }

        CStatusMessageList CAircraftModel::validate(bool withNestedObjects) const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation() }));
            CStatusMessageList msgs;
            if (!hasModelString()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Model: missing model string (aka key)")); }
            if (!hasValidSimulator()) {msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Model: no simulator set")); }
            if (!hasDescription()) {msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, "Model: no description")); }
            if (withNestedObjects)
            {
                msgs.push_back(m_aircraftIcao.validate());
                msgs.push_back(m_livery.validate());
                msgs.push_back(m_distributor.validate());
            }
            return msgs;
        }

        QString CAircraftModel::modelTypeToString(CAircraftModel::ModelType type)
        {
            switch (type)
            {
            case TypeQueriedFromNetwork: return "queried";
            case TypeModelMatching: return "matching";
            case TypeDatabaseEntry: return "database";
            case TypeModelMatchingDefaultModel: return "map. default";
            case TypeOwnSimulatorModel: return "own simulator";
            case TypeManuallySet: return "set";
            case TypeFSInnData: return "FSInn";
            case TypeUnknown:
            default: return "unknown";
            }
        }

        QString CAircraftModel::normalizeFileNameForDb(const QString &filePath)
        {
            QString n = CFileUtils::normalizeFilePathToQtStandard(filePath).toUpper();
            if (n.count('/') < 2) { return n; }
            return n.section('/', -2, -1);
        }

        CAircraftModel::ModelMode CAircraftModel::modelModeFromString(const QString &mode)
        {
            if (mode.isEmpty() || mode.startsWith('I', Qt::CaseInsensitive)) { return Include;}
            if (mode.startsWith('E', Qt::CaseInsensitive)) { return Exclude; }
            BLACK_VERIFY_X(false, Q_FUNC_INFO, "wrong mode");
            return Include; // default
        }

        const QString &CAircraftModel::modelModeToString(CAircraftModel::ModelMode mode)
        {
            static const QString i("Include");
            static const QString e("Exclude");

            switch (mode)
            {
            case Include: return i;
            case Exclude: return e;
            default: Q_ASSERT_X(false, Q_FUNC_INFO, "wrong mode");
            }
            return i; // default
        }

        CAircraftModel CAircraftModel::fromDatabaseJson(const QJsonObject &json, const QString prefix)
        {
            QString modelString(json.value(prefix + "modelstring").toString());
            QString modelDescription(json.value(prefix + "description").toString());
            QString modelName(json.value(prefix + "name").toString());
            QString modelMode(json.value(prefix + "mode").toString());

            const CSimulatorInfo simInfo = CSimulatorInfo::fromDatabaseJson(json, prefix);
            const CAircraftIcaoCode aircraftIcao(CAircraftIcaoCode::fromDatabaseJson(json, "ac_"));
            const CLivery livery(CLivery::fromDatabaseJson(json, "liv_"));
            const CDistributor distributor(CDistributor::fromDatabaseJson(json, "dist_"));

            CAircraftModel model(
                modelString, CAircraftModel::TypeDatabaseEntry, simInfo, modelName, modelDescription, aircraftIcao, livery
            );
            model.setDistributor(distributor);
            model.setModelModeAsString(modelMode);
            model.setKeyAndTimestampFromDatabaseJson(json, prefix);
            return model;
        }

        QStringList CAircraftModel::splitNetworkLiveryString(const QString &liveryString)
        {
            QStringList liveryModelStrings({ "", "" });
            if (liveryString.isEmpty()) { return liveryModelStrings; }
            const QString l(liveryString.toUpper().trimmed());
            if (liveryString.contains('[') && liveryString.contains(']'))
            {
                // seems to be a valid swift string
                const QStringList split = l.split("[");
                if (split.size() > 0)
                {
                    liveryModelStrings[0] = split[0].trimmed();
                }
                if (split.size() > 1)
                {
                    QString m = split[1];
                    m.replace('[', ' ');
                    m.replace(']', ' ');
                    liveryModelStrings[1] = m.trimmed();
                }
            }
            else
            {
                if (CLivery::isValidCombinedCode(l))
                {
                    liveryModelStrings[0] = l;
                }
            }
            return liveryModelStrings;
        }

        const QString &CAircraftModel::autoGenerated()
        {
            static const QString ag("swift auto generated");
            return ag;
        }
    } // namespace
} // namespace
