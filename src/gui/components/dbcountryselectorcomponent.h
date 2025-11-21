// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_DBCOUNTRYSELECTORCOMPONENT_H
#define SWIFT_GUI_DBCOUNTRYSELECTORCOMPONENT_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

#include "gui/dropbase.h"
#include "gui/swiftguiexport.h"
#include "misc/country.h"
#include "misc/network/entityflags.h"

class QCompleter;
class QDragEnterEvent;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QWidget;

namespace Ui
{
    class CDbCountrySelectorComponent;
}
namespace swift::gui::components
{
    /*!
     * Country selector
     */
    class SWIFT_GUI_EXPORT CDbCountrySelectorComponent : public QFrame, public swift::gui::CDropBase
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbCountrySelectorComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CDbCountrySelectorComponent() override;

        //! Current country
        void setCountry(const swift::misc::CCountry &country);

        //! Current country
        void setCountry(const QString &isoCode);

        //! Country
        swift::misc::CCountry getCountry() const;

        //! Read only
        void setReadOnly(bool readOnly);

        //! Set with valid country
        bool isSet() const;

        //! Clear selection
        void clear();

    signals:
        //! Country has been changed
        void countryChanged(const swift::misc::CCountry &country);

    protected:
        //! \copydoc QWidget::dragEnterEvent
        void dragEnterEvent(QDragEnterEvent *event) override;

        //! \copydoc QWidget::dragMoveEvent
        void dragMoveEvent(QDragMoveEvent *event) override;

        //! \copydoc QWidget::dragLeaveEvent
        void dragLeaveEvent(QDragLeaveEvent *event) override;

        //! \copydoc QWidget::dropEvent
        void dropEvent(QDropEvent *event) override;

    private:
        //! Countries have been read
        void onCountriesRead(swift::misc::network::CEntityFlags::Entity entity,
                             swift::misc::network::CEntityFlags::ReadState readState, int count);

        //! Data have been changed
        void onDataChanged();

        //! Data have been changed
        void onCompleterActivated(const QString &countryName);

        QScopedPointer<Ui::CDbCountrySelectorComponent> ui;
        QScopedPointer<QCompleter> m_completerCountryNames;
        swift::misc::CCountry m_currentCountry;
    };
} // namespace swift::gui::components
#endif // SWIFT_GUI_DBCOUNTRYSELECTORCOMPONENT_H
