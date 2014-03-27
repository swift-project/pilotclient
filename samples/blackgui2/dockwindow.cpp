#include "dockwindow.h"
#include "ui_dockwindow.h"
#include <QMouseEvent>

CDockWindow::CDockWindow(GuiModes::WindowMode windowMode, QWidget *parent) :
    QMainWindow(parent, windowMode == GuiModes::WindowFrameless ? (Qt::Window | Qt::FramelessWindowHint) : Qt::Tool),
    ui(new Ui::CDockWindow),
    m_windowMode(windowMode)
{
    ui->setupUi(this);
    connect(this->ui->dw_Edit, &QDockWidget::topLevelChanged, this, &CDockWindow::dockWindowTopLevelChanged);
}

void CDockWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (this->m_windowMode == GuiModes::WindowFrameless && event->buttons() & Qt::LeftButton)
    {
        move(event->globalPos() - this->m_dragPosition);
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void CDockWindow::mousePressEvent(QMouseEvent *event)
{
    if (this->m_windowMode == GuiModes::WindowFrameless && event->button() == Qt::LeftButton)
    {
        this->m_dragPosition = event->globalPos() - this->frameGeometry().topLeft();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void CDockWindow::dockWindowTopLevelChanged(bool topLevel)
{
    if (topLevel)
        this->adjustSize();
}

CDockWindow::~CDockWindow()
{
    delete ui;
}
