#ifndef BLACKGUI_SETTINGSSIMULATORCOMPONENT_H
#define BLACKGUI_SETTINGSSIMULATORCOMPONENT_H

#include "runtimebasedcomponent.h"
#include "blacksim/setsimulator.h"
#include <QFrame>

namespace Ui { class CSettingsSimulatorComponent; }

namespace BlackGui
{
    namespace Components
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
            void ps_pluginHasChanged(int index);

            //! Settings have been changed
            void ps_settingsHaveChanged(uint settingsType);

            //! A GUI value has been changed
            void ps_guiValueHasChanged();

        private:
            Ui::CSettingsSimulatorComponent *ui; //!< UI

            //! Smarter way to set current driver, avoids unnecessary signals and less formatting dependend
            void setCurrentPlugin(const BlackSim::CSimulatorInfo &plugin);

        };
    }
} // namespace
#endif // guard
