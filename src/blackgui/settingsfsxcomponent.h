#ifndef BLACKGUI_SETTINGSFSXCOMPONENT_H
#define BLACKGUI_SETTINGSFSXCOMPONENT_H

#include "blackgui/runtimebasedcomponent.h"
#include <QFrame>

namespace Ui { class CSettingsFsxComponent; }

namespace BlackGui
{

    /*!
     * \brief Settings for FSX
     */
    class CSettingsFsxComponent : public QFrame, public CRuntimeBasedComponent
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsFsxComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CSettingsFsxComponent();

    private slots:
        //! Test the SIM connect connectivity
        void testSimConnectConnection();

        //! Save a simconnect.cfg file for FSX
        void saveSimConnectCfg();

        //! simConnect.cfg: open, exists? delete
        void simConnectCfgFile();

    private:
        Ui::CSettingsFsxComponent *ui;
    };
}

#endif // guard
