// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_AUTOPUBLISHCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_AUTOPUBLISHCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "gui/overlaymessagesframe.h"
#include "misc/simulation/autopublishdata.h"

namespace Ui
{
    class CAutoPublishComponent;
}
namespace swift::gui::components
{
    //! Data automatically collected and be be sent to backend
    class CAutoPublishComponent : public COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CAutoPublishComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAutoPublishComponent() override;

        //! Read the files
        int readFiles();

        //! Data empty
        bool isEmpty() const;

    private:
        QScopedPointer<Ui::CAutoPublishComponent> ui;
        swift::misc::simulation::CAutoPublishData m_data;
        swift::misc::CData<swift::misc::simulation::data::TLastAutoPublish> m_lastAutoPublish { this };

        //! Analyze against DB data
        void analyzeAgainstDBData();

        //! Send to DB
        void sendToDb();

        //! Display data in JSON text field
        void displayData();

        //! Delete all files
        void deleteAllFiles();

        //! Auto publishing completed
        void onAutoPublished(bool success, const QString &url, const swift::misc::CStatusMessageList &msgs);

        //! Close parent dialog (if any)
        void closeParentDialog();
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_AUTOPUBLISHCOMPONENT_H
