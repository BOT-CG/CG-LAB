#include "CutterSweptVolume.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//包含std::set需要的头文件
#include <set>

//已知三维空间存在StartPoint和EndPoint组成一条线段，现在有一个点Point，求该点到线段的垂点的坐标
void getInterSectionPoint(const Vector3Df& StartPoint, const Vector3Df& EndPoint, const Vector3Df& Point, Vector3Df& res)
{
    Vector3Df result = Vector3Df(0.0f, 0.0f, 0.0f);
    Vector3Df v1 = EndPoint - StartPoint;
    // if (std::abs(v1.x + v1.y + v1.z) < 0.0001) {
    //     res = StartPoint;
    //     return;
    //     // std::cout<<"v1.x + v1.y + v1.z = 0"<<std::endl;
    // }
    Vector3Df v2 = Point - StartPoint;
    v1.z = 0;
    v2.z = 0;
    float t = v1.dot(v2) / v1.dot(v1);
    // t = std::abs(t);
    v1 = EndPoint - StartPoint;
    result = StartPoint + v1 * t;
    res = result;
    // return result;
}

void getArcInterSectionPoint(const Vector3Df& CenterPoint, const float& CutterR, const Vector3Df& Point, Vector3Df& res)
{
    Vector3Df result;
    Vector3Df v1 = Point - CenterPoint;
    float t = CutterR / v1.length();
    result = CenterPoint + v1 * t;
    res = result;
    // return result;
}

float DexelCutterSweptVolume::getLength() const
{
    switch (sweptVolmueParam.type) {
    case SweptVolmueParam::Type::Scatter:
        return 0;
    case SweptVolmueParam::Type::Line:
        return (endWithOffset.center - startWithOffset.center).length();
    case SweptVolmueParam::Type::Arc: {
        PostureArcInterpolator interpolator(startWithOffset, endWithOffset, plane, radius, sweptVolmueParam.isClockwise);
        return interpolator.getLength();
    }
    case SweptVolmueParam::Type::Line5Axis:
        return (endWithOffset.center - startWithOffset.center).length();
    default:
        return 0;
    }
}

void DexelCutterSweptVolume::setOffset(const Vector3Df& offset)
{
    this->offset = offset;
}

void DexelCutterSweptVolume::setScale(float scale)
{
    this->sweptVolmueParam.scale = scale;
}

void DexelCutterSweptVolume::setCutter(std::unique_ptr<Cutter> cutter)
{
    this->cutter = cutter->clone();
}

void DexelCutterSweptVolume::setScatter(const Posture& start)
{
    this->sweptVolmueParam.type = SweptVolmueParam::Type::Scatter;
    const Vector3Df startDexel = (start.center - offset) * sweptVolmueParam.scale;
    const float cutterRadiusDexel = this->cutter->getMaxRadius() * sweptVolmueParam.scale;
    const float cutterHeightDexel = this->cutter->getMaxHeight() * sweptVolmueParam.scale;

    // this->dexelGrid = DexelGrid<1000, 1000>([&](float x, float y) {
    //     float distance = Vector3Df(x, y, startDexel.z).distanceToPoint(startDexel);
    //     if (distance <= cutterRadiusDexel) {
    //         float minHeight = startDexel.z + this->cutter->getDexelHeightZ(distance / scale) * scale;
    //         return std::list<Dexel> { Dexel { minHeight }, Dexel { startDexel.z + cutterHeightDexel } };
    //     }
    //     return std::list<Dexel> {};
    // });
}

void DexelCutterSweptVolume::setLine(const Posture& start, const Posture& end)
{
    this->sweptVolmueParam.type = SweptVolmueParam::Type::Line;
    this->startWithOffset = start;
    this->startWithOffset.center -= offset;
    this->endWithOffset = end;
    this->endWithOffset.center -= offset;

    const Vector3Df startDexel = (start.center - offset) * sweptVolmueParam.scale;
    const Vector3Df endDexel = (end.center - offset) * sweptVolmueParam.scale;
    const Vector3Df direction = (endDexel - startDexel).normalize();

    const Vector3Df normal = direction.cross({ -startDexel.y, startDexel.x, 0 }).normalize();
    const float cutterRadiusDexel = this->cutter->getMaxRadius() * sweptVolmueParam.scale;
    const float cutterHeightDexel = this->cutter->getMaxHeight() * sweptVolmueParam.scale;

    // this->dexelGrid = DexelGrid<1000, 1000>([&](float x, float y) {
    //     // (x-start.x) * normal.x + (y-start.y) * normal.y + (z-start.z) * normal.z = 0
    //     float z;
    //     if (floatEqual(normal.z, 0.0f)) {
    //         z = startDexel.z;
    //     } else {
    //         z = (-normal.x * (x - startDexel.x) - normal.y * (y - startDexel.y)) / normal.z + startDexel.z;
    //     }

    //     float distance = Vector3Df(x, y, z).distanceToLineSegment(startDexel, endDexel);
    //     if (distance <= cutterRadiusDexel) {
    //         float minHeight = z + this->cutter->getDexelHeightZ(distance / scale) * scale;
    //         return std::list<Dexel> { Dexel { minHeight }, Dexel { z + cutterHeightDexel } };
    //     }
    //     return std::list<Dexel> {};
    // });
}

void DexelCutterSweptVolume::setArc(const Posture& start, const Posture& end, Plane plane, float radius, bool isClockwise)
{
    this->sweptVolmueParam.type = SweptVolmueParam::Type::Arc;
    this->startWithOffset = start;
    this->startWithOffset.center -= offset;
    this->endWithOffset = end;
    this->endWithOffset.center -= offset;
    this->plane = plane;
    this->radius = radius;
    // this->isClockwise = isClockwise;

    const Vector3Df startDexel = (start.center - offset) * sweptVolmueParam.scale;
    const Vector3Df endDexel = (end.center - offset) * sweptVolmueParam.scale;
    const Vector3Df center = startDexel + Vector3Df(-radius, 0, 0);
    const Vector3Df direction = (endDexel - startDexel).normalize();
    const Vector3Df normal = direction.cross({ -startDexel.y, startDexel.x, 0 }).normalize();
    const float cutterRadiusDexel = this->cutter->getMaxRadius() * sweptVolmueParam.scale;
    const float cutterHeightDexel = this->cutter->getMaxHeight() * sweptVolmueParam.scale;
    const float radiusDexel = radius * sweptVolmueParam.scale;

    // this->dexelGrid = DexelGrid<1000, 1000>([&](float x, float y) {
    //     float distance = Vector3Df(x, y, startDexel.z).distanceToArc(startDexel, endDexel, radiusDexel, isClockwise);
    //     if (distance <= cutterRadiusDexel) {
    //         float minHeight = startDexel.z + this->cutter->getDexelHeightZ(distance / scale) * scale;
    //         return std::list<Dexel> { Dexel { minHeight }, Dexel { startDexel.z + cutterHeightDexel } };
    //     }
    //     return std::list<Dexel> {};
    // });
}

DexelCutterSweptVolume& DexelCutterSweptVolume::operator=(const DexelCutterSweptVolume& other)
{
    // dexelGrid = other.dexelGrid;
    sweptVolmueParam.type = other.sweptVolmueParam.type;
    offset = other.offset;
    sweptVolmueParam.scale = other.sweptVolmueParam.scale;
    startWithOffset = other.startWithOffset;
    endWithOffset = other.endWithOffset;
    plane = other.plane;
    radius = other.radius;
    // isClockwise = other.isClockwise;
    cutter = other.cutter->clone();
    sweptVolmueParam = other.sweptVolmueParam;

    // sweptVolmueParam.StartDexel = other.sweptVolmueParam.StartDexel;
    // EndDexel = other.EndDexel;
    // Direction = other.Direction;
    // Normal = other.Normal;
    // Center = other.Center;
    // CutterRadiusDexel = other.CutterRadiusDexel;
    // CutterHeightDexel = other.CutterHeightDexel;
    // RadiusDexel = other.RadiusDexel;
    // Min_X = other.Min_X;
    // Max_X = other.Max_X;
    // Min_Y = other.Min_Y;
    // Max_Y = other.Max_Y;

    return *this;
}

void DexelCutterSweptVolume::setScatterRange(const Posture& start)
{
    //TODO: 更改范围计算，将int改为使用float
    this->sweptVolmueParam.type = SweptVolmueParam::Type::Scatter;
    sweptVolmueParam.StartDexel = (start.center - offset) * sweptVolmueParam.scale;
    sweptVolmueParam.CutterRadiusDexel = this->cutter->getMaxRadius() * sweptVolmueParam.scale;
    sweptVolmueParam.CutterHeightDexel = this->cutter->getMaxHeight() * sweptVolmueParam.scale;

    for (int i = 0; i < 4; i++) {
        float X = sweptVolmueParam.StartDexel.x + (i % 2) * 2 * sweptVolmueParam.CutterRadiusDexel;
        float Y = sweptVolmueParam.StartDexel.y + (i / 2) * 2 * sweptVolmueParam.CutterRadiusDexel;
        this->sweptVolmueParam.Min_X = std::min(this->sweptVolmueParam.Min_X, X);
        this->sweptVolmueParam.Max_X = std::max(this->sweptVolmueParam.Max_X, X);
        this->sweptVolmueParam.Min_Y = std::min(this->sweptVolmueParam.Min_Y, Y);
        this->sweptVolmueParam.Max_Y = std::max(this->sweptVolmueParam.Max_Y, Y);
    }
}

void DexelCutterSweptVolume::setLineRange(const Posture& start, const Posture& end)
{
    this->sweptVolmueParam.type = SweptVolmueParam::Type::Line;
    this->startWithOffset = start;
    this->startWithOffset.center -= offset;
    this->endWithOffset = end;
    this->endWithOffset.center -= offset;

    //offset = { 0, 0, 0 }并且 scale = 1.0;
    this->sweptVolmueParam.StartDexel = (start.center - offset) * sweptVolmueParam.scale;
    this->sweptVolmueParam.EndDexel = (end.center - offset) * sweptVolmueParam.scale;
    this->sweptVolmueParam.Direction = (sweptVolmueParam.EndDexel - sweptVolmueParam.StartDexel).normalize();

    this->sweptVolmueParam.Normal = sweptVolmueParam.Direction.cross({ -sweptVolmueParam.StartDexel.y, sweptVolmueParam.StartDexel.x, 0 }).normalize();
    this->sweptVolmueParam.CutterRadiusDexel = this->cutter->getMaxRadius() * sweptVolmueParam.scale;
    this->sweptVolmueParam.CutterHeightDexel = this->cutter->getMaxHeight() * sweptVolmueParam.scale;
    // //输出两个端点
    // std::cout << "StartDexel: " << StartDexel << std::endl;
    // std::cout << "EndDexel: " << EndDexel << std::endl;
    //搜集投影到的Dexel的XY索引
    // std::set<std::pair<int, int>> dexelIndices;
    //获取startDexel和endDexel的最小和最大的XY索引，向上取整
    //以startDexe和endDexel中心，以刀具的直径为边长，可以得到两个正方形，
    //共有8个顶点，从这8个顶点中找到最小和最大的XY索引，向上取整
    float Origin_Start_X = sweptVolmueParam.StartDexel.x - sweptVolmueParam.CutterRadiusDexel;
    float Origin_Start_Y = sweptVolmueParam.StartDexel.y - sweptVolmueParam.CutterRadiusDexel;
    float Origin_End_X = sweptVolmueParam.EndDexel.x - sweptVolmueParam.CutterRadiusDexel;
    float Origin_End_Y = sweptVolmueParam.EndDexel.y - sweptVolmueParam.CutterRadiusDexel;

    this->sweptVolmueParam.Min_X = std::min(Origin_Start_X, Origin_End_X);
    this->sweptVolmueParam.Max_X = std::max(Origin_Start_X, Origin_End_X);
    this->sweptVolmueParam.Min_Y = std::min(Origin_Start_Y, Origin_End_Y);
    this->sweptVolmueParam.Max_Y = std::max(Origin_Start_Y, Origin_End_Y);

    for (int i = 0; i < 4; i++) {
        float Start_X = Origin_Start_X + (i % 2) * 2 * sweptVolmueParam.CutterRadiusDexel;
        float Start_Y = Origin_Start_Y + (i / 2) * 2 * sweptVolmueParam.CutterRadiusDexel;

        float End_X = Origin_End_X + (i % 2) * 2 * sweptVolmueParam.CutterRadiusDexel;
        float End_Y = Origin_End_Y + (i / 2) * 2 * sweptVolmueParam.CutterRadiusDexel;

        this->sweptVolmueParam.Min_X = std::min(sweptVolmueParam.Min_X, std::min(Start_X, End_X));
        this->sweptVolmueParam.Max_X = std::max(sweptVolmueParam.Max_X, std::max(Start_X, End_X));

        this->sweptVolmueParam.Min_Y = std::min(sweptVolmueParam.Min_Y, std::min(Start_Y, End_Y));
        this->sweptVolmueParam.Max_Y = std::max(sweptVolmueParam.Max_Y, std::max(Start_Y, End_Y));
    }

    // std::cout << "Min_X: " << sweptVolmueParam.Min_X << std::endl;
    // std::cout << "Max_X: " << sweptVolmueParam.Max_X << std::endl;
    // std::cout << "Min_Y: " << sweptVolmueParam.Min_Y << std::endl;
    // std::cout << "Max_Y: " << sweptVolmueParam.Max_Y << std::endl;
    // std::cout << "StartDexel" << sweptVolmueParam.StartDexel << std::endl;
    // std::cout << "EndDexel" << sweptVolmueParam.EndDexel << std::endl;
    // std::cout << "setLineRange end" << std::endl;
}

void DexelCutterSweptVolume::setArcRange(const Posture& start, const Posture& end, Plane plane, float radius, bool isClockwise)
{
    // std::cout << "setArcRange" << std::endl;
    //TODO: 更改范围计算，将int改为使用float
    this->sweptVolmueParam.type = SweptVolmueParam::Type::Arc;
    this->startWithOffset = start;
    this->startWithOffset.center -= offset;
    this->endWithOffset = end;
    this->endWithOffset.center -= offset;
    this->plane = plane;
    this->radius = radius;
    // this->isClockwise = isClockwise;

    sweptVolmueParam.StartDexel = (start.center - offset) * sweptVolmueParam.scale;
    sweptVolmueParam.EndDexel = (end.center - offset) * sweptVolmueParam.scale;
    // sweptVolmueParam.StartDexel = (end.center - offset) * sweptVolmueParam.scale;
    // sweptVolmueParam.EndDexel = (start.center - offset) * sweptVolmueParam.scale;

    sweptVolmueParam.Center = sweptVolmueParam.StartDexel + Vector3Df(-radius, 0, 0);

    sweptVolmueParam.Direction = (sweptVolmueParam.EndDexel - sweptVolmueParam.StartDexel).normalize();
    sweptVolmueParam.Normal = sweptVolmueParam.Direction.cross({ -sweptVolmueParam.StartDexel.y, sweptVolmueParam.StartDexel.x, 0 }).normalize();
    sweptVolmueParam.CutterRadiusDexel = this->cutter->getMaxRadius() * sweptVolmueParam.scale;
    sweptVolmueParam.CutterHeightDexel = this->cutter->getMaxHeight() * sweptVolmueParam.scale;
    sweptVolmueParam.RadiusDexel = radius * sweptVolmueParam.scale;

    Vector3Df SE = sweptVolmueParam.EndDexel - sweptVolmueParam.StartDexel;
    Vector3Df mideSE = (sweptVolmueParam.EndDexel + sweptVolmueParam.StartDexel) / 2.0f;
    float length = sqrt(sweptVolmueParam.RadiusDexel * sweptVolmueParam.RadiusDexel - pow(SE.length(), 2) / 4.0f);
    Vector3Df orthoSE, ArcCenter;

    if (isClockwise) {
        if (radius > 0) {
            // std::cout << "setArcRange"  << std::endl;
            orthoSE = Vector3Df(SE.y, SE.x * (-1.0f), SE.z);
        } else {
            orthoSE = Vector3Df(SE.y * (-1.0f), SE.x, SE.z);
        }
    } else {
        if (radius > 0) {
            orthoSE = Vector3Df(SE.y * (-1.0f), SE.x, SE.z);
        } else {
            orthoSE = Vector3Df(SE.y, SE.x * (-1.0f), SE.z);
        }
    }
    ArcCenter = mideSE + orthoSE * length / orthoSE.length(); //mideSE 的z值不变
    sweptVolmueParam.Center = ArcCenter;

    float Origin_Start_X = sweptVolmueParam.Center.x - std::abs(sweptVolmueParam.RadiusDexel) - sweptVolmueParam.CutterRadiusDexel;
    float Origin_Start_Y = sweptVolmueParam.Center.y - std::abs(sweptVolmueParam.RadiusDexel) - sweptVolmueParam.CutterRadiusDexel;
    float Origin_End_X = sweptVolmueParam.Center.x + std::abs(sweptVolmueParam.RadiusDexel) + sweptVolmueParam.CutterRadiusDexel;
    float Origin_End_Y = sweptVolmueParam.Center.y + std::abs(sweptVolmueParam.RadiusDexel) + sweptVolmueParam.CutterRadiusDexel;

    // this->sweptVolmueParam.Min_X = sweptVolmueParam.Center.x - 3;
    // this->sweptVolmueParam.Min_Y = sweptVolmueParam.Center.y - 3;
    // this->sweptVolmueParam.Max_X = sweptVolmueParam.Center.x + 3;
    // this->sweptVolmueParam.Max_Y = sweptVolmueParam.Center.y + 3;
    // return;
    this->sweptVolmueParam.Min_X = Origin_Start_X;
    this->sweptVolmueParam.Min_Y = Origin_Start_Y;
    this->sweptVolmueParam.Max_X = Origin_End_X;
    this->sweptVolmueParam.Max_Y = Origin_End_Y;
}

bool DexelCutterSweptVolume::GetCutterDexelRange(float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y)
{
    return this->cutter->getSweptVolume(this->sweptVolmueParam, Heigth, CutterPoint, Dexel_X, Dexel_Y);

    // std::cout<<"GetCutterDexelRange_start"<<std::endl;
    if (this->sweptVolmueParam.type == SweptVolmueParam::Type::Line) {
        float z;
        float distance;

        // if ((this->StartDexel - this->EndDexel).length() <= 0.00001) {
        //     CutterPoint = this->StartDexel;
        //     distance = (this->StartDexel - Vector3Df(Dexel_X, Dexel_Y, CutterPoint.z)).length();
        //     if (distance <= CutterRadiusDexel) {
        //         Heigth = CutterPoint.z + this->cutter->getDexelHeightZ(distance / scale) * scale;
        //         return true;
        //     } else {
        //         return false;
        //     }
        // }
        Vector3Df TempPoint = Vector3Df(Dexel_X, Dexel_Y, 0);
        Vector3Df SP, ES, EP, SE;
        SE = sweptVolmueParam.EndDexel - sweptVolmueParam.StartDexel;

        float sinDir = (sweptVolmueParam.EndDexel.z - sweptVolmueParam.StartDexel.z) / SE.length();
        float cosDir = sqrt(1.0f - sinDir * sinDir);

        float ScaleSE = ((sweptVolmueParam.CutterRadiusDexel / cosDir) * sinDir) / (SE.length());
        //将StartDexel向EndDexel移动CutterRadiusDexel * sinnDir
        Vector3Df CorrectStartDexel = sweptVolmueParam.StartDexel + SE * ScaleSE;
        Vector3Df CorrectEndDexel = sweptVolmueParam.EndDexel + SE * ScaleSE;

        SP = TempPoint - CorrectStartDexel;
        EP = TempPoint - CorrectEndDexel;
        SE = CorrectEndDexel - CorrectStartDexel;
        ES = CorrectStartDexel - CorrectEndDexel;
        SP.z = 0.0f;
        SE.z = 0.0f;
        EP.z = 0.0f;
        ES.z = 0.0f;

        if (SP.dot(SE) < 0.0f || (EP.dot(ES) < 0.0f)) {
            //点在线段之外
            if (SP.dot(SE) < 0.0f) {
                CutterPoint = sweptVolmueParam.StartDexel;
            } else {
                CutterPoint = sweptVolmueParam.EndDexel;
            }

            TempPoint.z = CutterPoint.z;
            distance = (TempPoint - CutterPoint).length();
            if (distance <= sweptVolmueParam.CutterRadiusDexel) {
                Heigth = CutterPoint.z + this->cutter->getDexelHeightZ(distance / sweptVolmueParam.scale) * sweptVolmueParam.scale;
                return true;
            }
        } else {
            //点在线段之内
            float v = SE.dot(SP) / SE.dot(SE);
            SE = sweptVolmueParam.EndDexel - sweptVolmueParam.StartDexel;
            // v = std::abs(v);,记得判断计算出的刀位点是否在线段内
            CutterPoint = sweptVolmueParam.StartDexel + SE * v; //要看示意图
            TempPoint.z = CutterPoint.z;
            distance = (TempPoint - CutterPoint).length();
            if (distance <= sweptVolmueParam.CutterRadiusDexel) {
                TempPoint.z = CutterPoint.z;
                distance = (TempPoint - CutterPoint).length();
                Heigth = CutterPoint.z + this->cutter->getDexelHeightZ(distance / sweptVolmueParam.scale) * sweptVolmueParam.scale;
                return true;
            }
        }
        return false;
        // // if (floatEqual(Normal.z, 0.0f)) {
        // //     z = StartDexel.z;
        // // }
        // if (std::abs(StartDexel.z - EndDexel.z) <= 0.00001f) {
        //     z = StartDexel.z;
        // } else {
        //     // z = (-Normal.x * (Dexel_X - StartDexel.x) - Normal.y * (Dexel_Y - StartDexel.y)) / Normal.z + StartDexel.z; //插值获取在line上的z值
        //     Vector3Df TempSE = EndDexel - StartDexel;
        //     TempSE.z = 0.0f;
        //     float v = TempSE.dot(Vector3Df(Dexel_X - StartDexel.x, Dexel_Y - StartDexel.y, 0.0f)) / TempSE.dot(TempSE);
        //     z = (EndDexel.z - StartDexel.z) * v + StartDexel.z;
        // }
        // distance = Vector3Df(Dexel_X, Dexel_Y, z).distanceToLineSegment(StartDexel, EndDexel);
        // //获取交点
        // if (distance <= CutterRadiusDexel) {
        //     Vector3Df TempPoint = Vector3Df(Dexel_X, Dexel_Y, 0.0f);
        //     getInterSectionPoint(StartDexel, EndDexel, TempPoint, CutterPoint);
        //     if (std::abs(std::abs(StartDexel.z - EndDexel.z) + std::abs(StartDexel.x - EndDexel.x) + std::abs(StartDexel.y - EndDexel.y)) <= 0.0001f) {
        //         CutterPoint = StartDexel;
        //         Heigth = CutterPoint.z + this->cutter->getDexelHeightZ(distance / scale) * scale;
        //         return true;
        //     } else {
        //         if ((CutterPoint - StartDexel).dot(CutterPoint - EndDexel) > 0) {
        //             if ((CutterPoint - StartDexel).length() > (CutterPoint - EndDexel).length()) {
        //                 CutterPoint = EndDexel;
        //             } else {
        //                 CutterPoint = StartDexel;
        //             }
        //             TempPoint.z = CutterPoint.z;
        //             distance = (CutterPoint - TempPoint).length();
        //             if (distance > CutterRadiusDexel) {
        //                 return false;
        //             }
        //         }
        //         Heigth = CutterPoint.z + this->cutter->getDexelHeightZ(distance / scale) * scale;
        //         // Heigth = z + this->cutter->getDexelHeightZ(distance / scale) * scale;
        //     }
        //     return true; //返回刀具的高度，别的交给铣削函数判断
        // } else {
        //     return false;
        // }

    } else if (this->sweptVolmueParam.type == SweptVolmueParam::Type::Arc) {
        // std::cout << "Type::Arc" << std::endl;
        float distance = Vector3Df(Dexel_X, Dexel_Y, sweptVolmueParam.StartDexel.z).distanceToArc(sweptVolmueParam.StartDexel, sweptVolmueParam.EndDexel, sweptVolmueParam.RadiusDexel, sweptVolmueParam.isClockwise);
        if (distance <= sweptVolmueParam.CutterRadiusDexel) {
            float minHeight = sweptVolmueParam.StartDexel.z + this->cutter->getDexelHeightZ(distance / sweptVolmueParam.scale) * sweptVolmueParam.scale;
            Heigth = minHeight;
            Vector3Df TempPoint = Vector3Df(Dexel_X, Dexel_Y, 0.0f);
            // getInterSectionPoint(StartDexel, EndDexel, TempPoint, CutterPoint);
            getArcInterSectionPoint(sweptVolmueParam.Center, sweptVolmueParam.RadiusDexel, TempPoint, CutterPoint);
            return true;
        } else {
            return false;
        }
    } else if (this->sweptVolmueParam.type == SweptVolmueParam::Type::Scatter) {
        // std::cout<<"Type::Scatter"<<std::endl;
        float distance = Vector3Df(Dexel_X, Dexel_Y, sweptVolmueParam.StartDexel.z).distanceToPoint(sweptVolmueParam.StartDexel);
        if (distance <= sweptVolmueParam.CutterRadiusDexel) {
            float minHeight = sweptVolmueParam.StartDexel.z + this->cutter->getDexelHeightZ(distance / sweptVolmueParam.scale) * sweptVolmueParam.scale;
            Heigth = minHeight;
            CutterPoint = sweptVolmueParam.StartDexel;
            return true;
        } else {
            return false;
        }
    }
    return false;
}