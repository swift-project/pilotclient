/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSAMPLE_LED_H
#define BLACKSAMPLE_LED_H

#include <QWidget>
#include <QPainter>

//! \brief LED widget
class CLed : public QWidget
{
    Q_OBJECT
public:
    CLed(QWidget *parent = 0);

    /*!
     * \brief LED color
     * \return
     */
    QColor color() const { return m_color; }

    /*!
     * \brief Set LED color
     * \param color
     */
    void setColor(QColor color) { m_color = color; }

    /*!
     * \brief isChecked
     * \return
     */
    bool isChecked () const { return m_isChecked; }

    //! \copydoc QWidget::minimumSizeHint()
    virtual QSize minimumSizeHint () const override;

protected:
    //! \copydoc QWidget::minimumSizeHint()
    virtual void paintEvent(QPaintEvent * event) override;

signals:
    /*!
     * \brief Check value has changed
     * \param value
     */
    void checkChanged(bool value);

public slots:
    /*!
     * \brief setChecked
     * \param value
     */
    void setChecked(bool value) {m_isChecked = value;}

private:
    /*!
     * \brief drawLed
     * \param painter
     */
    void drawLed(QPainter & painter);

    /*!
     * \brief drawShine
     * \param painter
     */
    void drawShine(QPainter & painter);

    bool m_isChecked;
    QColor m_color;

};

#endif // BLACKSAMPLE_LED_H
