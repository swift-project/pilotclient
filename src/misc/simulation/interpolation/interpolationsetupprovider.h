// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATIONSETUPPROVIDER_H
#define SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATIONSETUPPROVIDER_H

#include <QMap>
#include <QReadWriteLock>

#include "misc/aviation/callsignset.h"
#include "misc/provider.h"
#include "misc/simulation/interpolation/interpolationrenderingsetup.h"
#include "misc/simulation/interpolation/interpolationsetuplist.h"

namespace swift::misc::simulation
{
    //! Direct in memory access to interpolation setup, normally implemented by simulator
    class SWIFT_MISC_EXPORT IInterpolationSetupProvider : public IProvider
    {
    public:
        using SetupsPerCallsign =
            QMap<aviation::CCallsign, CInterpolationAndRenderingSetupPerCallsign>; //!< setups per callsign

        //! Get the setup for callsign, if not existing the global setup
        //! \threadsafe
        CInterpolationAndRenderingSetupPerCallsign
        getInterpolationSetupPerCallsignOrDefault(const aviation::CCallsign &callsign) const;

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
        aviation::CCallsignSet getLogCallsigns() const;

        //! Is callsign marked for logging
        //! \threadsafe
        bool isLogCallsign(const aviation::CCallsign &callsign) const;

        //! Set mode as string
        //! \threadsafe
        bool setInterpolationMode(const QString &modeAsString, const aviation::CCallsign &callsign);

        //! Enable/disable logging
        //! \threadsafe
        bool setLogInterpolation(bool log, const aviation::CCallsign &callsign);

    protected:
        //! Set the global setup
        //! \threadsafe
        virtual bool setInterpolationSetupGlobal(const CInterpolationAndRenderingSetupGlobal &setup);

        //! Insert specialized setup per callsign
        //! \threadsafe
        virtual bool setInterpolationSetupPerCallsign(const CInterpolationAndRenderingSetupPerCallsign &setup,
                                                      const aviation::CCallsign &callsign,
                                                      bool removeGlobalSetup = true);

        //! Log/un-log given callsign
        //! \threadsafe
        void setLogCallsign(bool log, const aviation::CCallsign &callsign);

        //! Remove specialized setup per callsign
        bool removeInterpolationSetupPerCallsign(const aviation::CCallsign &callsign);

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
    class SWIFT_MISC_EXPORT CInterpolationSetupAware : public IProviderAware<IInterpolationSetupProvider>
    {
        virtual void anchor();

    public:
        //! \copydoc IInterpolationSetupProvider::getInterpolationSetupPerCallsignOrDefault
        CInterpolationAndRenderingSetupPerCallsign
        getInterpolationSetupPerCallsignOrDefault(const aviation::CCallsign &callsign) const;

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
} // namespace swift::misc::simulation

Q_DECLARE_INTERFACE(swift::misc::simulation::IInterpolationSetupProvider,
                    "org.swift-project.misc::network::iinterpolationsetupprovider")

#endif // SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATIONSETUPPROVIDER_H
