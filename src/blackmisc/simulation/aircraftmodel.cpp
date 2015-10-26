/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodel.h"
#include "distributor.h"
#include "blackmisc/datastoreutility.h"
#include "blackmisc/comparefunctions.h"
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>

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
            m_aircraftIcao(icao), m_livery(livery), m_simulator(simulator), m_modelString(model.trimmed().toUpper()), m_modelName(name.trimmed()), m_description(description.trimmed()), m_modelType(type)
        { }

        QString CAircraftModel::convertToQString(bool i18n) const
        {
            QString s = this->m_modelString;
            if (!s.isEmpty()) { s += ' '; }
            s += this->getModelTypeAsString();
            s += ' ';
            s += this->m_aircraftIcao.toQString(i18n);
            if (!this->m_fileName.isEmpty())
            {
                s += ' ';
                s += m_fileName;
            }
            return s;
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
                return CVariant(this->getModelModeAsString());
            case IndexDistributor:
                return m_distributor.propertyByIndex(index.copyFrontRemoved());
            case IndexSimulatorInfo:
                return m_simulator.propertyByIndex(index.copyFrontRemoved());
            case IndexSimulatorInfoAsString:
                return CVariant(m_simulator.toQString());
            case IndexDescription:
                return CVariant(this->m_description);
            case IndexName:
                return CVariant(this->m_modelName);
            case IndexFileName:
                return CVariant(this->m_fileName);
            case IndexAircraftIcaoCode:
                return m_aircraftIcao.propertyByIndex(index.copyFrontRemoved());
            case IndexLivery:
                return m_livery.propertyByIndex(index.copyFrontRemoved());
            case IndexCallsign:
                return m_callsign.propertyByIndex(index.copyFrontRemoved());
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
                this->m_modelName = variant.toQString();
                break;
            case IndexCallsign:
                this->m_callsign.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexFileName:
                this->m_fileName = variant.toQString();
                break;
            case IndexModelType:
                this->m_modelType = variant.value<ModelType>();
                break;
            case IndexModelMode:
                this->m_modelMode = variant.value<ModelMode>();
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
            case IndexSimulatorInfo:
                return this->m_simulator.comparePropertyByIndex(compareValue.getSimulatorInfo(), index.copyFrontRemoved());
            case IndexName:
                return this->m_modelName.compare(compareValue.getName(), Qt::CaseInsensitive);
            case IndexCallsign:
                break;
            case IndexFileName:
                return this->m_fileName.compare(compareValue.getFileName(), Qt::CaseInsensitive);
            case IndexModelType:
                return Compare::compare(this->m_modelType, compareValue.getModelType());
            case IndexModelMode:
                return Compare::compare(this->m_modelMode, compareValue.getModelMode());
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

        bool CAircraftModel::hasAircraftAndAirlineDesignator() const
        {
            return this->m_aircraftIcao.hasDesignator() && this->m_livery.hasValidAirlineDesignator();
        }

        bool CAircraftModel::hasAircraftDesignator() const
        {
            return this->m_aircraftIcao.hasDesignator();
        }

        bool CAircraftModel::hasAirlineDesignator() const
        {
            return this->m_livery.hasValidAirlineDesignator();
        }

        void CAircraftModel::updateMissingParts(const CAircraftModel &model)
        {
            if (this->m_modelString.isEmpty()) { this->setModelString(model.getModelString()); }
            if (this->m_description.isEmpty()) { this->setDescription(model.getDescription()); }
            if (this->m_fileName.isEmpty())    { this->setFileName(model.getFileName()); }
            if (this->m_callsign.isEmpty())    { this->setCallsign(model.getCallsign()); }
            if (this->m_modelType == TypeUnknown) { this->m_modelType = model.getModelType(); }
            if (this->m_simulator.isUnspecified())
            {
                this->setSimulatorInfo(model.getSimulatorInfo());
            }
            else
            {
                this->m_simulator.add(model.getSimulatorInfo());
            }

            this->m_livery.updateMissingParts(model.getLivery());
            this->m_aircraftIcao.updateMissingParts(model.getAircraftIcaoCode());
            this->m_distributor.updateMissingParts(model.getDistributor());
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

        bool CAircraftModel::matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            if (sensitivity == Qt::CaseSensitive)
            {
                return modelString == this->m_modelString;
            }
            else
            {
                return this->m_modelString.length() == modelString.length() &&
                       this->m_modelString.indexOf(modelString) == 0;
            }
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
            case TypeUnknown:
            default: return "unknown";
            }
        }

        CAircraftModel::ModelMode CAircraftModel::modelModeFromString(const QString &mode)
        {
            if (mode.isEmpty() || mode.startsWith('I', Qt::CaseInsensitive)) { return Include;}
            if (mode.startsWith('E', Qt::CaseInsensitive)) { return Exclude; }
            Q_ASSERT_X(false, Q_FUNC_INFO, "wrong mode");
            return Include; // default
        }

        QString CAircraftModel::modelModeToString(CAircraftModel::ModelMode mode)
        {
            switch (mode)
            {
            case Include: return "Include";
            case Exclude: return "Exclude";
            default: Q_ASSERT_X(false, Q_FUNC_INFO, "wrong mode");
            }
            return "Include";
        }

        CAircraftModel CAircraftModel::fromDatabaseJson(const QJsonObject &json, const QString prefix)
        {
            QString modelString(json.value(prefix + "simkey").toString());
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
            model.setModelMode(modelModeFromString(modelMode));
            model.setKeyAndTimestampFromDatabaseJson(json, prefix);
            return model;
        }
    } // namespace
} // namespace
