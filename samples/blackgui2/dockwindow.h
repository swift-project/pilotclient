#ifndef BLACKGUI_DOCKWINDOW_H
#define BLACKGUI_DOCKWINDOW_H

#include "guimodeenums.h"
#include <QMainWindow>

namespace Ui
{
    class CDockWindow;
}

class CDockWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CDockWindow(GuiModes::WindowMode windowMode, QWidget *parent = nullptr);
    ~CDockWindow();

protected:

    /*!
     * \brief Mouse moving, required for frameless window
     */
    void mouseMoveEvent(QMouseEvent *event);

    /*!
     * \brief Mouse press, required for frameless window
     */
    void mousePressEvent(QMouseEvent *event);

private:
    Ui::CDockWindow *ui;
    GuiModes::WindowMode m_windowMode;

    // frameless window
    QPoint m_dragPosition; /*!< position, if moving is handled with frameless window */

private slots:
    //! \brief Docked window is floating / docked again
    void dockWindowTopLevelChanged(bool topLevel);


};

#endif // DOCKWINDOW_H
