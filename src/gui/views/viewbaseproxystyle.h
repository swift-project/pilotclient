// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWBASE_PROXYSTYLE_H
#define SWIFT_GUI_VIEWBASE_PROXYSTYLE_H

#include <QPainter>
#include <QPen>
#include <QProxyStyle>
#include <QStyleOptionViewItem>

namespace swift::gui::views
{
    class CViewBaseNonTemplate;

    /*!
     * Proxy for style of our views
     */
    class CViewBaseProxyStyle : public QProxyStyle
    {
        Q_OBJECT

    public:
        //! Constructor
        CViewBaseProxyStyle(CViewBaseNonTemplate *view, QStyle *style = nullptr);

        //! \name Proxy style overrides
        //! @{

        //! \copydoc QProxyStyle::drawPrimitive
        virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;
        //! @}

    private:
        CViewBaseNonTemplate *m_view = nullptr; //!< "parent view"
    };
} // namespace swift::gui::views
#endif // guard
