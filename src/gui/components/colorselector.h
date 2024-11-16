// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_COLORSELECTOR_H
#define SWIFT_GUI_COMPONENTS_COLORSELECTOR_H

#include <QColor>
#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

#include "gui/swiftguiexport.h"
#include "misc/rgbcolor.h"

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

namespace swift::gui::components
{
    /*!
     * Select a color
     */
    class SWIFT_GUI_EXPORT CColorSelector : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CColorSelector(QWidget *parent = nullptr);

        //! Destructor
        ~CColorSelector() override;

        //! Color
        void setColor(const swift::misc::CRgbColor &color);

        //! Color
        void setColor(const QColor &color);

        //! Selected color
        swift::misc::CRgbColor getColor() const;

        //! Read only
        void setReadOnly(bool readOnly);

        //! Clear
        void clear();

    signals:
        //! Color has been changed
        void colorChanged(const swift::misc::CRgbColor &color);

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
        swift::misc::CRgbColor m_lastColor;
    };
} // namespace swift::gui::components

#endif // guard
