#include "MainWindow.h"

#include "ObjectLanguage.h"
#include "Settings.h"

#include <QApplication>
#include <QSurfaceFormat>
#include <QtWidgets>

int main(int argc, char* argv[])
{
    Q_INIT_RESOURCE(shaders);
    Q_INIT_RESOURCE(computeShaders);
    Q_INIT_RESOURCE(icons);
    Q_INIT_RESOURCE(images);
    Q_INIT_RESOURCE(ncs);
    Q_INIT_RESOURCE(stls);
    Q_INIT_RESOURCE(translations);

    QApplication app(argc, argv);

    ObjectLanguage::installTranslator(&app, Settings::language());

    MainWindow window;

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(4, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);

    window.show();
    return app.exec();
}
