// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBAIRCRAFTICAOSELECTORCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_DBAIRCRAFTICAOSELECTORCOMPONENT_H

#include "gui/swiftguiexport.h"
#include "gui/dropbase.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/network/entityflags.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

class QCompleter;
class QDragEnterEvent;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;

namespace Ui
{
    class CDbAircraftIcaoSelectorComponent;
}
namespace swift::gui::components
{
    /*!
     * Aircraft ICAO selector
     */
    class SWIFT_GUI_EXPORT CDbAircraftIcaoSelectorComponent :
        public QFrame,
        public CDropBase
    {
        Q_OBJECT

    public:
        //! How to display the info
        enum Display
        {
            DisplayIcaoAndId,
            DisplayCompleterString
        };

        //! Constructor
        explicit CDbAircraftIcaoSelectorComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbAircraftIcaoSelectorComponent() override;

        //! Current aircraft ICAO
        //! \return changed
        bool setAircraftIcao(const swift::misc::aviation::CAircraftIcaoCode &icao);

        //! Current aircraft ICAO
        //! \return changed
        bool setAircraftIcao(int key);

        //! ICAO code
        swift::misc::aviation::CAircraftIcaoCode getAircraftIcao() const;

        //! Raw designator
        QString getRawDesignator() const;

        //! Read only
        void setReadOnly(bool readOnly);

        //! Display distributor description
        void displayWithIcaoDescription(bool description);

        //! How to display string in text field
        void displayMode(Display mode) { m_display = mode; }

        //! Set with valid Distributor
        bool isSet() const;

        //! Clear selection
        void clear();

    signals:
        //! ICAO was changed
        void changedAircraftIcao(const swift::misc::aviation::CAircraftIcaoCode &icao);

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
        void onCodesRead(swift::misc::network::CEntityFlags::Entity entity, swift::misc::network::CEntityFlags::ReadState readState, int count);

        //! Data have been changed
        void onCompleterActivated(const QString &icaoString);

        //! Data have been changed
        void onDataChanged();

        //! Get the completer strings
        //! \remark shared for performance reasons
        const QStringList &completerStrings();

        QScopedPointer<Ui::CDbAircraftIcaoSelectorComponent> ui;
        QScopedPointer<QCompleter> m_completerIcaoDescription;
        swift::misc::aviation::CAircraftIcaoCode m_currentIcao;
        Display m_display = DisplayIcaoAndId;
        QStringList m_completerStrings; //!< the completer strings
    };
} // ns
#endif // guard
