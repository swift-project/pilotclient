// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/fscommon/vpilotmodelrule.h"
#include "blackmisc/simulation/simulatorinfo.h"

#include <Qt>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Simulation::FsCommon, CVPilotModelRule)

namespace BlackMisc::Simulation::FsCommon
{
    CVPilotModelRule::CVPilotModelRule() {}

    CVPilotModelRule::CVPilotModelRule(const QString &modelName, const QString &folder, const QString &typeCode, const QString &callsignPrefix, qint64 updated) : ITimestampBased(updated),
                                                                                                                                                                  m_modelName(modelName.trimmed().toUpper()), m_folder(folder.trimmed().toUpper()),
                                                                                                                                                                  m_typeCode(typeCode.trimmed().toUpper()), m_callsignPrefix(callsignPrefix.trimmed().toUpper()), m_updatedMsSinceEpoch(updated)
    {}

    QString CVPilotModelRule::getDistributor() const
    {
        QString f(this->getFolder().toUpper().simplified());
        f.replace(" ", "");
        if (f.isEmpty()) return ("UNKNOWN");
        if (f.startsWith("WOAI", Qt::CaseInsensitive)) { return "WOAI"; }
        if (f.startsWith("WORLDOFAI", Qt::CaseInsensitive)) { return "WOAI"; }
        if (f.startsWith("IVAO", Qt::CaseInsensitive)) { return "IVAO"; }
        if (f.startsWith("P3D", Qt::CaseInsensitive)) { return "P3D"; }
        if (f.startsWith("FSX", Qt::CaseInsensitive)) { return "FSX"; }
        if (f.startsWith("MYTRAFFIC", Qt::CaseInsensitive)) { return "MYTRAFFIC"; }
        if (f.startsWith("JUSTFLIGHT", Qt::CaseInsensitive)) { return "JUSTFLIGHT"; }
        if (f.startsWith("ULTIMATETRAFFIC", Qt::CaseInsensitive)) { return "ULTIMATETRAFFIC"; }
        if (f.startsWith("VIP", Qt::CaseInsensitive)) { return "VIP"; }
        return "??? - " + f;
    }

    QVariant CVPilotModelRule::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexModelName: return QVariant::fromValue(this->m_modelName);
        case IndexFolder: return QVariant::fromValue(this->m_folder);
        case IndexTypeCode: return QVariant::fromValue(this->m_typeCode);
        case IndexCallsignPrefix: return QVariant::fromValue(this->m_callsignPrefix);
        default:
            return CValueObject::propertyByIndex(index);
        }
    }

    void CVPilotModelRule::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CVPilotModelRule>();
            return;
        }
        if (ITimestampBased::canHandleIndex(index))
        {
            ITimestampBased::setPropertyByIndex(index, variant);
            return;
        }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexModelName: this->setModelName(variant.value<QString>()); break;
        case IndexFolder: this->setFolder(variant.value<QString>()); break;
        case IndexTypeCode: this->setTypeCode(variant.value<QString>()); break;
        case IndexCallsignPrefix: this->setCallsignPrefix(variant.value<QString>()); break;
        default:
            CValueObject::setPropertyByIndex(index, variant);
            break;
        }
    }

    QString CVPilotModelRule::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        QString s(this->m_modelName);
        return s;
    }

    CAircraftModel CVPilotModelRule::toAircraftModel() const
    {
        QString al(m_callsignPrefix);
        if (al.length() > 3)
        {
            // some known hardcoded fixes
            if (al.startsWith("USAF")) { al = "AIO"; }
        }
        const QString liveryPseudoCode(
            al.length() != 3 ?
                "" :
                al + "." + CLivery::standardLiveryMarker());
        const CAircraftIcaoCode aircraftIcao(m_typeCode);
        const CAirlineIcaoCode airlineIcao(al);
        const CLivery livery(liveryPseudoCode, airlineIcao, "vPilot rule based");
        const CDistributor distributor(getDistributor(), "vPilot based", "", "");
        CAircraftModel model(
            this->m_modelName, CAircraftModel::TypeVPilotRuleBased,
            CAircraftModel::autoGenerated(),
            aircraftIcao, livery);
        const CSimulatorInfo sim(CSimulatorInfo::FSX_P3D);
        model.setMSecsSinceEpoch(m_timestampMSecsSinceEpoch);
        model.setDistributor(distributor);
        model.setSimulator(sim);
        return model;
    }

} // namespace
