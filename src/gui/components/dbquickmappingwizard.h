// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBQUICKMAPPINGWIZARD_H
#define SWIFT_GUI_COMPONENTS_DBQUICKMAPPINGWIZARD_H

#include "misc/simulation/aircraftmodellist.h"
#include "misc/aviation/aircrafticaocode.h"
#include <QWizard>
#include <QScopedPointer>

namespace Ui
{
    class CDbQuickMappingWizard;
}
namespace swift::gui::components
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
        void presetAircraftIcao(const swift::misc::aviation::CAircraftIcaoCode &aircraftIcao);

        //! Preset a model
        void presetModel(const swift::misc::simulation::CAircraftModel &model);

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
        swift::misc::simulation::CAircraftModel m_model; // model to be mapped

        //! Set the filter
        void setAircraftIcaoFilter();

        //! Set the filter
        void setAirlineIcaoFilter();

        //! Set color filter
        void setColorFilter();

        //! Set distributor filter
        void setDistributorFilter();

        //! Livery assigned
        swift::misc::aviation::CLivery getFirstSelectedOrDefaultLivery() const;

        //! Aircraft ICAO assigned
        swift::misc::aviation::CAircraftIcaoCode getFirstSelectedOrDefaultAircraftIcao() const;

        //! Distributor assigned
        swift::misc::simulation::CDistributor getFirstSelectedOrDefaultDistributor() const;

        //! Validate the data
        swift::misc::CStatusMessageList validateData() const;

        //! Consolidate model data
        void consolidateModelWithUIData();

        //! Guess simulator
        swift::misc::simulation::CSimulatorInfo guessSimulator() const;

        //! Write the model to DB
        void writeModelToDb();

        //! Models published
        void onPublishedModels(const swift::misc::simulation::CAircraftModelList &modelsPublished,
                               const swift::misc::simulation::CAircraftModelList &modelsSkipped,
                               const swift::misc::CStatusMessageList &messages,
                               bool requestSuccessful, bool directWrite);

        //! Web data have been read
        void onWebDataRead();

        //! Current page has been changed
        void currentWizardPageChanged(int id);

        //! Airline selected
        void onAirlineSelected(const swift::misc::aviation::CAirlineIcaoCode &icao);

        //! Aircraft selected
        void onAircraftSelected(const swift::misc::aviation::CAircraftIcaoCode &icao);
    };
} // ns
#endif // guard
