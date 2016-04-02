/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SIMULATORSETUP_H
#define BLACKMISC_SIMULATION_SIMULATORSETUP_H

#include "blackmisc/namevariantpairlist.h"
#include "blackmisc/blackmiscexport.h"
#include <QMap>
#include <QString>

namespace BlackMisc
{
    namespace Simulation
    {
        //! Simulator settings for flight simulators.
        //! Those are set up at runtime and represent information about the simulator (like a small registry)
        class BLACKMISC_EXPORT CSimulatorSetup : public CValueObject<CSimulatorSetup>
        {
        public:
            //! Specific values
            enum ColumnIndex
            {
                IndexData = BlackMisc::CPropertyIndex::GlobalIndexCSimulatorSetup
            };

            //! Default constructor
            CSimulatorSetup() {}

            //! All values
            BlackMisc::CNameVariantPairList getData() const { return this->m_data;}

            //! Settings
            void setData(const BlackMisc::CNameVariantPairList &data) { this->m_data = data; }

            //! Set value
            void setValue(const QString &name, const QString &value);

            //! Get string value
            QString getStringValue(const QString &name) const;

            //! Simulator version info, something like "FSX 10.3.2"
            void setSimulatorVersion(const QString versionInfo);

            //! Path where simulator is installed
            void setSimulatorInstallationDirectory(const QString fullFilePath);

            //! Simulator version info, something like "FSX 10.3.2"
            QString getSimulatorVersion() const;

            //! Path where simulator is installed
            QString getSimulatorInstallationDirectory() const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! Register metadata
            void static registerMetadata();

            //! \copydoc BlackMisc::Mixin::String::toQString()
            QString convertToQString(bool i18n = false) const;

        private:
            BlackMisc::CNameVariantPairList m_data;

            BLACK_METACLASS(CSimulatorSetup,
                BLACK_METAMEMBER(data)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatorSetup)

#endif // guard
