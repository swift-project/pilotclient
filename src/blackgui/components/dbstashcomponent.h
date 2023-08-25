// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DBSTASHCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBSTASHCOMPONENT_H

#include "blackgui/components/dbmappingcomponentaware.h"
#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/blackguiexport.h"
#include "blackcore/data/authenticateduser.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/network/authenticateduser.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"

#include <QFrame>
#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QMessageBox>
#include <QStringList>
#include <functional>

class QWidget;

namespace BlackMisc
{
    class CLogCategoryList;
    namespace Aviation
    {
        class CAircraftIcaoCode;
        class CLivery;
    }
}
namespace Ui
{
    class CDbStashComponent;
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
         * Stashed objects
         */
        class BLACKGUI_EXPORT CDbStashComponent :
            public QFrame,
            public CDbMappingComponentAware,
            public CEnableForDockWidgetInfoArea
        {
            Q_OBJECT

        public:
            //! Number of models which can be published at once
            static constexpr int MaxModelPublished = 1000;

            //! Constructor
            explicit CDbStashComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbStashComponent();

            //! Test the given model if it can be stashed
            BlackMisc::CStatusMessage validateStashModel(const BlackMisc::Simulation::CAircraftModel &model, bool allowReplace) const;

            //! Unstash given models with keys
            int unstashModels(const QSet<int> &keys);

            //! Unstash given models by model string
            int unstashModels(const QStringList &modelStrings);

            //! Unstash given models
            int unstashModels(const BlackMisc::Simulation::CAircraftModelList &models);

            //! The embedded view
            BlackGui::Views::CAircraftModelView *view() const;

            //! Has stashed models
            bool hasStashedModels() const;

            //! Number of models
            int getStashedModelsCount() const;

            //! Stashed model strings
            QStringList getStashedModelStrings() const;

            //! The stashed models
            const BlackMisc::Simulation::CAircraftModelList &getStashedModels() const;

            //! Model for model string
            BlackMisc::Simulation::CAircraftModel getStashedModel(const QString &modelString) const;

            //! Apply livery to selected objects
            void applyToSelected(const BlackMisc::Aviation::CLivery &livery, bool acceptWarnings = true);

            //! Apply airline ICAO code to selected objects
            void applyToSelected(const BlackMisc::Aviation::CAircraftIcaoCode &icao, bool acceptWarnings = true);

            //! Apply aircraft ICAO code to selected objects
            void applyToSelected(const BlackMisc::Aviation::CAirlineIcaoCode &icao, bool acceptWarnings = true);

            //! Apply distributor to selected objects
            void applyToSelected(const BlackMisc::Simulation::CDistributor &distributor, bool acceptWarnings = true);

            //! Apply set of properties to selected objects
            void applyToSelected(const BlackMisc::CPropertyIndexVariantMap &vm);

            //! Consolidate with other available data
            BlackMisc::Simulation::CAircraftModel consolidateModel(const BlackMisc::Simulation::CAircraftModel &model) const;

            //! Show changed attributes of selected models
            void showChangedAttributes();

            //! Clear highlighting set by validations
            void clearValidationHighlighting();

        public slots:
            //! Stash given model (includes validation and consolidation with DB data)
            BlackMisc::CStatusMessage stashModel(const BlackMisc::Simulation::CAircraftModel &model, bool replace = false, bool consolidateWithDbData = true, bool clearHighlighting = true);

            //! Stash given models (includes validation and consolidation with DB data)
            BlackMisc::CStatusMessageList stashModels(const BlackMisc::Simulation::CAircraftModelList &models, bool replace = false, bool consolidateWithDbData = true, bool clearHighlighting = true);

            //! Replace models, no validation
            void replaceModelsUnvalidated(const BlackMisc::Simulation::CAircraftModelList &models);

        signals:
            //! Stashed models have been changed
            void stashedModelsChanged();

            //! Models succesfully published
            void modelsSuccessfullyPublished(const BlackMisc::Simulation::CAircraftModelList &publishedModels, bool directWrite);

        private:
            QScopedPointer<Ui::CDbStashComponent> ui;
            BlackMisc::CDataReadOnly<BlackCore::Data::TAuthenticatedDbUser> m_swiftDbUser { this, &CDbStashComponent::onUserChanged }; //!< authenticated user

            //! Unstash pressed
            void onUnstashPressed();

            //! Validate pressed
            void onValidatePressed();

            //! Remove invalid (validates and removes invalid models)
            void onRemoveInvalidPressed();

            //! Publish pressed
            void onPublishPressed();

            //! Publish response received
            void onPublishedModelsResponse(const BlackMisc::Simulation::CAircraftModelList &publishedModels,
                                           const BlackMisc::Simulation::CAircraftModelList &skippedModels,
                                           const BlackMisc::CStatusMessageList &msgs, bool sendingSuccesful, bool directWrite);

            //! Copy over values
            void copyOverValuesToSelectedModels();

            //! Display model dialog
            void modifyModelDialog();

            //! Row count changed
            void onRowCountChanged(int number, bool filter);

            //! User has been changed
            void onUserChanged();

            //! Display messages
            bool showOverlayMessages(const BlackMisc::CStatusMessageList &msgs, bool onlyErrors = false, bool appendOldMessages = false, int timeoutMs = -1);

            //! Display messages with confirmation
            bool showOverlayMessagesWithConfirmation(
                const BlackMisc::CStatusMessageList &msgs, bool appendOldMessages,
                const QString &confirmation, std::function<void()> okLambda,
                QMessageBox::StandardButton defaultButton, bool onlyErrors = false, int timeoutMs = -1);

            //! Display message
            bool showOverlayMessage(const BlackMisc::CStatusMessage &msg, int timeoutMs = -1);

            //! Clear messages
            void clearOverlayMessages();

            //! Validate
            BlackMisc::CStatusMessageList validate(BlackMisc::Simulation::CAircraftModelList &validModels, BlackMisc::Simulation::CAircraftModelList &invalidModels) const;

            //! Validate and display info messages
            bool validateAndDisplay(BlackMisc::Simulation::CAircraftModelList &validModels, BlackMisc::Simulation::CAircraftModelList &invalidModels, bool displayInfo = false);

            //! Set the button row
            void enableButtonRow();

            //! Validation categories
            const BlackMisc::CLogCategoryList &validationCategories() const;

            //! Get the selected only models or all models depending on checkbox
            BlackMisc::Simulation::CAircraftModelList getSelectedOrAllModels() const;

            //! Consolidate with any DB data (if available).
            BlackMisc::Simulation::CAircraftModel consolidateWithDbData(const BlackMisc::Simulation::CAircraftModel &model, bool forced) const;

            //! Consolidate with own models (if available). This updates mostly with model description, path etc.
            BlackMisc::Simulation::CAircraftModel consolidateWithOwnModels(const BlackMisc::Simulation::CAircraftModel &model) const;

            //! Authenticated DB user
            BlackMisc::Network::CAuthenticatedUser getSwiftDbUser() const;
        };
    } // ns
} // ns

#endif // guard
