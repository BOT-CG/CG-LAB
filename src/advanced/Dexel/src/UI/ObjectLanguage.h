#pragma once

#include <QObject>
#include <QTranslator>

class QApplication;

class ObjectLanguage : public QObject {
    Q_OBJECT

public:
    explicit ObjectLanguage(QObject* parent = nullptr)
        : QObject(parent)
    {
    }

    static QString name(const QString& qmFile);
    static QString code(const QString& qmFile);
    static bool match(QStringView languageCode, QStringView qmFile);
    static QStringList findQmFiles();
    static void installTranslator(QApplication* application, const QString& languageCode);

private:
    static QTranslator qtTranslator;
    static QTranslator appTranslator;
};
