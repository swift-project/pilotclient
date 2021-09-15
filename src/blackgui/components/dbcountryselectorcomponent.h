/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_DBCOUNTRYSELECTORCOMPONENT_H
#define BLACKGUI_DBCOUNTRYSELECTORCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/dropbase.h"
#include "blackmisc/country.h"
#include "blackmisc/network/entityflags.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

class QCompleter;
class QDragEnterEvent;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QWidget;

namespace Ui { class CDbCountrySelectorComponent; }
namespace BlackGui::Components
{
    /*!
     * Country selector
     */
    class BLACKGUI_EXPORT CDbCountrySelectorComponent :
        public QFrame,
        public BlackGui::CDropBase
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbCountrySelectorComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CDbCountrySelectorComponent() override;

        //! Current country
        void setCountry(const BlackMisc::CCountry &country);

        //! Current country
        void setCountry(const QString &isoCode);

        //! Country
        BlackMisc::CCountry getCountry() const;

        //! Read only
        void setReadOnly(bool readOnly);

        //! Set with valid country
        bool isSet() const;

        //! Clear selection
        void clear();

    signals:
        //! Country has been changed
        void countryChanged(const BlackMisc::CCountry &country);

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
        //! Countries have been read
        void onCountriesRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

        //! Data have been changed
        void onDataChanged();

        //! Data have been changed
        void onCompleterActivated(const QString &countryName);

        QScopedPointer<Ui::CDbCountrySelectorComponent> ui;
        QScopedPointer<QCompleter> m_completerCountryNames;
        BlackMisc::CCountry m_currentCountry;
    };
}
#endif // guard
