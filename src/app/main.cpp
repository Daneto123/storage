#include <QApplication>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QSettings>

#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setOrganizationName(QStringLiteral("Daneto123"));
    QApplication::setApplicationName(QStringLiteral("Storage"));
    QApplication::setApplicationVersion(QStringLiteral("2.0"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Складова програма"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QStringLiteral("file"), QStringLiteral("Файл на склада (.csv) за отваряне"));
    parser.process(app);

    MainWindow window;

    // File given on the command line, else the last one used, else the default.
    QString path = parser.positionalArguments().value(0);
    if (path.isEmpty()) path = QSettings().value(QStringLiteral("lastFile")).toString();
    if (path.isEmpty() || !window.openFile(QFileInfo(path).absoluteFilePath())) {
        window.openFile(MainWindow::defaultFilePath());
    }

    window.show();
    return app.exec();
}
