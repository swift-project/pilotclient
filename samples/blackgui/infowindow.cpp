#include "infowindow.h"
#include "ui_infowindow.h"
#include <QTimer>
#include <QDesktopWidget>


/*
 * Constructor
 */
CInfoWindow::CInfoWindow(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::InfoWindow)
{
    ui->setupUi(this);
}

/*
 * Destructor
 */
CInfoWindow::~CInfoWindow() { }

/*
 * Info message for some time
 */
void CInfoWindow::setInfoMessage(const QString &message, int displayTimeMs)
{
    // center
    const QRect parent = this->parentWidget()->geometry();
    const QRect myself = this->rect();
    int dx = (parent.width() - myself.width()) / 2;
    int dy = (parent.height() - myself.height()) / 2;
    dy -= 80; // some offset, in order to display further on top
    this->move(dx, dy);

    // message and display
    this->ui->te_Message->setText(message);
    this->show();

    // hide after some time
    QTimer::singleShot(displayTimeMs, this, SLOT(hide()));
}
