/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorsetup.h"

namespace BlackMisc
{
    namespace Simulation
    {
        void CSimulatorSetup::setValue(const QString &name, const QString &value)
        {
            this->m_data.addOrReplaceValue(name, value);
        }

        QString CSimulatorSetup::getStringValue(const QString &name) const
        {
            return m_data.getValueAsString(name);
        }

        void CSimulatorSetup::setSimulatorVersion(const QString versionInfo)
        {
            this->setValue("all/versionInfo", versionInfo);
        }

        void CSimulatorSetup::setSimulatorInstallationDirectory(const QString fullFilePath)
        {
            this->setValue("all/installDir", fullFilePath);
        }

        QString CSimulatorSetup::getSimulatorVersion() const
        {
            return this->getStringValue("all/versionInfo");
        }

        QString CSimulatorSetup::getSimulatorInstallationDirectory() const
        {
            return this->getStringValue("all/installDir");
        }

        void CSimulatorSetup::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::Simulation::CSimulatorSetup>();
            qDBusRegisterMetaType<BlackMisc::Simulation::CSimulatorSetup>();
            registerMetaValueType<BlackMisc::Simulation::CSimulatorSetup>();
        }

        QString CSimulatorSetup::convertToQString(bool i18n) const
        {
            return m_data.toQString(i18n);
        }

        CVariant CSimulatorSetup::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexData:
                return CVariant::from(m_data);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CSimulatorSetup::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromCVariant(variant);
                return;
            }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexData:
                this->m_data.convertFromCVariant(variant.value<QString>());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

    } // ns
} // ns
