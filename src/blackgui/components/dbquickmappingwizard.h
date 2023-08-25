// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DBQUICKMAPPINGWIZARD_H
#define BLACKGUI_COMPONENTS_DBQUICKMAPPINGWIZARD_H

#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include <QWizard>
#include <QScopedPointer>

namespace Ui
{
    class CDbQuickMappingWizard;
}
namespace BlackGui::Components
{
    /*!
     * Wizard to quickly provide a single mapping
     */
    class CDbQuickMappingWizard : public QWizard
    {
        Q_OBJECT

    public:
        //! The wizard pages
        enum Pages
        {
            PageIntro,
            PageAircraftSelect,
            PageColor,
            PageLiverySelect,
            PageDistributorSelect,
            PageConfirmation, //!< model from and validation
            PageCredentials,
            PageLastConfirmation, //!< just info to say data will be send
            PageSendStatus
        };

        //! Constructor
        explicit CDbQuickMappingWizard(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbQuickMappingWizard() override;

        //! Preset values
        void presetAircraftIcao(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcao);

        //! Preset a model
        void presetModel(const BlackMisc::Simulation::CAircraftModel &model);

        //! Clear wizard
        void clear();

        //! \copydoc QWizard::validateCurrentPage
        virtual bool validateCurrentPage() override;

        //! Log categories
        static const QStringList &getLogCategories();

    protected:
        //! \copydoc QWizard::keyPressEvent
        virtual void keyPressEvent(QKeyEvent *event) override;

    private:
        QScopedPointer<Ui::CDbQuickMappingWizard> ui;
        int m_lastId = 0;
        BlackMisc::Simulation::CAircraftModel m_model; // model to be mapped

        //! Set the filter
        void setAircraftIcaoFilter();

        //! Set the filter
        void setAirlineIcaoFilter();

        //! Set color filter
        void setColorFilter();

        //! Set distributor filter
        void setDistributorFilter();

        //! Livery assigned
        BlackMisc::Aviation::CLivery getFirstSelectedOrDefaultLivery() const;

        //! Aircraft ICAO assigned
        BlackMisc::Aviation::CAircraftIcaoCode getFirstSelectedOrDefaultAircraftIcao() const;

        //! Distributor assigned
        BlackMisc::Simulation::CDistributor getFirstSelectedOrDefaultDistributor() const;

        //! Validate the data
        BlackMisc::CStatusMessageList validateData() const;

        //! Consolidate model data
        void consolidateModelWithUIData();

        //! Guess simulator
        BlackMisc::Simulation::CSimulatorInfo guessSimulator() const;

        //! Write the model to DB
        void writeModelToDb();

        //! Models published
        void onPublishedModels(const BlackMisc::Simulation::CAircraftModelList &modelsPublished,
                               const BlackMisc::Simulation::CAircraftModelList &modelsSkipped,
                               const BlackMisc::CStatusMessageList &messages,
                               bool requestSuccessful, bool directWrite);

        //! Web data have been read
        void onWebDataRead();

        //! Current page has been changed
        void currentWizardPageChanged(int id);

        //! Airline selected
        void onAirlineSelected(const BlackMisc::Aviation::CAirlineIcaoCode &icao);

        //! Aircraft selected
        void onAircraftSelected(const BlackMisc::Aviation::CAircraftIcaoCode &icao);
    };
} // ns
#endif // guard
