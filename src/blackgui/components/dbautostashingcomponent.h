// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DBAUTOSTASHINGCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBAUTOSTASHINGCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/dbmappingcomponentaware.h"
#include "blackcore/progress.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/statusmessage.h"

#include <QDialog>
#include <QObject>
#include <QScopedPointer>

namespace BlackMisc
{
    class CLogCategoryList;
    namespace Simulation
    {
        class CAircraftModel;
    }
}
namespace Ui
{
    class CDbAutoStashingComponent;
}
namespace BlackGui
{
    namespace Views
    {
        class CAircraftModelView;
    }
    namespace Components
    {
        /*!
         * Stashing component
         */
        class BLACKGUI_EXPORT CDbAutoStashingComponent :
            public QDialog,
            public CDbMappingComponentAware,
            public BlackCore::IProgressIndicator
        {
            Q_OBJECT
            Q_INTERFACES(BlackGui::Components::CDbMappingComponentAware)

        public:
            //! Current state of this component
            enum State
            {
                Idle,
                Running,
                Completed
            };

            //! Log categories
            static const QStringList &getLogCategories();

            //! Constructor
            explicit CDbAutoStashingComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbAutoStashingComponent() override;

            //! At least run once and completed
            bool isCompleted() const { return m_state == Completed; }

            //! \copydoc BlackCore::IProgressIndicator::updateProgressIndicator
            virtual void updateProgressIndicator(int percent) override;

        public slots:
            //! \copydoc QDialog::accept
            virtual void accept() override;

            //! \copydoc QDialog::exec
            virtual int exec() override;

            //! Show last result
            void showLastResults();

        private:
            QScopedPointer<Ui::CDbAutoStashingComponent> ui;

            //! Reset the description settings
            void resetDescription();

            //! Init the component
            void initGui();

            //! Number of all or selected models
            int getSelectedOrAllCount() const;

            //! Model view to take models from
            BlackGui::Views::CAircraftModelView *currentModelView() const;

            //! Add a status message
            void addStatusMessage(const BlackMisc::CStatusMessage &msg);

            //! Add a status message for a given model (prefixed)
            void addStatusMessage(const BlackMisc::CStatusMessage &msg, const BlackMisc::Simulation::CAircraftModel &model);

            //! Try stashing selected or all models
            void tryToStashModels();

            //! Try stashing a model
            //! \param model this model can be updated with consolidated data
            //! \return true means stashing is possible
            bool tryToStashModel(BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::Aviation::CLivery &tempLivery);

            //! Set the model description
            void setModelDescription(BlackMisc::Simulation::CAircraftModel &model, const QString &description) const;

            //! Get the temp.livery if available
            static BlackMisc::Aviation::CLivery getTempLivery();

            int m_noStashed = 0; //!< stashed models
            int m_noData = 0; //!< not stashed because no data
            int m_noValidationFailed = 0; //!< not stashed because validation failed
            State m_state = Idle; //!< modus
            BlackMisc::Simulation::CAircraftModelList m_modelsToStash; //!< Models about to be stashed
        };
    } // ns
} // ns

#endif // guard
