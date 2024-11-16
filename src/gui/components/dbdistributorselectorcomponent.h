// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_DBDISTRIBUTORSELECTORCOMPONENT_H
#define SWIFT_GUI_DBDISTRIBUTORSELECTORCOMPONENT_H

#include <QFrame>
#include <QMetaObject>
#include <QObject>
#include <QScopedPointer>
#include <QString>

#include "gui/dropbase.h"
#include "gui/swiftguiexport.h"
#include "misc/network/entityflags.h"
#include "misc/simulation/distributor.h"

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
namespace swift::gui::components
{
    /*!
     * Distributor selector
     */
    class SWIFT_GUI_EXPORT CDbDistributorSelectorComponent :
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
        void setDistributor(const swift::misc::simulation::CDistributor &distributor);

        //! Current Distributor
        void setDistributor(const QString &distributorKeyOrAlias);

        //! Distributor
        swift::misc::simulation::CDistributor getDistributor() const;

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
        void changedDistributor(const swift::misc::simulation::CDistributor &distributor);

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
        void onDistributorsRead(swift::misc::network::CEntityFlags::Entity entity, swift::misc::network::CEntityFlags::ReadState readState, int count);

        //! Data have been changed
        void onDataChanged();

        //! Data have been changed
        void onCompleterActivated(const QString &distributorKeyOrAlias);

        QScopedPointer<Ui::CDbDistributorSelectorComponent> ui;
        QScopedPointer<QCompleter> m_completerDistributors;
        QMetaObject::Connection m_signalConnection;
        swift::misc::simulation::CDistributor m_currentDistributor;
    };
} // namespace swift::gui::components
#endif // guard
