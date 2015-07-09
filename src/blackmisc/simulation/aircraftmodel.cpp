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
#include <QString>

using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Simulation
    {
        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type) :
            m_modelString(model), m_modelType(type)
        {}

        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type, const QString &description, const Aviation::CAircraftIcaoData &icao, const Aviation::CLivery &livery) :
            m_icao(icao), m_livery(livery), m_modelString(model), m_description(description), m_modelType(type)
        {}

        CAircraftModel::CAircraftModel(const Aviation::CAircraft &aircraft) :
            m_callsign(aircraft.getCallsign()), m_icao(aircraft.getIcaoInfo()), m_livery(aircraft.getLivery())
        { }

        QString CAircraftModel::convertToQString(bool i18n) const
        {
            QString s = this->m_modelString;
            if (!s.isEmpty()) { s += ' '; }
            s += this->getModelTypeAsString();
            s += ' ';
            s += this->m_icao.toQString(i18n);
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
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModelString:
                return CVariant(this->m_modelString);
            case IndexHasQueriedModelString:
                return CVariant::fromValue(this->hasQueriedModelString());
            case IndexModelType:
                return CVariant::fromValue(static_cast<int>(this->m_modelType));
            case IndexModelTypeAsString:
                return CVariant(this->getModelTypeAsString());
            case IndexDescription:
                return CVariant(this->m_description);
            case IndexFileName:
                return CVariant(this->m_fileName);
            case IndexIcao:
                return m_icao.propertyByIndex(index.copyFrontRemoved());
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
            case IndexIcao:
                this->m_icao.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexLivery:
                this->m_livery.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexDescription:
                this->m_description = variant.toQString();
                break;
            case IndexCallsign:
                this->m_callsign.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexFileName:
                this->m_fileName = variant.toQString();
                break;
            case IndexModelType:
                this->m_modelType = static_cast<ModelType>(variant.toInt());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

        const CAircraftIcaoCode &CAircraftModel::getAircraftIcaoCode() const
        {
            return m_icao.getAircraftIcaoCode();
        }

        const CAirlineIcaoCode &CAircraftModel::getAirlineIcaoCode() const
        {
            return m_icao.getAirlineIcaoCode();
        }

        void CAircraftModel::updateMissingParts(const CAircraftModel &model)
        {
            if (this->m_modelString.isEmpty()) { this->m_modelString = model.getModelString(); }
            if (this->m_description.isEmpty()) { this->m_description = model.getDescription(); }
            if (this->m_fileName.isEmpty())    { this->m_fileName    = model.getFileName(); }
            if (this->m_callsign.isEmpty())    { this->m_callsign    = model.getCallsign(); }
            if (this->m_modelType == static_cast<int>(TypeUnknown)) { this->m_modelType = model.getModelType(); }
            this->m_icao.updateMissingParts(model.getIcao());
        }

        bool CAircraftModel::hasQueriedModelString() const
        {
            return this->m_modelType == TypeQueriedFromNetwork && this->hasModelString();
        }

        bool CAircraftModel::hasManuallySetString() const
        {
            return this->m_modelType == TypeManuallySet && this->hasModelString();
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

        QString CAircraftModel::modelTypeToString(CAircraftModel::ModelType type)
        {
            switch (type)
            {
            case TypeQueriedFromNetwork: return "queried";
            case TypeModelMatching: return "matching";
            case TypeModelMapping: return "mapping";
            case TypeModelMatchingDefaultModel: return "map. default";
            case TypeOwnSimulatorModel: return "own simulator";
            case TypeManuallySet: return "set";
            case TypeUnknown:
            default: return "unknown";
            }
        }

        CAircraftModel CAircraftModel::fromDatabaseJson(const QJsonObject &json)
        {
            QJsonArray inner = json["cell"].toArray();
            Q_ASSERT_X(!inner.isEmpty(), Q_FUNC_INFO, "Missing JSON");
            if (inner.isEmpty()) { return CAircraftModel(); }

            // int i = 0;

            int i = 0;
            int dbKey(inner.at(i++).toInt(-1));
            QString modelString(inner.at(i++).toString());
            QString distributorKey(inner.at(i++).toString());
            QString liveryDescription(inner.at(i++).toString());
            QString modelDescription;

            CAircraftIcaoData aircraftIcao;
            CAirlineIcaoCode airlineIcao;
            CLivery livery;
            CDistributor distributor(distributorKey, "", "", "");

            bool fsx = CDatastoreUtility::dbBoolStringToBool(inner.at(i++).toString());
            bool fs9 = CDatastoreUtility::dbBoolStringToBool(inner.at(i++).toString());
            bool xp = CDatastoreUtility::dbBoolStringToBool(inner.at(i++).toString());
            CSimulatorInfo simInfo(fsx, fs9, xp);

            CAircraftModel model(
                modelString, CAircraftModel::TypeModelMapping, modelDescription, aircraftIcao, livery
            );
            model.setDbKey(dbKey);
            model.setSimulatorInfo(simInfo);
            return model;
        }

    } // namespace
} // namespace
