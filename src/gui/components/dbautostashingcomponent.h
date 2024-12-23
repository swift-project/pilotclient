// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBAUTOSTASHINGCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_DBAUTOSTASHINGCOMPONENT_H

#include <QDialog>
#include <QObject>
#include <QScopedPointer>

#include "core/progress.h"
#include "gui/components/dbmappingcomponentaware.h"
#include "gui/swiftguiexport.h"
#include "misc/network/entityflags.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/statusmessage.h"

namespace swift::misc
{
    class CLogCategoryList;
    namespace simulation
    {
        class CAircraftModel;
    }
} // namespace swift::misc
namespace Ui
{
    class CDbAutoStashingComponent;
}
namespace swift::gui
{
    namespace views
    {
        class CAircraftModelView;
    }
    namespace components
    {
        /*!
         * Stashing component
         */
        class SWIFT_GUI_EXPORT CDbAutoStashingComponent :
            public QDialog,
            public CDbMappingComponentAware,
            public swift::core::IProgressIndicator
        {
            Q_OBJECT
            Q_INTERFACES(swift::gui::components::CDbMappingComponentAware)

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

            //! \copydoc swift::core::IProgressIndicator::updateProgressIndicator
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
            swift::gui::views::CAircraftModelView *currentModelView() const;

            //! Add a status message
            void addStatusMessage(const swift::misc::CStatusMessage &msg);

            //! Add a status message for a given model (prefixed)
            void addStatusMessage(const swift::misc::CStatusMessage &msg,
                                  const swift::misc::simulation::CAircraftModel &model);

            //! Try stashing selected or all models
            void tryToStashModels();

            //! Try stashing a model
            //! \param model this model can be updated with consolidated data
            //! \return true means stashing is possible
            bool tryToStashModel(swift::misc::simulation::CAircraftModel &model,
                                 const swift::misc::aviation::CLivery &tempLivery);

            //! Set the model description
            void setModelDescription(swift::misc::simulation::CAircraftModel &model, const QString &description) const;

            //! Get the temp.livery if available
            static swift::misc::aviation::CLivery getTempLivery();

            int m_noStashed = 0; //!< stashed models
            int m_noData = 0; //!< not stashed because no data
            int m_noValidationFailed = 0; //!< not stashed because validation failed
            State m_state = Idle; //!< modus
            swift::misc::simulation::CAircraftModelList m_modelsToStash; //!< Models about to be stashed
        };
    } // namespace components
} // namespace swift::gui

#endif // SWIFT_GUI_COMPONENTS_DBAUTOSTASHINGCOMPONENT_H
