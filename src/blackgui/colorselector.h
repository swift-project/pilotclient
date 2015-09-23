/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COLORSELECTOR_H
#define BLACKGUI_COLORSELECTOR_H

#include "blackmisc/rgbcolor.h"
#include <QFrame>

namespace Ui { class CColorSelector; }

namespace BlackGui
{
    /*!
     * Select a color
     */
    class CColorSelector : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CColorSelector(QWidget *parent = nullptr);

        //! Destructor
        ~CColorSelector();

        //! Color
        void setColor(const BlackMisc::CRgbColor &color);

        //! Color
        void setColor(const QColor &color);

        //! Selected color
        const BlackMisc::CRgbColor getColor() const;

        //! Read only
        void setReadOnly(bool readOnly);

        //! Clear
        void clear();

    signals:
        //! Color has been changed
        void colorChanged(const BlackMisc::CRgbColor &color);

    protected:
        //! \copydoc QWidget::dragEnterEvent
        virtual void dragEnterEvent(QDragEnterEvent *event) override;

        //! \copydoc QWidget::dragMoveEvent
        virtual void dragMoveEvent(QDragMoveEvent *event) override;

        //! \copydoc QWidget::dragLeaveEvent
        virtual void dragLeaveEvent(QDragLeaveEvent *event) override;

        //! \copydoc QWidget::dropEvent
        virtual void dropEvent(QDropEvent *event) override;

        //! For dragging the color
        virtual void mousePressEvent(QMouseEvent *event) override;

    private slots:
        //! Open up a color dilaog
        void ps_colorDialog();

        //! Enter pressed
        void ps_returnPressed();

        //! Set color by color name
        void ps_colorName(const QString &colorName);

    private:
        QScopedPointer<Ui::CColorSelector> ui;
        BlackMisc::CRgbColor m_lastColor;

        //! Reset to last valid color
        void resetToLastValidColor();
    };

} // ns

#endif // guard
