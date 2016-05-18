#include "blackgui/pluginconfigwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QStyle>
#include <Qt>
#include <QtGlobal>

namespace BlackGui {

    CPluginConfigWindow::CPluginConfigWindow(QWidget *parent) : QWidget(parent, Qt::Window)
    {
        setWindowModality(Qt::WindowModal);
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
