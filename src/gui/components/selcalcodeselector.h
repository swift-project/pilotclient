// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_SELCALCODESELECTOR_H
#define SWIFT_GUI_SELCALCODESELECTOR_H

#include "gui/swiftguiexport.h"
#include "misc/aviation/selcal.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

class QWidget;

namespace Ui
{
    class CSelcalCodeSelector;
}
namespace swift::gui::components
{
    //! SELCAL mode selector
    class SWIFT_GUI_EXPORT CSelcalCodeSelector : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSelcalCodeSelector(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSelcalCodeSelector();

        //! SELCAL code
        QString getSelcalCode() const;

        //! SELCAL
        swift::misc::aviation::CSelcal getSelcal() const;

        //! Reset the SELCAL code
        void resetSelcalCodes(bool allowEmptyValue = false);

        //! Set the SELCAL code
        void setSelcalCode(const QString &selcal);

        //! Set the SELCAL code
        void setSelcal(const swift::misc::aviation::CSelcal &selcal);

        //! Valid code?
        bool hasValidCode() const;

        //! Clear
        void clear();

        //! The height of the combobox
        int getComboBoxHeight() const;

        //! Set the combobox height
        void setComboBoxMinimumHeight(int h);

    signals:
        //! Value has been changed
        void valueChanged();

    private:
        QScopedPointer<Ui::CSelcalCodeSelector> ui;

        //! SELCAL changed
        void selcalIndexChanged(int index);

        //! Set valid/invalid icon
        void setValidityHint();
    };
} // ns

#endif // guard
