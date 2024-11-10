// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DBLOADDATADIALOG_H
#define BLACKGUI_COMPONENTS_DBLOADDATADIALOG_H

#include "blackgui/blackguiexport.h"
#include "misc/network/entityflags.h"
#include "misc/simulation/data/modelcaches.h"
#include <QDialog>
#include <QScopedPointer>
#include <QStringListModel>
#include <QUrl>
#include <QAbstractButton>

namespace Ui
{
    class CDbLoadDataDialog;
}
namespace BlackGui::Components
{
    /*!
     * Load data from DB as dialog
     */
    class BLACKGUI_EXPORT CDbLoadDataDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbLoadDataDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbLoadDataDialog();

        //! Newer or empty entities detected
        bool newerOrEmptyEntitiesDetected(swift::misc::network::CEntityFlags::Entity loadEntities);

    private:
        //! The string list model
        QStringListModel *entitiesModel() const;

        //! All selected items
        QStringList selectedEntities() const;

        //! Button clicked
        void onButtonClicked(QAbstractButton *button);

        //! Data are/have been read
        void onDataRead(swift::misc::network::CEntityFlags::Entity entity, swift::misc::network::CEntityFlags::ReadState state, int number, const QUrl &url);

        //! Download progress
        void onEntityDownloadProgress(swift::misc::network::CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url);

        //! Dialog rejected
        void onRejected();

        //! Consolidate
        void consolidate();

        QScopedPointer<Ui::CDbLoadDataDialog> ui;
        swift::misc::network::CEntityFlags::Entity m_pendingEntities = swift::misc::network::CEntityFlags::NoEntity;
        swift::misc::simulation::data::CModelSetCaches m_sets { true, this }; //!< caches
        swift::misc::simulation::data::CModelCaches m_models { true, this }; //!< models
        int m_pendingEntitiesCount = -1;
        bool m_consolidating = false; //! currently consolidating
        bool m_autoConsolidate = false;
    };
} // ns

#endif // guard
