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
    static bool use5axis(); // 新增使用五轴的选择
    static void setUse5axis(bool value); // 新增使用五轴的选择
private:
    static QSettings settings;
};
