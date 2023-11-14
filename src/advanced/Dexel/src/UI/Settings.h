#pragma once

#include <QSettings>

class Settings {
public:
    Settings() = delete;
    ~Settings() = delete;
    static QString language();
    static void setLanguage(const QString& language);
    static float centerStep();
    static void setCenterStep(float value);
    static float directionStep();
    static void setDirectionStep(float value);
    static bool useSweptVolume();
    static void setUseSweptVolume(bool value);
    static int maxLinePerExecute();
    static void setMaxLinePerExecute(int value);
    static bool renderWorkpieceSurfaceUsingDistanceDirectly();
    static void setRenderWorkpieceSurfaceUsingDistanceDirectly(bool value);

private:
    static QSettings settings;
};
