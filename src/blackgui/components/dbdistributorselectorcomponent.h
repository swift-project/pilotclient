// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_DBDISTRIBUTORSELECTORCOMPONENT_H
#define BLACKGUI_DBDISTRIBUTORSELECTORCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/dropbase.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/simulation/distributor.h"

#include <QFrame>
#include <QMetaObject>
#include <QObject>
#include <QScopedPointer>
#include <QString>

class QCompleter;
class QDragEnterEvent;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QWidget;

namespace Ui
{
    class CDbDistributorSelectorComponent;
}
namespace BlackGui::Components
{
    /*!
     * Distributor selector
     */
    class BLACKGUI_EXPORT CDbDistributorSelectorComponent :
        public QFrame,
        public CDropBase
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbDistributorSelectorComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbDistributorSelectorComponent() override;

        //! Current distributor
        void setDistributor(const BlackMisc::Simulation::CDistributor &distributor);

        //! Current Distributor
        void setDistributor(const QString &distributorKeyOrAlias);

        //! Distributor
        BlackMisc::Simulation::CDistributor getDistributor() const;

        //! Read only
        void setReadOnly(bool readOnly);

        //! Display distributor description
        void withDistributorDescription(bool description);

        //! Set with valid Distributor
        bool isSet() const;

        //! Clear selection
        void clear();

    signals:
        //! Distributor was changed
        void changedDistributor(const BlackMisc::Simulation::CDistributor &distributor);

    protected:
        //! \copydoc QWidget::dragEnterEvent
        virtual void dragEnterEvent(QDragEnterEvent *event) override;

        //! \copydoc QWidget::dragMoveEvent
        virtual void dragMoveEvent(QDragMoveEvent *event) override;

        //! \copydoc QWidget::dragLeaveEvent
        virtual void dragLeaveEvent(QDragLeaveEvent *event) override;

        //! \copydoc QWidget::dropEvent
        virtual void dropEvent(QDropEvent *event) override;

    private:
        //! Distributors have been read
        void onDistributorsRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

        //! Data have been changed
        void onDataChanged();

        //! Data have been changed
        void onCompleterActivated(const QString &distributorKeyOrAlias);

        QScopedPointer<Ui::CDbDistributorSelectorComponent> ui;
        QScopedPointer<QCompleter> m_completerDistributors;
        QMetaObject::Connection m_signalConnection;
        BlackMisc::Simulation::CDistributor m_currentDistributor;
    };
} // ns
#endif // guard
