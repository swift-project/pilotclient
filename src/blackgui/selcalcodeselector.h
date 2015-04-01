/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SELCALCODESELECTOR_H
#define BLACKGUI_SELCALCODESELECTOR_H

#include "blackmisc/aviation/selcal.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CSelcalCodeSelector; }
namespace BlackGui
{

    //! SELCAL mode selector
    class CSelcalCodeSelector : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSelcalCodeSelector(QWidget *parent = nullptr);

        //! Destructor
        ~CSelcalCodeSelector();

        //! SELCAL code
        QString getSelcalCode() const;

        //! SELCAL
        BlackMisc::Aviation::CSelcal getSelcal() const;

        //! Reset the SELCAL code
        void resetSelcalCodes(bool allowEmptyValue = false);

        //! Set the SELCAL code
        void setSelcalCode(const QString &selcal);

        //! Set the SELCAL code
        void setSelcalCode(const BlackMisc::Aviation::CSelcal &selcal);

        //! Valid code?
        bool hasValidCode() const;

        //! Clear
        void clear();

    signals:
        //! Value has been changed
        void valueChanged();

    private slots:
        //! SELCAL changed
        void ps_selcalIndexChanged();

    private:
        QScopedPointer<Ui::CSelcalCodeSelector> ui;

        //! Set valid/invalid icon
        void setValidityHint();
    };
}

#endif // guard
