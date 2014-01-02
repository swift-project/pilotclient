#include "mainwindow.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>

/*!
 * \brief Main
 * \param argc
 * \param argv
 * \return
 */
int main(int argc, char *argv[])
{
    // register
    BlackMisc::initResources();
    BlackMisc::registerMetadata();
    // BlackMisc::displayAllUserMetatypesTypes();

    QFile file(":/translations/blackmisc_i18n_de.qm");
    qDebug() << (file.exists() ? "Found translations in resources" : "No translations in resources");
    QTranslator translator;
    translator.load("blackmisc_i18n_de", ":/translations/");

    // app
    QApplication a(argc, argv);
    a.installTranslator(&translator);

    // window
    MainWindow w;
    bool withDBus = false;

    // Dialog to decide external or internal core
    QMessageBox msgBox;
    msgBox.setText("How to start the GUI");
    msgBox.setIcon(QMessageBox::Question);
    QPushButton *buttonNoDbus = msgBox.addButton("With core included", QMessageBox::AcceptRole);
    QPushButton *buttonDBus = msgBox.addButton("External core via DBus", QMessageBox::AcceptRole);
    QPushButton *buttonClose = msgBox.addButton("Close", QMessageBox::RejectRole);
    msgBox.setDefaultButton(buttonNoDbus);
    msgBox.exec();
    if (msgBox.clickedButton() == buttonDBus)
        withDBus = true;
    else if (msgBox.clickedButton() == buttonNoDbus)
        withDBus = false;
    else if (msgBox.clickedButton() == buttonClose)
        exit(4);
    msgBox.close();

    // show window
    w.show();
    w.init(withDBus); // object is complete by now
    return a.exec();
}
