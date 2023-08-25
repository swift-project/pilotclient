// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_SELCALCODESELECTOR_H
#define BLACKGUI_SELCALCODESELECTOR_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/selcal.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

class QWidget;

namespace Ui
{
    class CSelcalCodeSelector;
}
namespace BlackGui::Components
{
    //! SELCAL mode selector
    class BLACKGUI_EXPORT CSelcalCodeSelector : public QFrame
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
        BlackMisc::Aviation::CSelcal getSelcal() const;

        //! Reset the SELCAL code
        void resetSelcalCodes(bool allowEmptyValue = false);

        //! Set the SELCAL code
        void setSelcalCode(const QString &selcal);

        //! Set the SELCAL code
        void setSelcal(const BlackMisc::Aviation::CSelcal &selcal);

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
