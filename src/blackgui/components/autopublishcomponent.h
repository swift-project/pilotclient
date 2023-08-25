// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_AUTOPUBLISHCOMPONENT_H
#define BLACKGUI_COMPONENTS_AUTOPUBLISHCOMPONENT_H

#include "blackgui/overlaymessagesframe.h"
#include "blackmisc/simulation/autopublishdata.h"

#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CAutoPublishComponent;
}
namespace BlackGui::Components
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
        BlackMisc::Simulation::CAutoPublishData m_data;
        BlackMisc::CData<BlackMisc::Simulation::Data::TLastAutoPublish> m_lastAutoPublish { this };

        //! Analyze against DB data
        void analyzeAgainstDBData();

        //! Send to DB
        void sendToDb();

        //! Display data in JSON text field
        void displayData();

        //! Delete all files
        void deleteAllFiles();

        //! Auto publishing completed
        void onAutoPublished(bool success, const QString &url, const BlackMisc::CStatusMessageList &msgs);

        //! Close parent dialog (if any)
        void closeParentDialog();
    };
} // ns

#endif // guard
