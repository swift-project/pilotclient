/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATIONSETUPPROVIDER_H
#define BLACKMISC_SIMULATION_INTERPOLATIONSETUPPROVIDER_H

#include "interpolationsetuplist.h"
#include "interpolationrenderingsetup.h"
#include "interpolationsetuplist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/provider.h"
#include <QMap>
#include <QReadWriteLock>

namespace BlackMisc
{
    namespace Simulation
    {
        //! Direct in memory access to interpolation setup, normally implemented by simulator
        class BLACKMISC_EXPORT IInterpolationSetupProvider : public IProvider
        {
        public:
            using SetupsPerCallsign = QMap<Aviation::CCallsign, CInterpolationAndRenderingSetupPerCallsign>; //!< setups per callsign

            //! Get the setup for callsign, if not existing the global setup
            //! \threadsafe
            CInterpolationAndRenderingSetupPerCallsign getInterpolationSetupPerCallsignOrDefault(const Aviation::CCallsign &callsign) const;

            //! Get all setups per callsign
            //! \threadsafe
            virtual CInterpolationSetupList getInterpolationSetupsPerCallsign() const;

            //! Set all setups per callsign
            //! \threadsafe
            virtual void setInterpolationSetupsPerCallsign(const CInterpolationSetupList &setups);

            //! The global setup
            //! \threadsafe
            virtual CInterpolationAndRenderingSetupGlobal getInterpolationSetupGlobal() const;

            //! All callsigns marked to be logged
            //! \threadsafe
            Aviation::CCallsignSet getLogCallsigns() const;

        protected:
            //! Set the global setup
            //! \threadsafe
            bool setInterpolationSetupGlobal(const CInterpolationAndRenderingSetupGlobal &setup);

            //! Insert specialized setup
            //! \threadsafe
            bool setInterpolationSetupPerCallsign(const CInterpolationAndRenderingSetupPerCallsign &setup, const Aviation::CCallsign &callsign, bool removeGlobalSetup = true);

            //! Log/un-log given callsign
            //! \threadsafe
            void setLogCallsign(bool log, const Aviation::CCallsign &callsign);

            //! Clear all interpolation log callsigns
            //! \threadsafe
            void clearInterpolationLogCallsigns();

            //! Log any callsign?
            //! \threadsafe
            bool logAnyCallsign() const;

            //! The setups per callsign
            //! \threadsafe
            SetupsPerCallsign getSetupsPerCallsign() const;

        private:
            CInterpolationAndRenderingSetupGlobal m_globalSetup;
            SetupsPerCallsign m_setups;
            mutable QReadWriteLock m_lockSetup; //!< lock clients
        };

        //! Class which can be directly used to access an \sa IInterpolationSetupProvider object
        class BLACKMISC_EXPORT CInterpolationSetupAware : public IProviderAware<IInterpolationSetupProvider>
        {
        public:
            //! \copydoc IInterpolationSetupProvider::getInterpolationSetupPerCallsignOrDefault
            CInterpolationAndRenderingSetupPerCallsign getInterpolationSetupPerCallsignOrDefault(const Aviation::CCallsign &callsign) const;

            //! \copydoc IInterpolationSetupProvider::getInterpolationSetupGlobal
            CInterpolationAndRenderingSetupGlobal getInterpolationSetupGlobal() const;

        protected:
            //! Default constructor
            CInterpolationSetupAware() {}

            //! Constructor
            CInterpolationSetupAware(IInterpolationSetupProvider *setupProvider) : IProviderAware(setupProvider) { }

            //! Provider
            void setInterpolationSetupProvider(IInterpolationSetupProvider *provider) { this->setProvider(provider); }
        };
    } // namespace
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::Simulation::IInterpolationSetupProvider, "org.swift-project.blackmisc::network::iinterpolationsetupprovider")

#endif // guard
