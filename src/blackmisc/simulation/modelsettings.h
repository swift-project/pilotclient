/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_MODELSETTINGS_H
#define BLACKMISC_SIMULATION_MODELSETTINGS_H

#include "blackmisc/simulation/distributorlistpreferences.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/propertyindex.h"

namespace BlackMisc
{
    namespace Simulation
    {
        //! Settings for models
        class BLACKMISC_EXPORT CModelSettings :
            public BlackMisc::CValueObject<CModelSettings>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexAllowExclude = BlackMisc::CPropertyIndex::GlobalIndexCSimulatorSettings
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
            BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const BlackMisc::CVariant &variant);

        private:
            bool m_allowExcludeModels = false;  //!< Allow excluded models in sets

            BLACK_METACLASS(
                CModelSettings,
                BLACK_METAMEMBER(allowExcludeModels)
            );
        };

        //! Trait for simulator settings
        struct TModel : public BlackMisc::TSettingTrait<CModelSettings>
        {
            //! Key in data cache
            static const char *key() { return "settingsmodels"; }
        };

        //! Mapping preferences for model distributor list
        struct TDistributorListPreferences : public BlackMisc::TSettingTrait<BlackMisc::Simulation::CDistributorListPreferences>
        {
            //! \copydoc BlackMisc::TSettingTrait::key
            static const char *key() { return "mapping/distributorpreferences"; }
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::CModelSettings)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::CModelSettings>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Simulation::CModelSettings>)

#endif // guard
