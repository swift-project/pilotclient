// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_DBAIRLINEICAOSELECTORBASE_H
#define SWIFT_GUI_DBAIRLINEICAOSELECTORBASE_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

#include "gui/dropbase.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/network/entityflags.h"

class QCompleter;
class QDragEnterEvent;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QWidget;

namespace swift::gui::components
{
    /*!
     * Airline ICAO selector base class
     */
    class SWIFT_GUI_EXPORT CDbAirlineIcaoSelectorBase : public QFrame, public swift::gui::CDropBase
    {
        Q_OBJECT

    public:
        //! Destructor
        virtual ~CDbAirlineIcaoSelectorBase() override;

        //! Current airline ICAO
        virtual bool setAirlineIcao(const swift::misc::aviation::CAirlineIcaoCode &icao);

        //! Current airline ICAO by key
        bool setAirlineIcao(int key);

        //! Airline ICAO
        const swift::misc::aviation::CAirlineIcaoCode &getAirlineIcao() const { return m_currentIcao; }

        //! Read only
        virtual void setReadOnly(bool readOnly) = 0;

        //! Clear selection
        virtual void clear() = 0;

        //! Set with valid value
        bool isSet() const;

    signals:
        //! ICAO was changed
        void changedAirlineIcao(const swift::misc::aviation::CAirlineIcaoCode &icao);

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

        QScopedPointer<QCompleter> m_completer; //!< completer used
        swift::misc::aviation::CAirlineIcaoCode m_currentIcao; //!< current ICAO object

    private:
        //! Airlines have been read
        void onCodesRead(swift::misc::network::CEntityFlags::Entity entity,
                         swift::misc::network::CEntityFlags::ReadState readState, int count, const QUrl &url);

        //! Data have been changed
        void onCompleterActivated(const QString &icaoString);
    };
} // namespace swift::gui::components
#endif // SWIFT_GUI_DBAIRLINEICAOSELECTORBASE_H
