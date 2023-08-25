// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_SIMULATORINTERNALS_H
#define BLACKMISC_SIMULATION_SIMULATORINTERNALS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/containerbase.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/namevariantpairlist.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/valueobject.h"

#include <QHash>
#include <QMetaType>
#include <QString>
#include <tuple>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Simulation, CSimulatorInternals)

namespace BlackMisc::Simulation
{
    //! Simulator internals for flight simulators.
    //! Those are obtained from a running simulator and represent information about the simulator (like a small registry)
    class BLACKMISC_EXPORT CSimulatorInternals : public CValueObject<CSimulatorInternals>
    {
    public:
        //! Specific values
        enum ColumnIndex
        {
            IndexData = CPropertyIndexRef::GlobalIndexCSimulatorInternals
        };

        //! Default constructor
        CSimulatorInternals() {}

        //! All values
        BlackMisc::CNameVariantPairList getData() const { return this->m_data; }

        //! Settings
        void setData(const BlackMisc::CNameVariantPairList &data) { this->m_data = data; }

        //! Set value
        void setValue(const QString &name, const QString &value);

        //! Set value
        void setValue(const QString &name, int value);

        //! Get value
        CVariant getVariantValue(const QString &name) const;

        //! Get string value
        QString getStringValue(const QString &name) const;

        //! Get sorted names
        QStringList getSortedNames() const;

        //! Get the simulator name
        QString getSimulatorName() const;

        //! Set simulator name
        void setSimulatorName(const QString &name);

        //! Plugin name
        QString getSimulatorSwiftPluginName() const;

        //! Set plugin name
        void setSwiftPluginName(const QString &name);

        //! Simulator version info, something like "10.3.2"
        QString getSimulatorVersion() const;

        //! Simulator version info, something like "10.3.2"
        void setSimulatorVersion(const QString &versionInfo);

        //! Path where simulator is installed
        QString getSimulatorInstallationDirectory() const;

        //! Path where simulator is installed
        void setSimulatorInstallationDirectory(const QString &fullFilePath);

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

        //! Register metadata
        static void registerMetadata();

    private:
        BlackMisc::CNameVariantPairList m_data;

        BLACK_METACLASS(
            CSimulatorInternals,
            BLACK_METAMEMBER(data)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatorInternals)

#endif // guard
