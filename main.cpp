#include <QGuiApplication>
#include <QQmlApplicationEngine>


auto main(int argc, char *argv[]) -> int
{
    QGuiApplication app(argc, argv);

    // Set application parameters
    QCoreApplication::setOrganizationName("Akaflieg Freiburg");
    QCoreApplication::setOrganizationDomain("akaflieg_freiburg.de");
    QCoreApplication::setApplicationName( QCoreApplication::translate("C++ Main Program", "enrouteRenderer Test", "Application Name") );
    QGuiApplication::setDesktopFileName("de.akaflieg_freiburg.enrouteRenderer");

    // Start QML Engine
    QQmlApplicationEngine engine;
    engine.load("qrc:/gui/qml/main.qml");

    return QGuiApplication::exec();
}
