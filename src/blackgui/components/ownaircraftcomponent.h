/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_OWNAIRCRAFTCOMPONENT_H
#define BLACKGUI_COMPONENTS_OWNAIRCRAFTCOMPONENT_H

#include "blackmisc/simulation/data/lastmodel.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/network/user.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/callsign.h"

#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class COwnAircraftComponent;
}
namespace BlackGui::Components
{
    class CDbQuickMappingWizard;

    //! Own aircraft
    class COwnAircraftComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! GUI aircraft values, formatted
        struct CGuiAircraftValues
        {
            BlackMisc::Aviation::CCallsign ownCallsign; //!< own aircraft callsign
            BlackMisc::Aviation::CAircraftIcaoCode ownAircraftIcao; //!< ICAO aircraft
            BlackMisc::Aviation::CAirlineIcaoCode ownAirlineIcao; //!< ICAO airline
            QString ownAircraftCombinedType; //!< own aircraft combined type
            QString ownAircraftSimulatorModelString; //!< own aircraft model string
            QString ownAircraftModelStringSend; //!< send model string
            QString ownLiverySend; //!< send livery
            bool useModelString = true; //!< use model string
            bool useLivery = true; //!< use livery
        };

        //! Constructor
        explicit COwnAircraftComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~COwnAircraftComponent() override;

        //! Validate aircaft
        BlackMisc::CStatusMessageList validate() const;

        //! Set a user
        void setUser(const BlackMisc::Network::CUser &user);

        //! Values from GUI
        BlackMisc::Aviation::CCallsign getCallsignFromGui() const;

        //! Own model and ICAO data for GUI and own aircraft
        void setOwnModelAndIcaoValues(const BlackMisc::Simulation::CAircraftModel &ownModel = {});

        //! Update own ICAO values (own aircraft from what is set in the GUI)
        //! \return changed?
        bool updateOwnAircaftIcaoValuesFromGuiValues();

        //! Values from GUI
        CGuiAircraftValues getAircraftValuesFromGui() const;

    signals:
        //! Changed values
        void aircraftDataChanged();

    private:
        //! Launch mapping wizard
        void mappingWizard();

        //! Lookup own model
        void lookupOwnAircraftModel();

        //! Simulator model has been changed
        void onSimulatorModelChanged(const BlackMisc::Simulation::CAircraftModel &model);

        //! Model string send
        void onModelStringSendChanged();

        //! Status has changed
        void onSimulatorStatusChanged(int status);

        //! Set ICAO values
        //! \return changed values?
        bool setGuiIcaoValues(const BlackMisc::Simulation::CAircraftModel &model, bool onlyIfEmpty);

        //! Aircraft ICAO code has been changed
        void changedAircraftIcao(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

        //! Airline ICAO code has been changed
        void changedAirlineIcao(const BlackMisc::Aviation::CAirlineIcaoCode &icao);

        //! Highlight model field according to model data
        void highlightModelField(const BlackMisc::Simulation::CAircraftModel &model = {});

        //! Has contexts?
        bool hasValidContexts() const;

        //! Clear livery
        void clearLivery();

        //! Get a prefill model
        BlackMisc::Simulation::CAircraftModel getPrefillModel() const;

        bool m_autoPopupWizard = false; //!< automatically popup wizard if mapping is needed
        QScopedPointer<CDbQuickMappingWizard> m_mappingWizard; //!< mapping wizard
        BlackMisc::CData<BlackMisc::Simulation::Data::TLastModel> m_lastAircraftModel { this }; //!< recently used aircraft model
        QScopedPointer<Ui::COwnAircraftComponent> ui;
    };
} // ns

#endif // guard
