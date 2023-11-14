#include "Settings.h"

#include <QLocale>

QSettings Settings::settings = QSettings("settings.ini", QSettings::IniFormat);

QString Settings::language()
{
    return settings.value("language", QLocale::system().name()).toString();
}

void Settings::setLanguage(const QString& language)
{
    settings.setValue("language", language);
}

float Settings::centerStep()
{
    return settings.value("centerStep", 1.0f).toFloat();
}

void Settings::setCenterStep(float value)
{
    settings.setValue("centerStep", value);
}

float Settings::directionStep()
{
    return settings.value("directionStep", 0.5f).toFloat();
}

void Settings::setDirectionStep(float value)
{
    settings.setValue("directionStep", value);
}

bool Settings::useSweptVolume()
{
    return settings.value("useSweptVolume", true).toBool();
}

void Settings::setUseSweptVolume(bool value)
{
    settings.setValue("useSweptVolume", value);
}

int Settings::maxLinePerExecute()
{
    return settings.value("maxLinePerExecute", 1).toInt();
}

void Settings::setMaxLinePerExecute(int value)
{
    settings.setValue("maxLinePerExecute", value);
}

bool Settings::renderWorkpieceSurfaceUsingDistanceDirectly()
{
    return settings.value("renderWorkpieceSurfaceUsingDistanceDirectly", false).toBool();
}

void Settings::setRenderWorkpieceSurfaceUsingDistanceDirectly(bool value)
{
    settings.setValue("renderWorkpieceSurfaceUsingDistanceDirectly", value);
}
