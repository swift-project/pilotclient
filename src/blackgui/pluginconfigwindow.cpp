#include "pluginconfigwindow.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QStyle>

namespace BlackGui {

    CPluginConfigWindow::CPluginConfigWindow(QWidget *parent) : QWidget(parent, Qt::Window)
    {

    }

    void CPluginConfigWindow::showEvent(QShowEvent *event)
    {
        this->setGeometry(
            QStyle::alignedRect(
                Qt::LeftToRight,
                Qt::AlignCenter,
                this->size(),
                QDesktopWidget().screenGeometry(qApp->activeWindow())
            )
        );

        Q_UNUSED(event);
    }

}
