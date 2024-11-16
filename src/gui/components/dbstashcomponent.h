// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBSTASHCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_DBSTASHCOMPONENT_H

#include <functional>

#include <QFrame>
#include <QList>
#include <QMessageBox>
#include <QObject>
#include <QScopedPointer>
#include <QStringList>

#include "core/data/authenticateduser.h"
#include "gui/components/dbmappingcomponentaware.h"
#include "gui/enablefordockwidgetinfoarea.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/network/authenticateduser.h"
#include "misc/propertyindexvariantmap.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/distributor.h"
#include "misc/statusmessage.h"
#include "misc/statusmessagelist.h"

class QWidget;

namespace swift::misc
{
    class CLogCategoryList;
    namespace aviation
    {
        class CAircraftIcaoCode;
        class CLivery;
    } // namespace aviation
} // namespace swift::misc
namespace Ui
{
    class CDbStashComponent;
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
         * Stashed objects
         */
        class SWIFT_GUI_EXPORT CDbStashComponent :
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
            swift::misc::CStatusMessage validateStashModel(const swift::misc::simulation::CAircraftModel &model,
                                                           bool allowReplace) const;

            //! Unstash given models with keys
            int unstashModels(const QSet<int> &keys);

            //! Unstash given models by model string
            int unstashModels(const QStringList &modelStrings);

            //! Unstash given models
            int unstashModels(const swift::misc::simulation::CAircraftModelList &models);

            //! The embedded view
            swift::gui::views::CAircraftModelView *view() const;

            //! Has stashed models
            bool hasStashedModels() const;

            //! Number of models
            int getStashedModelsCount() const;

            //! Stashed model strings
            QStringList getStashedModelStrings() const;

            //! The stashed models
            const swift::misc::simulation::CAircraftModelList &getStashedModels() const;

            //! Model for model string
            swift::misc::simulation::CAircraftModel getStashedModel(const QString &modelString) const;

            //! Apply livery to selected objects
            void applyToSelected(const swift::misc::aviation::CLivery &livery, bool acceptWarnings = true);

            //! Apply airline ICAO code to selected objects
            void applyToSelected(const swift::misc::aviation::CAircraftIcaoCode &icao, bool acceptWarnings = true);

            //! Apply aircraft ICAO code to selected objects
            void applyToSelected(const swift::misc::aviation::CAirlineIcaoCode &icao, bool acceptWarnings = true);

            //! Apply distributor to selected objects
            void applyToSelected(const swift::misc::simulation::CDistributor &distributor, bool acceptWarnings = true);

            //! Apply set of properties to selected objects
            void applyToSelected(const swift::misc::CPropertyIndexVariantMap &vm);

            //! Consolidate with other available data
            swift::misc::simulation::CAircraftModel
            consolidateModel(const swift::misc::simulation::CAircraftModel &model) const;

            //! Show changed attributes of selected models
            void showChangedAttributes();

            //! Clear highlighting set by validations
            void clearValidationHighlighting();

        public slots:
            //! Stash given model (includes validation and consolidation with DB data)
            swift::misc::CStatusMessage stashModel(const swift::misc::simulation::CAircraftModel &model,
                                                   bool replace = false, bool consolidateWithDbData = true,
                                                   bool clearHighlighting = true);

            //! Stash given models (includes validation and consolidation with DB data)
            swift::misc::CStatusMessageList stashModels(const swift::misc::simulation::CAircraftModelList &models,
                                                        bool replace = false, bool consolidateWithDbData = true,
                                                        bool clearHighlighting = true);

            //! Replace models, no validation
            void replaceModelsUnvalidated(const swift::misc::simulation::CAircraftModelList &models);

        signals:
            //! Stashed models have been changed
            void stashedModelsChanged();

            //! Models succesfully published
            void modelsSuccessfullyPublished(const swift::misc::simulation::CAircraftModelList &publishedModels,
                                             bool directWrite);

        private:
            QScopedPointer<Ui::CDbStashComponent> ui;
            swift::misc::CDataReadOnly<swift::core::data::TAuthenticatedDbUser> m_swiftDbUser {
                this, &CDbStashComponent::onUserChanged
            }; //!< authenticated user

            //! Unstash pressed
            void onUnstashPressed();

            //! Validate pressed
            void onValidatePressed();

            //! Remove invalid (validates and removes invalid models)
            void onRemoveInvalidPressed();

            //! Publish pressed
            void onPublishPressed();

            //! Publish response received
            void onPublishedModelsResponse(const swift::misc::simulation::CAircraftModelList &publishedModels,
                                           const swift::misc::simulation::CAircraftModelList &skippedModels,
                                           const swift::misc::CStatusMessageList &msgs, bool sendingSuccesful,
                                           bool directWrite);

            //! Copy over values
            void copyOverValuesToSelectedModels();

            //! Display model dialog
            void modifyModelDialog();

            //! Row count changed
            void onRowCountChanged(int number, bool filter);

            //! User has been changed
            void onUserChanged();

            //! Display messages
            bool showOverlayMessages(const swift::misc::CStatusMessageList &msgs, bool onlyErrors = false,
                                     bool appendOldMessages = false, int timeoutMs = -1);

            //! Display messages with confirmation
            bool showOverlayMessagesWithConfirmation(const swift::misc::CStatusMessageList &msgs,
                                                     bool appendOldMessages, const QString &confirmation,
                                                     std::function<void()> okLambda,
                                                     QMessageBox::StandardButton defaultButton, bool onlyErrors = false,
                                                     int timeoutMs = -1);

            //! Display message
            bool showOverlayMessage(const swift::misc::CStatusMessage &msg, int timeoutMs = -1);

            //! Clear messages
            void clearOverlayMessages();

            //! Validate
            swift::misc::CStatusMessageList validate(swift::misc::simulation::CAircraftModelList &validModels,
                                                     swift::misc::simulation::CAircraftModelList &invalidModels) const;

            //! Validate and display info messages
            bool validateAndDisplay(swift::misc::simulation::CAircraftModelList &validModels,
                                    swift::misc::simulation::CAircraftModelList &invalidModels,
                                    bool displayInfo = false);

            //! Set the button row
            void enableButtonRow();

            //! Validation categories
            const swift::misc::CLogCategoryList &validationCategories() const;

            //! Get the selected only models or all models depending on checkbox
            swift::misc::simulation::CAircraftModelList getSelectedOrAllModels() const;

            //! Consolidate with any DB data (if available).
            swift::misc::simulation::CAircraftModel
            consolidateWithDbData(const swift::misc::simulation::CAircraftModel &model, bool forced) const;

            //! Consolidate with own models (if available). This updates mostly with model description, path etc.
            swift::misc::simulation::CAircraftModel
            consolidateWithOwnModels(const swift::misc::simulation::CAircraftModel &model) const;

            //! Authenticated DB user
            swift::misc::network::CAuthenticatedUser getSwiftDbUser() const;
        };
    } // namespace components
} // namespace swift::gui

#endif // guard
