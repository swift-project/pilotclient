// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_SIMULATORINTERNALS_H
#define SWIFT_MISC_SIMULATION_SIMULATORINTERNALS_H

#include <tuple>

#include <QHash>
#include <QMetaType>
#include <QString>

#include "misc/containerbase.h"
#include "misc/dictionary.h"
#include "misc/metaclass.h"
#include "misc/namevariantpairlist.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation, CSimulatorInternals)

namespace swift::misc::simulation
{
    //! Simulator internals for flight simulators.
    //! Those are obtained from a running simulator and represent information about the simulator (like a small registry)
    class SWIFT_MISC_EXPORT CSimulatorInternals : public CValueObject<CSimulatorInternals>
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
        swift::misc::CNameVariantPairList getData() const { return this->m_data; }

        //! Settings
        void setData(const swift::misc::CNameVariantPairList &data) { this->m_data = data; }

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

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

        //! Register metadata
        static void registerMetadata();

    private:
        swift::misc::CNameVariantPairList m_data;

        SWIFT_METACLASS(
            CSimulatorInternals,
            SWIFT_METAMEMBER(data));
    };
} // namespace swift::misc::simulation

Q_DECLARE_METATYPE(swift::misc::simulation::CSimulatorInternals)

#endif // guard
