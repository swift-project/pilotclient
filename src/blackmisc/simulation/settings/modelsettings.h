/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SETTINGS_MODELSETTINGS_H
#define BLACKMISC_SIMULATION_SETTINGS_MODELSETTINGS_H

#include "blackmisc/simulation/distributorlistpreferences.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/propertyindex.h"

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Settings
        {
            //! Settings for models
            class BLACKMISC_EXPORT CModelSettings : public CValueObject<CModelSettings>
            {
            public:
                //! Properties by index
                enum ColumnIndex
                {
                    IndexAllowExclude = CPropertyIndex::GlobalIndexCSimulatorSettings
                };

                //! Default constructor
                CModelSettings();

                //! Allow excluded models?
                bool getAllowExcludedModels() const { return m_allowExcludeModels; }

                //! Allow excluded models?
                void setAllowExcludedModels(bool allow) { m_allowExcludeModels = allow; }

                //! \copydoc BlackMisc::Mixin::String::toQString
                QString convertToQString(bool i18n = false) const;

                //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
                QVariant propertyByIndex(CPropertyIndexRef index) const;

                //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
                void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            private:
                bool m_allowExcludeModels = false;  //!< Allow excluded models in sets

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

                //! \copydoc BlackMisc::TSettingTrait::humanReadable
                static const QString &humanReadable() { static const QString name("Model settings"); return name; }
            };

            //! Mapping preferences for model distributor list
            struct TDistributorListPreferences : public TSettingTrait<Simulation::CDistributorListPreferences>
            {
                //! \copydoc BlackMisc::TSettingTrait::humanReadable
                static const QString &humanReadable() { static const QString name("Distributor preferences"); return name; }

                //! \copydoc BlackMisc::TSettingTrait::key
                static const char *key() { return "mapping/distributorpreferences"; }
            };
        } // ns
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::Settings::CModelSettings)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::Settings::CModelSettings>)

#endif // guard
