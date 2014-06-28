#ifndef BLACKGUI_SETTINGSSIMULATORCOMPONENT_H
#define BLACKGUI_SETTINGSSIMULATORCOMPONENT_H

#include "runtimebasedcomponent.h"
#include <QFrame>

namespace Ui { class CSettingsSimulatorComponent; }

namespace BlackGui
{
    /*!
     * All simulator settings component (GUI)
     */
    class CSettingsSimulatorComponent : public QFrame, public CRuntimeBasedComponent
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsSimulatorComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CSettingsSimulatorComponent();

    protected:
        //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet()
        virtual void runtimeHasBeenSet() override;

    private slots:
        //! Driver changed
        void pluginHasChanged(int index);

    private:
        Ui::CSettingsSimulatorComponent *ui; //!< UI

        //! Smarter way to set current driver, avoids unnecessary signals and less formatting dependend
        void setCurrentPlugin(const BlackSim::CSimulatorInfo &driver);

    };

} // namespace
#endif // guard
