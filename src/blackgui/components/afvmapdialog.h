// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_AFVMAPDIALOG_H
#define BLACKGUI_COMPONENTS_AFVMAPDIALOG_H

#include "blackgui/blackguiexport.h"
#include <QDialog>
#include <QScopedPointer>

namespace BlackCore::Afv
{
    namespace Model
    {
        class CAfvMapReader;
    }
    namespace Clients
    {
        class CAfvClient;
    }
}
namespace Ui
{
    class CAfvMapDialog;
}
namespace BlackGui::Components
{
    //! QML map to display ATC stations
    class BLACKGUI_EXPORT CAfvMapDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CAfvMapDialog(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CAfvMapDialog() override;

    private:
        QScopedPointer<Ui::CAfvMapDialog> ui;
        BlackCore::Afv::Model::CAfvMapReader *m_afvMapReader = nullptr;
        BlackCore::Afv::Clients::CAfvClient *m_afvClient = nullptr;
    };
} // ns

#endif // guard
