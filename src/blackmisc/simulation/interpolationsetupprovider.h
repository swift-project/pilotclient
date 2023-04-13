/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATIONSETUPPROVIDER_H
#define BLACKMISC_SIMULATION_INTERPOLATIONSETUPPROVIDER_H

#include "blackmisc/simulation/interpolationsetuplist.h"
#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/simulation/interpolationsetuplist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/provider.h"
#include <QMap>
#include <QReadWriteLock>

namespace BlackMisc::Simulation
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
        CInterpolationSetupList getInterpolationSetupsPerCallsign() const;

        //! Get all setups per callsign
        //! \threadsafe
        bool hasSetupsPerCallsign() const;

        //! Set all setups per callsign
        //! \threadsafe
        bool setInterpolationSetupsPerCallsign(const CInterpolationSetupList &setups, bool ignoreSameAsGlobal = true);

        //! The global setup
        //! \threadsafe
        virtual CInterpolationAndRenderingSetupGlobal getInterpolationSetupGlobal() const;

        //! All callsigns marked to be logged
        //! \threadsafe
        Aviation::CCallsignSet getLogCallsigns() const;

        //! Is callsign marked for logging
        //! \threadsafe
        bool isLogCallsign(const Aviation::CCallsign &callsign) const;

        //! Set mode as string
        //! \threadsafe
        bool setInterpolationMode(const QString &modeAsString, const Aviation::CCallsign &callsign);

        //! Enable/disable logging
        //! \threadsafe
        bool setLogInterpolation(bool log, const Aviation::CCallsign &callsign);

    protected:
        //! Set the global setup
        //! \threadsafe
        virtual bool setInterpolationSetupGlobal(const CInterpolationAndRenderingSetupGlobal &setup);

        //! Insert specialized setup per callsign
        //! \threadsafe
        virtual bool setInterpolationSetupPerCallsign(const CInterpolationAndRenderingSetupPerCallsign &setup, const Aviation::CCallsign &callsign, bool removeGlobalSetup = true);

        //! Log/un-log given callsign
        //! \threadsafe
        void setLogCallsign(bool log, const Aviation::CCallsign &callsign);

        //! Remove specialized setup per callsign
        bool removeInterpolationSetupPerCallsign(const Aviation::CCallsign &callsign);

        //! Clear all interpolation log callsigns
        //! \threadsafe
        void clearInterpolationLogCallsigns();

        //! Clear all setups
        //! \threadsafe
        int clearInterpolationSetupsPerCallsign();

        //! Log any callsign?
        //! \threadsafe
        bool logAnyCallsign() const;

        //! The setups per callsign
        //! \threadsafe
        SetupsPerCallsign getSetupsPerCallsign() const;

        //! Pseudo signal, override to emit signal
        virtual void emitInterpolationSetupChanged() {}

    private:
        CInterpolationAndRenderingSetupGlobal m_globalSetup;
        SetupsPerCallsign m_setupsPerCallsign;
        mutable QReadWriteLock m_lockSetup; //!< lock clients
    };

    //! Class which can be directly used to access an \sa IInterpolationSetupProvider object
    class BLACKMISC_EXPORT CInterpolationSetupAware : public IProviderAware<IInterpolationSetupProvider>
    {
        virtual void anchor();

    public:
        //! \copydoc IInterpolationSetupProvider::getInterpolationSetupPerCallsignOrDefault
        CInterpolationAndRenderingSetupPerCallsign getInterpolationSetupPerCallsignOrDefault(const Aviation::CCallsign &callsign) const;

        //! \copydoc IInterpolationSetupProvider::getInterpolationSetupGlobal
        CInterpolationAndRenderingSetupGlobal getInterpolationSetupGlobal() const;

    protected:
        //! Default constructor
        CInterpolationSetupAware() {}

        //! Constructor
        CInterpolationSetupAware(IInterpolationSetupProvider *setupProvider) : IProviderAware(setupProvider) {}

        //! Provider
        void setInterpolationSetupProvider(IInterpolationSetupProvider *provider) { this->setProvider(provider); }
    };
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::Simulation::IInterpolationSetupProvider, "org.swift-project.blackmisc::network::iinterpolationsetupprovider")

#endif // guard
