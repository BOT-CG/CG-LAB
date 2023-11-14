#pragma once
#include "AABB3D.h"
#include "BBox3D.h"
#include "CutterFactory.h"
// #include "GeometryMilling.h"
#include "CutterSweptVolume.h"
#include "OBB3D.h"
#include "Triangle3D.h"
#include "Vector3D.h"
#include "utils.h"
#include <execution>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>
#include <memory>
#include <stack>
#include <vector>

//Dexel数据结构
struct PowerDexel {
    float ChangedVolume = 0.0f; //体积
    bool IsChanged = false;
    float First = 0.0f;
    float Second = 0.0f;
    PowerDexel* Next = nullptr;
    Vector3Df CuttePoint = Vector3Df(10000.0f, 10000.0f, 10000.0f); 
    PowerDexel() = default;
    PowerDexel(float first, float second)
        : First(first)
        , Second(second)
    {
    }
    PowerDexel(const PowerDexel* dexel)
    {
        First = dexel->First;
        Second = dexel->Second;
        // Normal = dexel->Normal;
        PowerDexel* dexelNext = dexel->Next;
        PowerDexel* tempDexel = this;
        while (dexelNext != nullptr) {
            PowerDexel* newDexel = new PowerDexel(dexelNext->First, dexelNext->Second);
            tempDexel->Next = newDexel;
            tempDexel = newDexel;
            dexelNext = dexelNext->Next;
        }
    }

    float getMaxZ()
    {
        float maxZ = Second;
        PowerDexel* dexelNext = Next;
        while (dexelNext != nullptr) {
            if (dexelNext->Second > maxZ) {
                maxZ = dexelNext->Second;
            }
            dexelNext = dexelNext->Next;
        }
        return maxZ;
    }

    float getMinZ()
    {
        return First;
    }

    float getDexelVolume()
    {
        float volume = 0.0f;
        PowerDexel* dexelNext = this;
        while (dexelNext != nullptr) {
            volume += dexelNext->Second - dexelNext->First;
            dexelNext = dexelNext->Next;
        }
        return volume;
    }
};


struct TriangleDexel {
    std::vector<Vector3Df> TrianglePoints;
    std::vector<Vector3Df> CutterPoints; //保存与刀具相交时，刀具的坐标
    TriangleDexel* Next = nullptr;
    void deleteTriangleDexel()
    {
        TriangleDexel* temp = this;
        while (temp != nullptr) {
            TriangleDexel* tempNext = temp->Next;
            delete temp;
            temp = tempNext;
        }
    }
};

struct TriangleRenderDexel {
    int TriangleIndex = -1;
    bool IsAddSide = false;
    bool IsChanged = false;
    bool isDeleta = false;
    TriangleRenderDexel* Next = nullptr;
    void deleteTriangleDexel()
    {
        TriangleRenderDexel* temp = this;
        while (temp != nullptr) {
            TriangleRenderDexel* tempNext = temp->Next;
            delete temp;
            temp = tempNext;
        }
    }
};

class MRR_Dexel {

public:
    MRR_Dexel()
    {
        // DexelArraySize = 1024;
        //DexelArray.clear();
        DexelArray.resize(DexelArraySize);
        for (int i = 0; i < DexelArraySize; i++) {
            DexelArray[i].resize(DexelArraySize);
            for (int j = 0; j < DexelArraySize; j++) {
                DexelArray[i][j] = nullptr;
            }
        }
    };
    virtual ~MRR_Dexel() = default;

    Vector3Df getDexelCenter(int i, int j)
    {
        Vector3Df Center = Vector3Df(ABBox.getMin().x + UnitLength * i + UnitLength / 2, ABBox.getMin().y + UnitLength * j + UnitLength / 2, 0);
        return Center;
    }

    Vector3Df getStaticDexelCenter(int i, int j)
    {
        Vector3Df Center = Vector3Df(ABBox.getMin().x + StaticUnitLength * i + StaticUnitLength / 2, ABBox.getMin().y + StaticUnitLength * j + StaticUnitLength / 2, 0);
        return Center;
    }

    Vector3Df getDexelCenterWithCoordOffset(int i, int j)
    {
        Vector3Df Center = Vector3Df(ABBox.getMin().x + UnitLength * i + UnitLength / 2 - CoordOffset.x, ABBox.getMin().y + UnitLength * j + UnitLength / 2 - CoordOffset.y, 0);
        return Center;
    }

    void InitializeDexelArray(const AABB3Df& bbox)
    {
        IsBlankFile = false;
        ABBox = bbox;
        // DexeArraySize = ArraylSize;
        int ArraylSize = DexelArraySize;
        // DexelArray.resize(ArraylSize);
        for (int i = 0; i < ArraylSize; i++) {
            DexelArray[i].resize(ArraylSize);
            for (int j = 0; j < ArraylSize; j++) {
                DexelArray[i][j] = nullptr;
            }
        }
        //使用AABB3Df的Min点和Max点初始化DexelArray
        // Vector3Df Min = bbox.getMin();
        // Vector3Df Max = bbox.getMax();

        Vector3Df Size = bbox.getMax() - bbox.getMin();
        UnitLength = std::max(Size.x, Size.y) / float(ArraylSize);

        // DexelArray.resize(ArraylSize);

        std::for_each(std::execution::par, DexelArray.begin(), DexelArray.end(), [&](std::vector<PowerDexel*>& dexelArray) {
            int X = &dexelArray - &DexelArray[0];
            // dexelArray.resize(ArraylSize);
            std::for_each(std::execution::par, dexelArray.begin(), dexelArray.end(), [&](PowerDexel*& dexel) {
                // dexel = nullptr;
                int Y = &dexel - &dexelArray[0];
                if (UnitLength * X > Size.x || UnitLength * Y > Size.y) {
                    dexel = nullptr;
                    return;
                }
                dexel = new PowerDexel();
                dexel->First = bbox.getMin().z;
                dexel->Second = bbox.getMax().z;
                dexel->Next = nullptr;
            });
        });

        std::cout << "UnitLength: " << UnitLength << std::endl;
        std::cout << "ABBox_min: " << ABBox.getMin() << std::endl;
        std::cout << "ABBox_max: " << ABBox.getMax() << std::endl;
    }

    void InitializeDexelArray(const std::vector<Triangle3Df>& trianglesFromFile);

    void getXYFromOBB(const OBB3Df& obb, int& x1, int& x2, int& y1, int& y2)
    {
        Vector3Df center = obb.getCenter();
        // std::cout << "OBBcenter: " << center << std::endl;
        // std::cout << "MRR_CoordOffset" << CoordOffset << std::endl;
        // std::cout << "CenterWithCoordOffset" << center + CoordOffset << std::endl;
        // std::cout << "OBBDirection" << obb.getAxis(0) << obb.getAxis(1) << obb.getAxis(2) << std::endl;
        // std::cout << "OBBSize(" << obb.getHalfSize(0)<<"," << obb.getHalfSize(1)<<"," << obb.getHalfSize(2)<<")" << std::endl;
        //std::cout<<"ObbDirection"<<obb.getDirection(0)<<obb.getDirection(1)<<obb.getDirection(2)<<std::endl;

        //刀具位置存在偏移，其原点存在于中心点，所以需要加上偏移量
        float Min_x = center.x - obb.getHalfSize(0) + CoordOffset.x;
        float Min_y = center.y - obb.getHalfSize(1) + CoordOffset.y;

        float Max_x = center.x + obb.getHalfSize(0) + CoordOffset.x;
        float Max_y = center.y + obb.getHalfSize(1) + CoordOffset.y;

        // std::cout << "Min_x: " << Min_x << " Min_y: " << Min_y << " Max_x: " << Max_x << " Max_y: " << Max_y << std::endl;

        x1 = ceil((Min_x - ABBox.getMin().x) / UnitLength);
        x2 = ceil((Max_x - ABBox.getMin().x) / UnitLength);
        y1 = ceil((Min_y - ABBox.getMin().y) / UnitLength);
        y2 = ceil((Max_y - ABBox.getMin().y) / UnitLength);

        // std::cout << "x1: " << x1 << " x2: " << x2 << " y1: " << y1 << " y2: " << y2 << std::endl;
        x1 = std::max(0, x1);
        x2 = std::min(DexelArraySize - 1, x2);
        y1 = std::max(0, y1);
        y2 = std::min(DexelArraySize - 1, y2);
    }

    float CountDexelVolume(PowerDexel* dexel)
    {
        if (dexel == nullptr) {
            return 0;
        }
        return (dexel->Second - dexel->First) + CountDexelVolume(dexel->Next);
    }

    //需要执行测试
    PowerDexel* DexelBoolCalculation(PowerDexel* currNode, const float& min, const float& max)
    {
        if (currNode == nullptr) {
            return nullptr;
        }

        if (currNode->Second < min) {
            //当前Node保留
            //       -------
            //-----
            return currNode;
        }
        if (currNode->Second < min) {
            //当前Node保留
            // -----
            //          ------
            //下一个Node可能被铣削，对NextNode也执行布尔运算
            currNode->Next = DexelBoolCalculation(currNode->Next, min, max);
            return currNode;
        }
        if (currNode->First > min && currNode->Second < max) {
            //当前Node删除
            //    ------
            //--------------
            //对下一个Node执行布尔运算
            PowerDexel* tempRes = DexelBoolCalculation(currNode->Next, min, max);
            currNode->Next = nullptr; //断开当前Node的Next指针,避免delete的时候被一起删掉
            delete currNode; //删除当前Node
            return tempRes;
        }

        if (currNode->First >= min && currNode->First <= max && currNode->Second >= max) {
            //当前Node的前端缩短
            //      -------
            //  ------
            currNode->First = max;
            return currNode;
        }

        if (currNode->First <= min && currNode->Second >= max) {
            //当前Node分裂成两个Node
            //  ---------------
            //      ------
            PowerDexel* temp = new PowerDexel(max, currNode->Second);
            currNode->Second = min;

            temp->Next = currNode->Next;
            currNode->Next = temp;
            return currNode;
        }

        if (currNode->First <= min && currNode->Second >= min && currNode->Second <= max) {
            //当前Node的后端缩短
            // ------
            //    --------
            currNode->Second = min;
            //下一个Node可能也会被切到，对NextNode也执行布尔运算
            currNode->Next = DexelBoolCalculation(currNode->Next, min, max);
            return currNode;
        }

        return currNode;
    }

    void DeleteDexel(PowerDexel* dexel)
    {
        if (dexel == nullptr) {
            return;
        }
        DeleteDexel(dexel->Next);
        //std::cout<<"dexel->Next"<<dexel->Next<<std::endl;
        // dexel->Next = nullptr;
        delete dexel;
    }

    //使用CornerDexel对WhiteZoneDexel进行消除
    PowerDexel* DexelBoolGetWhiteZone(PowerDexel* WhiteZoneDexel, PowerDexel* CornerDexel)
    {
        if (WhiteZoneDexel == nullptr || CornerDexel == nullptr) {
            //白域不会被消除，返回其本身
            return WhiteZoneDexel;
        }
        // std::cout << "WhiteZoneDexel->First" << WhiteZoneDexel->First << "WhiteZoneDexel->Second" << WhiteZoneDexel->Second << std::endl;
        // float CornerDexelSegmentLength = CornerDexel->Second - CornerDexel->First;
        // float k = 0.10;
        /***********************************************************************************************/
        if (WhiteZoneDexel->Second < CornerDexel->First) {
            //WhiteZoneDexel   ------
            //CornerDexel                ------
            //该段白域被不与CornerDexel相交，判断白域的下一个节点
            WhiteZoneDexel->Next = DexelBoolGetWhiteZone(WhiteZoneDexel->Next, CornerDexel);
            return WhiteZoneDexel;
        }

        if (WhiteZoneDexel->First > CornerDexel->Second) {
            //WhiteZoneDexel           --------
            //CornerDexel     ------
            //白域不会被消除，返回其本身,判断下一个CornerDexel
            WhiteZoneDexel = DexelBoolGetWhiteZone(WhiteZoneDexel, CornerDexel->Next);
            return WhiteZoneDexel;
        }
        /***********************************************************************************************/
        //WhiteZoneDexel->First < CornerDexel->Second  && WhiteZoneDexel->Second > CornerDexel->First

        //WhiteZoneDexel->Second > CornerDexel->First
        //WhiteZoneDexel->First < CornerDexel->Second
        if ((WhiteZoneDexel->First >= CornerDexel->First)) {
            if (WhiteZoneDexel->Second <= CornerDexel->Second) {
                //WhiteZoneDexel     ------
                //CornerDexel     ------------
                //该段白域被该CornerDexel完全消除，判断白域的下一个节点
                // PowerDexel* tempNext = WhiteZoneDexel->Next;
                // WhiteZoneDexel->Next = nullptr;
                // delete WhiteZoneDexel;

                return DexelBoolGetWhiteZone(WhiteZoneDexel->Next, CornerDexel);
            } else {
                //WhiteZoneDexel->First < CornerDexel->Second
                //WhiteZoneDexel->First >= CornerDexel->First
                //WhiteZoneDexel->Second > CornerDexel->Second
                //WhiteZoneDexel     ------
                //CornerDexel     ------
                //该段白域被该CornerDexel部分消除，返回消除后的白域
                WhiteZoneDexel->First = CornerDexel->Second; //+ CornerDexelSegmentLength * k;
                //判断CornerDexel的下一个节点
                return DexelBoolGetWhiteZone(WhiteZoneDexel, CornerDexel->Next);
                return WhiteZoneDexel;
            }
        } else {
            //WhiteZoneDexel->Second > CornerDexel->First
            //WhiteZoneDexel->First <  CornerDexel->First
            if (WhiteZoneDexel->Second < CornerDexel->Second) {
                //WhiteZoneDexel   ------
                //CornerDexel          ------
                //该段白域被该CornerDexel部分消除，返回消除后的白域
                WhiteZoneDexel->Second = CornerDexel->First;
                //判读白域的下一个节点
                WhiteZoneDexel->Next = DexelBoolGetWhiteZone(WhiteZoneDexel->Next, CornerDexel);
                return WhiteZoneDexel;
            } else {
                //WhiteZoneDexel->First <  CornerDexel->First
                //WhiteZoneDexel->Second > CornerDexel->Second
                //WhiteZoneDexel   ----------
                //CornerDexel        ----
                //该段白域被该CornerDexel截断，新增一个白域，返回被处理后的白域
                PowerDexel* temp = new PowerDexel(CornerDexel->Second, WhiteZoneDexel->Second);
                WhiteZoneDexel->Second = CornerDexel->First;
                temp->Next = WhiteZoneDexel->Next; //链接新节点与下一个节点
                WhiteZoneDexel->Next = temp; //链接当前节点与新节点
                //判断CornerDexel的下一个节点
                WhiteZoneDexel->Next = DexelBoolGetWhiteZone(WhiteZoneDexel->Next, CornerDexel->Next);
                return WhiteZoneDexel;
            }
        }
    }

    PowerDexel* GetAntiDexel(PowerDexel* dexel)
    {
        if (dexel == nullptr) {
            return nullptr;
        }
        PowerDexel* result = new PowerDexel(-10000.0f, 10000.0f);
        PowerDexel* temp = result;
        while (dexel != nullptr) {
            temp->Next = new PowerDexel(dexel->Second, temp->Second);
            temp->Second = dexel->First;
            temp = temp->Next;
            dexel = dexel->Next;
        }

        return result;
    }

    PowerDexel* DexelBoolGetBlackZone(PowerDexel* BlackZoneDexel, PowerDexel* CornerDexel)
    {
        if (BlackZoneDexel == nullptr || CornerDexel == nullptr) {
            return BlackZoneDexel;
            // return nullptr;
        }
        //记得删除该Dexel*，节省空间
        PowerDexel* AntiCornerDexel = GetAntiDexel(CornerDexel);
        PowerDexel* AntiCornerDexel_temp = AntiCornerDexel;
        // std::cout << "AntiCornerDexel" << std::endl;
        // while (AntiCornerDexel_temp != nullptr) {
        //     std::cout << AntiCornerDexel_temp->First << " " << AntiCornerDexel_temp->Second << std::endl;
        //     AntiCornerDexel_temp = AntiCornerDexel_temp->Next;
        // }
        BlackZoneDexel = DexelBoolGetWhiteZone(BlackZoneDexel, AntiCornerDexel);
        DeleteDexel(AntiCornerDexel);
        return BlackZoneDexel;
    }

    //获取该Dexel在z1和z2之间的最小值
    float GetMinDexelPointFromGrayZone(PowerDexel* CornerDexel, const float& z1, const float& z2)
    {
        // std::cout << "GetDexelPointFromBottomGrayZone" << std::endl;
        PowerDexel* tempDexel = CornerDexel;
        float min = 10000.0f;

        while (tempDexel != nullptr) {
            //灰域与该段不相交
            if (tempDexel->Second < z1) {
                //ConerDexel -------
                //GrayZone            -------
                tempDexel = tempDexel->Next;
                continue;
            }

            if ((tempDexel->First < z1 && tempDexel->Second > z2) || (tempDexel->First > z2)) {
                //ConerDexel    --------------             ---------
                //GrayZone         -------        -------
                return min;
            }

            if (tempDexel->First >= z1 && tempDexel->First <= z2) {
                //first点在z1和z2之间，取最小值first返回
                //ConerDexel     -------         -------
                //GrayZone    -------------   -------
                min = std::min(min, tempDexel->First);
                return min;
            }

            if (tempDexel->First < z1 && tempDexel->Second >= z1 && tempDexel->Second <= z2) {
                //First点在z1和z2之外，second点在z1和z2之间，取最小值second返回
                //ConerDexel     ---------
                //GrayZone           ---------
                min = std::min(min, tempDexel->Second);
                return min;
            }

            tempDexel = tempDexel->Next;
        }
        return min;
    }

    //获取该Dexel在z1和z2之间的最小值
    float GetMaxDexelPointFromGrayZone(PowerDexel* CornerDexel, const float& z1, const float& z2)
    {

        // std::cout << "GetDexelPointFromTopGrayZone" << std::endl;
        PowerDexel* tempDexel = CornerDexel;
        float max = -10000.0f;

        while (tempDexel != nullptr) {
            if (z1 >= tempDexel->First && tempDexel->Second >= z1 && tempDexel->Second <= z2) {
                //second点在z1和z2之间，取最大值second,继续探索下一个节点(如果有)
                //ConerDexel   -------
                //GrayZone          -------
                max = std::max(max, tempDexel->Second);
            } else if (tempDexel->First >= z1 && tempDexel->First <= z2 && tempDexel->Second > z2) {
                //不需要在判断下一个节点了
                //ConerDexel        -------
                //GrayZone      -------
                max = std::max(max, tempDexel->First);
                return max;
            } else if (tempDexel->First >= z1 && tempDexel->Second < z2) {
                //ConerDexel         --------
                //GrayZone        --------------
                max = std::max(max, tempDexel->Second);
            } else if (tempDexel->First > z2) {
                //ConerDexel               ---------
                //GrayZone      ---------
                return max;
            } else if (tempDexel->Second < z1) {
                //ConerDexel   -------
                //GrayZone             -------
            } else if (tempDexel->First <= z1 && tempDexel->Second >= z2) {
                //ConerDexel   ---------------
                //GrayZone         -------
                return max;
            }

            tempDexel = tempDexel->Next;
        }

        return max;
    }

    void FillVectorAndNormal(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals)
    {
        Vector3Df Vertex1, Vertex2, Vertex3, Normal;
        Vector3Df Min = ABBox.getMin();
        Vector3Df Max = ABBox.getMax();
        float MaxLength = std::max(std::max(Max.x - Min.x, Max.y - Min.y), Max.z - Min.z);
        vertices.clear();
        normals.clear();
        for (int i = 0; i < DexelTriangles.size(); i = i + 3) {
            Vertex1 = DexelTriangles[i] / MaxLength;
            Vertex2 = DexelTriangles[i + 1] / MaxLength;
            Vertex3 = DexelTriangles[i + 2] / MaxLength;
            Normal = DexelTrianglesNormals[i];
            vertices.push_back(Vertex1);
            vertices.push_back(Vertex2);
            vertices.push_back(Vertex3);
            normals.push_back(Normal.normalize());
            normals.push_back(Normal.normalize());
            normals.push_back(Normal.normalize());
        }
    }

    void changeCoord()
    {

        Vector3Df Min = ABBox.getMin();
        Vector3Df Max = ABBox.getMax();
        float MaxLength = std::max(std::max(Max.x - Min.x, Max.y - Min.y), Max.z - Min.z);

        GLCoordSize = UnitLength / MaxLength;

        for (int i = 0; i < CubeCenter.size(); i++) {
            CubeCenter[i] = CubeCenter[i]; /// MaxLength;
            // std::cout<<"CubeCente: "<<CubeCenter[i]<<std::endl;
        }

        for (int i = 0; i < PointHeight.size(); i++) {
            PointHeight[i] = PointHeight[i]; // / MaxLength;
        }
        std::cout << "GLCoordSize: " << GLCoordSize << std::endl;
        std::cout << "CubeCenter.size():  " << CubeCenter.size() << std::endl;
        std::cout << "PointHeight.size():  " << PointHeight.size() << std::endl;
    }

    void TestDexel()
    {

        PowerDexel* Dexel_1 = new PowerDexel(1.0f, 4.0f);
        PowerDexel* Dexel_2 = new PowerDexel(2.0f, 3.0f);
        PowerDexel* Dexel_3 = new PowerDexel(1.5f, 3.5f);
        PowerDexel* Dexel_4 = new PowerDexel(0.8f, 3.7f);
        Dexel_1->Next = new PowerDexel(6.0f, 8.0f);
        Dexel_2->Next = new PowerDexel(5.5f, 7.5f);
        Dexel_3->Next = new PowerDexel(6.5f, 8.5f);
        Dexel_4->Next = new PowerDexel(6.1f, 7.4f);
        PowerDexel* WhiteZone = new PowerDexel(-1000.0f, 1000.0f);
        PowerDexel* BlackZone = new PowerDexel(Dexel_1);

        // Dexel_1->First = 4.75f;
        // Dexel_1->Second = 4.76f;
        // Dexel_1->Next
        // Dexel_2->First = 4.61f;

        std::cout << "BlackZone" << std::endl;
        PowerDexel* tempDexel = BlackZone;
        while (tempDexel != nullptr) {
            std::cout << tempDexel->First << " " << tempDexel->Second << std::endl;
            tempDexel = tempDexel->Next;
        }

        WhiteZone = DexelBoolGetWhiteZone(WhiteZone, Dexel_1);
        WhiteZone = DexelBoolGetWhiteZone(WhiteZone, Dexel_2);
        WhiteZone = DexelBoolGetWhiteZone(WhiteZone, Dexel_3);
        WhiteZone = DexelBoolGetWhiteZone(WhiteZone, Dexel_4);

        std::cout << "WhiteZone" << std::endl;
        tempDexel = WhiteZone;
        while (tempDexel != nullptr) {
            std::cout << tempDexel->First << " " << tempDexel->Second << std::endl;
            tempDexel = tempDexel->Next;
        }

        BlackZone = DexelBoolGetBlackZone(BlackZone, Dexel_1);
        BlackZone = DexelBoolGetBlackZone(BlackZone, Dexel_2);
        BlackZone = DexelBoolGetBlackZone(BlackZone, Dexel_3);
        BlackZone = DexelBoolGetBlackZone(BlackZone, Dexel_4);

        std::cout << "BlackZone_AfterBool" << std::endl;
        tempDexel = BlackZone;
        while (tempDexel != nullptr) {
            std::cout << tempDexel->First << " " << tempDexel->Second << std::endl;
            tempDexel = tempDexel->Next;
        }

        //先对白域进行适当扩展，再对黑域进行适当缩小,再获取灰域
        tempDexel = BlackZone;
        //对初始黑域进行适当缩小,对白域进行适当扩展
        float SegmentLength;
        while (tempDexel != nullptr) {
            SegmentLength = tempDexel->Second - tempDexel->First;
            tempDexel->First += (SegmentLength * 0.1);
            tempDexel->Second -= (SegmentLength * 0.1);
            tempDexel = tempDexel->Next;
        }
        //对白域的空档进行适当扩张
        tempDexel = WhiteZone;
        while (tempDexel->Next != nullptr) {
            SegmentLength = std::min(tempDexel->Next->First - tempDexel->Second, tempDexel->Next->Second - tempDexel->Next->First);
            SegmentLength = std::min(SegmentLength, tempDexel->Second - tempDexel->First);
            // SegmentLength = tempDexel->Next->First - tempDexel->Second;
            tempDexel->Second -= (SegmentLength * 0.1);
            tempDexel->Next->First += (SegmentLength * 0.1);
            tempDexel = tempDexel->Next;
        }
        PowerDexel* CornerDexel[4];
        CornerDexel[0] = Dexel_1;
        CornerDexel[1] = Dexel_2;
        CornerDexel[2] = Dexel_3;
        CornerDexel[3] = Dexel_4;
        //根据黑域和白域获取灰域并获取三角形
        tempDexel = BlackZone;
        //获取灰域
        float BottomGrayMin = WhiteZone->First;
        float BottomGrayMax = tempDexel->First;
        float TopGrayMin, TopGrayMax;

        PowerDexel* tempCornerDexel;
        Vector3Df CornerPoint_Z;
        TriangleDexel* TriDexel = new TriangleDexel();
        while (tempDexel != nullptr) {
            if (WhiteZone->Next == nullptr) {
                break;
            }
            if (tempDexel->Next != nullptr) {
                //黑域还未结束，必然存在下一个白域
                while (tempDexel->Next->Second < WhiteZone->Next->First) {
                    tempDexel = tempDexel->Next;
                }
                // if (tempDexel->Next->Second < WhiteZone->Next->First) {
                //     //tempDexel与tempDexel->Next只有灰域，没有白域，跳过该黑域，前往下一个黑域
                //     tempDexel = tempDexel->Next;
                //     continue;
                //     //否则，tempDexel与tempDexel->Next存在白域，可以在灰域中获取三角形
                // }
                //否则，tempDexel已经没有下一个顶点，它的下一个区域就是白域，可以在灰域中获取三角形顶点
            }
            //获取灰域
            TopGrayMin = tempDexel->Second;
            TopGrayMax = WhiteZone->Next->First;

            for (int i = 0; i < 4; i++) {

                tempCornerDexel = CornerDexel[i];

                if (tempCornerDexel == nullptr) {
                    continue;
                }
                CornerPoint_Z.z = GetMinDexelPointFromGrayZone(tempCornerDexel, BottomGrayMin, BottomGrayMax);
                if (std::abs(CornerPoint_Z.z) < 1000.0f) {
                    TriDexel->TrianglePoints.push_back(CornerPoint_Z);
                }
                CornerPoint_Z.z = GetMaxDexelPointFromGrayZone(tempCornerDexel, TopGrayMin, TopGrayMax);
                if (std::abs(CornerPoint_Z.z) < 1000.0f) {
                    TriDexel->TrianglePoints.push_back(CornerPoint_Z);
                }
            }

            if (tempDexel->Next != nullptr) {
                BottomGrayMin = WhiteZone->Next->Second;
                BottomGrayMax = tempDexel->Next->First;
                std::cout << "BottomGrayMin " << BottomGrayMin << " BottomGrayMax " << BottomGrayMax << std::endl;
            }

            tempDexel = tempDexel->Next;
            WhiteZone = WhiteZone->Next;
        }
        std::cout << "CornerPoint" << std::endl;
        for (int i = 0; i < (TriDexel->TrianglePoints.size()); i++) {
            std::cout << TriDexel->TrianglePoints[i].z << " " << std::endl;
        }

        std::cout << "Test_end" << std::endl;
    }

    void PrintDexel(PowerDexel* DexelHead)
    {
        PowerDexel* tempDexel = DexelHead;
        while (tempDexel != nullptr) {
            std::cout << " (" << tempDexel->First << " , " << tempDexel->Second << ")"
                      << "    ";
            tempDexel = tempDexel->Next;
        }
        std::cout << std::endl;
    }

    void InitDexelTriangleTest()
    {
        return;
        TriangleDexel TestTriangleDexel[3][3];
        std::vector<std::vector<TriangleDexel*>> TestTriangleDexelArray;
        TestTriangleDexelArray.resize(3);
        for (int i = 0; i < TestTriangleDexelArray.size(); i++) {
            TestTriangleDexelArray[i].resize(3);
        }
        std::cout << "IinitTestDexel_Begin" << std::endl;
        TestTriangleDexelArray[0][1] = new TriangleDexel();
        TestTriangleDexelArray[0][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -1.0f));
        TestTriangleDexelArray[0][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 1.0f));
        TestTriangleDexelArray[0][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -1.0f));
        TestTriangleDexelArray[0][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 1.0f));
        TestTriangleDexelArray[0][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -2.6f));
        TestTriangleDexelArray[0][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 2.6f));
        TestTriangleDexelArray[0][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -2.5f));
        TestTriangleDexelArray[0][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 2.5f));

        TestTriangleDexelArray[1][0] = new TriangleDexel();
        TestTriangleDexelArray[1][0]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -1.0f));
        TestTriangleDexelArray[1][0]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 1.0f));
        TestTriangleDexelArray[1][0]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -2.6f));
        TestTriangleDexelArray[1][0]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 2.6f));
        TestTriangleDexelArray[1][0]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -1.0f));
        TestTriangleDexelArray[1][0]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 1.0f));
        TestTriangleDexelArray[1][0]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -3.3f));
        TestTriangleDexelArray[1][0]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 3.3f));

        TestTriangleDexelArray[2][1] = new TriangleDexel();
        TestTriangleDexelArray[2][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -3.3f));
        TestTriangleDexelArray[2][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 3.3f));
        TestTriangleDexelArray[2][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -3.4f));
        TestTriangleDexelArray[2][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 3.4f));
        TestTriangleDexelArray[2][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -1.0f));
        TestTriangleDexelArray[2][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 1.0f));
        TestTriangleDexelArray[2][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -1.0f));
        TestTriangleDexelArray[2][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 1.0f));

        TestTriangleDexelArray[1][2] = new TriangleDexel();
        TestTriangleDexelArray[1][2]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -2.5f));
        TestTriangleDexelArray[1][2]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 2.5f));
        TestTriangleDexelArray[1][2]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -1.0f));
        TestTriangleDexelArray[1][2]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 1.0f));
        TestTriangleDexelArray[1][2]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -3.4f));
        TestTriangleDexelArray[1][2]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 3.4f));
        TestTriangleDexelArray[1][2]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -1.0f));
        TestTriangleDexelArray[1][2]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 1.0f));

        TestTriangleDexelArray[1][1] = new TriangleDexel();

        TestTriangleDexelArray[1][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -2.6f));
        TestTriangleDexelArray[1][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 2.6f));
        TestTriangleDexelArray[1][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -2.5f));
        TestTriangleDexelArray[1][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 2.5f));

        TestTriangleDexelArray[1][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -3.3f));
        TestTriangleDexelArray[1][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 3.3f));
        TestTriangleDexelArray[1][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, -3.4f));
        TestTriangleDexelArray[1][1]->TrianglePoints.push_back(Vector3Df(0.0f, 0.0f, 3.4f));

        std::cout << "IinitTestDexel_End" << std::endl;
        float TopZ_0, TopZ_1, BotZ_0, BotZ_1;
        float Nerighbor_TopZ_0, Nerighbor_TopZ_1, Nerighbor_BotZ_0, Nerighbor_BotZ_1;
        float Point_Offset = 0.0001; //偏差值可以由目标Dexel与邻居Dexel的距离决定
        int Neighbor_X, Neighbor_Y;
        //判断是否添加侧面面片
        //存在较大偏差则改为true
        int AddSide = 0;
        int X = 1;
        int Y = 1;

        TriangleDexel* tempTriDexel = TestTriangleDexelArray[X][Y];

        for (int i = 0; i < 4; i++) {
            std::cout << "i:" << i << std::endl;
            Neighbor_X = X + GetNeighbor[i][0];
            Neighbor_Y = Y + GetNeighbor[i][1];
            std::cout << "Neighbor_X: " << Neighbor_X << " Neighbor_Y: " << Neighbor_Y << std::endl;
            // if ((Neighbor_X < 0) || (Neighbor_X >= (DexeArraySize - 1)) || (Neighbor_Y < 0) || (Neighbor_Y >= (DexeArraySize - 1)) || (TriangleDexelArray[Neighbor_X][Neighbor_Y] == nullptr)) {
            //     //AddSide = true;
            //     AddSide = 0;
            //     break;
            // }

            TriangleDexel* NeighborDexel = TestTriangleDexelArray[Neighbor_X][Neighbor_Y];

            float length_0, length_1;
            BotZ_0 = tempTriDexel->TrianglePoints[NeighToSide[i][0] * 2].z;
            BotZ_1 = tempTriDexel->TrianglePoints[NeighToSide[i][1] * 2].z;
            TopZ_0 = tempTriDexel->TrianglePoints[NeighToSide[i][0] * 2 + 1].z;
            TopZ_1 = tempTriDexel->TrianglePoints[NeighToSide[i][1] * 2 + 1].z;
            std::cout << "Point_Z:" << BotZ_0 << " , " << BotZ_1 << " , " << TopZ_0 << ", " << TopZ_1 << std::endl;

            while (NeighborDexel != nullptr) {

                if (NeighborDexel->TrianglePoints.size() != 8) {
                    NeighborDexel = NeighborDexel->Next;
                    continue;
                }
                Nerighbor_BotZ_0 = NeighborDexel->TrianglePoints[SideToNeighborSide[i][0] * 2].z;
                Nerighbor_BotZ_1 = NeighborDexel->TrianglePoints[SideToNeighborSide[i][1] * 2].z;
                Nerighbor_TopZ_0 = NeighborDexel->TrianglePoints[SideToNeighborSide[i][0] * 2 + 1].z;
                Nerighbor_TopZ_1 = NeighborDexel->TrianglePoints[SideToNeighborSide[i][1] * 2 + 1].z;
                std::cout << "Neighbor_Z:" << Nerighbor_BotZ_0 << " , " << Nerighbor_BotZ_1 << " , " << Nerighbor_TopZ_0 << ", " << Nerighbor_TopZ_1 << std::endl;

                length_0 = std::abs(Nerighbor_TopZ_0 - Nerighbor_BotZ_0);
                length_1 = std::abs(Nerighbor_TopZ_1 - Nerighbor_BotZ_1);
                Nerighbor_BotZ_0 = Nerighbor_BotZ_0 - length_0 * 0.01;
                Nerighbor_BotZ_1 = Nerighbor_BotZ_1 - length_1 * 0.01;
                Nerighbor_TopZ_0 = Nerighbor_TopZ_0 + length_0 * 0.01;
                Nerighbor_TopZ_1 = Nerighbor_TopZ_1 + length_1 * 0.01;

                bool IsAddSide = ((Nerighbor_TopZ_0 >= TopZ_0) && (Nerighbor_TopZ_1 >= TopZ_1) && (Nerighbor_BotZ_0 <= BotZ_0) && (Nerighbor_BotZ_1 <= BotZ_1));
                if (IsAddSide) {
                    std::cout << "NO_Side" << std::endl;
                    AddSide++;
                    break;
                } else {
                    std::cout << "AddSide" << std::endl;
                }
                NeighborDexel = NeighborDexel->Next;
            }
        }
        std::cout << "AddSide: " << AddSide << std::endl;
        return;

        TriangleDexelArray.resize(DexelArraySize);
        for (int i = 0; i < TriangleDexelArray.size(); i++) {
            TriangleDexelArray[i].resize(DexelArraySize);
        }

        float halfUnitLength = UnitLength / 2.0f;

        for (int X = 0; X < DexelArraySize - 1; X++) {
            // std::cout << "X: " << X << std::endl;
            for (int Y = 0; Y < DexelArraySize - 1; Y++) {
                if (DexelArray[X][Y] == nullptr) {
                    continue;
                };

                TriangleDexel* TriDexel = new TriangleDexel();
                TriangleDexelArray[X][Y] = TriDexel;
                Vector3Df DexelCenter = getDexelCenter(X, Y);
                DexelCenter.x = DexelCenter.x - halfUnitLength;
                DexelCenter.y = DexelCenter.y - halfUnitLength;

                for (int i = 0; i < 8; i += 2) {
                    Vector3Df DexelPoint = DexelCenter;
                    DexelPoint.x = DexelPoint.x + VertexMaskFloat[i] * UnitLength;
                    DexelPoint.y = DexelPoint.y + VertexMaskFloat[i + 1] * UnitLength;
                    DexelPoint.z = DexelArray[X][Y]->First;
                    TriDexel->TrianglePoints.push_back(DexelPoint);
                    DexelPoint.z = DexelArray[X][Y]->Second;
                    TriDexel->TrianglePoints.push_back(DexelPoint);
                }
            }
        }

        for (int X = 0; X < (DexelArraySize - 1); X++) {

            for (int Y = 0; Y < (DexelArraySize - 1); Y++) {
                if (TriangleDexelArray[X][Y] == nullptr) {
                    continue;
                }

                if ((TriangleDexelArray[X][Y]->TrianglePoints.size() >= 8) && (TriangleDexelArray[X][Y]->TrianglePoints.size() % 8) == 0) {

                    //TriangleDexelArray[X][Y]->TriangleIndex = DexelTriangles.size();
                    for (int i = 0; i < TriangleDexelArray[X][Y]->TrianglePoints.size(); i = i + 8) {
                        Vector3Df Point1 = TriangleDexelArray[X][Y]->TrianglePoints[i + 0];
                        Vector3Df Point2 = TriangleDexelArray[X][Y]->TrianglePoints[i + 6];
                        Vector3Df Point3 = TriangleDexelArray[X][Y]->TrianglePoints[i + 2];
                        Vector3Df Point4 = TriangleDexelArray[X][Y]->TrianglePoints[i + 4];
                        Vector3Df Normal1 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                        float dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, -1.0f));
                        if (dot1 < 0.0f) {
                            Normal1 = -Normal1;
                        }

                        DexelTriangles.push_back(Point3);
                        DexelTriangles.push_back(Point2);
                        DexelTriangles.push_back(Point1);
                        DexelTriangles.push_back(Point2);
                        DexelTriangles.push_back(Point4);
                        DexelTriangles.push_back(Point1);
                        for (int i = 0; i < 6; i++) {
                            DexelTrianglesNormals.push_back(Normal1 * (1.0f));
                        }

                        Point1 = TriangleDexelArray[X][Y]->TrianglePoints[i + 1];
                        Point2 = TriangleDexelArray[X][Y]->TrianglePoints[i + 7];
                        Point3 = TriangleDexelArray[X][Y]->TrianglePoints[i + 3];
                        Point4 = TriangleDexelArray[X][Y]->TrianglePoints[i + 5];

                        Normal1 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                        dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, 1.0f));
                        if (dot1 < 0.0f) {
                            Normal1 = -Normal1;
                        }

                        DexelTriangles.push_back(Point1);
                        DexelTriangles.push_back(Point2);
                        DexelTriangles.push_back(Point3);
                        DexelTriangles.push_back(Point1);
                        DexelTriangles.push_back(Point4);
                        DexelTriangles.push_back(Point2);
                        for (int i = 0; i < 6; i++) {
                            DexelTrianglesNormals.push_back(Normal1);
                        }
                    }
                }
            }
        }
    }

    void updateVerticesAndNormals(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals);

    bool CountTriDexelIsSide(int X, int Y, TriangleDexel* tempTriDexel)
    {
        float TopZ_0, TopZ_1, BotZ_0, BotZ_1;
        float Nerighbor_TopZ_0, Nerighbor_TopZ_1, Nerighbor_BotZ_0, Nerighbor_BotZ_1;
        float Point_Offset = 0.0001; //偏差值可以由目标Dexel与邻居Dexel的距离决定
        int Neighbor_X, Neighbor_Y;
        int AddSide = 0; //判断是否添加侧面面片//存在较大偏差则改为true

        //遍历四个邻居Dexel
        for (int i = 0; i < 4; i++) {
            Neighbor_X = X + GetNeighbor[i][0];
            Neighbor_Y = Y + GetNeighbor[i][1];

            if ((Neighbor_X < 0) || (Neighbor_X >= (DexelArraySize - 1)) || (Neighbor_Y < 0) || (Neighbor_Y >= (DexelArraySize - 1)) || (TriangleDexelArray[Neighbor_X][Neighbor_Y] == nullptr)) {
                AddSide = 0;
                break;
            }
            TriangleDexel* NeighborDexel = TriangleDexelArray[Neighbor_X][Neighbor_Y];
            // NeighborDexel->TrianglePoint可能有多段柱体，碰到存在多段柱体时，要一起判断该侧面是否被遮掩
            float length_0, length_1;
            BotZ_0 = tempTriDexel->TrianglePoints[NeighToSide[i][0] * 2].z;
            BotZ_1 = tempTriDexel->TrianglePoints[NeighToSide[i][1] * 2].z;
            TopZ_0 = tempTriDexel->TrianglePoints[NeighToSide[i][0] * 2 + 1].z;
            TopZ_1 = tempTriDexel->TrianglePoints[NeighToSide[i][1] * 2 + 1].z;
            while (NeighborDexel != nullptr) {

                if (NeighborDexel->TrianglePoints.size() != 8) {
                    NeighborDexel = NeighborDexel->Next;
                    continue;
                }
                Nerighbor_BotZ_0 = NeighborDexel->TrianglePoints[SideToNeighborSide[i][0] * 2].z;
                Nerighbor_BotZ_1 = NeighborDexel->TrianglePoints[SideToNeighborSide[i][1] * 2].z;
                Nerighbor_TopZ_0 = NeighborDexel->TrianglePoints[SideToNeighborSide[i][0] * 2 + 1].z;
                Nerighbor_TopZ_1 = NeighborDexel->TrianglePoints[SideToNeighborSide[i][1] * 2 + 1].z;
                length_0 = std::abs(Nerighbor_TopZ_0 - Nerighbor_BotZ_0);
                length_1 = std::abs(Nerighbor_TopZ_1 - Nerighbor_BotZ_1);
                //将其扩大
                Nerighbor_BotZ_0 = Nerighbor_BotZ_0 - length_0 * 0.01;
                Nerighbor_BotZ_1 = Nerighbor_BotZ_1 - length_1 * 0.01;
                Nerighbor_TopZ_0 = Nerighbor_TopZ_0 + length_0 * 0.01;
                Nerighbor_TopZ_1 = Nerighbor_TopZ_1 + length_1 * 0.01;
                //应该给一个偏差值，当偏差值小于某个值时，认为该侧面被遮掩 ->Point_Offset
                bool IsAddSide = ((Nerighbor_TopZ_0 >= TopZ_0) && (Nerighbor_TopZ_1 >= TopZ_1) && (Nerighbor_BotZ_0 <= BotZ_0) && (Nerighbor_BotZ_1 <= BotZ_1));
                if (IsAddSide) {
                    //该侧面被遮掩
                    AddSide++;
                    break;
                }
                NeighborDexel = NeighborDexel->Next;
            }
        }

        if (AddSide < 4) //未被完全遮掩，添加侧面
            return true;
        else
            return false;
    }

    void GetNeigthborTriDexelPoint_new(std::vector<Vector3Df>& NeigthborPoints, int X, int Y, TriangleDexel* tempTriDexel)
    {
        PowerDexel* CornerDexel;
        int NeighborX, NeighborY, NeighborPointIndex, PointIndex;
        float TopZ, BotZ, Neighbor_TopZ, Neighbor_BotZ, Neighbor_length;
        bool CoverFlag = false;
        TriangleDexel* CornerNeighborTriDexel;
        Vector3Df tempVector;
        //根据目标点周围是否有面片来判断
        for (int NeighborIndex = 0; NeighborIndex < 4; NeighborIndex++) {
            CoverFlag = false;

            NeighborX = X + GetCornerNeighbor[NeighborIndex][0];
            NeighborY = Y + GetCornerNeighbor[NeighborIndex][1];
            if ((NeighborX < 0) || (NeighborX >= (DexelArraySize - 1)) || (NeighborY < 0) || (NeighborY >= (DexelArraySize - 1))) {
                continue;
            }
            if ((TriangleDexelArray[NeighborX][NeighborY] == nullptr)) {
                //该邻居Dexel为空，添加其顶点
                tempVector = getDexelCenter(NeighborX, NeighborY);
                NeigthborPoints.push_back(tempVector);
                continue;
            }

            NeighborPointIndex = CornerNeighborToCornerDexel[NeighborIndex][0] * 2;
            PointIndex = CornerNeighborToCornerDexel[NeighborIndex][1] * 2;

            BotZ = tempTriDexel->TrianglePoints[PointIndex].z;
            TopZ = tempTriDexel->TrianglePoints[PointIndex + 1].z;
            CornerNeighborTriDexel = TriangleDexelArray[NeighborX][NeighborX];
            //Neighbor可能有多段
            while (CornerNeighborTriDexel != nullptr) {

                if (CornerNeighborTriDexel->TrianglePoints.size() != 8) {
                    CornerNeighborTriDexel = CornerNeighborTriDexel->Next;
                    continue;
                }

                Neighbor_BotZ = CornerNeighborTriDexel->TrianglePoints[NeighborPointIndex].z;
                Neighbor_TopZ = CornerNeighborTriDexel->TrianglePoints[NeighborPointIndex + 1].z;
                Neighbor_length = std::abs(Neighbor_TopZ - Neighbor_BotZ);

                Neighbor_BotZ = Neighbor_BotZ - Neighbor_length * 0.01;
                Neighbor_TopZ = Neighbor_TopZ + Neighbor_length * 0.01;

                CoverFlag = (BotZ > Neighbor_BotZ) && (TopZ < Neighbor_TopZ);
                if (CoverFlag) {
                    break;
                }
                CornerNeighborTriDexel = CornerNeighborTriDexel->Next;
            }

            if (!CoverFlag) {
                //该邻居Dexel不参与遮掩目标dexel，添加其顶点
                tempVector = getDexelCenter(NeighborX, NeighborY);
                NeigthborPoints.push_back(tempVector);
            }
        }

        float TopZ_0, TopZ_1, BotZ_0, BotZ_1;
        float Nerighbor_TopZ_0, Nerighbor_TopZ_1, Nerighbor_BotZ_0, Nerighbor_BotZ_1;
        int Neighbor_X, Neighbor_Y;

        //遍历四个邻居Dexel,判断四个侧面是否需要添加侧面面片
        for (int i = 0; i < 4; i++) {
            CoverFlag = false;
            Neighbor_X = X + GetNeighbor[i][0];
            Neighbor_Y = Y + GetNeighbor[i][1];
            if ((Neighbor_X < 0) || (Neighbor_X >= (DexelArraySize - 1)) || (Neighbor_Y < 0) || (Neighbor_Y >= (DexelArraySize - 1)) || (TriangleDexelArray[Neighbor_X][Neighbor_Y] == nullptr)) {
                continue;
            }
            TriangleDexel* NeighborDexel = TriangleDexelArray[Neighbor_X][Neighbor_Y];
            // NeighborDexel->TrianglePoint可能有多段柱体，碰到存在多段柱体时，要一起判断该侧面是否被遮掩
            float length_0, length_1;
            BotZ_0 = tempTriDexel->TrianglePoints[NeighToSide[i][0] * 2].z;
            BotZ_1 = tempTriDexel->TrianglePoints[NeighToSide[i][1] * 2].z;
            TopZ_0 = tempTriDexel->TrianglePoints[NeighToSide[i][0] * 2 + 1].z;
            TopZ_1 = tempTriDexel->TrianglePoints[NeighToSide[i][1] * 2 + 1].z;
            while (NeighborDexel != nullptr) {

                if (NeighborDexel->TrianglePoints.size() != 8) {
                    NeighborDexel = NeighborDexel->Next;
                    continue;
                }
                Nerighbor_BotZ_0 = NeighborDexel->TrianglePoints[SideToNeighborSide[i][0] * 2].z;
                Nerighbor_BotZ_1 = NeighborDexel->TrianglePoints[SideToNeighborSide[i][1] * 2].z;
                Nerighbor_TopZ_0 = NeighborDexel->TrianglePoints[SideToNeighborSide[i][0] * 2 + 1].z;
                Nerighbor_TopZ_1 = NeighborDexel->TrianglePoints[SideToNeighborSide[i][1] * 2 + 1].z;
                length_0 = std::abs(Nerighbor_TopZ_0 - Nerighbor_BotZ_0);
                length_1 = std::abs(Nerighbor_TopZ_1 - Nerighbor_BotZ_1);
                //将其扩大
                Nerighbor_BotZ_0 = Nerighbor_BotZ_0 - length_0 * 0.01;
                Nerighbor_BotZ_1 = Nerighbor_BotZ_1 - length_1 * 0.01;
                Nerighbor_TopZ_0 = Nerighbor_TopZ_0 + length_0 * 0.01;
                Nerighbor_TopZ_1 = Nerighbor_TopZ_1 + length_1 * 0.01;
                //应该给一个偏差值，当偏差值小于某个值时，认为该侧面被遮掩 ->Point_Offset
                CoverFlag = ((Nerighbor_TopZ_0 >= TopZ_0) && (Nerighbor_TopZ_1 >= TopZ_1) && (Nerighbor_BotZ_0 <= BotZ_0) && (Nerighbor_BotZ_1 <= BotZ_1));
                if (CoverFlag) {
                    break;
                }
                NeighborDexel = NeighborDexel->Next;
            }

            if (!CoverFlag) {
                //该邻居Dexel不参与遮掩目标dexel，添加其顶点
                tempVector = getDexelCenter(NeighborX, NeighborY);
                NeigthborPoints.push_back(tempVector);
            }
        }
    }

    void GetNeigthborTriDexelPoint(std::vector<Vector3Df>& NeigthborPoints, int X, int Y, TriangleDexel* tempTriDexel)
    {
        PowerDexel* CornerDexel;
        int NeighborX, NeighborY, NeighborPointIndex, PointIndex;
        float TopZ, BotZ, Neighbor_TopZ, Neighbor_BotZ, Neighbor_length;
        bool CoverFlag = false;
        TriangleDexel* CornerNeighborTriDexel;
        Vector3Df tempVector;

        for (int NeighborIndex = X - 2; NeighborIndex <= X + 2; NeighborIndex++) {
            CoverFlag = false;

            NeighborX = X + GetCornerNeighbor[NeighborIndex][0];
            NeighborY = Y + GetCornerNeighbor[NeighborIndex][1];

            if ((NeighborX < 0) || (NeighborX >= (DexelArraySize - 1)) || (NeighborY < 0) || (NeighborY >= (DexelArraySize - 1)) || (TriangleDexelArray[NeighborX][NeighborY] == nullptr)) {
                continue;
            }

            NeighborPointIndex = CornerNeighborToCornerDexel[NeighborIndex][0] * 2;
            PointIndex = CornerNeighborToCornerDexel[NeighborIndex][1] * 2;

            BotZ = tempTriDexel->TrianglePoints[PointIndex].z;
            TopZ = tempTriDexel->TrianglePoints[PointIndex + 1].z;

            CornerNeighborTriDexel = TriangleDexelArray[NeighborX][NeighborX];
            //Neighbor可能有多段
            while (CornerNeighborTriDexel != nullptr) {

                if (CornerNeighborTriDexel->TrianglePoints.size() != 8) {
                    CornerNeighborTriDexel = CornerNeighborTriDexel->Next;
                    continue;
                }

                Neighbor_BotZ = CornerNeighborTriDexel->TrianglePoints[NeighborPointIndex].z;
                Neighbor_TopZ = CornerNeighborTriDexel->TrianglePoints[NeighborPointIndex + 1].z;
                Neighbor_length = std::abs(Neighbor_TopZ - Neighbor_BotZ);

                Neighbor_BotZ = Neighbor_BotZ - Neighbor_length * 0.01;
                Neighbor_TopZ = Neighbor_TopZ + Neighbor_length * 0.01;

                CoverFlag = (BotZ > Neighbor_BotZ) && (TopZ < Neighbor_TopZ);
                if (CoverFlag) {
                    break;
                }
                CornerNeighborTriDexel = CornerNeighborTriDexel->Next;
            }

            // if (CoverFlag) {
            //     //到时候只取XY值即可
            //     //判断该TriDexel是否为需要添加面片的段
            //     if (CountTriDexelIsSide(NeighborX, NeighborY, CornerNeighborTriDexel)) {
            //         tempVector = CornerNeighborTriDexel->TrianglePoints[0];
            //         tempVector.z = 0;
            //         // tempVector = getDexelCenter(NeighborX, NeighborY);
            //         NeigthborPoints.push_back(tempVector);
            //     }
            // }

            if (!CoverFlag) {
                //该邻居Dexel不参与遮掩目标dexel，添加其顶点
                tempVector = getDexelCenter(NeighborX, NeighborY);
                NeigthborPoints.push_back(tempVector);
            }
        }

        return;

        float TopZ_0, TopZ_1, BotZ_0, BotZ_1;
        float Nerighbor_TopZ_0, Nerighbor_TopZ_1, Nerighbor_BotZ_0, Nerighbor_BotZ_1;
        int Neighbor_X, Neighbor_Y;

        //遍历四个邻居Dexel,判断四个侧面是否需要添加侧面面片
        for (int i = 0; i < 4; i++) {
            Neighbor_X = X + GetNeighbor[i][0];
            Neighbor_Y = Y + GetNeighbor[i][1];
            if ((Neighbor_X < 0) || (Neighbor_X >= (DexelArraySize - 1)) || (Neighbor_Y < 0) || (Neighbor_Y >= (DexelArraySize - 1)) || (TriangleDexelArray[Neighbor_X][Neighbor_Y] == nullptr)) {
                continue;
            }
            TriangleDexel* NeighborDexel = TriangleDexelArray[Neighbor_X][Neighbor_Y];
            // NeighborDexel->TrianglePoint可能有多段柱体，碰到存在多段柱体时，要一起判断该侧面是否被遮掩
            float length_0, length_1;
            BotZ_0 = tempTriDexel->TrianglePoints[NeighToSide[i][0] * 2].z;
            BotZ_1 = tempTriDexel->TrianglePoints[NeighToSide[i][1] * 2].z;
            TopZ_0 = tempTriDexel->TrianglePoints[NeighToSide[i][0] * 2 + 1].z;
            TopZ_1 = tempTriDexel->TrianglePoints[NeighToSide[i][1] * 2 + 1].z;
            while (NeighborDexel != nullptr) {

                if (NeighborDexel->TrianglePoints.size() != 8) {
                    NeighborDexel = NeighborDexel->Next;
                    continue;
                }
                Nerighbor_BotZ_0 = NeighborDexel->TrianglePoints[SideToNeighborSide[i][0] * 2].z;
                Nerighbor_BotZ_1 = NeighborDexel->TrianglePoints[SideToNeighborSide[i][1] * 2].z;
                Nerighbor_TopZ_0 = NeighborDexel->TrianglePoints[SideToNeighborSide[i][0] * 2 + 1].z;
                Nerighbor_TopZ_1 = NeighborDexel->TrianglePoints[SideToNeighborSide[i][1] * 2 + 1].z;
                length_0 = std::abs(Nerighbor_TopZ_0 - Nerighbor_BotZ_0);
                length_1 = std::abs(Nerighbor_TopZ_1 - Nerighbor_BotZ_1);
                //将其扩大
                Nerighbor_BotZ_0 = Nerighbor_BotZ_0 - length_0 * 0.01;
                Nerighbor_BotZ_1 = Nerighbor_BotZ_1 - length_1 * 0.01;
                Nerighbor_TopZ_0 = Nerighbor_TopZ_0 + length_0 * 0.01;
                Nerighbor_TopZ_1 = Nerighbor_TopZ_1 + length_1 * 0.01;
                //应该给一个偏差值，当偏差值小于某个值时，认为该侧面被遮掩 ->Point_Offset
                bool IsAddSide = ((Nerighbor_TopZ_0 >= TopZ_0) && (Nerighbor_TopZ_1 >= TopZ_1) && (Nerighbor_BotZ_0 <= BotZ_0) && (Nerighbor_BotZ_1 <= BotZ_1));
                if (IsAddSide) {
                    //已经获得覆盖目标Tri的邻居Dexel段，接下来需要判断它是否为需要添加侧面的那段
                    //邻居Dexel的该段与当前Dexel的该段相邻，判断邻居Dexel的该段与是否为需要添加侧面面片的部分
                    if (CountTriDexelIsSide(Neighbor_X, Neighbor_Y, NeighborDexel)) {
                        tempVector = NeighborDexel->TrianglePoints[0];
                        tempVector.z = 0;
                        // tempVector = getDexelCenter(NeighborX, NeighborY);
                        NeigthborPoints.push_back(tempVector);
                    }
                    break;
                }
                NeighborDexel = NeighborDexel->Next;
            }
        }
    }

    //用不到
    void InitDexelTriangle_Delta();
    //用不到
    void GetDeltaDexelTriangle(int X, int Y, TriangleDexel* TriDexel, TriangleRenderDexel* TriRenderDexel, std::vector<Vector3Df>& TrianglePoint)
    {

        //初始化白域和黑域
        //先判断A情况，如果A不行，则判断C情况，如果C不行，则判断B情况，如果B不行，则判断D情况
        //期间任意一种情况成功即可退出
        PowerDexel* White;
        PowerDexel* Black;
        int DexelIndex_X, DexelIndex_Y;
        PowerDexel* tempDexel = nullptr;

        for (int m = 0; m < 4; m++) {

            TrianglePoint.clear();
            White = new PowerDexel(-1000.0f, 1000.0f);
            Black = nullptr;
            tempDexel = nullptr;

            for (int n = 0; n < 3; n++) {
                int VertexMaskIndex = DelteTypeToVertexIndex[m][n] * 2;
                DexelIndex_X = X + VertexMask[VertexMaskIndex];
                DexelIndex_Y = Y + VertexMask[VertexMaskIndex + 1];
                tempDexel = DexelArray[DexelIndex_X][DexelIndex_Y];
                if (tempDexel == nullptr) {
                    DeleteDexel(Black);
                    DeleteDexel(White);
                    Black = nullptr;
                    White = nullptr;
                    break; //不成立，直接判断下一种情况类型,并清空白域和黑域
                }
                if (Black == nullptr && tempDexel != nullptr) {
                    Black = new PowerDexel(tempDexel); //初始化黑域
                }

                if (Black != nullptr && tempDexel != nullptr) {
                    //获取白域
                    White = DexelBoolGetWhiteZone(White, DexelArray[DexelIndex_X][DexelIndex_Y]);
                    //获取黑域
                    Black = DexelBoolGetBlackZone(Black, DexelArray[DexelIndex_X][DexelIndex_Y]);
                }
            }

            if (White == nullptr || Black == nullptr) {
                continue;
            }

            //先对白域进行适当扩展，再对黑域进行适当缩小,再获取灰域
            tempDexel = Black;
            //对初始黑域进行适当缩小,对白域进行适当扩展
            float SegmentLength;
            while (tempDexel != nullptr) {
                SegmentLength = tempDexel->Second - tempDexel->First;
                tempDexel->First += (SegmentLength * 0.1);
                tempDexel->Second -= (SegmentLength * 0.1);
                tempDexel = tempDexel->Next;
            }
            //对白域的空档进行适当扩张
            tempDexel = White;
            while (tempDexel->Next != nullptr) {
                SegmentLength = std::min(tempDexel->Next->First - tempDexel->Second, tempDexel->Next->Second - tempDexel->Next->First);
                SegmentLength = std::min(SegmentLength, tempDexel->Second - tempDexel->First);
                SegmentLength = std::abs(SegmentLength);
                tempDexel->Second -= (SegmentLength * 0.1);
                tempDexel->Next->First += (SegmentLength * 0.1);
                tempDexel = tempDexel->Next;
            }
            //根据黑域和白域获取灰域并获取三角形
            tempDexel = Black;
            float BottomGrayMin = White->Second;
            float BottomGrayMax = tempDexel->First;
            float TopGrayMin, TopGrayMax;
            PowerDexel* tempCornerDexel = nullptr;
            Vector3Df CornerPoint;
            PowerDexel* tempWhite = White;

            while (tempDexel != nullptr) {
                if (White->Next == nullptr) {
                    break;
                }

                if (tempDexel->Next != nullptr) {
                    //黑域还未结束，必然存在下一个白域
                    if (tempDexel->Next->Second < White->Next->First) {
                        tempDexel = tempDexel->Next;
                        continue;
                        //否则，tempDexel与tempDexel->Next存在白域，可以在灰域中获取三角形
                    }
                    //否则，tempDexel已经没有下一个顶点，它的下一个区域就是白域，可以在灰域中获取三角形顶点
                }
                //获取灰域
                TopGrayMin = tempDexel->Second;
                TopGrayMax = White->Next->First;
                //遍历3个顶点，获取三角形的顶点
                for (int n = 0; n < 3; n++) {

                    int VertexMaskIndex = DelteTypeToVertexIndex[m][n] * 2;
                    DexelIndex_X = X + VertexMask[VertexMaskIndex];
                    DexelIndex_Y = Y + VertexMask[VertexMaskIndex + 1];
                    tempCornerDexel = DexelArray[DexelIndex_X][DexelIndex_Y];

                    if (tempCornerDexel == nullptr) {
                        continue;
                    }

                    CornerPoint = getDexelCenter(DexelIndex_X, DexelIndex_Y);
                    CornerPoint.z = GetMinDexelPointFromGrayZone(tempCornerDexel, BottomGrayMin, BottomGrayMax);
                    if (std::abs(CornerPoint.z) < 1000.0f) {
                        TrianglePoint.push_back(CornerPoint);
                    }
                    CornerPoint.z = GetMaxDexelPointFromGrayZone(tempCornerDexel, TopGrayMin, TopGrayMax);
                    if (std::abs(CornerPoint.z) < 1000.0f) {
                        TrianglePoint.push_back(CornerPoint);
                    }
                }

                if (tempDexel->Next != nullptr) {
                    BottomGrayMin = White->Next->Second;
                    BottomGrayMax = tempDexel->Next->First;
                }

                tempDexel = tempDexel->Next;
                White = White->Next;
            }

            if (TrianglePoint.size() < 6 || TrianglePoint.size() % 6 != 0) {
                continue;
            }

            if (TrianglePoint.size() >= 6 && TrianglePoint.size() % 6 == 0) {
                TriRenderDexel->isDeleta = true;
                for (int i = 0; i < TrianglePoint.size(); i = i + 6) {
                    for (int j = 0; j < 6; j++) {
                        TriDexel->TrianglePoints.push_back(TrianglePoint[i + j]);
                    }
                    if ((i + 6) < TrianglePoint.size()) {
                        TriDexel->Next = new TriangleDexel();
                        TriRenderDexel->Next = new TriangleRenderDexel();
                        TriDexel = TriDexel->Next;
                        TriRenderDexel = TriRenderDexel->Next;
                    }
                }
            }

            DeleteDexel(Black);
            DeleteDexel(tempWhite);
        }
    }

    void SetLastWorkPieceVolume(double Volume)
    {
        LastWorkpieceVolume = Volume;
    }

    double getWorkPieceVolume()
    {
        return WorkpieceVolume;
    }

    void GetPatchVertices()
    {
        // PointHeight.clear();
        // CubeCenter.clear();
        // CubeNormals.clear();
        std::cout << "cutter->getMaxRadius(): " << cutter->getMaxRadius() << std::endl;
        std::cout << "cutter->getMaxZ(): " << cutter->getDexelHeightZ(cutter->getMaxRadius() / 2.0f) << std::endl;

        std::cout << "BeginGetPatchVertices" << std::endl;
        RenderStatic = true;
        PatchVertices.clear();
        PatchNormals.clear();
        PatchCuterPoints.clear();
        Vector3Df Min = ABBox.getMin();
        Vector3Df Max = ABBox.getMax();
        float MaxLength = std::max(std::max(Max.x - Min.x, Max.y - Min.y), Max.z - Min.z);
        Vector3Df Point1, Point2, Point3, Point4;
        Vector3Df Normal1;
        TriangleDexel* tempTriDexel = nullptr;
        TriangleRenderDexel* tempTriRenderDexel = nullptr;

        std::vector<Vector3Df> CutterPoint = std::vector<Vector3Df>(4, Vector3Df(0.0f, 0.0f, 0.0f));

        std::cout << "Begin_FillPatch" << std::endl;
        for (int X = 0; X < DexelArraySize - 1; X++) {
            for (int Y = 0; Y < DexelArraySize - 1; Y++) {

                if ((X + 1) >= DexelArray.size() || (Y + 1) >= DexelArray[X].size()) {
                    continue;
                }
                if ((DexelArray[X][Y] == nullptr) || (DexelArray[X + 1][Y] == nullptr) || (DexelArray[X][Y + 1] == nullptr) || (DexelArray[X + 1][Y + 1] == nullptr)) {
                    continue;
                };

                int count = 0;
                for (int i = 0; i < 8; i = i + 2) { //查看是否需要修改该三角面片
                    int DexelIndex_X = X + VertexMask[i];
                    int DexelIndex_Y = Y + VertexMask[i + 1];

                    if (DexelArray[DexelIndex_X][DexelIndex_Y]->getMaxZ() - DexelArray[DexelIndex_X][DexelIndex_Y]->getMinZ() < 0.0002f) {
                        count++;
                    }
                }

                tempTriDexel = nullptr;
                tempTriRenderDexel = nullptr;

                if (TriangleDexelArray[X][Y] == nullptr) {
                    continue;
                }

                tempTriDexel = TriangleDexelArray[X][Y];
                tempTriRenderDexel = TriangleRenderDexelArray[X][Y];
                for (int i = 0; i < 4; i++) {
                    CutterPoint[i] = DexelArray[X + VertexMask[i * 2]][Y + VertexMask[i * 2 + 1]]->CuttePoint;
                }
                while (tempTriDexel != nullptr && (tempTriDexel->TrianglePoints.size() >= 8) && (tempTriDexel->TrianglePoints.size() % 8 == 0)) {

                    Point1 = tempTriDexel->TrianglePoints[0]; //0
                    Point2 = tempTriDexel->TrianglePoints[6]; //3
                    Point3 = tempTriDexel->TrianglePoints[2]; //1
                    Point4 = tempTriDexel->TrianglePoints[4]; //2
                    Vector3Df Normal1 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                    float dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, -1.0f));
                    if (dot1 < 0.0f) {
                        Normal1 = -Normal1;
                        Normal1.normalize();
                    }

                    if (count >= 1) {
                        Normal1 = Normal1.normalize() * 2.0f;
                    }

                    if (std::abs(Normal1.z) < 0.05f) {
                        Normal1 = CubeNormals[tempTriRenderDexel->TriangleIndex];
                        if (CubeNormals[tempTriRenderDexel->TriangleIndex].length() > 4.0f) {
                            Normal1 = CubeNormals[tempTriRenderDexel->TriangleIndex];
                        }
                    }

                    // Point1 = Point1 / MaxLength;
                    // Point2 = Point2 / MaxLength;
                    // Point3 = Point3 / MaxLength;
                    // Point4 = Point4 / MaxLength;
                    PatchVertices.push_back(Point3);
                    PatchVertices.push_back(Point2);
                    PatchVertices.push_back(Point1);
                    PatchVertices.push_back(Point2);
                    PatchVertices.push_back(Point4);
                    PatchVertices.push_back(Point1);
                    PatchCuterPoints.push_back(CutterPoint[1]);
                    PatchCuterPoints.push_back(CutterPoint[3]);
                    PatchCuterPoints.push_back(CutterPoint[0]);
                    PatchCuterPoints.push_back(CutterPoint[3]);
                    PatchCuterPoints.push_back(CutterPoint[2]);
                    PatchCuterPoints.push_back(CutterPoint[0]);

                    for (int i = 0; i < 6; i++) {
                        PatchNormals.push_back(Normal1 * (1.0f));
                    }

                    Point1 = tempTriDexel->TrianglePoints[1]; //0
                    Point2 = tempTriDexel->TrianglePoints[7]; //3
                    Point3 = tempTriDexel->TrianglePoints[3]; //1
                    Point4 = tempTriDexel->TrianglePoints[5]; //2
                    Vector3Df Normal2 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                    dot1 = Normal2.dot(Vector3Df(0.0f, 0.0f, 1.0f));
                    if (dot1 < 0.0f) {
                        Normal2 = -Normal2;
                        Normal2.normalize();
                    }
                    if (count >= 1) {
                        Normal2 = Normal2.normalize() * 2.0f;
                    }
                    if (std::abs(Normal2.z) < 0.05f) {
                        Normal2 = CubeNormals[tempTriRenderDexel->TriangleIndex];
                        if (CubeNormals[tempTriRenderDexel->TriangleIndex].length() > 4.0f) {
                            Normal2 = CubeNormals[tempTriRenderDexel->TriangleIndex];
                        }
                    }

                    // Point1 = Point1 / MaxLength;
                    // Point2 = Point2 / MaxLength;
                    // Point3 = Point3 / MaxLength;
                    // Point4 = Point4 / MaxLength;
                    PatchVertices.push_back(Point1);
                    PatchVertices.push_back(Point2);
                    PatchVertices.push_back(Point3);
                    PatchVertices.push_back(Point1);
                    PatchVertices.push_back(Point4);
                    PatchVertices.push_back(Point2);
                    PatchCuterPoints.push_back(CutterPoint[0]);
                    PatchCuterPoints.push_back(CutterPoint[3]);
                    PatchCuterPoints.push_back(CutterPoint[1]);
                    PatchCuterPoints.push_back(CutterPoint[0]);
                    PatchCuterPoints.push_back(CutterPoint[2]);
                    PatchCuterPoints.push_back(CutterPoint[3]);
                    for (int i = 0; i < 6; i++) {
                        PatchNormals.push_back(Normal2);
                    }

                    if (CountTriDexelIsSide(X, Y, tempTriDexel)) {
                        for (int m = 0; m < 4; m++) {
                            Point1 = tempTriDexel->TrianglePoints[NeighToSide[m][0] * 2]; //BotZ_0
                            Point2 = tempTriDexel->TrianglePoints[NeighToSide[m][1] * 2]; //BotZ_1
                            Point3 = tempTriDexel->TrianglePoints[NeighToSide[m][0] * 2 + 1]; //TopZ_0
                            Point4 = tempTriDexel->TrianglePoints[NeighToSide[m][1] * 2 + 1]; //TopZ_1
                            Vector3Df SideNormal = (Point1 - Point2).cross(Point1 - Point3);
                            SideNormal = SideNormal.normalize();
                            //判断Normal2是否接近垂直XY平面，是的话再判断Normal1是否接近垂直XY平面，如果Normal1也接近垂直XY平面，则直接用SideNormal
                            Vector3Df MaskNormal = Vector3Df(0, 0, 1);
                            if (std::abs(Normal2.dot(MaskNormal)) > 0.95) {
                                //Normal2接近垂直XY平面
                                if (std::abs(Normal1.dot(MaskNormal)) > 0.95) {
                                } else {
                                    //Normal1不接近垂直XY平面
                                    SideNormal = Normal1;
                                }
                            } else {
                                //Normal2不接近垂直XY平面
                                SideNormal = Normal2;
                            }
                            SideNormal = SideNormal.normalize();
                            //112,122
                            PatchVertices.push_back(Point1);
                            PatchVertices.push_back(Point2);
                            PatchVertices.push_back(Point4);
                            PatchVertices.push_back(Point1);
                            PatchVertices.push_back(Point4);
                            PatchVertices.push_back(Point3);
                            PatchCuterPoints.push_back(Vector3Df(-10000.0f, -10000.0f, -10000.0f));
                            PatchCuterPoints.push_back(Vector3Df(-10000.0f, -10000.0f, -10000.0f));
                            PatchCuterPoints.push_back(Vector3Df(-10000.0f, -10000.0f, -10000.0f));
                            PatchCuterPoints.push_back(Vector3Df(-10000.0f, -10000.0f, -10000.0f));
                            PatchCuterPoints.push_back(Vector3Df(-10000.0f, -10000.0f, -10000.0f));
                            PatchCuterPoints.push_back(Vector3Df(-10000.0f, -10000.0f, -10000.0f));
                            for (int n = 0; n < 6; n++) {
                                PatchNormals.push_back(Normal1);
                            }
                        }
                    }

                    tempTriDexel = tempTriDexel->Next;
                    tempTriRenderDexel = tempTriRenderDexel->Next;
                }
            }
        }
        std::cout << "End_FillPatch" << std::endl;
        std::cout << "PatchVerticesSize: " << PatchVertices.size() << std::endl;
        std::cout << "PatchNormalSize: " << PatchNormals.size() << std::endl;
        std::cout << "PatchCuterPointsSize: " << PatchCuterPoints.size() << std::endl;
    }

    void InitDexelTriangle_RenderType();

    double CalculateWorkPieceVolume();

    void updateDexelRenderList();

    void fullDexelRenderList();

    void InitDexelTriangle();

    void MillingWithCutter_5axis_SweptVolume(DexelCutterSweptVolume& SweptVolume);

    //三轴
    void MillingWithCutter(std::unique_ptr<Cutter>& cutter, const OBB3Df& obb);

    //五轴
    void MillingWithCutter_5axis(std::unique_ptr<Cutter>& cutter, const OBB3Df& obb, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals);
    //五轴并行
    void MillingWithCutter_5axis_Par(std::unique_ptr<Cutter>& cutter, const OBB3Df& obb, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals);

    void MillingWithCutter_5axis_RenderType(std::unique_ptr<Cutter>& cutter, const OBB3Df& obb);

public:

    std::vector<std::vector<PowerDexel*>> DexelArray = std::vector<std::vector<PowerDexel*>>();
    std::vector<std::vector<TriangleDexel*>> TriangleDexelArray = std::vector<std::vector<TriangleDexel*>>();
    std::vector<std::vector<TriangleRenderDexel*>> TriangleRenderDexelArray = std::vector<std::vector<TriangleRenderDexel*>>();


    std::stack<int> VoidRenderIndexList, VoidRenderIndexForSideList;

    std::vector<Vector3Df> DexelTriangles;
    std::vector<Vector3Df> DexelTrianglesNormals;

    std::vector<float> PointHeight;
    std::vector<Vector3Df> CubeCenter;
    std::vector<Vector3Df> CubeNormals;
    std::vector<Vector3Df> PatchVertices;
    std::vector<Vector3Df> PatchNormals;
    std::vector<Vector3Df> PatchCuterPoints;

    bool IsBlankFile = false;
    std::vector<Triangle3Df> trianglesFromBlank;
    AABB3Df ABBox; //工件的整体包围盒，记录三轴的最小值和最大值
    Vector3Df CoordOffset = Vector3Df(0.0f, 0.0f, 0.0f);
    Vector3Df BoxSize;

    int DexelArraySize = 256;
    
    float UnitLength = 0;
    float StaticUnitLength = 0;
    float GLCoordSize = 0;
    float DeletedVolume = 0;
    double WorkpieceVolume = 0;
    double LastWorkpieceVolume = 0;
    bool RenderStatic = false;
    Cutter* cutter;

    int VertexMask[8] = {
        0, 0,
        0, 1,
        1, 0,
        1, 1
    };

    //ABCD四种情况对应的顶点索引
    int DelteTypeToVertexIndex[4][3] = {
        { 0, 2, 1 },
        { 0, 2, 3 },
        { 1, 2, 3 },
        { 3, 1, 0 }
    };

    float VertexMaskFloat[8] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
    };
    //获取TriDexele的邻居
    int GetNeighbor[4][2] = {
        { -1, 0 }, //Left
        { 1, 0 }, //Rigth
        { 0, 1 }, //Behind
        { 0, -1 } //Front
    };

    int NeighToSide[4][2] = {
        { 1, 0 }, //Left
        { 2, 3 }, //Rigth
        { 3, 1 }, //Behind
        { 0, 2 } //Front

    };
    //top为*2，bottom为*2+1
    int SideToNeighborSide[4][2] = {
        { 3, 2 }, //Left->Rgith
        { 0, 1 }, //Rigth->Left
        { 2, 0 }, //Behind->Front
        { 1, 3 } //Front->Behind
    };
    //获取三角柱体所需的侧面点的顺序
    int TriangleIndex[4][2] = {
        { 2, 1 },
        { 0, 2 },
        { 2, 1 },
        { 2, 0 }
    };

    //获取角邻居
    int GetCornerNeighbor[8][2] = {
        { -1, 1 },
        { 1, 1 },
        { -1, -1 },
        { 1, -1 }
    };

    //角邻居对应的角Dexel的索引
    int CornerNeighborToCornerDexel[4][2] = {
        { 2, 1 }, //左边为CornerNeigthbor的角，右边为对应的CornerDexel的角
        { 0, 3 },
        { 3, 0 },
        { 1, 2 }
    };

    //获取周围8个点
    int StaticNeighbor[9][2] = {
        { 0, 0 },
        { -1, 1 },
        { 0, 1 },
        { 1, 1 },
        { -1, 0 },
        { 1, 0 },
        { -1, -1 },
        { 0, -1 },
        { 1, -1 }
    };

    Vector3Df LastCutterPos = Vector3Df(0.0f, 0.0f, 0.0f);
};