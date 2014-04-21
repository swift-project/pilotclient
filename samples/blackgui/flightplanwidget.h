#ifndef BLACKGUI_FLIGHTPLANWIDGET_H
#define BLACKGUI_FLIGHTPLANWIDGET_H

#include <QFrame>

namespace Ui
{
    class CFlightplanWidget;
}

namespace BlackGui
{
    class CFlightplanWidget : public QFrame
    {
        Q_OBJECT

    public:
        explicit CFlightplanWidget(QWidget *parent = nullptr);
        ~CFlightplanWidget();

    private:
        Ui::CFlightplanWidget *ui;
    };

}
#endif // guard
