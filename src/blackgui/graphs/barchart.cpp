/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "barchart.h"
#include <qwt_plot_renderer.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_multi_barchart.h>
#include <qwt_column_symbol.h>
#include <qwt_plot_layout.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>

namespace BlackGui
{
    namespace Graphs
    {
        CBarChart::CBarChart(QWidget *parent): QwtPlot(parent)
        {
            this->setAutoFillBackground(true);
            this->setPalette(Qt::white);
            this->canvas()->setPalette(QColor("LemonChiffon"));

            this->setTitle("Bar Chart");
            // setAxisTitle(QwtPlot::yLeft, "Whatever");
            // setAxisTitle(QwtPlot::xBottom, "Whatever");

            m_barChartItem.reset(new QwtPlotMultiBarChart("Bar Chart "));
            m_barChartItem->setLayoutPolicy(QwtPlotMultiBarChart::AutoAdjustSamples);
            m_barChartItem->setSpacing(5);
            m_barChartItem->setMargin(3);
            m_barChartItem->attach(this);

            // this->insertLegend(new QwtLegend());
            // this->setAutoReplot(true);
            // this->setOrientation(Qt::Vertical);
        }

        void CBarChart::setStyle(QwtPlotMultiBarChart::ChartStyle style)
        {
            m_barChartItem->setStyle(style);
        }

        void CBarChart::setOrientation(Qt::Orientation orientation, const QStringList &labels)
        {
            QwtPlot::Axis axis1, axis2;
            CTextScaleDraw *scaleDraw = labels.isEmpty() ? nullptr : new CTextScaleDraw(labels);

            if (orientation == Qt::Vertical)
            {
                axis1 = QwtPlot::xBottom;
                axis2 = QwtPlot::yLeft;

                m_barChartItem->setOrientation(Qt::Vertical);
                if (scaleDraw)
                {
                    scaleDraw->setLabelRotation(90);
                    scaleDraw->setLabelAlignment(Qt::AlignRight);
                }
            }
            else
            {
                axis1 = QwtPlot::yLeft;
                axis2 = QwtPlot::xBottom;

                m_barChartItem->setOrientation(Qt::Horizontal);
                if (scaleDraw)
                {
                    scaleDraw->setLabelRotation(0);
                    scaleDraw->setLabelAlignment(Qt::AlignLeft);
                }
            }

            setAxisScale(axis1, 0, m_barChartItem->dataSize() - 1, 1.0);
            setAxisAutoScale(axis2);

            QwtScaleDraw *scaleDraw1 = axisScaleDraw(axis1);
            scaleDraw1->enableComponent(QwtScaleDraw::Backbone, true);
            scaleDraw1->enableComponent(QwtScaleDraw::Ticks, false);

            QwtScaleDraw *scaleDraw2 = axisScaleDraw(axis2);
            scaleDraw2->enableComponent(QwtScaleDraw::Backbone, true);
            scaleDraw2->enableComponent(QwtScaleDraw::Ticks, false);

            this->setAxisScaleDraw(axis1, scaleDraw ? scaleDraw : new QwtScaleDraw());
            this->setAxisScaleDraw(axis2, new QwtScaleDraw());

            plotLayout()->setAlignCanvasToScale(axis1, true);
            plotLayout()->setAlignCanvasToScale(axis2, true);

            plotLayout()->setCanvasMargin(0);
            this->updateCanvasMargins();

            this->replot();
        }

        void CBarChart::setSymbols(const QString &title, const QString &color)
        {
            const QStringList titles({ title });
            const QStringList colors({ color });
            this->setSymbols(titles, colors);
        }

        void CBarChart::setSymbols(const QStringList &titles, const QStringList &colors)
        {
            Q_ASSERT_X(!titles.isEmpty(), Q_FUNC_INFO, "Need titles");
            Q_ASSERT_X(titles.size() == colors.size(), Q_FUNC_INFO, "Need same size");

            int i = 0;
            m_barChartItem->resetSymbolMap();
            for (const QString &color : colors)
            {
                QwtColumnSymbol *symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
                symbol->setLineWidth(2);
                symbol->setFrameStyle(QwtColumnSymbol::Raised);
                symbol->setPalette(QPalette(color));
                m_barChartItem->setSymbol(i++, symbol);
            }

            QList<QwtText> qwtTitles;
            for (const QString &title : titles)
            {
                qwtTitles << title;
            }
            m_barChartItem->setBarTitles(qwtTitles);
            m_barChartItem->setLegendIconSize(QSize(10, 14));
        }

        void CBarChart::setSamples1Bar(const QList<double> &samples, const QStringList &labels, Qt::Orientation orientation)
        {
            QVector< QVector<double> > series;
            for (double d : samples)
            {
                series << QVector<double> { d };
            }

            if (orientation == Qt::Vertical)
            {
                int w = samples.size() * 10;
                if (w < 200) w = 200;
                this->setMinimumWidth(w);
                this->setMinimumHeight(0);
            }
            else
            {
                int h = samples.size() * 10;
                if (h < 200) h = 200;
                this->setMinimumHeight(h);
                this->setMinimumWidth(0);
            }

            m_barChartItem->setSamples(series);
            this->setOrientation(orientation, labels);
        }

        CTextScaleDraw::CTextScaleDraw(const QStringList &texts) : m_labels(texts)
        {
            this->setLabelRotation(0);
            this->setLabelAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            this->setSpacing(5);
        }

        QwtText CTextScaleDraw::label(double v) const
        {
            if (m_labels.isEmpty()) { return QString::number(v); }
            const int i = qRound(v);
            if (i < 0 || m_labels.size() <= i) { return QwtText("invalid"); }
            return m_labels.at(i);
        }

    } // ns
} // ns

