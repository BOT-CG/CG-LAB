#include "ObjectLanguage.h"

#include <QApplication>
#include <QDir>
#include <QLibraryInfo>
#include <QLocale>
#include <QSettings>
#include <QTranslator>

QTranslator ObjectLanguage::qtTranslator = QTranslator();
QTranslator ObjectLanguage::appTranslator = QTranslator();

QString ObjectLanguage::name(const QString& qmFile)
{
    QTranslator translator;
    if (!translator.load(qmFile)) {
        qWarning("Unable to load %s", qPrintable(QDir::toNativeSeparators(qmFile)));
        return QString();
    }
    return translator.translate("ObjectLanguage", "English");
}

QString ObjectLanguage::code(const QString& qmFile)
{
    QTranslator translator;
    if (!translator.load(qmFile)) {
        qWarning("Unable to load %s", qPrintable(QDir::toNativeSeparators(qmFile)));
        return QString();
    }
    return translator.translate("ObjectLanguage", "en");
}

bool ObjectLanguage::match(QStringView languageCode, QStringView qmFile)
{
    const QStringView prefix { u"i18n_" };
    const int langTokenLength = 2;
    return qmFile.mid(qmFile.indexOf(prefix) + prefix.length(), langTokenLength) == languageCode.left(langTokenLength);
}

QStringList ObjectLanguage::findQmFiles()
{
    QDir dir(":/translations");
    QStringList fileNames = dir.entryList(QStringList("*.qm"), QDir::Files, QDir::Name);
    for (QString& fileName : fileNames) {
        fileName = dir.filePath(fileName);
    }
    return fileNames;
}

void ObjectLanguage::installTranslator(QApplication* application, const QString& languageCode)
{
    if (qtTranslator.load(QLocale(languageCode), u"qtbase"_qs, u"_"_qs, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        application->installTranslator(&qtTranslator);
    }
    if (appTranslator.load(QLocale(languageCode), QLatin1String("i18n"), QLatin1String("_"), QLatin1String(":/translations"))) {
        application->installTranslator(&appTranslator);
    }
}
