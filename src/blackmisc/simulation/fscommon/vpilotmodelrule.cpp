/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "vpilotmodelrule.h"

using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            CVPilotModelRule::CVPilotModelRule() { }

            CVPilotModelRule::CVPilotModelRule(const QString &modelName, const QString &folder, const QString &typeCode, const QString &callsignPrefix, qint64 updated) :
                m_modelName(modelName.trimmed().toUpper()), m_folder(folder.trimmed().toUpper()),
                m_typeCode(typeCode.trimmed().toUpper()), m_callsignPrefix(callsignPrefix.trimmed().toUpper()), m_updatedMsSinceEpoch(updated)
            { }

            const QString CVPilotModelRule::getDistributor() const
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

            CVariant CVPilotModelRule::propertyByIndex(const CPropertyIndex &index) const
            {
                if (index.isMyself()) { return CVariant::from(*this); }
                ColumnIndex i = index.frontCasted<ColumnIndex>();
                switch (i)
                {
                case IndexModelName: return CVariant::from(this->m_modelName);
                case IndexFolder: return CVariant::from(this->m_folder);
                case IndexTypeCode: return CVariant::from(this->m_typeCode);
                case IndexCallsignPrefix: return CVariant::from(this->m_callsignPrefix);
                case IndexUpdatedTimestamp: return CVariant::from(this->getUpdateTimestamp());
                case IndexUpdatedMsSinceEpoch: return CVariant::from(this->m_updatedMsSinceEpoch);
                default:
                    return CValueObject::propertyByIndex(index);
                }
            }

            void CVPilotModelRule::setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index)
            {
                if (index.isMyself()) { (*this) = variant.to<CVPilotModelRule>(); return; }
                ColumnIndex i = index.frontCasted<ColumnIndex>();
                switch (i)
                {
                case IndexModelName: this->setModelName(variant.value<QString>()); break;
                case IndexFolder: this->setFolder(variant.value<QString>()); break;
                case IndexTypeCode: this->setTypeCode(variant.value<QString>()); break;
                case IndexCallsignPrefix: this->setCallsignPrefix(variant.value<QString>()); break;
                case IndexUpdatedTimestamp: this->setUpdateTimestamp(variant.value<QDateTime>()); break;
                case IndexUpdatedMsSinceEpoch: this->setUpdateTimestamp(variant.value<qint64>()); break;
                default:
                    CValueObject::setPropertyByIndex(variant, index);
                    break;
                }
            }

            QString CVPilotModelRule::convertToQString(bool i18n) const
            {
                Q_UNUSED(i18n);
                QString s(this->m_modelName);
                return s;
            }

            CAircraftMapping CVPilotModelRule::toMapping() const
            {
                return CAircraftMapping("vpilot", this->getDistributor(), this->getTypeCode(), this->getCallsignPrefix(), this->getModelName());
            }
        } // namespace
    } // namespace
} // namespace
