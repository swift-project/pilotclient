/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_DBAIRLINEICAOSELECTORBASE_H
#define BLACKGUI_DBAIRLINEICAOSELECTORBASE_H

#include "blackgui/blackguiexport.h"
#include "blackgui/dropbase.h"
#include "blackmisc/aviation/airlineicaocode.h"
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

namespace BlackGui::Components
{
    /*!
     * Airline ICAO selector base class
     */
    class BLACKGUI_EXPORT CDbAirlineIcaoSelectorBase :
        public QFrame,
        public BlackGui::CDropBase
    {
        Q_OBJECT

    public:
        //! Destructor
        virtual ~CDbAirlineIcaoSelectorBase() override;

        //! Current airline ICAO
        virtual bool setAirlineIcao(const BlackMisc::Aviation::CAirlineIcaoCode &icao);

        //! Current airline ICAO by key
        bool setAirlineIcao(int key);

        //! Airline ICAO
        const BlackMisc::Aviation::CAirlineIcaoCode &getAirlineIcao() const { return m_currentIcao; }

        //! Read only
        virtual void setReadOnly(bool readOnly) = 0;

        //! Clear selection
        virtual void clear() = 0;

        //! Set with valid value
        bool isSet() const;

    signals:
        //! ICAO was changed
        void changedAirlineIcao(const BlackMisc::Aviation::CAirlineIcaoCode &icao);

    protected:
        //! Constructor
        explicit CDbAirlineIcaoSelectorBase(QWidget *parent = nullptr);

        //! Create a new completer
        virtual QCompleter *createCompleter() = 0;

        //! \copydoc QWidget::dragEnterEvent
        virtual void dragEnterEvent(QDragEnterEvent *event) override;

        //! \copydoc QWidget::dragMoveEvent
        virtual void dragMoveEvent(QDragMoveEvent *event) override;

        //! \copydoc QWidget::dragLeaveEvent
        virtual void dragLeaveEvent(QDragLeaveEvent *event) override;

        //! \copydoc QWidget::dropEvent
        virtual void dropEvent(QDropEvent *event) override;

        QScopedPointer<QCompleter>            m_completer;   //!< completer used
        BlackMisc::Aviation::CAirlineIcaoCode m_currentIcao; //!< current ICAO object

    private:
        //! Airlines have been read
        void onCodesRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count, const QUrl &url);

        //! Data have been changed
        void onCompleterActivated(const QString &icaoString);
    };
} // ns
#endif // guard
