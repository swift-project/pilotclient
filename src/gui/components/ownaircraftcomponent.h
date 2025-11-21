// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_OWNAIRCRAFTCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_OWNAIRCRAFTCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/callsign.h"
#include "misc/network/user.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/data/lastmodel.h"

namespace Ui
{
    class COwnAircraftComponent;
}
namespace swift::gui::components
{
    //! Own aircraft
    class COwnAircraftComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! GUI aircraft values, formatted
        struct CGuiAircraftValues
        {
            swift::misc::aviation::CCallsign ownCallsign; //!< own aircraft callsign
            swift::misc::aviation::CAircraftIcaoCode ownAircraftIcao; //!< ICAO aircraft
            swift::misc::aviation::CAirlineIcaoCode ownAirlineIcao; //!< ICAO airline
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
        ~COwnAircraftComponent() override;

        //! Validate aircaft
        swift::misc::CStatusMessageList validate() const;

        //! Set a user
        void setUser(const swift::misc::network::CUser &user);

        //! Values from GUI
        swift::misc::aviation::CCallsign getCallsignFromGui() const;

        //! Own model and ICAO data for GUI and own aircraft
        void setOwnModelAndIcaoValues(const swift::misc::simulation::CAircraftModel &ownModel = {});

        //! Update own ICAO values (own aircraft from what is set in the GUI)
        //! \return changed?
        bool updateOwnAircaftIcaoValuesFromGuiValues();

        //! Values from GUI
        CGuiAircraftValues getAircraftValuesFromGui() const;

    signals:
        //! Changed values
        void aircraftDataChanged();

    private:
        //! Simulator model has been changed
        void onSimulatorModelChanged(const swift::misc::simulation::CAircraftModel &model);

        //! Model string send
        void onModelStringSendChanged();

        //! Status has changed
        void onSimulatorStatusChanged(int status);

        //! Set ICAO values
        //! \return changed values?
        bool setGuiIcaoValues(const swift::misc::simulation::CAircraftModel &model, bool onlyIfEmpty);

        //! Aircraft ICAO code has been changed
        void changedAircraftIcao(const swift::misc::aviation::CAircraftIcaoCode &icao);

        //! Airline ICAO code has been changed
        void changedAirlineIcao(const swift::misc::aviation::CAirlineIcaoCode &icao);

        //! Highlight model field according to model data
        void highlightModelField(const swift::misc::simulation::CAircraftModel &model = {});

        //! Has contexts?
        bool hasValidContexts() const;

        //! Clear livery
        void clearLivery();

        //! Get a prefill model
        swift::misc::simulation::CAircraftModel getPrefillModel() const;

        swift::misc::CData<swift::misc::simulation::data::TLastModel> m_lastAircraftModel {
            this
        }; //!< recently used aircraft model
        QScopedPointer<Ui::COwnAircraftComponent> ui;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_OWNAIRCRAFTCOMPONENT_H
