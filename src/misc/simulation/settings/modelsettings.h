// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_SETTINGS_MODELSETTINGS_H
#define SWIFT_MISC_SIMULATION_SETTINGS_MODELSETTINGS_H

#include "misc/simulation/distributorlistpreferences.h"
#include "misc/settingscache.h"
#include "misc/swiftmiscexport.h"
#include "misc/propertyindexref.h"

BLACK_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation::Settings, CModelSettings)

namespace swift::misc::simulation::settings
{
    //! Settings for models
    class SWIFT_MISC_EXPORT CModelSettings : public CValueObject<CModelSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexAllowExclude = CPropertyIndexRef::GlobalIndexCSimulatorSettings
        };

        //! Default constructor
        CModelSettings();

        //! Allow excluded models?
        bool getAllowExcludedModels() const { return m_allowExcludeModels; }

        //! Allow excluded models?
        void setAllowExcludedModels(bool allow) { m_allowExcludeModels = allow; }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

    private:
        bool m_allowExcludeModels = false; //!< Allow excluded models in sets

        BLACK_METACLASS(
            CModelSettings,
            BLACK_METAMEMBER(allowExcludeModels)
        );
    };

    //! Trait for simulator settings
    struct TModel : public TSettingTrait<CModelSettings>
    {
        //! Key in data cache
        static const char *key() { return "Models"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Model settings");
            return name;
        }
    };

    //! Mapping preferences for model distributor list
    struct TDistributorListPreferences : public TSettingTrait<simulation::CDistributorListPreferences>
    {
        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Distributor preferences");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "mapping/distributorpreferences"; }
    };
} // ns

Q_DECLARE_METATYPE(swift::misc::simulation::settings::CModelSettings)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::simulation::settings::CModelSettings>)

#endif // guard
