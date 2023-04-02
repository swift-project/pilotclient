/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWBASE_PROXYSTYLE_H
#define BLACKGUI_VIEWBASE_PROXYSTYLE_H

#include <QProxyStyle>
#include <QPen>
#include <QPainter>
#include <QStyleOptionViewItem>

namespace BlackGui::Views
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
} // namespace
#endif // guard
