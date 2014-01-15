/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAMPLE_INFOWINDOW_H
#define SAMPLE_INFOWINDOW_H

#include <QWizardPage>
#include <QScopedPointer>

namespace Ui
{
    class InfoWindow;
}

class CInfoWindow : public QWizardPage
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor
     * \param parent
     */
    explicit CInfoWindow(QWidget *parent = nullptr);

    /*!
     * \brief Destructor
     */
    ~CInfoWindow();

    /*!
     * \brief Set info message displayed for some time
     * \param message
     * \param displayTimeMs
     */
    void setInfoMessage(const QString &message, int displayTimeMs = 4000);

private:
    QScopedPointer<Ui::InfoWindow> ui;
};

#endif // guard
