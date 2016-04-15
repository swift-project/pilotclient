/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodel.h"
#include "distributorlist.h"
#include "blackmisc/datastoreutility.h"
#include "blackmisc/comparefunctions.h"
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include "blackmisc/verify.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

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

        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type, const QString &description, const CAircraftIcaoCode &icao, const Aviation::CLivery &livery) :
            m_aircraftIcao(icao), m_livery(livery), m_modelString(model.trimmed().toUpper()), m_description(description.trimmed()), m_modelType(type)
        {}

        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type, CSimulatorInfo &simulator, const QString &name, const QString &description, const CAircraftIcaoCode &icao, const CLivery &livery) :
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
            obj.insert("modelstring", QJsonValue(this->m_modelString));
            obj.insert("description", QJsonValue(this->m_description));
            obj.insert("mode", QJsonValue(getModelModeAsString().left(1).toUpper()));

            // sims
            const CSimulatorInfo sim(getSimulatorInfo());
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
            return QJsonDocument(toDatabaseJson()).toJson(format);
        }

        bool CAircraftModel::canInitializeFromFsd() const
        {
            bool nw = this->getModelType() == CAircraftModel::TypeQueriedFromNetwork ||
                      this->getModelType() == CAircraftModel::TypeFsdData ||
                      this->getModelType() == CAircraftModel::TypeUnknown;
            return nw;
        }

        CVariant CAircraftModel::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }
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

        void CAircraftModel::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself()) { (*this) = variant.to<CAircraftModel>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModelString:
                this->m_modelString = variant.toQString();
                break;
            case IndexAircraftIcaoCode:
                this->m_aircraftIcao.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexLivery:
                this->m_livery.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexDistributor:
                this->m_distributor.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexDescription:
                this->m_description = variant.toQString();
                break;
            case IndexSimulatorInfo:
                this->m_simulator.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexName:
                this->m_name = variant.toQString();
                break;
            case IndexCallsign:
                this->m_callsign.setPropertyByIndex(variant, index.copyFrontRemoved());
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
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

        int CAircraftModel::comparePropertyByIndex(const CAircraftModel &compareValue, const CPropertyIndex &index) const
        {
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::comparePropertyByIndex(compareValue, index);}
            if (index.isMyself()) { return this->m_modelString.compare(compareValue.getModelString(), Qt::CaseInsensitive); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModelString:
                return this->m_modelString.compare(compareValue.getModelString(), Qt::CaseInsensitive);
            case IndexAircraftIcaoCode:
                return this->m_aircraftIcao.comparePropertyByIndex(compareValue.getAircraftIcaoCode(), index.copyFrontRemoved());
            case IndexLivery:
                return this->m_livery.comparePropertyByIndex(compareValue.getLivery(), index.copyFrontRemoved());
            case IndexDistributor:
                return this->m_distributor.comparePropertyByIndex(compareValue.getDistributor(), index.copyFrontRemoved());
            case IndexDescription:
                return this->m_description.compare(compareValue.getDescription(), Qt::CaseInsensitive);
            case IndexSimulatorInfoAsString:
            case IndexSimulatorInfo:
                return this->m_simulator.comparePropertyByIndex(compareValue.getSimulatorInfo(), index.copyFrontRemoved());
            case IndexName:
                return this->m_name.compare(compareValue.getName(), Qt::CaseInsensitive);
            case IndexCallsign:
                return this->m_callsign.comparePropertyByIndex(compareValue.getCallsign(), index.copyFrontRemoved());
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

        bool CAircraftModel::isMilitary() const
        {
            return this->getAircraftIcaoCode().isMilitary() ||
                   this->getLivery().isMilitary();
        }

        bool CAircraftModel::hasDistributor() const
        {
            return this->m_distributor.hasValidDbKey();
        }

        bool CAircraftModel::matchesDistributor(const CDistributor &distributor) const
        {
            if (!distributor.hasValidDbKey()) { return false; }
            if (!this->hasDistributor()) { return false; }
            return this->m_distributor.getDbKey() == distributor.getDbKey();
        }

        bool CAircraftModel::matchesAnyDistributor(const CDistributorList &distributors) const
        {
            if (distributors.isEmpty()) { return false; }
            if (!this->hasDistributor()) { return false; }
            return distributors.matchesAnyKeyOrAlias(this->m_distributor.getDbKey());
        }

        bool CAircraftModel::matchesMode(ModelModeFilter mode) const
        {
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
            return (static_cast<int>(simulator.getSimulator()) & static_cast<int>(this->getSimulatorInfo().getSimulator())) > 0;
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

            if (this->m_callsign.isEmpty())       { this->setCallsign(otherModel.getCallsign()); }
            if (this->m_modelString.isEmpty())    { this->setModelString(otherModel.getModelString()); }
            if (this->m_description.isEmpty())    { this->setDescription(otherModel.getDescription()); }
            if (this->m_fileName.isEmpty())       { this->setFileName(otherModel.getFileName()); }
            if (this->m_iconPath.isEmpty())       { this->setIconPath(otherModel.getIconPath()); }
            if (this->m_callsign.isEmpty())       { this->setCallsign(otherModel.getCallsign()); }
            if (this->m_modelType == TypeUnknown) { this->m_modelType = otherModel.getModelType(); }
            if (this->m_modelMode == Undefined)   { this->m_modelType = otherModel.getModelType(); }
            if (this->m_simulator.isUnspecified())
            {
                this->setSimulatorInfo(otherModel.getSimulatorInfo());
            }
            else
            {
                this->m_simulator.add(otherModel.getSimulatorInfo());
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

        bool CAircraftModel::hasValidSimulator() const
        {
            return m_simulator.isAnySimulator();
        }

        QString CAircraftModel::getMembersDbStatus() const
        {
            QString s(hasValidDbKey() ? "M" : "m");
            s = s.append(getDistributor().hasValidDbKey() ? 'D' : 'd');
            s = s.append(getAircraftIcaoCode().hasValidDbKey() ? 'A' : 'a');
            s = s.append(getLivery().hasValidDbKey() ? 'L' : 'l');
            s = s.append(getLivery().getAirlineIcaoCode().hasValidDbKey() ? 'A' : 'a');
            return s;
        }

        bool CAircraftModel::matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            return this->m_modelString.length() == modelString.length() &&
                   this->m_modelString.startsWith(modelString, sensitivity);
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
            case TypeFsdData: return "FSD";
            case TypeUnknown:
            default: return "unknown";
            }
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

            bool fsx = CDatastoreUtility::dbBoolStringToBool(json.value(prefix + "simfsx").toString());
            bool fs9 = CDatastoreUtility::dbBoolStringToBool(json.value(prefix + "simfs9").toString());
            bool xp = CDatastoreUtility::dbBoolStringToBool(json.value(prefix + "simxplane").toString());
            bool p3d = CDatastoreUtility::dbBoolStringToBool(json.value(prefix + "simp3d").toString());

            CAircraftIcaoCode aircraftIcao(CAircraftIcaoCode::fromDatabaseJson(json, "ac_"));
            CLivery livery(CLivery::fromDatabaseJson(json, "liv_"));
            CDistributor distributor(CDistributor::fromDatabaseJson(json, "dist_"));

            CSimulatorInfo simInfo(fsx, fs9, xp, p3d);
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
    } // namespace
} // namespace
