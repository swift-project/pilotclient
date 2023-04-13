/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_COLORSELECTOR_H
#define BLACKGUI_COMPONENTS_COLORSELECTOR_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/rgbcolor.h"

#include <QColor>
#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

class QDragEnterEvent;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QMouseEvent;
class QWidget;

namespace Ui
{
    class CColorSelector;
}

namespace BlackGui::Components
{
    /*!
     * Select a color
     */
    class BLACKGUI_EXPORT CColorSelector : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CColorSelector(QWidget *parent = nullptr);

        //! Destructor
        ~CColorSelector() override;

        //! Color
        void setColor(const BlackMisc::CRgbColor &color);

        //! Color
        void setColor(const QColor &color);

        //! Selected color
        BlackMisc::CRgbColor getColor() const;

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

    private:
        //! Open up a color dilaog
        void colorDialog();

        //! Enter pressed
        void onReturnPressed();

        //! Set color by color name
        void setColorByName(const QString &colorName);

        //! Reset to last valid color
        void resetToLastValidColor();

        QScopedPointer<Ui::CColorSelector> ui;
        BlackMisc::CRgbColor m_lastColor;
    };
} // ns

#endif // guard
