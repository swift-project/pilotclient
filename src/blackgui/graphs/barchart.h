/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_GRAPH_BARCHART_H
#define BLACKGUI_GRAPH_BARCHART_H

#include "blackgui/blackguiexport.h"

#include <qwt_plot.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_multi_barchart.h>

#include <QScopedPointer>
#include <QStringList>
#include <QList>

namespace BlackGui::Graphs
{
    /*!
     * For axis labels
     */
    class CTextScaleDraw : public QwtScaleDraw
    {
    public:
        //! Constructorxs
        CTextScaleDraw(const QStringList &texts);

        //! Value to label
        virtual QwtText label(double v) const override;

        //! Labels
        void setLabels(const QStringList &labels) { m_labels = labels; }

        //! Any labels?
        bool hasLabels() const { return !m_labels.isEmpty(); }

    private:
        QStringList m_labels;
    };

    /*!
     * Bar chart, modified version of QWT examples
     */
    class BLACKGUI_EXPORT CBarChart : public QwtPlot
    {
    public:
        //! Constructor
        CBarChart(QWidget * = nullptr);

        //! Style
        void setStyle(QwtPlotMultiBarChart::ChartStyle style);

        //! Set number of symbols
        void setSymbols(const QString &title, const QString &color);

        //! Set number of symbols
        void setSymbols(const QStringList &titles, const QStringList &colors);

        //! Samples with 1 bar per sample
        void setSamples1Bar(const QList<double> &samples, const QStringList &labels, Qt::Orientation orientation);

    private:
        //! Orientation
        void setOrientation(Qt::Orientation, const QStringList &labels);

        QScopedPointer<QwtPlotMultiBarChart> m_barChartItem;
    };
} // ns

#endif
