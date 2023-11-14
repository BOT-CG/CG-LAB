#include "DexelMilling.h"
#include <execution>
#include <numeric>
bool pointintriangle(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
{
    glm::vec2 v0 = c - a;
    glm::vec2 v1 = b - a;
    glm::vec2 v2 = p - a;
    float dot00 = glm::dot(v0, v0);
    float dot01 = glm::dot(v0, v1);
    float dot02 = glm::dot(v0, v2);
    float dot11 = glm::dot(v1, v1);
    float dot12 = glm::dot(v1, v2);
    float invDenom = (1.0f) / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // glm::vec2 ab = b - a;
    // glm::vec2 ac = c - a;
    // glm::vec2 ap = p - a;
    // float u = (glm::dot(ab, ab) * glm::dot(ap, ac) - glm::dot(ac, ab) * glm::dot(ap, ab) ) / (glm::dot(ac, ac) * glm::dot(ab, ab) - glm::dot(ac, ab) * glm::dot(ab, ac));
    // float v = (glm::dot(ac, ac) * glm::dot(ap, ab) - glm::dot(ac, ab) * glm::dot(ap, ac)) / (glm::dot(ac, ac) * glm::dot(ab, ab) - glm::dot(ac, ab) * glm::dot(ab, ac));

    return (u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f);
}

//ray intersection triangle glm::vec3
glm::vec3 rayintersectiontriangle(const glm::vec3& orig, const glm::vec3& dir, const glm::vec3& vert0, const glm::vec3& vert1, const glm::vec3& vert2)
{
    glm::vec3 edge1, edge2, tvec, pvec, qvec;
    float det, inv_det;
    float u, v;
    edge1 = vert1 - vert0;
    edge2 = vert2 - vert0;
    pvec = glm::cross(dir, edge2);
    det = glm::dot(edge1, pvec);
    if (det > -0.000001 && det < 0.000001)
        return glm::vec3(-1.0f, -1.0f, -1.0f);
    inv_det = 1.0f / det;
    tvec = orig - vert0;
    u = glm::dot(tvec, pvec) * inv_det;
    if (u < 0.0f || u > 1.0f)
        return glm::vec3(-1.0f, -1.0f, -1.0f);
    qvec = glm::cross(tvec, edge1);
    v = glm::dot(dir, qvec) * inv_det;
    if (v < 0.0f || u + v > 1.0f)
        return glm::vec3(-1.0f, -1.0f, -1.0f);
    return vert0 + u * edge1 + v * edge2;
}

void MRR_Dexel::MillingWithCutter(std::unique_ptr<Cutter>& cutter, const OBB3Df& obb)
{
    // std::cout << "cutter->CurrentPosture.center" << cutter->CurrentPosture.center << std::endl;
    //std::cout << "MillingWithCutter_3axis_Begin" << std::endl;
    int x1, x2, y1, y2;
    getXYFromOBB(obb, x1, x2, y1, y2);
    int WholeSize = (x2 - x1) * (y2 - y1);
    int countDexelChanged = 0;
    float PieceOfchangedVolume = 0;
    // std::cout << "CutterCenter" << cutter->CurrentPosture.center << std::endl;
    // std::cout << "CutterCenterWithCoordOffset" << cutter->CurrentPosture.center + CoordOffset << std::endl;
    //std::cout << "Begin PowerDexel Milling" << std::endl;
    for (int i = x1; i <= x2; i++) {
        for (int j = y1; j <= y2; j++) {
            if (DexelArray[i][j] != nullptr) {
                PowerDexel* Firstdexel = DexelArray[i][j];
                PowerDexel* dexel = DexelArray[i][j];
                PowerDexel* LastDexel = nullptr;
                while (dexel != nullptr) {
                    Vector3Df Center = getDexelCenterWithCoordOffset(i, j);
                    float z = 10000.0f; //cutter->getCutterSurfaceHeight(Center.x, Center.y) + CoordOffset.z; //添加偏移量//修正

                    if (z > dexel->First && z < dexel->Second) {
                        countDexelChanged++;
                        PieceOfchangedVolume += (dexel->Second - z) * UnitLength * UnitLength;
                        DeletedVolume += (dexel->Second - z) * UnitLength * UnitLength;
                        dexel->Second = z;
                    } else if (z < dexel->First) {
                        DeletedVolume += CountDexelVolume(dexel) * UnitLength * UnitLength;
                        DeleteDexel(dexel);
                        //消灭数组中存储的野指针
                        if (LastDexel == nullptr) {
                            DexelArray[i][j] = nullptr;
                        } else {
                            LastDexel->Next = nullptr;
                        }
                        break;
                    }
                    LastDexel = dexel;
                    dexel = dexel->Next;
                }
            }
        }
    }

    std::cout << "DeletedVolume: " << DeletedVolume << std::endl;
}

void MRR_Dexel::MillingWithCutter_5axis(std::unique_ptr<Cutter>& cutter, const OBB3Df& obb, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals)
{

    Vector3Df Min = ABBox.getMin();
    Vector3Df Max = ABBox.getMax();
    float MaxLength = std::max(std::max(Max.x - Min.x, Max.y - Min.y), Max.z - Min.z);

    int x1, x2, y1, y2;
    getXYFromOBB(obb, x1, x2, y1, y2);
    int WholeSize = (x2 - x1) * (y2 - y1);
    int countDexelChanged = 0;
    float PieceOfchangedVolume = 0;
    for (int i = x1; i < x2; i++) {
        for (int j = y1; j < y2; j++) {
            if (DexelArray[i][j] == nullptr) {
                continue;
            }

            DexelArray[i][j]->IsChanged = false; //初始化为未改变,后面用于生成面片的Dexel全都用的这里的Dexel，因此不必要管别的
            PowerDexel* dexel = DexelArray[i][j];
            //获取Dexel的中心点
            Vector3Df Center = getDexelCenterWithCoordOffset(i, j);

            //获取中心点对应的射线与刀具的交点高度，
            float z = 10000.0f; //cutter->getCutterSurfaceHeight(Center.x, Center.y) + CoordOffset.z;//修正

            //如果交点高度大于Dexel的最高点，那么就不需要进行切割
            if (z > dexel->getMaxZ()) {
                continue;
            }
            //暂时构造一个符合五轴条件的刀具高度，之后再改
            Vector3Df cutterHeigth = Vector3Df(z, 10000, 0);
            //去除率怎么办？
            DexelArray[i][j] = DexelBoolCalculation(DexelArray[i][j], cutterHeigth.x, cutterHeigth.y);
            DexelArray[i][j]->IsChanged = true;
        }
    }

    //计算要在TriangleArray中使用的坐标
    x1 = std::max(x1 - 1, 0);
    x2 = std::min(x2, int(TriangleDexelArray.size() - 1));
    y1 = std::max(y1 - 1, 0);
    y2 = std::min(y2, int(TriangleDexelArray.size() - 1));

    //遍历TriangleDexel，如果CornerDexel被切割过，那么就重新计算TrianglePoint,并将IsChanged设为true
    for (int X = x1; X <= x2; X++) {
        for (int Y = y1; Y <= y2; Y++) {
            if (TriangleDexelArray[X][Y] == nullptr) {
                continue;
            }

            bool Ischanged = false;
            bool SetToVoid = false;
            int DexelIndex_X, DexelIndex_Y;
            TriangleDexel* tempTriDexel = TriangleDexelArray[X][Y];
            TriangleRenderDexel* tempTriRender = TriangleRenderDexelArray[X][Y];

            for (int i = 0; i < 8; i = i + 2) { //查看是否需要修改该三角面片
                DexelIndex_X = X + VertexMask[i];
                DexelIndex_Y = Y + VertexMask[i + 1];
                //cornerDexel中存在空指针，后续无法生成面片，需要仔细判断原有面片
                if (DexelArray[DexelIndex_X][DexelIndex_Y] == nullptr) {
                    //将其设为changed,清空TriPoint,后续合成三角面片填入渲染列表时再将其TriDexel和TriRender删除
                    //并将空出来的位置填入填入空闲列表以待备用
                    tempTriRender->IsChanged = true;
                    // while (tempTriDexel != nullptr) {
                    //     tempTriDexel->TrianglePoints.clear();
                    //     tempTriDexel = tempTriDexel->Next;
                    //     tempTriRender = tempTriRender->Next;
                    // }
                    Ischanged = false; //助其跳过后续的计算
                    break;
                }
                if (DexelArray[DexelIndex_X][DexelIndex_Y]->IsChanged) {
                    Ischanged = true;
                    break;
                }
                // Ischanged = Ischanged || DexelArray[DexelIndex_X][DexelIndex_Y]->IsChanged;
            }

            if (!Ischanged) {
                continue;
            }

            /******************通过DexelArray的四条边，计算DexelTriangleArray**********************/
            //删除原有的TriDexel三角面片顶点并回收内存,创建新的TriDexel，TriRender等到下一步再删除并创建新的TriRender，这里只是将其设为changed，
            tempTriRender->IsChanged = true;

            while (tempTriDexel != nullptr) {
                TriangleDexel* childTridexel = tempTriDexel->Next;
                tempTriDexel->Next = nullptr;
                delete tempTriDexel;
                tempTriDexel = childTridexel;
            }
            // tempTriDexel->deleteTriangleDexel();
            TriangleDexel* TriDexel = new TriangleDexel();
            TriangleDexelArray[X][Y] = TriDexel;
            tempTriDexel = TriDexel;
            /*********************************获取白域与黑域开始***********************************/
            PowerDexel* White = new PowerDexel(-1000.0f, 1000.0f);
            PowerDexel* Black = nullptr; //初始化白域和黑域
            PowerDexel* tempDexel = nullptr;
            for (int i = 0; i < 8; i += 2) { //初始化黑域和白域
                DexelIndex_X = X + VertexMask[i];
                DexelIndex_Y = Y + VertexMask[i + 1];
                tempDexel = DexelArray[DexelIndex_X][DexelIndex_Y];
                if (tempDexel == nullptr) {
                    Black == nullptr;
                    break;
                }
                if (Black == nullptr && tempDexel != nullptr) {
                    Black = new PowerDexel(tempDexel); //初始化黑域
                }
                if (Black != nullptr && tempDexel != nullptr) {
                    White = DexelBoolGetWhiteZone(White, DexelArray[DexelIndex_X][DexelIndex_Y]); //获取白域
                    Black = DexelBoolGetBlackZone(Black, DexelArray[DexelIndex_X][DexelIndex_Y]); //获取黑域
                }
            }
            if (Black == nullptr) {
                continue; //四个边角点均为空，跳过
            }
            tempDexel = Black; //先对白域进行适当扩展，再对黑域进行适当缩小,再获取灰域
            float SegmentLength;
            while (tempDexel != nullptr) { //对初始黑域进行适当缩小,对白域进行适当扩展
                SegmentLength = tempDexel->Second - tempDexel->First;
                tempDexel->First += (SegmentLength * 0.1);
                tempDexel->Second -= (SegmentLength * 0.1);
                tempDexel = tempDexel->Next;
            }
            tempDexel = White; //对白域的空档进行适当扩张
            while (tempDexel->Next != nullptr) {
                SegmentLength = std::min(tempDexel->Next->First - tempDexel->Second, tempDexel->Next->Second - tempDexel->Next->First);
                SegmentLength = std::min(SegmentLength, tempDexel->Second - tempDexel->First);
                SegmentLength = std::abs(SegmentLength);
                tempDexel->Second -= (SegmentLength * 0.1);
                tempDexel->Next->First += (SegmentLength * 0.1);
                tempDexel = tempDexel->Next;
            }
            /*********************************获取白域与黑域结束***********************************/

            /*********************************计算灰域并获取三角面片顶点****************************/
            tempDexel = Black; //根据黑域和白域获取灰域并获取三角形
            float BottomGrayMin = White->Second; //获取底部面片的灰域
            float BottomGrayMax = tempDexel->First;
            float TopGrayMin, TopGrayMax;
            PowerDexel* tempCornerDexel = nullptr;
            Vector3Df CornerPoint;
            PowerDexel* tempWhite = White;
            std::vector<Vector3Df> TrianglePoint;

            while (tempDexel != nullptr) { //遍历黑域
                if (White->Next == nullptr) {
                    break;
                }
                if (tempDexel->Next != nullptr) {
                    if (tempDexel->Next->Second < White->Next->First) { //黑域还未结束，必然存在下一个白域
                        tempDexel = tempDexel->Next;
                        continue;
                    } //否则，tempDexel与tempDexel->Next存在白域，可以在灰域中获取三角形
                } //否则，tempDexel已经没有下一个顶点，它的下一个区域就是白域，可以在灰域中获取三角形顶点
                TopGrayMin = tempDexel->Second; //获取顶部面片的灰域
                TopGrayMax = White->Next->First;
                for (int i = 0; i < 8; i = i + 2) { //遍历4个顶点，获取三角形的顶点
                    DexelIndex_X = X + VertexMask[i];
                    DexelIndex_Y = Y + VertexMask[i + 1];
                    tempCornerDexel = DexelArray[DexelIndex_X][DexelIndex_Y];
                    if (tempCornerDexel == nullptr) {
                        break;
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
            /*********************************计算灰域并获取三角面片顶点结束****************************/
            /*********************************初始化TriDexel****************************/
            if ((TrianglePoint.size() >= 8) && (TrianglePoint.size() % 8) == 0) {
                for (int i = 0; i < TrianglePoint.size(); i = i + 8) {
                    for (int j = 0; j < 8; j++) {
                        TriDexel->TrianglePoints.push_back(TrianglePoint[i + j]);
                    }
                    if ((i + 8) < TrianglePoint.size()) {
                        TriDexel->Next = new TriangleDexel();
                        //TriRenderDexel->Next = new TriangleRenderDexel();只能等到修改渲染列表时再初始化
                        TriDexel = TriDexel->Next;
                    }
                }
            }
            DeleteDexel(Black);
            DeleteDexel(tempWhite);
            /*********************************初始化TriDexel结束***********************/
        }
    }
    //修改渲染列表
    for (int X = x1; X <= x2; X++) {

        for (int Y = y1; Y <= y2; Y++) {
            if (TriangleDexelArray[X][Y] == nullptr) {
                continue;
            }

            TriangleDexel* tempTriDexel = TriangleDexelArray[X][Y];
            TriangleRenderDexel* tempTriRenderDexel = TriangleRenderDexelArray[X][Y]; //用于回收旧RenderIndex
            // std::cout << "Begin_getRenderIndex" << std::endl;
            /*********************************回收渲染列表坐标***********************/
            if (tempTriRenderDexel->IsChanged) { //首先回收渲染列表坐标，后面再分配
                while (tempTriRenderDexel != nullptr) {

                    if (tempTriRenderDexel->TriangleIndex >= 0) {
                        Vector3Df vector = Vector3Df(0.0f, 0.0f, 0.0f);
                        if (tempTriRenderDexel->IsAddSide) {
                            //Point数量为(2 + 4) * 2 * 3
                            VoidRenderIndexForSideList.push(tempTriRenderDexel->TriangleIndex);
                            for (int i = 0; i < 36; i++) {
                                vertices[tempTriRenderDexel->TriangleIndex + i] = vector;
                                normals[tempTriRenderDexel->TriangleIndex + i] = vector;
                            }
                        } else {
                            //Point数量为2 * 2 * 3
                            VoidRenderIndexList.push(tempTriRenderDexel->TriangleIndex);
                            for (int i = 0; i < 12; i++) {
                                vertices[tempTriRenderDexel->TriangleIndex + i] = vector;
                                normals[tempTriRenderDexel->TriangleIndex + i] = vector;
                            }
                        }
                    }
                    TriangleRenderDexel* temp = tempTriRenderDexel->Next;
                    tempTriRenderDexel->Next = nullptr;
                    delete tempTriRenderDexel;
                    tempTriRenderDexel = temp;
                }

            } else {
                continue;
            }
            // std::cout << "GetRenderIndex_End" << std::endl;
            /***************************回收渲染列表坐标结束**************************/
            /****************根据TriDexel分段初始化TriRenderDexel********************/
            TriangleRenderDexelArray[X][Y] = new TriangleRenderDexel(); //放到循环里创建
            tempTriRenderDexel = TriangleRenderDexelArray[X][Y];

            while ((tempTriDexel != nullptr)) { //初始化TriRenderDexel
                if (tempTriDexel->Next != nullptr) {
                    tempTriRenderDexel->Next = new TriangleRenderDexel();
                }
                tempTriDexel = tempTriDexel->Next;
                tempTriRenderDexel = tempTriRenderDexel->Next;
            }
            tempTriDexel = TriangleDexelArray[X][Y];
            tempTriRenderDexel = TriangleRenderDexelArray[X][Y];
            // std::cout << "Init_TriRenderDexel_End" << std::endl;
            /**********************初始化TriRenderDexel结束*************************/
            /************************开始组合面片并压入渲染列表************************/
            float TopZ_0, TopZ_1, BotZ_0, BotZ_1;
            float Nerighbor_TopZ_0, Nerighbor_TopZ_1, Nerighbor_BotZ_0, Nerighbor_BotZ_1;
            int Neighbor_X, Neighbor_Y;
            int AddSide = 0; //判断是否添加侧面面片
            while ((tempTriDexel != nullptr)) {
                if (tempTriDexel->TrianglePoints.size() != 8) {
                    tempTriDexel = tempTriDexel->Next;
                    tempTriRenderDexel = tempTriRenderDexel->Next;
                    continue;
                }
                AddSide = 0; //遍历四个邻居Dexel，判断是否需要添加侧面面片
                for (int i = 0; i < 4; i++) {
                    Neighbor_X = X + GetNeighbor[i][0];
                    Neighbor_Y = Y + GetNeighbor[i][1];
                    if ((Neighbor_X < 0) || (Neighbor_X >= (DexelArraySize - 1)) || (Neighbor_Y < 0) || (Neighbor_Y >= (DexelArraySize - 1)) || (TriangleDexelArray[Neighbor_X][Neighbor_Y] == nullptr)) {
                        AddSide = 0;
                        break;
                    }
                    TriangleDexel* NeighborDexel = TriangleDexelArray[Neighbor_X][Neighbor_Y];
                    float length_0, length_1;
                    BotZ_0 = tempTriDexel->TrianglePoints[NeighToSide[i][0] * 2].z;
                    BotZ_1 = tempTriDexel->TrianglePoints[NeighToSide[i][1] * 2].z;
                    TopZ_0 = tempTriDexel->TrianglePoints[NeighToSide[i][0] * 2 + 1].z;
                    TopZ_1 = tempTriDexel->TrianglePoints[NeighToSide[i][1] * 2 + 1].z;
                    while (NeighborDexel != nullptr) { //判断所有柱体
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
                        Nerighbor_BotZ_0 = Nerighbor_BotZ_0 - length_0 * 0.01; //适当扩大遮掩范围
                        Nerighbor_BotZ_1 = Nerighbor_BotZ_1 - length_1 * 0.01;
                        Nerighbor_TopZ_0 = Nerighbor_TopZ_0 + length_0 * 0.01;
                        Nerighbor_TopZ_1 = Nerighbor_TopZ_1 + length_1 * 0.01;

                        bool IsAddSide = ((Nerighbor_TopZ_0 >= TopZ_0) && (Nerighbor_TopZ_1 >= TopZ_1) && (Nerighbor_BotZ_0 <= BotZ_0) && (Nerighbor_BotZ_1 <= BotZ_1));

                        if (IsAddSide) { //该侧面被遮掩
                            AddSide++;
                            break;
                        }
                        NeighborDexel = NeighborDexel->Next;
                    }
                }

                tempTriRenderDexel->IsAddSide = (AddSide < 4) ? true : false;
                // std::cout << "Begin_FillPoint" << std::endl;
                //添加顶部和底部面片
                Vector3Df Point1 = tempTriDexel->TrianglePoints[0];
                Vector3Df Point2 = tempTriDexel->TrianglePoints[6];
                Vector3Df Point3 = tempTriDexel->TrianglePoints[2];
                Vector3Df Point4 = tempTriDexel->TrianglePoints[4];
                Vector3Df Normal1 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                float dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, -1.0f));
                if (dot1 < 0.0f) {
                    Normal1 = -Normal1;
                }
                // bool IsPush = false;
                // if (tempTriRenderDexel->IsAddSide) {
                //     if (VoidRenderIndexForSideList.empty()) {
                //         IsPush = true;
                //         tempTriRenderDexel->TriangleIndex = vertices.size(); //顶点索引
                //     } else {
                //         IsPush = false;
                //         tempTriRenderDexel->TriangleIndex = VoidRenderIndexForSideList.top();
                //         VoidRenderIndexForSideList.pop();
                //     }
                // } else {
                //     if (VoidRenderIndexList.empty()) {
                //         IsPush = true;
                //         tempTriRenderDexel->TriangleIndex = vertices.size(); //顶点索引
                //     } else {
                //         IsPush = false;
                //         tempTriRenderDexel->TriangleIndex = VoidRenderIndexList.top();
                //         VoidRenderIndexList.pop();
                //     }
                // }

                if (tempTriRenderDexel->IsAddSide) {
                    if (VoidRenderIndexForSideList.empty()) {
                        tempTriRenderDexel->TriangleIndex = vertices.size(); //顶点索引
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices.push_back(Point3);
                        vertices.push_back(Point2);
                        vertices.push_back(Point1);
                        vertices.push_back(Point2);
                        vertices.push_back(Point4);
                        vertices.push_back(Point1);
                        for (int i = 0; i < 6; i++) {
                            normals.push_back(Normal1 * (1.0f));
                        }
                        Point1 = tempTriDexel->TrianglePoints[1];
                        Point2 = tempTriDexel->TrianglePoints[7];
                        Point3 = tempTriDexel->TrianglePoints[3];
                        Point4 = tempTriDexel->TrianglePoints[5];
                        Normal1 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                        dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, 1.0f));
                        if (dot1 < 0.0f) {
                            Normal1 = -Normal1;
                        }
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices.push_back(Point1);
                        vertices.push_back(Point2);
                        vertices.push_back(Point3);
                        vertices.push_back(Point1);
                        vertices.push_back(Point4);
                        vertices.push_back(Point2);
                        for (int i = 0; i < 6; i++) {
                            normals.push_back(Normal1);
                        }

                        //添加侧面
                        for (int m = 0; m < 4; m++) {
                            Point1 = tempTriDexel->TrianglePoints[NeighToSide[m][0] * 2]; //BotZ_0
                            Point2 = tempTriDexel->TrianglePoints[NeighToSide[m][1] * 2]; //BotZ_1
                            Point3 = tempTriDexel->TrianglePoints[NeighToSide[m][0] * 2 + 1]; //TopZ_0
                            Point4 = tempTriDexel->TrianglePoints[NeighToSide[m][1] * 2 + 1]; //TopZ_1
                            Normal1 = (Point1 - Point2).cross(Point1 - Point3);
                            Normal1.normalize();
                            Point1 = Point1 / MaxLength;
                            Point2 = Point2 / MaxLength;
                            Point3 = Point3 / MaxLength;
                            Point4 = Point4 / MaxLength;
                            DexelTriangles.push_back(Point1);
                            DexelTriangles.push_back(Point2);
                            DexelTriangles.push_back(Point4);
                            DexelTriangles.push_back(Point1);
                            DexelTriangles.push_back(Point4);
                            DexelTriangles.push_back(Point3);
                            for (int n = 0; n < 6; n++) {
                                DexelTrianglesNormals.push_back(Normal1);
                            }
                        }

                    } else {
                        tempTriRenderDexel->TriangleIndex = VoidRenderIndexForSideList.top();
                        VoidRenderIndexForSideList.pop();
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices[tempTriRenderDexel->TriangleIndex + 0] = Point3;
                        vertices[tempTriRenderDexel->TriangleIndex + 1] = Point2;
                        vertices[tempTriRenderDexel->TriangleIndex + 2] = Point1;
                        vertices[tempTriRenderDexel->TriangleIndex + 3] = Point2;
                        vertices[tempTriRenderDexel->TriangleIndex + 4] = Point4;
                        vertices[tempTriRenderDexel->TriangleIndex + 5] = Point1;
                        for (int i = 0; i < 6; i++) {
                            normals[tempTriRenderDexel->TriangleIndex + i] = Normal1;
                        }
                        Point1 = tempTriDexel->TrianglePoints[1];
                        Point2 = tempTriDexel->TrianglePoints[7];
                        Point3 = tempTriDexel->TrianglePoints[3];
                        Point4 = tempTriDexel->TrianglePoints[5];
                        Normal1 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                        dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, 1.0f));
                        if (dot1 < 0.0f) {
                            Normal1 = -Normal1;
                        }
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices[tempTriRenderDexel->TriangleIndex + 6] = Point1;
                        vertices[tempTriRenderDexel->TriangleIndex + 7] = Point2;
                        vertices[tempTriRenderDexel->TriangleIndex + 8] = Point3;
                        vertices[tempTriRenderDexel->TriangleIndex + 9] = Point1;
                        vertices[tempTriRenderDexel->TriangleIndex + 10] = Point4;
                        vertices[tempTriRenderDexel->TriangleIndex + 11] = Point2;
                        for (int i = 0; i < 6; i++) {
                            normals[tempTriRenderDexel->TriangleIndex + 6 + i] = Normal1;
                        }

                        int SideRenderIndex = tempTriRenderDexel->TriangleIndex + 12;
                        //添加侧面
                        for (int m = 0; m < 4; m++) {
                            Point1 = tempTriDexel->TrianglePoints[NeighToSide[m][0] * 2]; //BotZ_0
                            Point2 = tempTriDexel->TrianglePoints[NeighToSide[m][1] * 2]; //BotZ_1
                            Point3 = tempTriDexel->TrianglePoints[NeighToSide[m][0] * 2 + 1]; //TopZ_0
                            Point4 = tempTriDexel->TrianglePoints[NeighToSide[m][1] * 2 + 1]; //TopZ_1
                            Normal1 = (Point1 - Point2).cross(Point1 - Point3);
                            Normal1.normalize();
                            Point1 = Point1 / MaxLength;
                            Point2 = Point2 / MaxLength;
                            Point3 = Point3 / MaxLength;
                            Point4 = Point4 / MaxLength;
                            vertices[SideRenderIndex + m * 6 + 0] = Point1;
                            vertices[SideRenderIndex + m * 6 + 1] = Point2;
                            vertices[SideRenderIndex + m * 6 + 2] = Point4;
                            vertices[SideRenderIndex + m * 6 + 3] = Point1;
                            vertices[SideRenderIndex + m * 6 + 4] = Point4;
                            vertices[SideRenderIndex + m * 6 + 5] = Point3;
                            for (int n = 0; n < 6; n++) {
                                normals[SideRenderIndex + m * 6 + n] = Normal1;
                            }
                        }
                    }
                } else {
                    if (VoidRenderIndexList.empty()) { //不添加侧面面片
                        tempTriRenderDexel->TriangleIndex = vertices.size(); //顶点索引
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices.push_back(Point3);
                        vertices.push_back(Point2);
                        vertices.push_back(Point1);
                        vertices.push_back(Point2);
                        vertices.push_back(Point4);
                        vertices.push_back(Point1);
                        for (int i = 0; i < 6; i++) {
                            normals.push_back(Normal1 * (1.0f));
                        }
                        Point1 = tempTriDexel->TrianglePoints[1];
                        Point2 = tempTriDexel->TrianglePoints[7];
                        Point3 = tempTriDexel->TrianglePoints[3];
                        Point4 = tempTriDexel->TrianglePoints[5];
                        Normal1 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                        dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, 1.0f));
                        if (dot1 < 0.0f) {
                            Normal1 = -Normal1;
                        }
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices.push_back(Point1);
                        vertices.push_back(Point2);
                        vertices.push_back(Point3);
                        vertices.push_back(Point1);
                        vertices.push_back(Point4);
                        vertices.push_back(Point2);
                        for (int i = 0; i < 6; i++) {
                            normals.push_back(Normal1);
                        }
                    } else {
                        tempTriRenderDexel->TriangleIndex = VoidRenderIndexList.top();
                        VoidRenderIndexList.pop();
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices[tempTriRenderDexel->TriangleIndex + 0] = Point3;
                        vertices[tempTriRenderDexel->TriangleIndex + 1] = Point2;
                        vertices[tempTriRenderDexel->TriangleIndex + 2] = Point1;
                        vertices[tempTriRenderDexel->TriangleIndex + 3] = Point2;
                        vertices[tempTriRenderDexel->TriangleIndex + 4] = Point4;
                        vertices[tempTriRenderDexel->TriangleIndex + 5] = Point1;
                        for (int i = 0; i < 6; i++) {
                            normals[tempTriRenderDexel->TriangleIndex + i] = Normal1;
                        }
                        Point1 = tempTriDexel->TrianglePoints[1];
                        Point2 = tempTriDexel->TrianglePoints[7];
                        Point3 = tempTriDexel->TrianglePoints[3];
                        Point4 = tempTriDexel->TrianglePoints[5];
                        Normal1 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                        dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, 1.0f));
                        if (dot1 < 0.0f) {
                            Normal1 = -Normal1;
                        }
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices[tempTriRenderDexel->TriangleIndex + 6] = Point1;
                        vertices[tempTriRenderDexel->TriangleIndex + 7] = Point2;
                        vertices[tempTriRenderDexel->TriangleIndex + 8] = Point3;
                        vertices[tempTriRenderDexel->TriangleIndex + 9] = Point1;
                        vertices[tempTriRenderDexel->TriangleIndex + 10] = Point4;
                        vertices[tempTriRenderDexel->TriangleIndex + 11] = Point2;
                        for (int i = 0; i < 6; i++) {
                            normals[tempTriRenderDexel->TriangleIndex + 6 + i] = Normal1;
                        }
                    }
                }

                tempTriDexel = tempTriDexel->Next;
                tempTriRenderDexel = tempTriRenderDexel->Next;
            }

            // std::cout << "FillPoint_End" << std::endl;
        }
    }

    // std::cout << "vertices.size():" << vertices.size() << std::endl;
}

//五轴并行
void MRR_Dexel::MillingWithCutter_5axis_Par(std::unique_ptr<Cutter>& cutter, const OBB3Df& obb, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals)
{
    Vector3Df Min = ABBox.getMin();
    Vector3Df Max = ABBox.getMax();
    float MaxLength = std::max(std::max(Max.x - Min.x, Max.y - Min.y), Max.z - Min.z);

    int x1, x2, y1, y2;
    getXYFromOBB(obb, x1, x2, y1, y2);
    int WholeSize = (x2 - x1) * (y2 - y1);
    int countDexelChanged = 0;
    float PieceOfchangedVolume = 0;
    //不知道为什么需要加1
    std::for_each(std::execution::par, DexelArray.begin() + x1, DexelArray.begin() + x2 + 1, [&](std::vector<PowerDexel*>& DexelArrayX) {
        int X = &DexelArrayX - &DexelArray[0];
        std::for_each(std::execution::par, DexelArrayX.begin() + y1, DexelArrayX.begin() + y2 + 1, [&](PowerDexel*& dexel) {
            if (dexel == nullptr) {
                return;
            }
            int Y = &dexel - &DexelArrayX[0];
            dexel->IsChanged = false;
            Vector3Df Center = getDexelCenterWithCoordOffset(X, Y);
            float z = 10000.0; //cutter->getCutterSurfaceHeight(Center.x, Center.y) + CoordOffset.z;//修正
            if (z > dexel->getMaxZ()) {
                return;
            }
            Vector3Df cutterHeigth = Vector3Df(z, 10000, 0);
            dexel = DexelBoolCalculation(dexel, cutterHeigth.x, cutterHeigth.y);
            dexel->IsChanged = true;
        });
    });

    //计算要在TriangleArray中使用的坐标
    x1 = std::max(x1 - 1, 0);
    x2 = std::min(x2, int(TriangleDexelArray.size() - 1));
    y1 = std::max(y1 - 1, 0);
    y2 = std::min(y2, int(TriangleDexelArray.size() - 1));

    std::for_each(std::execution::par, TriangleDexelArray.begin() + x1, TriangleDexelArray.begin() + x2 + 1, [&](std::vector<TriangleDexel*>& TriangleDexelArrayX) {
        int X = &TriangleDexelArrayX - &TriangleDexelArray[0];
        std::for_each(std::execution::par, TriangleDexelArrayX.begin() + y1, TriangleDexelArrayX.begin() + y2 + 1, [&](TriangleDexel*& TriDexel) {
            if (TriDexel == nullptr) {
                return;
            }
            int Y = &TriDexel - &TriangleDexelArrayX[0];
            bool Ischanged = false;
            bool SetToVoid = false;
            int DexelIndex_X, DexelIndex_Y;
            TriangleDexel* tempTriDexel = TriDexel;
            TriangleRenderDexel* tempTriRender = TriangleRenderDexelArray[X][Y];

            for (int i = 0; i < 8; i = i + 2) { //查看是否需要修改该三角面片
                DexelIndex_X = X + VertexMask[i];
                DexelIndex_Y = Y + VertexMask[i + 1];
                //cornerDexel中存在空指针，后续无法生成面片，需要仔细判断原有面片
                if (DexelArray[DexelIndex_X][DexelIndex_Y] == nullptr) {
                    //将其设为changed,清空TriPoint,后续合成三角面片填入渲染列表时再将其TriDexel和TriRender删除
                    //并将空出来的位置填入填入空闲列表以待备用
                    tempTriRender->IsChanged = true;
                    Ischanged = false; //助其跳过后续的计算
                    break;
                }
                if (DexelArray[DexelIndex_X][DexelIndex_Y]->IsChanged) {
                    Ischanged = true;
                    break;
                }
            }

            if (!Ischanged) {
                return;
            }
            /******************通过DexelArray的四条边，计算DexelTriangleArray**********************/
            //删除原有的TriDexel三角面片顶点并回收内存,创建新的TriDexel，TriRender等到下一步再删除并创建新的TriRender，这里只是将其设为changed，
            tempTriRender->IsChanged = true;
            while (tempTriDexel != nullptr) {
                TriangleDexel* childTridexel = tempTriDexel->Next;
                tempTriDexel->Next = nullptr;
                delete tempTriDexel;
                tempTriDexel = childTridexel;
            }

            TriDexel = new TriangleDexel();
            tempTriDexel = TriDexel;
            /*********************************获取白域与黑域开始***********************************/
            PowerDexel* White = new PowerDexel(-1000.0f, 1000.0f);
            PowerDexel* Black = nullptr; //初始化白域和黑域
            PowerDexel* tempDexel = nullptr;

            for (int i = 0; i < 8; i += 2) { //初始化黑域和白域
                DexelIndex_X = X + VertexMask[i];
                DexelIndex_Y = Y + VertexMask[i + 1];
                tempDexel = DexelArray[DexelIndex_X][DexelIndex_Y];
                if (tempDexel == nullptr) {
                    Black == nullptr;
                    break;
                }
                if (Black == nullptr && tempDexel != nullptr) {
                    Black = new PowerDexel(tempDexel); //初始化黑域
                }
                if (Black != nullptr && tempDexel != nullptr) {
                    White = DexelBoolGetWhiteZone(White, DexelArray[DexelIndex_X][DexelIndex_Y]); //获取白域
                    Black = DexelBoolGetBlackZone(Black, DexelArray[DexelIndex_X][DexelIndex_Y]); //获取黑域
                }
            }

            //重点关照处理
            if (Black == nullptr) {
                return; //四个边角点均为空，跳过
            }
            tempDexel = Black; //先对白域进行适当扩展，再对黑域进行适当缩小,再获取灰域
            float SegmentLength;
            while (tempDexel != nullptr) { //对初始黑域进行适当缩小,对白域进行适当扩展
                SegmentLength = tempDexel->Second - tempDexel->First;
                tempDexel->First += (SegmentLength * 0.1);
                tempDexel->Second -= (SegmentLength * 0.1);
                tempDexel = tempDexel->Next;
            }
            tempDexel = White; //对白域的空档进行适当扩张
            while (tempDexel->Next != nullptr) {
                SegmentLength = std::min(tempDexel->Next->First - tempDexel->Second, tempDexel->Next->Second - tempDexel->Next->First);
                SegmentLength = std::min(SegmentLength, tempDexel->Second - tempDexel->First);
                SegmentLength = std::abs(SegmentLength);
                tempDexel->Second -= (SegmentLength * 0.1);
                tempDexel->Next->First += (SegmentLength * 0.1);
                tempDexel = tempDexel->Next;
            }
            /*********************************获取白域与黑域结束***********************************/
            /*********************************计算灰域并获取三角面片顶点****************************/
            tempDexel = Black; //根据黑域和白域获取灰域并获取三角形
            float BottomGrayMin = White->Second; //获取底部面片的灰域
            float BottomGrayMax = tempDexel->First;
            float TopGrayMin, TopGrayMax;
            PowerDexel* tempCornerDexel = nullptr;
            Vector3Df CornerPoint;
            PowerDexel* tempWhite = White;
            std::vector<Vector3Df> TrianglePoint;

            while (tempDexel != nullptr) { //遍历黑域
                if (White->Next == nullptr) {
                    break;
                }
                if (tempDexel->Next != nullptr) {
                    if (tempDexel->Next->Second < White->Next->First) { //黑域还未结束，必然存在下一个白域
                        tempDexel = tempDexel->Next;
                        continue;
                    } //否则，tempDexel与tempDexel->Next存在白域，可以在灰域中获取三角形
                } //否则，tempDexel已经没有下一个顶点，它的下一个区域就是白域，可以在灰域中获取三角形顶点
                TopGrayMin = tempDexel->Second; //获取顶部面片的灰域
                TopGrayMax = White->Next->First;
                for (int i = 0; i < 8; i = i + 2) { //遍历4个顶点，获取三角形的顶点
                    DexelIndex_X = X + VertexMask[i];
                    DexelIndex_Y = Y + VertexMask[i + 1];
                    tempCornerDexel = DexelArray[DexelIndex_X][DexelIndex_Y];
                    if (tempCornerDexel == nullptr) {
                        break;
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
            /*********************************计算灰域并获取三角面片顶点结束****************************/
            /*********************************初始化TriDexel****************************/
            if ((TrianglePoint.size() >= 8) && (TrianglePoint.size() % 8) == 0) {
                for (int i = 0; i < TrianglePoint.size(); i = i + 8) {
                    for (int j = 0; j < 8; j++) {
                        TriDexel->TrianglePoints.push_back(TrianglePoint[i + j]);
                    }
                    if ((i + 8) < TrianglePoint.size()) {
                        TriDexel->Next = new TriangleDexel();
                        //TriRenderDexel->Next = new TriangleRenderDexel();只能等到修改渲染列表时再初始化
                        TriDexel = TriDexel->Next;
                    }
                }
            }
            DeleteDexel(Black);
            DeleteDexel(tempWhite);
            /*********************************初始化TriDexel结束***********************/
        });
    });

    //修改渲染列表
    for (int X = x1; X <= x2; X++) {
        for (int Y = y1; Y <= y2; Y++) {
            if (TriangleDexelArray[X][Y] == nullptr) {
                continue;
            }
            TriangleDexel* tempTriDexel = TriangleDexelArray[X][Y];
            TriangleRenderDexel* tempTriRenderDexel = TriangleRenderDexelArray[X][Y]; //用于回收旧RenderIndex
            // std::cout << "Begin_getRenderIndex" << std::endl;
            /*********************************回收渲染列表坐标***********************/
            if (tempTriRenderDexel->IsChanged) { //首先回收渲染列表坐标，后面再分配
                while (tempTriRenderDexel != nullptr) {

                    if (tempTriRenderDexel->TriangleIndex >= 0) {
                        Vector3Df vector = Vector3Df(0.0f, 0.0f, 0.0f);
                        if (tempTriRenderDexel->IsAddSide) {
                            //Point数量为(2 + 4) * 2 * 3
                            VoidRenderIndexForSideList.push(tempTriRenderDexel->TriangleIndex);
                            for (int i = 0; i < 36; i++) {
                                vertices[tempTriRenderDexel->TriangleIndex + i] = vector;
                                normals[tempTriRenderDexel->TriangleIndex + i] = vector;
                            }
                        } else {
                            //Point数量为2 * 2 * 3
                            VoidRenderIndexList.push(tempTriRenderDexel->TriangleIndex);
                            for (int i = 0; i < 12; i++) {
                                vertices[tempTriRenderDexel->TriangleIndex + i] = vector;
                                normals[tempTriRenderDexel->TriangleIndex + i] = vector;
                            }
                        }
                    }
                    TriangleRenderDexel* temp = tempTriRenderDexel->Next;
                    tempTriRenderDexel->Next = nullptr;
                    delete tempTriRenderDexel;
                    tempTriRenderDexel = temp;
                }

            } else {
                continue;
            }
            // std::cout << "GetRenderIndex_End" << std::endl;
            /***************************回收渲染列表坐标结束**************************/
            /****************根据TriDexel分段初始化TriRenderDexel********************/
            TriangleRenderDexelArray[X][Y] = new TriangleRenderDexel(); //放到循环里创建
            tempTriRenderDexel = TriangleRenderDexelArray[X][Y];

            while ((tempTriDexel != nullptr)) { //初始化TriRenderDexel
                if (tempTriDexel->Next != nullptr) {
                    tempTriRenderDexel->Next = new TriangleRenderDexel();
                }
                tempTriDexel = tempTriDexel->Next;
                tempTriRenderDexel = tempTriRenderDexel->Next;
            }
            tempTriDexel = TriangleDexelArray[X][Y];
            tempTriRenderDexel = TriangleRenderDexelArray[X][Y];
            // std::cout << "Init_TriRenderDexel_End" << std::endl;
            /**********************初始化TriRenderDexel结束*************************/
            /************************开始组合面片并压入渲染列表************************/
            float TopZ_0, TopZ_1, BotZ_0, BotZ_1;
            float Nerighbor_TopZ_0, Nerighbor_TopZ_1, Nerighbor_BotZ_0, Nerighbor_BotZ_1;
            int Neighbor_X, Neighbor_Y;
            int AddSide = 0; //判断是否添加侧面面片
            while ((tempTriDexel != nullptr)) {
                if (tempTriDexel->TrianglePoints.size() != 8) {
                    tempTriDexel = tempTriDexel->Next;
                    tempTriRenderDexel = tempTriRenderDexel->Next;
                    continue;
                }
                AddSide = 0; //遍历四个邻居Dexel，判断是否需要添加侧面面片
                for (int i = 0; i < 4; i++) {
                    Neighbor_X = X + GetNeighbor[i][0];
                    Neighbor_Y = Y + GetNeighbor[i][1];
                    if ((Neighbor_X < 0) || (Neighbor_X >= (DexelArraySize - 1)) || (Neighbor_Y < 0) || (Neighbor_Y >= (DexelArraySize - 1)) || (TriangleDexelArray[Neighbor_X][Neighbor_Y] == nullptr)) {
                        AddSide = 0;
                        break;
                    }
                    TriangleDexel* NeighborDexel = TriangleDexelArray[Neighbor_X][Neighbor_Y];
                    float length_0, length_1;
                    BotZ_0 = tempTriDexel->TrianglePoints[NeighToSide[i][0] * 2].z;
                    BotZ_1 = tempTriDexel->TrianglePoints[NeighToSide[i][1] * 2].z;
                    TopZ_0 = tempTriDexel->TrianglePoints[NeighToSide[i][0] * 2 + 1].z;
                    TopZ_1 = tempTriDexel->TrianglePoints[NeighToSide[i][1] * 2 + 1].z;
                    while (NeighborDexel != nullptr) { //判断所有柱体
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
                        Nerighbor_BotZ_0 = Nerighbor_BotZ_0 - length_0 * 0.01; //适当扩大遮掩范围
                        Nerighbor_BotZ_1 = Nerighbor_BotZ_1 - length_1 * 0.01;
                        Nerighbor_TopZ_0 = Nerighbor_TopZ_0 + length_0 * 0.01;
                        Nerighbor_TopZ_1 = Nerighbor_TopZ_1 + length_1 * 0.01;

                        bool IsAddSide = ((Nerighbor_TopZ_0 >= TopZ_0) && (Nerighbor_TopZ_1 >= TopZ_1) && (Nerighbor_BotZ_0 <= BotZ_0) && (Nerighbor_BotZ_1 <= BotZ_1));

                        if (IsAddSide) { //该侧面被遮掩
                            AddSide++;
                            break;
                        }
                        NeighborDexel = NeighborDexel->Next;
                    }
                }

                tempTriRenderDexel->IsAddSide = (AddSide < 4) ? true : false;
                // std::cout << "Begin_FillPoint" << std::endl;
                //添加顶部和底部面片
                Vector3Df Point1 = tempTriDexel->TrianglePoints[0];
                Vector3Df Point2 = tempTriDexel->TrianglePoints[6];
                Vector3Df Point3 = tempTriDexel->TrianglePoints[2];
                Vector3Df Point4 = tempTriDexel->TrianglePoints[4];
                Vector3Df Normal1 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                float dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, -1.0f));
                if (dot1 < 0.0f) {
                    Normal1 = -Normal1;
                }

                if (tempTriRenderDexel->IsAddSide) {
                    if (VoidRenderIndexForSideList.empty()) {
                        tempTriRenderDexel->TriangleIndex = vertices.size(); //顶点索引
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices.push_back(Point3);
                        vertices.push_back(Point2);
                        vertices.push_back(Point1);
                        vertices.push_back(Point2);
                        vertices.push_back(Point4);
                        vertices.push_back(Point1);
                        for (int i = 0; i < 6; i++) {
                            normals.push_back(Normal1 * (1.0f));
                        }
                        Point1 = tempTriDexel->TrianglePoints[1];
                        Point2 = tempTriDexel->TrianglePoints[7];
                        Point3 = tempTriDexel->TrianglePoints[3];
                        Point4 = tempTriDexel->TrianglePoints[5];
                        Normal1 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                        dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, 1.0f));
                        if (dot1 < 0.0f) {
                            Normal1 = -Normal1;
                        }
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices.push_back(Point1);
                        vertices.push_back(Point2);
                        vertices.push_back(Point3);
                        vertices.push_back(Point1);
                        vertices.push_back(Point4);
                        vertices.push_back(Point2);
                        for (int i = 0; i < 6; i++) {
                            normals.push_back(Normal1);
                        }

                        //添加侧面
                        for (int m = 0; m < 4; m++) {
                            Point1 = tempTriDexel->TrianglePoints[NeighToSide[m][0] * 2]; //BotZ_0
                            Point2 = tempTriDexel->TrianglePoints[NeighToSide[m][1] * 2]; //BotZ_1
                            Point3 = tempTriDexel->TrianglePoints[NeighToSide[m][0] * 2 + 1]; //TopZ_0
                            Point4 = tempTriDexel->TrianglePoints[NeighToSide[m][1] * 2 + 1]; //TopZ_1
                            Normal1 = (Point1 - Point2).cross(Point1 - Point3);
                            Normal1.normalize();
                            Point1 = Point1 / MaxLength;
                            Point2 = Point2 / MaxLength;
                            Point3 = Point3 / MaxLength;
                            Point4 = Point4 / MaxLength;
                            DexelTriangles.push_back(Point1);
                            DexelTriangles.push_back(Point2);
                            DexelTriangles.push_back(Point4);
                            DexelTriangles.push_back(Point1);
                            DexelTriangles.push_back(Point4);
                            DexelTriangles.push_back(Point3);
                            for (int n = 0; n < 6; n++) {
                                DexelTrianglesNormals.push_back(Normal1);
                            }
                        }

                    } else {
                        tempTriRenderDexel->TriangleIndex = VoidRenderIndexForSideList.top();
                        VoidRenderIndexForSideList.pop();
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices[tempTriRenderDexel->TriangleIndex + 0] = Point3;
                        vertices[tempTriRenderDexel->TriangleIndex + 1] = Point2;
                        vertices[tempTriRenderDexel->TriangleIndex + 2] = Point1;
                        vertices[tempTriRenderDexel->TriangleIndex + 3] = Point2;
                        vertices[tempTriRenderDexel->TriangleIndex + 4] = Point4;
                        vertices[tempTriRenderDexel->TriangleIndex + 5] = Point1;
                        for (int i = 0; i < 6; i++) {
                            normals[tempTriRenderDexel->TriangleIndex + i] = Normal1;
                        }
                        Point1 = tempTriDexel->TrianglePoints[1];
                        Point2 = tempTriDexel->TrianglePoints[7];
                        Point3 = tempTriDexel->TrianglePoints[3];
                        Point4 = tempTriDexel->TrianglePoints[5];
                        Normal1 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                        dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, 1.0f));
                        if (dot1 < 0.0f) {
                            Normal1 = -Normal1;
                        }
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices[tempTriRenderDexel->TriangleIndex + 6] = Point1;
                        vertices[tempTriRenderDexel->TriangleIndex + 7] = Point2;
                        vertices[tempTriRenderDexel->TriangleIndex + 8] = Point3;
                        vertices[tempTriRenderDexel->TriangleIndex + 9] = Point1;
                        vertices[tempTriRenderDexel->TriangleIndex + 10] = Point4;
                        vertices[tempTriRenderDexel->TriangleIndex + 11] = Point2;
                        for (int i = 0; i < 6; i++) {
                            normals[tempTriRenderDexel->TriangleIndex + 6 + i] = Normal1;
                        }

                        int SideRenderIndex = tempTriRenderDexel->TriangleIndex + 12;
                        //添加侧面
                        for (int m = 0; m < 4; m++) {
                            Point1 = tempTriDexel->TrianglePoints[NeighToSide[m][0] * 2]; //BotZ_0
                            Point2 = tempTriDexel->TrianglePoints[NeighToSide[m][1] * 2]; //BotZ_1
                            Point3 = tempTriDexel->TrianglePoints[NeighToSide[m][0] * 2 + 1]; //TopZ_0
                            Point4 = tempTriDexel->TrianglePoints[NeighToSide[m][1] * 2 + 1]; //TopZ_1
                            Normal1 = (Point1 - Point2).cross(Point1 - Point3);
                            Normal1.normalize();
                            Point1 = Point1 / MaxLength;
                            Point2 = Point2 / MaxLength;
                            Point3 = Point3 / MaxLength;
                            Point4 = Point4 / MaxLength;
                            vertices[SideRenderIndex + m * 6 + 0] = Point1;
                            vertices[SideRenderIndex + m * 6 + 1] = Point2;
                            vertices[SideRenderIndex + m * 6 + 2] = Point4;
                            vertices[SideRenderIndex + m * 6 + 3] = Point1;
                            vertices[SideRenderIndex + m * 6 + 4] = Point4;
                            vertices[SideRenderIndex + m * 6 + 5] = Point3;
                            for (int n = 0; n < 6; n++) {
                                normals[SideRenderIndex + m * 6 + n] = Normal1;
                            }
                        }
                    }
                } else {
                    if (VoidRenderIndexList.empty()) { //不添加侧面面片
                        tempTriRenderDexel->TriangleIndex = vertices.size(); //顶点索引
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices.push_back(Point3);
                        vertices.push_back(Point2);
                        vertices.push_back(Point1);
                        vertices.push_back(Point2);
                        vertices.push_back(Point4);
                        vertices.push_back(Point1);
                        for (int i = 0; i < 6; i++) {
                            normals.push_back(Normal1 * (1.0f));
                        }
                        Point1 = tempTriDexel->TrianglePoints[1];
                        Point2 = tempTriDexel->TrianglePoints[7];
                        Point3 = tempTriDexel->TrianglePoints[3];
                        Point4 = tempTriDexel->TrianglePoints[5];
                        Normal1 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                        dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, 1.0f));
                        if (dot1 < 0.0f) {
                            Normal1 = -Normal1;
                        }
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices.push_back(Point1);
                        vertices.push_back(Point2);
                        vertices.push_back(Point3);
                        vertices.push_back(Point1);
                        vertices.push_back(Point4);
                        vertices.push_back(Point2);
                        for (int i = 0; i < 6; i++) {
                            normals.push_back(Normal1);
                        }
                    } else {
                        tempTriRenderDexel->TriangleIndex = VoidRenderIndexList.top();
                        VoidRenderIndexList.pop();
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices[tempTriRenderDexel->TriangleIndex + 0] = Point3;
                        vertices[tempTriRenderDexel->TriangleIndex + 1] = Point2;
                        vertices[tempTriRenderDexel->TriangleIndex + 2] = Point1;
                        vertices[tempTriRenderDexel->TriangleIndex + 3] = Point2;
                        vertices[tempTriRenderDexel->TriangleIndex + 4] = Point4;
                        vertices[tempTriRenderDexel->TriangleIndex + 5] = Point1;
                        for (int i = 0; i < 6; i++) {
                            normals[tempTriRenderDexel->TriangleIndex + i] = Normal1;
                        }
                        Point1 = tempTriDexel->TrianglePoints[1];
                        Point2 = tempTriDexel->TrianglePoints[7];
                        Point3 = tempTriDexel->TrianglePoints[3];
                        Point4 = tempTriDexel->TrianglePoints[5];
                        Normal1 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                        dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, 1.0f));
                        if (dot1 < 0.0f) {
                            Normal1 = -Normal1;
                        }
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices[tempTriRenderDexel->TriangleIndex + 6] = Point1;
                        vertices[tempTriRenderDexel->TriangleIndex + 7] = Point2;
                        vertices[tempTriRenderDexel->TriangleIndex + 8] = Point3;
                        vertices[tempTriRenderDexel->TriangleIndex + 9] = Point1;
                        vertices[tempTriRenderDexel->TriangleIndex + 10] = Point4;
                        vertices[tempTriRenderDexel->TriangleIndex + 11] = Point2;
                        for (int i = 0; i < 6; i++) {
                            normals[tempTriRenderDexel->TriangleIndex + 6 + i] = Normal1;
                        }
                    }
                }

                tempTriDexel = tempTriDexel->Next;
                tempTriRenderDexel = tempTriRenderDexel->Next;
            }

            // std::cout << "FillPoint_End" << std::endl;
        }
    }
}

void MRR_Dexel::MillingWithCutter_5axis_RenderType(std::unique_ptr<Cutter>& cutter, const OBB3Df& obb)
{
    Vector3Df Min = ABBox.getMin();
    Vector3Df Max = ABBox.getMax();
    float MaxLength = std::max(std::max(Max.x - Min.x, Max.y - Min.y), Max.z - Min.z);

    int x1, x2, y1, y2;
    getXYFromOBB(obb, x1, x2, y1, y2);
    int WholeSize = (x2 - x1) * (y2 - y1);
    int countDexelChanged = 0;
    float PieceOfchangedVolume = 0;

    //不知道为什么需要加1
    //Dexel模型与刀具进行布尔运算并更新
    std::for_each(std::execution::par, DexelArray.begin() + x1, DexelArray.begin() + x2 + 1, [&](std::vector<PowerDexel*>& DexelArrayX) {
        int X = &DexelArrayX - &DexelArray[0];
        std::for_each(std::execution::par, DexelArrayX.begin() + y1, DexelArrayX.begin() + y2 + 1, [&](PowerDexel*& dexel) {
            if (dexel == nullptr) {
                return;
            }
            int Y = &dexel - &DexelArrayX[0];
            dexel->IsChanged = false;
            Vector3Df Center = getDexelCenterWithCoordOffset(X, Y);
            float z = 10000.0f; //cutter->getCutterSurfaceHeight(Center.x, Center.y) + CoordOffset.z;//修正
            if (z > dexel->getMaxZ()) {
                return;
            }
            Vector3Df cutterHeigth = Vector3Df(z, 10000, 0);
            dexel = DexelBoolCalculation(dexel, cutterHeigth.x, cutterHeigth.y);
            dexel->IsChanged = true;

            dexel->CuttePoint = Vector3Df(0, 0, 1); //cutter->CurrentPosture.center;//修正
        });
    });

    //计算要在TriangleArray中使用的坐标
    x1 = std::max(x1 - 1, 0);
    x2 = std::min(x2, int(TriangleDexelArray.size() - 1));
    y1 = std::max(y1 - 1, 0);
    y2 = std::min(y2, int(TriangleDexelArray.size() - 1));

    //更新Dexel模型对应的TriangleDexel模型，需要更新的地方将IsChanged设为为true，并更新存储的对应的顶点
    std::for_each(std::execution::par, TriangleDexelArray.begin() + x1, TriangleDexelArray.begin() + x2 + 1, [&](std::vector<TriangleDexel*>& TriangleDexelArrayX) {
        int X = &TriangleDexelArrayX - &TriangleDexelArray[0];
        std::for_each(std::execution::par, TriangleDexelArrayX.begin() + y1, TriangleDexelArrayX.begin() + y2 + 1, [&](TriangleDexel*& TriDexel) {
            if (TriDexel == nullptr) {
                return;
            }
            int Y = &TriDexel - &TriangleDexelArrayX[0];
            bool Ischanged = false;
            bool SetToVoid = false;
            int DexelIndex_X, DexelIndex_Y;
            TriangleDexel* tempTriDexel = TriDexel;
            TriangleRenderDexel* tempTriRender = TriangleRenderDexelArray[X][Y];

            for (int i = 0; i < 8; i = i + 2) { //查看是否需要修改该三角面片
                DexelIndex_X = X + VertexMask[i];
                DexelIndex_Y = Y + VertexMask[i + 1];
                //cornerDexel中存在空指针，后续无法生成面片，需要仔细判断原有面片
                if (DexelArray[DexelIndex_X][DexelIndex_Y] == nullptr) {
                    //将其设为changed,清空TriPoint,后续合成三角面片填入渲染列表时再将其TriDexel和TriRender删除
                    //并将空出来的位置填入填入空闲列表以待备用
                    tempTriRender->IsChanged = true;
                    Ischanged = false; //助其跳过后续的计算
                    break;
                }
                if (DexelArray[DexelIndex_X][DexelIndex_Y]->IsChanged) {
                    Ischanged = true;
                    break;
                }
            }

            if (!Ischanged) {
                return;
            }
            /******************通过DexelArray的四条边，计算DexelTriangleArray**********************/
            //删除原有的TriDexel三角面片顶点并回收内存,创建新的TriDexel，TriRender等到下一步再删除并创建新的TriRender，这里只是将其设为changed，
            tempTriRender->IsChanged = true;
            while (tempTriDexel != nullptr) {
                TriangleDexel* childTridexel = tempTriDexel->Next;
                tempTriDexel->Next = nullptr;
                delete tempTriDexel;
                tempTriDexel = childTridexel;
            }

            TriDexel = new TriangleDexel();
            tempTriDexel = TriDexel;
            /*********************************获取白域与黑域开始***********************************/
            PowerDexel* White = new PowerDexel(-1000.0f, 1000.0f);
            PowerDexel* Black = nullptr; //初始化白域和黑域
            PowerDexel* tempDexel = nullptr;

            for (int i = 0; i < 8; i += 2) { //初始化黑域和白域
                DexelIndex_X = X + VertexMask[i];
                DexelIndex_Y = Y + VertexMask[i + 1];
                tempDexel = DexelArray[DexelIndex_X][DexelIndex_Y];
                if (tempDexel == nullptr) {
                    Black == nullptr;
                    break;
                }
                if (Black == nullptr && tempDexel != nullptr) {
                    Black = new PowerDexel(tempDexel); //初始化黑域
                }
                if (Black != nullptr && tempDexel != nullptr) {
                    White = DexelBoolGetWhiteZone(White, DexelArray[DexelIndex_X][DexelIndex_Y]); //获取白域
                    Black = DexelBoolGetBlackZone(Black, DexelArray[DexelIndex_X][DexelIndex_Y]); //获取黑域
                }
            }

            //重点关照处理
            if (Black == nullptr) {
                return; //四个边角点均为空，跳过
            }
            tempDexel = Black; //先对白域进行适当扩展，再对黑域进行适当缩小,再获取灰域
            float SegmentLength;
            while (tempDexel != nullptr) { //对初始黑域进行适当缩小,对白域进行适当扩展
                SegmentLength = tempDexel->Second - tempDexel->First;
                tempDexel->First += (SegmentLength * 0.1);
                tempDexel->Second -= (SegmentLength * 0.1);
                tempDexel = tempDexel->Next;
            }
            tempDexel = White; //对白域的空档进行适当扩张
            while (tempDexel->Next != nullptr) {
                SegmentLength = std::min(tempDexel->Next->First - tempDexel->Second, tempDexel->Next->Second - tempDexel->Next->First);
                SegmentLength = std::min(SegmentLength, tempDexel->Second - tempDexel->First);
                SegmentLength = std::abs(SegmentLength);
                tempDexel->Second -= (SegmentLength * 0.1);
                tempDexel->Next->First += (SegmentLength * 0.1);
                tempDexel = tempDexel->Next;
            }
            /*********************************获取白域与黑域结束***********************************/
            /*********************************计算灰域并获取三角面片顶点****************************/
            tempDexel = Black; //根据黑域和白域获取灰域并获取三角形
            float BottomGrayMin = White->Second; //获取底部面片的灰域
            float BottomGrayMax = tempDexel->First;
            float TopGrayMin, TopGrayMax;
            PowerDexel* tempCornerDexel = nullptr;
            Vector3Df CornerPoint;
            PowerDexel* tempWhite = White;
            std::vector<Vector3Df> TrianglePoint;
            while (tempDexel != nullptr) { //遍历黑域
                if (White->Next == nullptr) {
                    break;
                }
                if (tempDexel->Next != nullptr) {
                    if (tempDexel->Next->Second < White->Next->First) { //黑域还未结束，必然存在下一个白域
                        tempDexel = tempDexel->Next;
                        continue;
                    } //否则，tempDexel与tempDexel->Next存在白域，可以在灰域中获取三角形
                } //否则，tempDexel已经没有下一个顶点，它的下一个区域就是白域，可以在灰域中获取三角形顶点
                TopGrayMin = tempDexel->Second; //获取顶部面片的灰域
                TopGrayMax = White->Next->First;
                for (int i = 0; i < 8; i = i + 2) { //遍历4个顶点，获取三角形的顶点
                    DexelIndex_X = X + VertexMask[i];
                    DexelIndex_Y = Y + VertexMask[i + 1];
                    tempCornerDexel = DexelArray[DexelIndex_X][DexelIndex_Y];
                    if (tempCornerDexel == nullptr) {
                        break;
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
            /*********************************计算灰域并获取三角面片顶点结束****************************/
            /*********************************初始化TriDexel****************************/
            if ((TrianglePoint.size() >= 8) && (TrianglePoint.size() % 8) == 0) {
                for (int i = 0; i < TrianglePoint.size(); i = i + 8) {
                    for (int j = 0; j < 8; j++) {
                        TriDexel->TrianglePoints.push_back(TrianglePoint[i + j]);
                    }
                    if ((i + 8) < TrianglePoint.size()) {
                        TriDexel->Next = new TriangleDexel();
                        //TriRenderDexel->Next = new TriangleRenderDexel();只能等到修改渲染列表时再初始化
                        TriDexel = TriDexel->Next;
                    }
                }
            }
            DeleteDexel(Black);
            DeleteDexel(tempWhite);
            /*********************************初始化TriDexel结束***********************/
        });
    });

    std::vector<int> SideTriIndex;
    //修改渲染列表
    for (int X = x1; X <= x2; X++) {
        for (int Y = y1; Y <= y2; Y++) {
            if (TriangleDexelArray[X][Y] == nullptr) {
                continue;
            }
            TriangleDexel* tempTriDexel = TriangleDexelArray[X][Y];
            TriangleRenderDexel* tempTriRenderDexel = TriangleRenderDexelArray[X][Y]; //用于回收旧RenderIndex
            // std::cout << "Begin_getRenderIndex" << std::endl;
            /*********************************回收渲染列表坐标***********************/
            if (tempTriRenderDexel->IsChanged) { //首先回收渲染列表坐标，后面再分配
                while (tempTriRenderDexel != nullptr) {

                    if (tempTriRenderDexel->TriangleIndex >= 0) {
                        Vector3Df vector = Vector3Df(-10000.0f, -10000.0f, -10000.0f);
                        VoidRenderIndexList.push(tempTriRenderDexel->TriangleIndex);
                        CubeCenter[tempTriRenderDexel->TriangleIndex] = vector;
                        CubeNormals[tempTriRenderDexel->TriangleIndex] = Vector3Df(0.0f, 0.0f, 0.0f);
                        //注意检查fill_n的使用是否正确
                        std::fill_n(PointHeight.begin() + tempTriRenderDexel->TriangleIndex * 8, 8, 0.0f);
                    }

                    TriangleRenderDexel* temp = tempTriRenderDexel->Next;
                    tempTriRenderDexel->Next = nullptr;
                    delete tempTriRenderDexel;
                    tempTriRenderDexel = temp;
                }

            } else {
                continue;
            }
            /***************************回收渲染列表坐标结束**************************/
            /****************根据TriDexel分段初始化TriRenderDexel********************/
            TriangleRenderDexelArray[X][Y] = new TriangleRenderDexel(); //放到循环里创建
            tempTriRenderDexel = TriangleRenderDexelArray[X][Y];

            while ((tempTriDexel != nullptr)) { //初始化TriRenderDexel
                if (tempTriDexel->Next != nullptr) {
                    tempTriRenderDexel->Next = new TriangleRenderDexel();
                }
                tempTriDexel = tempTriDexel->Next;
                tempTriRenderDexel = tempTriRenderDexel->Next;
            }
            tempTriDexel = TriangleDexelArray[X][Y];
            tempTriRenderDexel = TriangleRenderDexelArray[X][Y];
            // std::cout << "Init_TriRenderDexel_End" << std::endl;
            /**********************初始化TriRenderDexel结束*************************/

            /************************开始组合面片并压入渲染列表************************/
            float TopZ_0, TopZ_1, BotZ_0, BotZ_1;
            float Nerighbor_TopZ_0, Nerighbor_TopZ_1, Nerighbor_BotZ_0, Nerighbor_BotZ_1;
            int Neighbor_X, Neighbor_Y;
            int AddSide = 0; //判断是否添加侧面面片
            while ((tempTriDexel != nullptr)) {
                if (tempTriDexel->TrianglePoints.size() != 8) {
                    tempTriDexel = tempTriDexel->Next;
                    tempTriRenderDexel = tempTriRenderDexel->Next;
                    continue;
                }
                AddSide = 0; //遍历四个邻居Dexel，判断是否需要添加侧面面片
                for (int i = 0; i < 4; i++) {
                    Neighbor_X = X + GetNeighbor[i][0];
                    Neighbor_Y = Y + GetNeighbor[i][1];
                    if ((Neighbor_X < 0) || (Neighbor_X >= (DexelArraySize - 1)) || (Neighbor_Y < 0) || (Neighbor_Y >= (DexelArraySize - 1)) || (TriangleDexelArray[Neighbor_X][Neighbor_Y] == nullptr)) {
                        AddSide = 0;
                        break;
                    }
                    TriangleDexel* NeighborDexel = TriangleDexelArray[Neighbor_X][Neighbor_Y];
                    float length_0, length_1;
                    BotZ_0 = tempTriDexel->TrianglePoints[NeighToSide[i][0] * 2].z;
                    BotZ_1 = tempTriDexel->TrianglePoints[NeighToSide[i][1] * 2].z;
                    TopZ_0 = tempTriDexel->TrianglePoints[NeighToSide[i][0] * 2 + 1].z;
                    TopZ_1 = tempTriDexel->TrianglePoints[NeighToSide[i][1] * 2 + 1].z;
                    while (NeighborDexel != nullptr) { //判断所有柱体
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
                        Nerighbor_BotZ_0 = Nerighbor_BotZ_0 - length_0 * 0.01; //适当扩大遮掩范围
                        Nerighbor_BotZ_1 = Nerighbor_BotZ_1 - length_1 * 0.01;
                        Nerighbor_TopZ_0 = Nerighbor_TopZ_0 + length_0 * 0.01;
                        Nerighbor_TopZ_1 = Nerighbor_TopZ_1 + length_1 * 0.01;

                        bool IsAddSide = ((Nerighbor_TopZ_0 >= TopZ_0) && (Nerighbor_TopZ_1 >= TopZ_1) && (Nerighbor_BotZ_0 <= BotZ_0) && (Nerighbor_BotZ_1 <= BotZ_1));

                        if (IsAddSide) { //该侧面被遮掩
                            AddSide++;
                            break;
                        }
                        NeighborDexel = NeighborDexel->Next;
                    }
                }

                tempTriRenderDexel->IsAddSide = (AddSide < 4) ? true : false;
                // std::cout << "Begin_FillPoint" << std::endl;
                //添加顶部和底部面片
                Vector3Df SideNormal = Vector3Df(0.0f, 0.0f, 0.0f);
                if (VoidRenderIndexList.empty()) {

                    tempTriRenderDexel->TriangleIndex = CubeCenter.size();
                    CubeCenter.push_back((tempTriDexel->TrianglePoints[0] / MaxLength));
                    for (int i = 0; i < 8; i++) {
                        PointHeight.push_back((tempTriDexel->TrianglePoints[i].z / MaxLength));
                    }
                    CubeNormals.push_back(SideNormal); //在着色器中通过法向量的模来判断是否需要添加面片
                } else {
                    tempTriRenderDexel->TriangleIndex = VoidRenderIndexList.top();
                    VoidRenderIndexList.pop();
                    CubeCenter[tempTriRenderDexel->TriangleIndex] = (tempTriDexel->TrianglePoints[0] / MaxLength);
                    for (int i = 0; i < 8; i++) {
                        PointHeight[tempTriRenderDexel->TriangleIndex * 8 + i] = (tempTriDexel->TrianglePoints[i].z / MaxLength);
                    }
                }

                //修改CubeNormal用于使用侧面面片
                if (tempTriRenderDexel->IsAddSide) {
                    // //记录是否有添加测面面片，用来计算Dexel节点在渲染列表该位置填入了多少顶点：顶面+底面+侧面=6+6+4*6=36

                    SideNormal = Vector3Df(0.0f, 0.0f, 3.0f);
                }
                CubeNormals[tempTriRenderDexel->TriangleIndex] = SideNormal; //添加侧面

                //判断是否为垂直面片，垂直面片的法向量需要根据相邻的垂直面片的拓扑信息进行计算
                //TopFace: 1357 ->15,13,17，先测试顶部面片的效果，底部面片稍后添加
                Vector3Df Line17 = (tempTriDexel->TrianglePoints[7] - tempTriDexel->TrianglePoints[1]).normalize();
                Vector3Df Line13 = (tempTriDexel->TrianglePoints[3] - tempTriDexel->TrianglePoints[1]).normalize();
                Vector3Df Line15 = (tempTriDexel->TrianglePoints[5] - tempTriDexel->TrianglePoints[1]).normalize();

                Vector3Df TopNormal1 = (Line15.cross(Line17)).normalize();
                Vector3Df TopNormal2 = (Line17.cross(Line13)).normalize();

                if ((abs(TopNormal1.z) < 0.05f) || (abs(TopNormal2.z) < 0.05f)) {
                    // tempTriRenderDexel->isDeleta = true; //面片接近垂直，对其进行标记
                    // SideTriIndex.push_back(X); //记录需要重置法向量的面片的索引
                    // SideTriIndex.push_back(Y);
                    // CubeNormals[tempTriRenderDexel->TriangleIndex] = TopNormal1;

                    Vector3Df LastNormal = Vector3Df(0.0f, 0.0f, 1.0f); //cutter->CurrentPosture.center - LastCutterPos;////修正
                    LastNormal.z = 0;
                    LastNormal = LastNormal.normalize();
                    LastNormal = LastNormal.cross(Vector3Df(0.0f, 0.0f, 1.0f));
                    LastNormal.normalize();
                    LastNormal = LastNormal * 5.0f; //将该向量的模设为5，用于在shader中判断是否为需要处理top面片
                    CubeNormals[tempTriRenderDexel->TriangleIndex] = LastNormal;

                } else {
                    tempTriRenderDexel->isDeleta = false;
                }

                tempTriDexel = tempTriDexel->Next;
                tempTriRenderDexel = tempTriRenderDexel->Next;
            }
        }
    }

    Vector3Df MaskNormal = Vector3Df(0, 0, 1);

    for (int i = 0; i < SideTriIndex.size(); i = i + 2) {
        int X = SideTriIndex[i];
        int Y = SideTriIndex[i + 1];
        TriangleDexel* tempTriDexel = TriangleDexelArray[X][Y];
        TriangleRenderDexel* tempTriRenderDexel = TriangleRenderDexelArray[X][Y];
        Vector3Df CenterPoint = getDexelCenter(X, Y);

        //为当前需要重置法向量的面片寻找相邻的垂直面片用于计算拓扑法向量
        while (tempTriDexel != nullptr) {

            if ((!tempTriRenderDexel->isDeleta)) {
                tempTriDexel = tempTriDexel->Next;
                tempTriRenderDexel = tempTriRenderDexel->Next;
                continue;
            }

            // faceNormal = DexelTrianglesNormals[tempTriRenderDexel->TriangleIndex + 13];
            Vector3Df LastNormal = Vector3Df(0.0f, 0.0f, 0.0f);
            Vector3Df SideNormal, FaceNormal;
            Vector3Df NeigthborPoint;
            FaceNormal = CubeNormals[tempTriRenderDexel->TriangleIndex];
            int OperatorSize = 2; //算子大小(OperatorSize * 2 + 1)
            //5 * 5 的int数组
            std::vector<std::vector<int>> AddSideArray = std::vector<std::vector<int>>(OperatorSize * 2 + 1, std::vector<int>(OperatorSize * 2 + 1, 0));

            //搜索25格内所有Delta为true的TriDexel
            for (int m = X - OperatorSize; m <= X + OperatorSize; m++) {
                for (int n = Y - OperatorSize; n <= Y + OperatorSize; n++) {

                    if ((m == X) && (n == Y)) {
                        continue;
                    }
                    if ((m < 0) || (m >= (DexelArraySize - 1)) || (n < 0) || (n >= (DexelArraySize - 1)) || (TriangleDexelArray[m][n] == nullptr)) {
                        continue;
                    }
                    TriangleRenderDexel* tempNeiTriRenderDexel = TriangleRenderDexelArray[m][n];
                    //本来还需要判断两者是否处于同一个平面，目前暂时先简单处理
                    while (tempNeiTriRenderDexel != nullptr) {
                        if (tempNeiTriRenderDexel->isDeleta) {
                            int ArrayIndex_X = m - (X - OperatorSize);
                            int ArrayIndex_Y = n - (Y - OperatorSize);
                            AddSideArray[ArrayIndex_X][ArrayIndex_Y] = 1;
                            break;
                        }
                        tempNeiTriRenderDexel = tempNeiTriRenderDexel->Next;
                    }
                }
            }
            // std::cout << "Begin_CountNormal" << std::endl;
            //整合拓扑信息计算最终法向量
            int count = 0;
            Vector3Df FirstPoint = Vector3Df(0.0f, 0.0f, 0.0f);
            Vector3Df TempPoint = Vector3Df(0.0f, 0.0f, 0.0f);
            ;
            int FirstPointIndex = -1;

            for (int m = X - OperatorSize; m <= X + OperatorSize; m++) {
                for (int n = Y - OperatorSize; n <= Y + OperatorSize; n++) {

                    if ((m == X) && (n == Y)) {
                        continue;
                    }
                    int ArrayIndex_X = m - (X - OperatorSize);
                    int ArrayIndex_Y = n - (Y - OperatorSize);

                    if (AddSideArray[ArrayIndex_X][ArrayIndex_Y] == 0)
                        continue;
                    // std::cout << "ArrayIndex_X:" << ArrayIndex_X << "ArrayIndex_Y:" << ArrayIndex_Y << std::endl;
                    if (FirstPointIndex == -1) {
                        FirstPointIndex = 0;
                        FirstPoint = getDexelCenter(m, n);
                        // NeigthborPoint = CenterPoint - NeigthborPoint;
                        // float SideLength = NeigthborPoint.length();
                        // NeigthborPoint = NeigthborPoint.normalize();
                        // SideNormal = NeigthborPoint.cross(MaskNormal);
                        // SideNormal = SideNormal.normalize();
                        // LastNormal = LastNormal + SideNormal * SideLength; //SideLength;
                        continue;
                    }

                    NeigthborPoint = getDexelCenter(m, n);

                    // if (m < X) {
                    //     NeigthborPoint = NeigthborPoint - CenterPoint;
                    // } else if (m == X) {
                    //     NeigthborPoint = Vector3Df(0.0f, 0.0f, 0.0f);
                    // } else {
                    //     //m>x
                    //     NeigthborPoint = CenterPoint - NeigthborPoint;
                    // }

                    // TempPoint = NeigthborPoint - FirstPoint;
                    NeigthborPoint = NeigthborPoint - FirstPoint;

                    // if (TempPoint.dot(NeigthborPoint) > 0.0f) {
                    //     NeigthborPoint = NeigthborPoint * (-1.0f);
                    //     //NeigthborPoint.x = NeigthborPoint.x * (-1.0f);
                    //     // NeigthborPoint.x = NeigthborPoint.y * (-1.0f);
                    // }

                    // NeigthborPoint = NeigthborPoint - FirstPoint;

                    float SideLength = NeigthborPoint.length();
                    // if ((m == X) && (n == Y)) {
                    //     SideLength = NeigthborPoint.length() / 2;
                    // }

                    NeigthborPoint = NeigthborPoint.normalize();
                    SideNormal = NeigthborPoint.cross(MaskNormal);

                    // float weigth = std::abs(m - X) + std::abs(n - Y);
                    // if (FaceNormal.dot(SideNormal) < 0.0f)
                    //     SideNormal = SideNormal * (-1.0f);

                    // //重要,用于同一法向量的朝向
                    // if (m < X) {
                    //     SideNormal = SideNormal * (-1.0f);
                    // }
                    // else if (m == X) {
                    //     if (n >= Y) {
                    //         SideNormal = SideNormal * (-1.0f);
                    //     }
                    //     // SideNormal = SideNormal * (-1.0f);
                    // }
                    //以邻居点和中心点的长度作为加权，它们之间的距离越远，权重越大
                    SideNormal = SideNormal.normalize();
                    LastNormal = LastNormal + SideNormal * SideLength; //SideLength;
                }
            }
            LastNormal = LastNormal.normalize();
            //将LastNormal中的float的小数点去掉
            // LastNormal.x = floor(LastNormal.x * 10.0f) / (10.0f);
            // LastNormal.y = floor(LastNormal.y * 10.0f) / (10.0f);
            // LastNormal.z = 0;
            // // std::cout << "LastNormal: " << LastNormal;
            // std::cout << "End_CountNormal" << std::endl;

            /**************************************************************/
            //取巧，直接使用刀位点来计算法向量
            LastNormal = Vector3Df(0.0f, 0.0f, 1.0f); //cutter->CurrentPosture.center - LastCutterPos;//修正
            LastNormal.z = 0;
            LastNormal = LastNormal.normalize();
            LastNormal = LastNormal.cross(Vector3Df(0.0f, 0.0f, 1.0f));
            LastNormal.normalize();
            /**************************************************************/

            // int NormalIndex = tempTriRenderDexel->TriangleIndex + 1;
            int NormalIndex = tempTriRenderDexel->TriangleIndex;
            // LastNormal = Vector3Df(0.0f, 0.0f, 1.0f);
            LastNormal = LastNormal * 5.0f; //将该向量的模设为5，用于在shader中判断是否为需要处理top面片
            // LastNormal = Vector3Df(0.0f, 0.0f, 0.0f);
            CubeNormals[NormalIndex] = LastNormal;

            // tempTriRenderDexel->isDeleta = false;
            tempTriDexel = tempTriDexel->Next;
            tempTriRenderDexel = tempTriRenderDexel->Next;
        }
    }

    LastCutterPos = Vector3Df(0, 0, 1); //cutter->CurrentPosture.center;//修正

    this->CalculateWorkPieceVolume();
}

void MRR_Dexel::updateVerticesAndNormals(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals)
{
    // if (CountStep < 1000) {
    //     CountStep++;
    //     return;
    // } else {
    //     CountStep = 0;
    // }
    // std::cout << "updateVerticesAndNormals_Begin" << std::endl;
    Vector3Df Vertex1, Vertex2, Vertex3, Normal;
    Vector3Df Min = ABBox.getMin();
    Vector3Df Max = ABBox.getMax();
    float MaxLength = std::max(std::max(Max.x - Min.x, Max.y - Min.y), Max.z - Min.z);
    vertices.clear();
    normals.clear();

    for (int X = 0; X < DexelArraySize - 1; X++) {
        for (int Y = 0; Y < DexelArraySize - 1; Y++) {
            if (DexelArray[X + 1][Y + 1] == nullptr) {
                continue;
            };
            TriangleDexel* TriDexel = new TriangleDexel();
            TriangleRenderDexel* TriRenderDexel = new TriangleRenderDexel();
            TriangleDexelArray[X][Y] = TriDexel;
            TriangleRenderDexelArray[X][Y] = TriRenderDexel;
            PowerDexel* White = new PowerDexel(-1000.0f, 1000.0f);
            PowerDexel* Black = nullptr;
            int DexelIndex_X, DexelIndex_Y;
            PowerDexel* tempDexel = nullptr;
            for (int i = 0; i < 8; i += 2) {
                DexelIndex_X = X + VertexMask[i];
                DexelIndex_Y = Y + VertexMask[i + 1];
                tempDexel = DexelArray[DexelIndex_X][DexelIndex_Y];
                if (tempDexel == nullptr) {
                    Black == nullptr;
                    break;
                }
                if (Black == nullptr && tempDexel != nullptr) {
                    Black = new PowerDexel(tempDexel); //初始化黑域
                }
                if (Black != nullptr && tempDexel != nullptr) {
                    White = DexelBoolGetWhiteZone(White, DexelArray[DexelIndex_X][DexelIndex_Y]);
                    Black = DexelBoolGetBlackZone(Black, DexelArray[DexelIndex_X][DexelIndex_Y]);
                }
            }
            if (Black == nullptr) {
                continue;
            }
            tempDexel = Black;
            float SegmentLength;
            while (tempDexel != nullptr) {
                SegmentLength = tempDexel->Second - tempDexel->First;
                tempDexel->First += (SegmentLength * 0.1);
                tempDexel->Second -= (SegmentLength * 0.1);
                tempDexel = tempDexel->Next;
            }
            tempDexel = White;
            while (tempDexel->Next != nullptr) {
                SegmentLength = std::min(tempDexel->Next->First - tempDexel->Second, tempDexel->Next->Second - tempDexel->Next->First);
                SegmentLength = std::min(SegmentLength, tempDexel->Second - tempDexel->First);
                SegmentLength = std::abs(SegmentLength);
                tempDexel->Second -= (SegmentLength * 0.1);
                tempDexel->Next->First += (SegmentLength * 0.1);
                tempDexel = tempDexel->Next;
            }
            tempDexel = Black;
            float BottomGrayMin = White->Second;
            float BottomGrayMax = tempDexel->First;
            float TopGrayMin, TopGrayMax;
            PowerDexel* tempCornerDexel = nullptr;
            Vector3Df CornerPoint;
            PowerDexel* tempWhite = White;
            std::vector<Vector3Df> TrianglePoint;
            while (tempDexel != nullptr) {
                if (White->Next == nullptr) {
                    break;
                }
                if (tempDexel->Next != nullptr) {
                    //黑域还未结束，必然存在下一个白域
                    if (tempDexel->Next->Second < White->Next->First) {
                        tempDexel = tempDexel->Next;
                        continue;
                    }
                }
                TopGrayMin = tempDexel->Second;
                TopGrayMax = White->Next->First;
                for (int i = 0; i < 8; i = i + 2) {
                    DexelIndex_X = X + VertexMask[i];
                    DexelIndex_Y = Y + VertexMask[i + 1];
                    tempCornerDexel = DexelArray[DexelIndex_X][DexelIndex_Y];
                    if (tempCornerDexel == nullptr) {
                        break;
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
            if ((TrianglePoint.size() >= 8) && (TrianglePoint.size() % 8) == 0) {
                for (int i = 0; i < TrianglePoint.size(); i = i + 8) {
                    for (int j = 0; j < 8; j++) {
                        TriDexel->TrianglePoints.push_back(TrianglePoint[i + j]);
                    }
                    if ((i + 8) < TrianglePoint.size()) {
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

    for (int X = 0; X < (DexelArraySize - 1); X++) {
        for (int Y = 0; Y < (DexelArraySize - 1); Y++) {
            if (TriangleDexelArray[X][Y] == nullptr) {
                continue;
            }
            TriangleDexel* tempTriDexel = TriangleDexelArray[X][Y];
            TriangleRenderDexel* tempTriRenderDexel = TriangleRenderDexelArray[X][Y];
            while ((tempTriDexel != nullptr)) {
                if (tempTriDexel->TrianglePoints.size() != 8) {
                    tempTriDexel = tempTriDexel->Next;
                    tempTriRenderDexel = tempTriRenderDexel->Next;
                    continue;
                }
                tempTriRenderDexel->TriangleIndex = DexelTriangles.size();
                Vector3Df Point1 = tempTriDexel->TrianglePoints[0];
                Vector3Df Point2 = tempTriDexel->TrianglePoints[6];
                Vector3Df Point3 = tempTriDexel->TrianglePoints[2];
                Vector3Df Point4 = tempTriDexel->TrianglePoints[4];
                Vector3Df Normal1 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                float dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, -1.0f));
                if (dot1 < 0.0f) {
                    Normal1 = -Normal1;
                }
                Point1 = Point1 / MaxLength;
                Point2 = Point2 / MaxLength;
                Point3 = Point3 / MaxLength;
                Point4 = Point4 / MaxLength;
                vertices.push_back(Point3);
                vertices.push_back(Point2);
                vertices.push_back(Point1);
                vertices.push_back(Point2);
                vertices.push_back(Point4);
                vertices.push_back(Point1);
                for (int i = 0; i < 6; i++) {
                    normals.push_back(Normal1 * (1.0f));
                }
                Point1 = tempTriDexel->TrianglePoints[1];
                Point2 = tempTriDexel->TrianglePoints[7];
                Point3 = tempTriDexel->TrianglePoints[3];
                Point4 = tempTriDexel->TrianglePoints[5];
                Normal1 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, 1.0f));
                if (dot1 < 0.0f) {
                    Normal1 = -Normal1;
                }
                Point1 = Point1 / MaxLength;
                Point2 = Point2 / MaxLength;
                Point3 = Point3 / MaxLength;
                Point4 = Point4 / MaxLength;
                vertices.push_back(Point1);
                vertices.push_back(Point2);
                vertices.push_back(Point3);
                vertices.push_back(Point1);
                vertices.push_back(Point4);
                vertices.push_back(Point2);
                for (int i = 0; i < 6; i++) {
                    normals.push_back(Normal1);
                }
                float TopZ_0, TopZ_1, BotZ_0, BotZ_1;
                float Nerighbor_TopZ_0, Nerighbor_TopZ_1, Nerighbor_BotZ_0, Nerighbor_BotZ_1;
                float Point_Offset = 0.0001; //偏差值可以由目标Dexel与邻居Dexel的距离决定
                int Neighbor_X, Neighbor_Y;
                int AddSide = 0; //判断是否添加侧面面片//存在较大偏差则改为true
                for (int i = 0; i < 4; i++) {
                    Neighbor_X = X + GetNeighbor[i][0];
                    Neighbor_Y = Y + GetNeighbor[i][1];
                    if ((Neighbor_X < 0) || (Neighbor_X >= (DexelArraySize - 1)) || (Neighbor_Y < 0) || (Neighbor_Y >= (DexelArraySize - 1)) || (TriangleDexelArray[Neighbor_X][Neighbor_Y] == nullptr)) {
                        AddSide = 0;
                        break;
                    }
                    TriangleDexel* NeighborDexel = TriangleDexelArray[Neighbor_X][Neighbor_Y];
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
                        Nerighbor_BotZ_0 = Nerighbor_BotZ_0 - length_0 * 0.01;
                        Nerighbor_BotZ_1 = Nerighbor_BotZ_1 - length_1 * 0.01;
                        Nerighbor_TopZ_0 = Nerighbor_TopZ_0 + length_0 * 0.01;
                        Nerighbor_TopZ_1 = Nerighbor_TopZ_1 + length_1 * 0.01;
                        bool IsAddSide = ((Nerighbor_TopZ_0 >= TopZ_0) && (Nerighbor_TopZ_1 >= TopZ_1) && (Nerighbor_BotZ_0 <= BotZ_0) && (Nerighbor_BotZ_1 <= BotZ_1));
                        if (IsAddSide) {
                            AddSide++;
                            break;
                        }
                        NeighborDexel = NeighborDexel->Next;
                    }
                }
                if (AddSide < 4) { // 柱体未被完全遮掩，添加侧面
                    tempTriRenderDexel->IsAddSide = true;
                    for (int m = 0; m < 4; m++) {
                        Point1 = tempTriDexel->TrianglePoints[NeighToSide[m][0] * 2]; //BotZ_0
                        Point2 = tempTriDexel->TrianglePoints[NeighToSide[m][1] * 2]; //BotZ_1
                        Point3 = tempTriDexel->TrianglePoints[NeighToSide[m][0] * 2 + 1]; //TopZ_0
                        Point4 = tempTriDexel->TrianglePoints[NeighToSide[m][1] * 2 + 1]; //TopZ_1
                        Normal1 = (Point1 - Point2).cross(Point1 - Point3);
                        Normal1.normalize();
                        Point1 = Point1 / MaxLength;
                        Point2 = Point2 / MaxLength;
                        Point3 = Point3 / MaxLength;
                        Point4 = Point4 / MaxLength;
                        vertices.push_back(Point1);
                        vertices.push_back(Point2);
                        vertices.push_back(Point4);
                        vertices.push_back(Point1);
                        vertices.push_back(Point4);
                        vertices.push_back(Point3);
                        for (int n = 0; n < 6; n++) {
                            normals.push_back(Normal1);
                        }
                    }
                }
                tempTriDexel = tempTriDexel->Next;
                tempTriRenderDexel = tempTriRenderDexel->Next;
            }
        }
    }

    //std::cout << "updateVerticesAndNormals_End" << std::endl;
}

void MRR_Dexel::InitializeDexelArray(const std::vector<Triangle3Df>& trianglesFromFile)
{
    IsBlankFile = true;
    trianglesFromBlank = trianglesFromFile;

    std::cout << "STL_InitializeDexelArray" << std::endl;

    float workpieceMaxEdge = 0.0f;
    float workpieceMinEdge = 0.0f;
    float MinX = 100000.0f;
    float MinY = 100000.0f;
    float MinZ = 100000.0f;
    float MaxX = -100000.0f;
    float MaxY = -100000.0f;
    float MaxZ = -100000.0f;

    std::for_each(trianglesFromFile.begin(), trianglesFromFile.end(), [&](const Triangle3Df& triangle) {
        auto MinXdistance = {
            MinX,
            triangle.Vertices[0].x,
            triangle.Vertices[1].x,
            triangle.Vertices[2].x,
        };
        auto MinYdistance = {
            MinY,
            triangle.Vertices[0].y,
            triangle.Vertices[1].y,
            triangle.Vertices[2].y,
        };
        auto MinZdistance = {
            MinZ,
            triangle.Vertices[0].z,
            triangle.Vertices[1].z,
            triangle.Vertices[2].z,
        };

        auto MaxXdistance = {
            MaxX,
            triangle.Vertices[0].x,
            triangle.Vertices[1].x,
            triangle.Vertices[2].x,
        };
        auto MaxYdistance = {
            MaxY,
            triangle.Vertices[0].y,
            triangle.Vertices[1].y,
            triangle.Vertices[2].y,
        };
        auto MaxZdistance = {
            MaxZ,
            triangle.Vertices[0].z,
            triangle.Vertices[1].z,
            triangle.Vertices[2].z,
        };
        // auto Xdistance = {
        //     MinX,
        //     MaxX,
        //     triangle.Vertices[0].x,
        //     triangle.Vertices[1].x,
        //     triangle.Vertices[2].x,
        // };
        // auto Ydistance = {
        //     MinY,
        //     MaxY,
        //     triangle.Vertices[0].y,
        //     triangle.Vertices[1].y,
        //     triangle.Vertices[2].y,
        // };
        // auto Zdistance = {
        //     MinZ,
        //     MaxZ,
        //     triangle.Vertices[0].z,
        //     triangle.Vertices[1].z,
        //     triangle.Vertices[2].z,
        // };

        MinX = *std::min_element(MinXdistance.begin(), MinXdistance.end());
        MinY = *std::min_element(MinYdistance.begin(), MinYdistance.end());
        MinZ = *std::min_element(MinZdistance.begin(), MinZdistance.end());

        MaxX = *std::max_element(MaxXdistance.begin(), MaxXdistance.end());
        MaxY = *std::max_element(MaxYdistance.begin(), MaxYdistance.end());
        MaxZ = *std::max_element(MaxZdistance.begin(), MaxZdistance.end());

        // workpieceMaxEdge = std::max({ MaxX, MaxY, MaxZ });
    });
    std::cout << "MinX: " << MinX << " MaxX: " << MaxX << std::endl;
    std::cout << "MinY: " << MinY << " MaxY: " << MaxY << std::endl;
    std::cout << "MinZ: " << MinZ << " MaxZ: " << MaxZ << std::endl;
    AABB3Df tempbox = AABB3Df(Vector3Df(MinX, MinY, MinZ), Vector3Df(MaxX, MaxY, MaxZ));

    ABBox = tempbox;
    std::cout << "ABBox" << ABBox << std::endl;

    UnitLength = std::max({ MaxX - MinX, MaxY - MinY, MaxZ - MinZ }) / DexelArraySize;

    Vector3Df Size = ABBox.getMax() - ABBox.getMin();

    //转换下思路，遍历面片，找到每个面片覆盖的dexel，然后将该dexel射线与面片的交点的高度加入到该dexel的链表中
    //先创建一个vector<vector<vector<float>>>，保存每个链表的高度，然后再遍历一次，将链表中的高度转化为PowerDexel
    std::vector<std::vector<std::vector<float>>> dexelHeigthArray;
    dexelHeigthArray.resize(DexelArraySize);
    for (int i = 0; i < DexelArraySize; i++) {
        dexelHeigthArray[i].resize(DexelArraySize);
    }

    for (const Triangle3Df& triangle : trianglesFromFile) {
        const Vector3Df a = Vector3Df(triangle.Vertices[0].x, triangle.Vertices[0].y, 0);
        const Vector3Df b = Vector3Df(triangle.Vertices[1].x, triangle.Vertices[1].y, 0);
        const Vector3Df c = Vector3Df(triangle.Vertices[2].x, triangle.Vertices[2].y, 0);
        //获取最小xy和最大xy
        const float MinX = std::min({ a.x, b.x, c.x });
        const float MinY = std::min({ a.y, b.y, c.y });
        const float MaxX = std::max({ a.x, b.x, c.x });
        const float MaxY = std::max({ a.y, b.y, c.y });

        //转化为int,std::floor向下取整，std::ceil向上取整
        const int MinXIndex = std::max(std::floor((MinX - ABBox.getMin().x) / UnitLength), 0.0f);
        const int MinYIndex = std::max(std::floor((MinY - ABBox.getMin().y) / UnitLength), 0.0f);
        const int MaxXIndex = std::min(std::ceil((MaxX - ABBox.getMin().x) / UnitLength), (float)DexelArraySize - 1.0f);
        const int MaxYIndex = std::min(std::ceil((MaxY - ABBox.getMin().y) / UnitLength), (float)DexelArraySize - 1.0f);

        Vector3Df point;

        for (int i = MinXIndex; i <= MaxXIndex; i++) {
            for (int j = MinYIndex; j <= MaxYIndex; j++) {
                point = getDexelCenter(i, j);
                if (Triangle3Df(a, b, c).isInside(point) && std::abs(triangle.Normal.z) > 0.0001f) {
                    const float A = triangle.Normal.x, B = triangle.Normal.y, C = triangle.Normal.z;
                    const float x0 = triangle.Vertices[0].x, y0 = triangle.Vertices[0].y, z0 = triangle.Vertices[0].z;
                    const float z = (A * (x0 - point.x) + B * (y0 - point.y)) / C + z0;
                    dexelHeigthArray[i][j].push_back(z);
                }
            }
        }
    }

    for (int i = 0; i < DexelArraySize; i++) {
        for (int j = 0; j < DexelArraySize; j++) {
            auto& dexelHeigth = dexelHeigthArray[i][j];
            if (dexelHeigth.size() >= 2 && dexelHeigth.size() % 2 == 0) {
                std::sort(dexelHeigth.begin(), dexelHeigth.end());
                PowerDexel* tempdexel = new PowerDexel();
                PowerDexel* nextDexel = tempdexel;

                for (int i = 0; i < dexelHeigth.size(); i = i + 2) {

                    nextDexel->First = dexelHeigth[i];
                    nextDexel->Second = dexelHeigth[i + 1];
                    if (i + 2 < dexelHeigth.size()) {
                        nextDexel->Next = new PowerDexel();
                        nextDexel = nextDexel->Next;
                    }
                }
                DexelArray[i][j] = tempdexel;
            }
        }
    }

    BoxSize = Vector3Df(MaxX - MinX, MaxY - MinY, MaxZ - MinZ);
    std::cout << "STL_InitializeDexelArray_END " << std::endl;

    return;

    auto getIntersectionZ = [&](const Vector3Df& p, std::vector<float>& intersectionHeights) {
        std::mutex mutex;
        std::for_each(std::execution::par, trianglesFromFile.begin(), trianglesFromFile.end(), [&](const Triangle3Df& triangle) {
            const Vector3Df a = Vector3Df(triangle.Vertices[0].x, triangle.Vertices[0].y, 0);
            const Vector3Df b = Vector3Df(triangle.Vertices[1].x, triangle.Vertices[1].y, 0);
            const Vector3Df c = Vector3Df(triangle.Vertices[2].x, triangle.Vertices[2].y, 0);
            if (Triangle3Df(a, b, c).isInside(p) && std::abs(triangle.Normal.z) > 0.0001f) {
                const float A = triangle.Normal.x, B = triangle.Normal.y, C = triangle.Normal.z;
                const float x0 = triangle.Vertices[0].x, y0 = triangle.Vertices[0].y, z0 = triangle.Vertices[0].z;
                const float z = (A * (x0 - p.x) + B * (y0 - p.y)) / C + z0;
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    intersectionHeights.push_back((z));
                }
            }
        });
    };

    std::for_each(std::execution::par, DexelArray.begin(), DexelArray.end(), [&](std::vector<PowerDexel*>& dexelArray) {
        const size_t i = &dexelArray - DexelArray.data();
        if (UnitLength * i > Size.x) {
            return;
        }
        std::for_each(std::execution::par, dexelArray.begin(), dexelArray.end(), [&](PowerDexel*& dexel) {
            const size_t j = &dexel - dexelArray.data();
            if (UnitLength * j > Size.y) {
                return;
            }
            dexel = nullptr;
            const Vector3Df point = getDexelCenter(i, j);
            std::vector<float> intersectionHeights;
            getIntersectionZ(point, intersectionHeights);

            if (intersectionHeights.size() >= 2 && intersectionHeights.size() % 2 == 0) {
                std::sort(intersectionHeights.begin(), intersectionHeights.end());
                PowerDexel* tempdexel = new PowerDexel();
                PowerDexel* nextDexel = tempdexel;

                for (int i = 0; i < intersectionHeights.size(); i = i + 2) {

                    nextDexel->First = intersectionHeights[i];
                    nextDexel->Second = intersectionHeights[i + 1];
                    if (i + 2 < intersectionHeights.size()) {
                        nextDexel->Next = new PowerDexel();
                        nextDexel = nextDexel->Next;
                    }
                }
                dexel = tempdexel;
            }
        });
    });

    BoxSize = Vector3Df(MaxX - MinX, MaxY - MinY, MaxZ - MinZ);

    std::cout << "STL_InitializeDexelArray_END " << std::endl;
    return;

    //DexelArray.resize(DexeArraySize);
    int Count = 0;
    for_each(std::execution::par, DexelArray.begin(), DexelArray.end(), [&](std::vector<PowerDexel*>& dexelArray) {
        size_t i = &dexelArray - &DexelArray[0];
        //std::cout << "i: " << i << std::endl;
        if (UnitLength * i > Size.x) {
            return;
        }
        for_each(std::execution::par, dexelArray.begin(), dexelArray.end(), [&](PowerDexel*& dexel) {
            size_t j = &dexel - &dexelArray[0];
            if (UnitLength * j > Size.y) {
                return;
            }
            dexel = nullptr;

            std::vector<float> LinePoint;
            Vector3Df Center = getDexelCenter(i, j);
            glm::vec2 point = glm::vec2(Center.x, Center.y);
            glm::vec3 rayOrigin = glm::vec3(point.x, point.y, 0);

            for_each(trianglesFromFile.begin(), trianglesFromFile.end(), [&](const Triangle3Df& triangle) {
                glm::vec2 a = glm::vec2(triangle.Vertices[0].x, triangle.Vertices[0].y);
                glm::vec2 b = glm::vec2(triangle.Vertices[1].x, triangle.Vertices[1].y);
                glm::vec2 c = glm::vec2(triangle.Vertices[2].x, triangle.Vertices[2].y);

                if (pointintriangle(point, a, b, c)) {
                    glm::vec3 TriNormal = glm::normalize(glm::vec3(triangle.Normal.x, triangle.Normal.y, triangle.Normal.z));
                    if (std::abs(TriNormal.z) > 0.0001f) {
                        glm::vec3 vert1 = glm::vec3(triangle.Vertices[0].x, triangle.Vertices[0].y, triangle.Vertices[0].z);
                        glm::vec3 vert2 = glm::vec3(triangle.Vertices[1].x, triangle.Vertices[1].y, triangle.Vertices[1].z);
                        glm::vec3 vert3 = glm::vec3(triangle.Vertices[2].x, triangle.Vertices[2].y, triangle.Vertices[2].z);
                        glm::vec3 intersectPoint = rayintersectiontriangle(rayOrigin, glm::vec3(0.0f, 0.0f, 1.0f), vert1, vert2, vert3);

                        bool IsInsert = true;
                        for (int i = 0; i < LinePoint.size(); i++) {
                            if (floatEqual(LinePoint[i], intersectPoint.z)) {
                                IsInsert = false;
                                break;
                            }
                        }
                        if (IsInsert) {
                            LinePoint.push_back(intersectPoint.z); /// (MaxLength)
                        }
                    }
                }
            });
            // if (LinePoint.size() >= 3) {
            //     std::cout << "LinePoint.size()" << LinePoint.size() << std::endl;
            // }

            if (LinePoint.size() >= 2 && LinePoint.size() % 2 == 0) {
                std::sort(LinePoint.begin(), LinePoint.end());
                PowerDexel* tempdexel = new PowerDexel();
                PowerDexel* nextDexel = tempdexel;

                for (int i = 0; i < LinePoint.size(); i = i + 2) {

                    nextDexel->First = LinePoint[i];
                    nextDexel->Second = LinePoint[i + 1];
                    if (i + 2 < LinePoint.size()) {
                        nextDexel->Next = new PowerDexel();
                        nextDexel = nextDexel->Next;
                    }
                }
                dexel = tempdexel;
            }
        });
    });

    this->CalculateWorkPieceVolume();
    return;
}

void MRR_Dexel::InitDexelTriangle()
{
    TriangleDexelArray.resize(DexelArraySize - 1);
    TriangleRenderDexelArray.resize(DexelArraySize - 1);

    for (int i = 0; i < TriangleDexelArray.size(); i++) {
        TriangleDexelArray[i].resize(DexelArraySize - 1);
    }
    for (int i = 0; i < TriangleRenderDexelArray.size(); i++) {
        TriangleRenderDexelArray[i].resize(DexelArraySize - 1);
    }
    std::cout << "DexelTriangleInitialize_START" << std::endl;

    int TriDexelCount = 0;

    for (int X = 0; X < DexelArraySize - 1; X++) {
        // std::cout << "X: " << X << std::endl;
        for (int Y = 0; Y < DexelArraySize - 1; Y++) {
            // std::cout << "X: " << X << "  Y: " << Y << std::endl;
            if (DexelArray[X + 1][Y + 1] == nullptr) {
                continue;
            };

            TriangleDexel* TriDexel = new TriangleDexel();
            TriangleRenderDexel* TriRenderDexel = new TriangleRenderDexel();

            TriangleDexelArray[X][Y] = TriDexel;
            TriangleRenderDexelArray[X][Y] = TriRenderDexel;

            //通过DexelArray的四条边，初始化DexelTriangleArray
            //初始化白域和黑域
            PowerDexel* White = new PowerDexel(-1000.0f, 1000.0f);
            PowerDexel* Black = nullptr;
            int DexelIndex_X, DexelIndex_Y;
            PowerDexel* tempDexel = nullptr;

            int CountCornerDexel = 4;
            // int TriType = 0;
            //初始化黑域
            for (int i = 0; i < 8; i += 2) {
                DexelIndex_X = X + VertexMask[i];
                DexelIndex_Y = Y + VertexMask[i + 1];

                tempDexel = DexelArray[DexelIndex_X][DexelIndex_Y];
                if (tempDexel == nullptr) {
                    // CountCornerDexel--; //三角柱体
                    // continue;
                    // //三角柱体
                    Black == nullptr;
                    break;
                }
                // TriType = TriType + i / 2;
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
            // TriType = TriType - 3;
            //四个边角点均为空，跳过
            if (Black == nullptr) {
                continue;
            }
            // //不足以构成柱体，跳过，三角柱体
            // if ((CountCornerDexel <= 2) || (Black == nullptr)) {
            //     continue;
            // }

            // std::cout << "Prepare BlackZone" << std::endl;
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
                // SegmentLength = tempDexel->Next->First - tempDexel->Second;
                tempDexel->Second -= (SegmentLength * 0.1);
                tempDexel->Next->First += (SegmentLength * 0.1);
                tempDexel = tempDexel->Next;
            }

            //根据黑域和白域获取灰域并获取三角形
            tempDexel = Black;

            //获取灰域
            float BottomGrayMin = White->Second;
            float BottomGrayMax = tempDexel->First;
            float TopGrayMin, TopGrayMax;

            PowerDexel* tempCornerDexel = nullptr;
            Vector3Df CornerPoint;
            //通过灰域获取三角形，
            //黑域      ----    ----
            //白域   -------------------

            PowerDexel* tempWhite = White;
            std::vector<Vector3Df> TrianglePoint;

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
                //遍历4个顶点，获取三角形的顶点
                for (int i = 0; i < 8; i = i + 2) {
                    DexelIndex_X = X + VertexMask[i];
                    DexelIndex_Y = Y + VertexMask[i + 1];
                    tempCornerDexel = DexelArray[DexelIndex_X][DexelIndex_Y];
                    if (tempCornerDexel == nullptr) {
                        continue; //三角柱体
                        //三角柱体
                        //break;
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

            //初始化三角形渲染Dexel结构，每个Dexel的一个节点对应一个柱形，有n个节点代表这条射线上有n个柱形
            if ((TrianglePoint.size() >= 8) && (TrianglePoint.size() % 8) == 0) {
                for (int i = 0; i < TrianglePoint.size(); i = i + 8) {

                    for (int j = 0; j < 8; j++) {
                        TriDexel->TrianglePoints.push_back(TrianglePoint[i + j]);
                    }

                    if ((i + 8) < TrianglePoint.size()) {
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

    std::cout << "TriDexelCount: " << TriDexelCount << std::endl;
    int count1 = 0;

    std::cout << "DexelTriangleInitialize_End" << std::endl;
    std::vector<int> SideTriIndex;
    for (int X = 0; X < (DexelArraySize - 1); X++) {

        for (int Y = 0; Y < (DexelArraySize - 1); Y++) {
            if (TriangleDexelArray[X][Y] == nullptr) {
                continue;
            }
            TriangleDexel* tempTriDexel = TriangleDexelArray[X][Y];
            TriangleRenderDexel* tempTriRenderDexel = TriangleRenderDexelArray[X][Y];
            while ((tempTriDexel != nullptr)) {
                if (tempTriDexel->TrianglePoints.size() == 6) {
                    break;
                }
                if (tempTriDexel->TrianglePoints.size() != 8) {
                    tempTriDexel = tempTriDexel->Next;
                    tempTriRenderDexel = tempTriRenderDexel->Next;
                    continue;
                }

                //记录渲染列表下标
                tempTriRenderDexel->TriangleIndex = DexelTriangles.size();
                Vector3Df Point1 = tempTriDexel->TrianglePoints[0];
                Vector3Df Point2 = tempTriDexel->TrianglePoints[6];
                Vector3Df Point3 = tempTriDexel->TrianglePoints[2];
                Vector3Df Point4 = tempTriDexel->TrianglePoints[4];
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

                Point1 = tempTriDexel->TrianglePoints[1]; //0
                Point2 = tempTriDexel->TrianglePoints[7]; //3
                Point3 = tempTriDexel->TrianglePoints[3]; //1
                Point4 = tempTriDexel->TrianglePoints[5]; //2
                Vector3Df Normal2 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                dot1 = Normal2.dot(Vector3Df(0.0f, 0.0f, 1.0f));
                if (dot1 < 0.0f) {
                    Normal2 = -Normal2;
                }

                DexelTriangles.push_back(Point1);
                DexelTriangles.push_back(Point2);
                DexelTriangles.push_back(Point3);

                DexelTriangles.push_back(Point1);
                DexelTriangles.push_back(Point4);
                DexelTriangles.push_back(Point2);
                for (int i = 0; i < 6; i++) {
                    DexelTrianglesNormals.push_back(Normal2);
                }

                if (CountTriDexelIsSide(X, Y, tempTriDexel)) { // 柱体未被完全遮掩，添加侧面
                    //记录是否有添加测面面片，用来计算Dexel节点在渲染列表该位置填入了多少顶点：顶面+底面+侧面=6+6+4*6=36
                    tempTriRenderDexel->IsAddSide = true;
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
                                SideTriIndex.push_back(X);
                                SideTriIndex.push_back(Y);
                                //Normal1也接近垂直XY平面
                                //对周围的8个邻居进行采样
                                // std::vector<Vector3Df> NeigthborPoints;
                                // GetNeigthborTriDexelPoint(NeigthborPoints, X, Y, tempTriDexel);
                                // if (NeigthborPoints.size() >= 1) {
                                //     //与目标TriDexel的相减获得边缘
                                //     //获取TriDexel的point
                                //     Vector3Df TriDexelPoint = tempTriDexel->TrianglePoints[0];
                                //     // TriDexelPoint = getDexelCenter(X, Y);
                                //     TriDexelPoint.z = 0;
                                //     std::vector<Vector3Df> EdgeNormals;
                                //     Vector3Df tempVector = Vector3Df(0.0f, 0.0f, 1.0f);
                                //     Vector3Df Edge;
                                //     Vector3Df EdgeNormal;
                                //     for (Vector3Df& NeigthborPoint : NeigthborPoints) {
                                //         Edge.x = std::abs(NeigthborPoint.x - TriDexelPoint.x);
                                //         Edge.y = std::abs(NeigthborPoint.y - TriDexelPoint.y);
                                //         Edge.z = 0.0f;
                                //         EdgeNormal = Edge.cross(tempVector);
                                //         EdgeNormal = EdgeNormal.normalize();
                                //         EdgeNormals.push_back(EdgeNormal);
                                //         // EdgeNormal = NeigthborPoint - TriDexelPoint;
                                //         // EdgeNormal .normalize();
                                //         // EdgeNormals.push_back(EdgeNormal);
                                //     }
                                //     EdgeNormal = Vector3Df(0.0f, 0.0f, 0.0f);
                                //     for (Vector3Df& en : EdgeNormals) {
                                //         EdgeNormal += en;
                                //     }
                                //     EdgeNormal = EdgeNormal.normalize();
                                //     if (EdgeNormal.dot(SideNormal) < 0.0f) {
                                //         SideNormal = EdgeNormal * (-1.0f);
                                //     } else {
                                //         SideNormal = EdgeNormal;
                                //     }
                                // }
                            } else {
                                // Vector3Df tempNormal1 = Normal1;
                                // tempNormal1.z = 0.0f;
                                // tempNormal1.normalize();
                                // //Normal1不接近垂直XY平面
                                // if (SideNormal.dot(tempNormal1) < 0.0f) {
                                //     SideNormal = tempNormal1 * (-1.0f);
                                // } else {
                                //     SideNormal = tempNormal1;
                                // }
                                SideNormal = Normal1;
                                // SideNormal.x = Normal1.x;
                                // SideNormal.y = Normal1.y;
                            }
                        } else {
                            // Vector3Df tempNormal2 = Normal2;
                            // tempNormal2.z = 0.0f;
                            // tempNormal2.normalize();
                            // //Normal2不接近垂直XY平面
                            // if (SideNormal.dot(tempNormal2) < 0.0f) {
                            //     SideNormal = tempNormal2 * (-1.0f);
                            // } else {
                            //     SideNormal = tempNormal2;
                            // }
                            SideNormal = Normal2;
                            // SideNormal.x = Normal2.x;
                            // SideNormal.y = Normal2.y;
                        }
                        SideNormal.normalize();
                        //112,122
                        DexelTriangles.push_back(Point1);
                        DexelTrianglesNormals.push_back(SideNormal);
                        DexelTriangles.push_back(Point2);
                        DexelTrianglesNormals.push_back(SideNormal);
                        DexelTriangles.push_back(Point4);
                        DexelTrianglesNormals.push_back(SideNormal);

                        DexelTriangles.push_back(Point1);
                        DexelTrianglesNormals.push_back(SideNormal);
                        DexelTriangles.push_back(Point4);
                        DexelTrianglesNormals.push_back(SideNormal);
                        DexelTriangles.push_back(Point3);
                        DexelTrianglesNormals.push_back(SideNormal);

                        // for (int n = 0; n < 6; n++) {
                        //     DexelTrianglesNormals.push_back(Normal1);
                        // }
                    }
                }

                tempTriDexel = tempTriDexel->Next;
                tempTriRenderDexel = tempTriRenderDexel->Next;
            }
            continue;
        }
    }

    Vector3Df MaskNormal = Vector3Df(0, 0, 1);
    Vector3Df faceNormal;
    count1 = 0;

    for (int i = 0; i < SideTriIndex.size(); i = i + 2) {
        count1 = 0;
        int X = SideTriIndex[i];
        int Y = SideTriIndex[i + 1];
        TriangleDexel* tempTriDexel = TriangleDexelArray[X][Y];
        TriangleRenderDexel* tempTriRenderDexel = TriangleRenderDexelArray[X][Y];
        Vector3Df CenterPoint = getDexelCenter(X, Y);
        //搜索25格内所有为addside的TriDexel
        // std::vector<Vector3Df> NeigthborPoints;
        while (tempTriDexel != nullptr) {
            faceNormal = DexelTrianglesNormals[tempTriRenderDexel->TriangleIndex + 13];
            // if (std::abs(faceNormal.dot(MaskNormal)) <= 0.95 || (!tempTriRenderDexel->IsAddSide)) {
            //     tempTriDexel = tempTriDexel->Next;
            //     tempTriRenderDexel = tempTriRenderDexel->Next;
            //     continue;
            // }

            Vector3Df LastNormal = Vector3Df(0.0f, 0.0f, 0.0f);
            Vector3Df SideNormal;
            Vector3Df NeigthborPoint;

            int OperatorSize = 2; //算子大小(OperatorSize * 2 + 1)
            //5 * 5 的int数组
            std::vector<std::vector<int>> AddSideArray = std::vector<std::vector<int>>(OperatorSize * 2 + 1, std::vector<int>(OperatorSize * 2 + 1, 0));
            for (int m = X - OperatorSize; m <= X + OperatorSize; m++) {

                for (int n = Y - OperatorSize; n <= Y + OperatorSize; n++) {
                    if ((m == X) && (n == Y)) {
                        continue;
                    }
                    if ((m < 0) || (m >= (DexelArraySize - 1)) || (n < 0) || (n >= (DexelArraySize - 1)) || (TriangleDexelArray[m][n] == nullptr)) {
                        continue;
                    }

                    TriangleRenderDexel* tempNeiTriRenderDexel = TriangleRenderDexelArray[m][n];

                    while (tempNeiTriRenderDexel != nullptr) {
                        if (tempNeiTriRenderDexel->IsAddSide) {
                            count1++;
                            int ArrayIndex_X = m - X + OperatorSize;
                            int ArrayIndex_Y = n - Y + OperatorSize;
                            AddSideArray[ArrayIndex_X][ArrayIndex_Y] = 1;
                            // NeigthborPoint = getDexelCenter(m, n);
                            // NeigthborPoint = NeigthborPoint - CenterPoint;
                            // float SideLength = NeigthborPoint.length();
                            // NeigthborPoint = NeigthborPoint.normalize();
                            // SideNormal = NeigthborPoint.cross(MaskNormal);
                            // if (m < X) {
                            //     SideNormal = SideNormal * (-1.0f);
                            // } else if (m == X) {
                            //     if (n < Y) {
                            //         SideNormal = SideNormal * (-1.0f);
                            //     }
                            // }
                            // //判断该邻居对面的部分是否为addside，不是的话降低它的权重，是的话增加权重
                            // //以邻居点和中心点的长度作为加权，它们之间的距离越远，权重越大
                            // LastNormal = LastNormal + SideNormal * SideLength;
                            // NeigthborPoints.push_back(getDexelCenter(m, n));
                            break;
                        }
                        tempNeiTriRenderDexel = tempNeiTriRenderDexel->Next;
                    }
                }
            }

            for (int m = X - OperatorSize; m <= X + OperatorSize; m++) {
                for (int n = Y - OperatorSize; n <= Y + OperatorSize; n++) {
                    if ((m == X) && (n == Y)) {
                        continue;
                    }

                    int ArrayIndex_X = m - X + OperatorSize;
                    int ArrayIndex_Y = n - Y + OperatorSize;

                    if (AddSideArray[ArrayIndex_X][ArrayIndex_Y] == 0)
                        continue;

                    NeigthborPoint = getDexelCenter(m, n);
                    NeigthborPoint = NeigthborPoint - CenterPoint;
                    float SideLength = NeigthborPoint.length();
                    NeigthborPoint = NeigthborPoint.normalize();
                    SideNormal = NeigthborPoint.cross(MaskNormal);
                    //重要
                    if (m < X) {
                        SideNormal = SideNormal * (-1.0f);
                    } else if (m == X) {
                        if (n < Y) {
                            SideNormal = SideNormal * (-1.0f);
                        }
                    }
                    //判断该邻居对面的部分是否为addside，不是的话降低它的权重，是的话增加权重
                    //(OperatorSize,OperatorSize)为中心点，(0,0)为左下角点，(2 * OperatorSize,2 * OperatorSize)为右上角点
                    int deltaX = ArrayIndex_X - OperatorSize;
                    int deltaY = ArrayIndex_X - OperatorSize;
                    int CaterCornerX = OperatorSize - deltaX;
                    int CaterCornerY = OperatorSize - deltaY;

                    if (AddSideArray[CaterCornerX][CaterCornerY] == 0) {
                        SideLength = 0.30f * SideLength;
                    } else {
                        SideLength = 1.50f * SideLength;
                    }
                    //以邻居点和中心点的长度作为加权，它们之间的距离越远，权重越大
                    LastNormal = LastNormal + SideNormal * SideLength;
                }
            }

            LastNormal = LastNormal.normalize();
            // //以邻居点和中心点的长度作为加权，它们之间的距离越近远，权重越大
            // for (auto& NeigthborPoint : NeigthborPoints) {
            //     NeigthborPoint = NeigthborPoint - CenterPoint;
            //     float SideLength = NeigthborPoint.length();
            //     NeigthborPoint = NeigthborPoint.normalize();
            //     SideNormal = NeigthborPoint.cross(MaskNormal);
            //     if ((NeigthborPoint.x - CenterPoint.x) < 0.00001f) {
            //         if()
            //         SideNormal = SideNormal * (-1.0f);
            //     }
            //     LastNormal = LastNormal + SideNormal * SideLength;
            // }

            int NormalIndex = tempTriRenderDexel->TriangleIndex + 12;
            // std::cout << "LastNormal: " << LastNormal << std::endl;
            for (int m = 0; m < 24; m++) {
                NormalIndex = NormalIndex + 1;
                SideNormal = DexelTrianglesNormals[NormalIndex];

                if (SideNormal.dot(LastNormal) < 0.0f) {
                    SideNormal = LastNormal * (-1.0f);
                } else {
                    SideNormal = LastNormal;
                }
                DexelTrianglesNormals[NormalIndex] = SideNormal;
            }
            tempTriDexel = tempTriDexel->Next;
            tempTriRenderDexel = tempTriRenderDexel->Next;
        }

        // std::cout << "count1: " << count1 << std::endl;
    }

    //再次增添一个列表用于处理侧面面片法向量粗糙的问题

    std::cout << "DexelTriangles.size():" << DexelTriangles.size() << std::endl;
    std::cout << "count1:" << count1 << std::endl;
}

void MRR_Dexel::InitDexelTriangle_Delta()
{
    TriangleDexelArray.resize(DexelArraySize - 1);
    TriangleRenderDexelArray.resize(DexelArraySize - 1);

    for (int i = 0; i < TriangleDexelArray.size(); i++) {
        TriangleDexelArray[i].resize(DexelArraySize - 1);
    }
    for (int i = 0; i < TriangleRenderDexelArray.size(); i++) {
        TriangleRenderDexelArray[i].resize(DexelArraySize - 1);
    }
    std::cout << "DexelTriangleInitialize_START" << std::endl;

    int TriDexelCount = 0;
    for (int X = 0; X < DexelArraySize - 1; X++) {
        // std::cout << "X: " << X << std::endl;
        for (int Y = 0; Y < DexelArraySize - 1; Y++) {
            // std::cout << "X: " << X << "  Y: " << Y << std::endl;
            if (DexelArray[X + 1][Y + 1] == nullptr) {
                continue;
            };

            TriangleDexel* TriDexel = new TriangleDexel();
            TriangleRenderDexel* TriRenderDexel = new TriangleRenderDexel();
            TriangleDexelArray[X][Y] = TriDexel;
            TriangleRenderDexelArray[X][Y] = TriRenderDexel;

            std::vector<Vector3Df> TrianglePoint;

            //通过DexelArray的四条边，初始化DexelTriangleArray
            //初始化白域和黑域
            PowerDexel* White = new PowerDexel(-1000.0f, 1000.0f);
            PowerDexel* Black = nullptr;
            int DexelIndex_X, DexelIndex_Y;
            PowerDexel* tempDexel = nullptr;

            int CountCornerDexel = 4;
            // int TriType = 0;
            //初始化黑域
            for (int i = 0; i < 8; i += 2) {
                DexelIndex_X = X + VertexMask[i];
                DexelIndex_Y = Y + VertexMask[i + 1];

                tempDexel = DexelArray[DexelIndex_X][DexelIndex_Y];
                if (tempDexel == nullptr) {
                    CountCornerDexel--; //三角柱体
                    // continue;
                    // //三角柱体
                    Black == nullptr;
                    continue;
                    // break;
                }
                // TriType = TriType + i / 2;
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
            //四个边角点均为空，跳过
            if (Black == nullptr) {
                if (CountCornerDexel == 3) {
                    TrianglePoint.clear();
                    TriDexel = TriangleDexelArray[X][Y];
                    TriRenderDexel = TriangleRenderDexelArray[X][Y];
                    GetDeltaDexelTriangle(X, Y, TriDexel, TriRenderDexel, TrianglePoint);
                }
                continue;
            }

            // std::cout << "Prepare BlackZone" << std::endl;
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
                // SegmentLength = tempDexel->Next->First - tempDexel->Second;
                tempDexel->Second -= (SegmentLength * 0.1);
                tempDexel->Next->First += (SegmentLength * 0.1);
                tempDexel = tempDexel->Next;
            }

            //根据黑域和白域获取灰域并获取三角形
            tempDexel = Black;

            //获取灰域
            float BottomGrayMin = White->Second;
            float BottomGrayMax = tempDexel->First;
            float TopGrayMin, TopGrayMax;

            PowerDexel* tempCornerDexel = nullptr;
            Vector3Df CornerPoint;
            //通过灰域获取三角形，
            //黑域      ----    ----
            //白域   -------------------

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
                //遍历4个顶点，获取三角形的顶点
                for (int i = 0; i < 8; i = i + 2) {
                    DexelIndex_X = X + VertexMask[i];
                    DexelIndex_Y = Y + VertexMask[i + 1];
                    tempCornerDexel = DexelArray[DexelIndex_X][DexelIndex_Y];
                    if (tempCornerDexel == nullptr) {
                        continue; //三角柱体
                        //三角柱体
                        //break;
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

            //初始化三角形渲染Dexel结构，每个Dexel的一个节点对应一个柱形，有n个节点代表这条射线上有n个柱形
            if ((TrianglePoint.size() >= 8) && (TrianglePoint.size() % 8) == 0) {
                for (int i = 0; i < TrianglePoint.size(); i = i + 8) {

                    for (int j = 0; j < 8; j++) {
                        TriDexel->TrianglePoints.push_back(TrianglePoint[i + j]);
                    }

                    if ((i + 8) < TrianglePoint.size()) {
                        TriDexel->Next = new TriangleDexel();
                        TriRenderDexel->Next = new TriangleRenderDexel();
                        TriDexel = TriDexel->Next;
                        TriRenderDexel = TriRenderDexel->Next;
                    }
                }
            }

            if (TrianglePoint.size() < 0 || TrianglePoint.size() % 8 != 0) {
                //判断是否存在三角柱体
                TrianglePoint.clear();
                TriDexel = TriangleDexelArray[X][Y];
                TriRenderDexel = TriangleRenderDexelArray[X][Y];
                GetDeltaDexelTriangle(X, Y, TriDexel, TriRenderDexel, TrianglePoint);
            }

            DeleteDexel(Black);
            DeleteDexel(tempWhite);
        }
    }
    std::cout << "TriDexelCount: " << TriDexelCount << std::endl;

    std::cout << "DexelTriangleInitialize_End" << std::endl;
    int CountDelta = 0;
    int CountSide = 0;
    int CountQuad = 0;
    for (int X = 0; X < (DexelArraySize - 1); X++) {

        for (int Y = 0; Y < (DexelArraySize - 1); Y++) {
            if (TriangleDexelArray[X][Y] == nullptr) {
                continue;
            }
            TriangleDexel* tempTriDexel = TriangleDexelArray[X][Y];
            TriangleRenderDexel* tempTriRenderDexel = TriangleRenderDexelArray[X][Y];
            while ((tempTriDexel != nullptr)) {
                if (tempTriDexel->TrianglePoints.size() == 6) {
                    break;
                }
                if (tempTriDexel->TrianglePoints.size() != 8) {
                    tempTriDexel = tempTriDexel->Next;
                    tempTriRenderDexel = tempTriRenderDexel->Next;
                    continue;
                }
                CountQuad++;
                //记录渲染列表下标
                tempTriRenderDexel->TriangleIndex = DexelTriangles.size();
                Vector3Df Point1 = tempTriDexel->TrianglePoints[0];
                Vector3Df Point2 = tempTriDexel->TrianglePoints[6];
                Vector3Df Point3 = tempTriDexel->TrianglePoints[2];
                Vector3Df Point4 = tempTriDexel->TrianglePoints[4];
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

                Point1 = tempTriDexel->TrianglePoints[1];
                Point2 = tempTriDexel->TrianglePoints[7];
                Point3 = tempTriDexel->TrianglePoints[3];
                Point4 = tempTriDexel->TrianglePoints[5];

                Vector3Df Normal2 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                dot1 = Normal2.dot(Vector3Df(0.0f, 0.0f, 1.0f));
                if (dot1 < 0.0f) {
                    Normal2 = -Normal2;
                }

                DexelTriangles.push_back(Point1);
                DexelTriangles.push_back(Point2);
                DexelTriangles.push_back(Point3);
                DexelTriangles.push_back(Point1);
                DexelTriangles.push_back(Point4);
                DexelTriangles.push_back(Point2);

                for (int i = 0; i < 6; i++) {
                    DexelTrianglesNormals.push_back(Normal2);
                }

                if (CountTriDexelIsSide(X, Y, tempTriDexel)) { // 柱体未被完全遮掩，添加侧面
                    CountSide++;
                    //记录是否有添加测面面片，用来计算Dexel节点在渲染列表该位置填入了多少顶点：顶面+底面+侧面=6+6+4*6=36
                    tempTriRenderDexel->IsAddSide = true;
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

                        //112,122
                        DexelTriangles.push_back(Point1);
                        DexelTrianglesNormals.push_back(SideNormal);
                        DexelTriangles.push_back(Point2);
                        DexelTrianglesNormals.push_back(SideNormal);
                        DexelTriangles.push_back(Point4);
                        DexelTrianglesNormals.push_back(SideNormal);

                        DexelTriangles.push_back(Point1);
                        DexelTrianglesNormals.push_back(SideNormal);
                        DexelTriangles.push_back(Point4);
                        DexelTrianglesNormals.push_back(SideNormal);
                        DexelTriangles.push_back(Point3);
                        DexelTrianglesNormals.push_back(SideNormal);

                        // for (int n = 0; n < 6; n++) {
                        //     DexelTrianglesNormals.push_back(Normal1);
                        // }
                    }
                }

                tempTriDexel = tempTriDexel->Next;
                tempTriRenderDexel = tempTriRenderDexel->Next;
            }
            DexelTriangles.clear();
            DexelTrianglesNormals.clear();

            tempTriDexel = TriangleDexelArray[X][Y];
            tempTriRenderDexel = TriangleRenderDexelArray[X][Y];
            while ((tempTriDexel != nullptr)) {
                if (tempTriDexel->TrianglePoints.size() == 8) {
                    break;
                }
                if (tempTriDexel->TrianglePoints.size() != 6) {
                    tempTriDexel = tempTriDexel->Next;
                    tempTriRenderDexel = tempTriRenderDexel->Next;
                    continue;
                }
                CountDelta++;
                //记录渲染列表下标
                tempTriRenderDexel->TriangleIndex = DexelTriangles.size();
                Vector3Df Point1 = tempTriDexel->TrianglePoints[0];
                Vector3Df Point2 = tempTriDexel->TrianglePoints[2];
                Vector3Df Point3 = tempTriDexel->TrianglePoints[4];
                Vector3Df Normal1 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                float dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, -1.0f));
                if (dot1 < 0.0f) {
                    Normal1 = -Normal1;
                }

                DexelTriangles.push_back(Point1);
                DexelTriangles.push_back(Point2);
                DexelTriangles.push_back(Point3);
                for (int i = 0; i < 3; i++) {
                    DexelTrianglesNormals.push_back(Normal1 * (1.0f));
                }

                Point1 = tempTriDexel->TrianglePoints[1];
                Point3 = tempTriDexel->TrianglePoints[3];
                Point2 = tempTriDexel->TrianglePoints[5];

                Vector3Df Normal2 = (Point1 - Point3).cross(Point1 - Point2).normalize();
                dot1 = Normal2.dot(Vector3Df(0.0f, 0.0f, 1.0f));
                if (dot1 < 0.0f) {
                    Normal2 = -Normal2;
                }

                DexelTriangles.push_back(Point1);
                DexelTriangles.push_back(Point2);
                DexelTriangles.push_back(Point3);

                for (int i = 0; i < 3; i++) {
                    DexelTrianglesNormals.push_back(Normal2);
                }
                Vector3Df Point4;
                for (int i = 0; i < 3; i++) {
                    Point1 = tempTriDexel->TrianglePoints[i * 2];
                    Point2 = tempTriDexel->TrianglePoints[i * 2 + 1];
                    Point3 = tempTriDexel->TrianglePoints[(i * 2 + 2) % 6];
                    Point4 = tempTriDexel->TrianglePoints[(i * 2 + 3) % 6];

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
                }

                tempTriDexel = tempTriDexel->Next;
                tempTriRenderDexel = tempTriRenderDexel->Next;
            }

            continue;
        }
    }

    std::cout << "CountDelta:" << CountDelta << std::endl;
    std::cout << "CountQuad" << CountQuad << std::endl;
    std::cout << "CountSide" << CountSide << std::endl;
    std::cout << "DexelTriangles.size():" << DexelTriangles.size() / 3 << std::endl;
}

void MRR_Dexel::InitDexelTriangle_RenderType()
{
    TriangleDexelArray.resize(DexelArraySize - 1);
    TriangleRenderDexelArray.resize(DexelArraySize - 1);

    // for (int i = 0; i < TriangleDexelArray.size(); i++) {
    //     TriangleDexelArray[i].resize(DexeArraySize - 1);
    // }
    // for (int i = 0; i < TriangleRenderDexelArray.size(); i++) {
    //     TriangleRenderDexelArray[i].resize(DexeArraySize - 1);
    // }
    std::cout << "DexelTriangleInitialize_START" << std::endl;

    int TriDexelCount = 0;

    std::for_each(std::execution::par, TriangleDexelArray.begin(), TriangleDexelArray.end(), [&](std::vector<TriangleDexel*>& TriDexelArray) {
        // for (int Y = 0; Y < TriDexelArray.size(); Y++) {
        //     TriDexelArray[Y] = new TriangleDexel();
        //     TriDexelCount++;
        // }

        int X = &TriDexelArray - &TriangleDexelArray[0];
        // if (X >= TriangleDexelArray.size()) {
        //     return;
        // }
        TriDexelArray.resize(DexelArraySize - 1);
        TriangleRenderDexelArray[X].resize(DexelArraySize - 1);
        std::for_each(std::execution::par, TriDexelArray.begin(), TriDexelArray.end(), [&](TriangleDexel*& TriDexel) {
            int Y = &TriDexel - &TriDexelArray[0];
            // if (Y >= TriDexelArray.size()) {
            //     return;
            // }
            //判读四个角的DexelArray是否不为空，有一个为空则返回
            if ((X + 1) >= DexelArray.size() || (Y + 1) >= DexelArray[X].size()) {
                return;
            }
            if ((DexelArray[X][Y] == nullptr) || (DexelArray[X + 1][Y] == nullptr) || (DexelArray[X][Y + 1] == nullptr) || (DexelArray[X + 1][Y + 1] == nullptr)) {
                return;
            };

            TriDexel = new TriangleDexel();
            TriangleRenderDexel* TriRenderDexel = new TriangleRenderDexel();
            TriangleRenderDexelArray[X][Y] = TriRenderDexel;

            PowerDexel* White = new PowerDexel(-1000.0f, 1000.0f);
            PowerDexel* Black = nullptr;
            int DexelIndex_X, DexelIndex_Y;
            PowerDexel* tempDexel = nullptr;

            int CountCornerDexel = 4;
            for (int i = 0; i < 8; i += 2) { //初始化黑域
                DexelIndex_X = X + VertexMask[i];
                DexelIndex_Y = Y + VertexMask[i + 1];

                tempDexel = DexelArray[DexelIndex_X][DexelIndex_Y];
                if (tempDexel == nullptr) {
                    Black == nullptr;
                    break;
                }
                if (Black == nullptr && tempDexel != nullptr) {
                    Black = new PowerDexel(tempDexel); //初始化黑域
                }

                if (Black != nullptr && tempDexel != nullptr) {
                    White = DexelBoolGetWhiteZone(White, DexelArray[DexelIndex_X][DexelIndex_Y]);
                    Black = DexelBoolGetBlackZone(Black, DexelArray[DexelIndex_X][DexelIndex_Y]);
                }
            }
            if (Black == nullptr) {
                delete Black;
                delete White;
                delete TriDexel;
                delete TriangleRenderDexelArray[X][Y];
                TriDexel = nullptr;
                TriangleRenderDexelArray[X][Y] = nullptr;
                return;
            }
            tempDexel = Black; //先对白域进行适当扩展，再对黑域进行适当缩小,再获取灰域
            float SegmentLength; //对初始黑域进行适当缩小,对白域进行适当扩展

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
            //获取灰域
            float BottomGrayMin = White->Second;
            float BottomGrayMax = tempDexel->First;
            float TopGrayMin, TopGrayMax;
            PowerDexel* tempCornerDexel = nullptr;
            Vector3Df CornerPoint;
            PowerDexel* tempWhite = White;
            std::vector<Vector3Df> TrianglePoint;

            while (tempDexel != nullptr) {
                if (White->Next == nullptr) {
                    break;
                }
                if (tempDexel->Next != nullptr) {
                    if (tempDexel->Next->Second < White->Next->First) {
                        tempDexel = tempDexel->Next;
                        continue;
                    }
                }
                TopGrayMin = tempDexel->Second;
                TopGrayMax = White->Next->First;
                for (int i = 0; i < 8; i = i + 2) {
                    DexelIndex_X = X + VertexMask[i];
                    DexelIndex_Y = Y + VertexMask[i + 1];
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

            //初始化三角形渲染Dexel结构，每个Dexel的一个节点对应一个柱形，有n个节点代表这条射线上有n个柱形
            if ((TrianglePoint.size() >= 8) && (TrianglePoint.size() % 8) == 0) {
                for (int i = 0; i < TrianglePoint.size(); i = i + 8) {
                    for (int j = 0; j < 8; j++) {
                        TriDexel->TrianglePoints.push_back(TrianglePoint[i + j]);
                    }
                    if ((i + 8) < TrianglePoint.size()) {
                        TriDexel->Next = new TriangleDexel();
                        TriRenderDexel->Next = new TriangleRenderDexel();
                        TriDexel = TriDexel->Next;
                        TriRenderDexel = TriRenderDexel->Next;
                    }
                }
            }
            DeleteDexel(Black);
            DeleteDexel(tempWhite);
        });
    });

    std::vector<int> SideTriIndex;

    for (int X = 0; X < (DexelArraySize - 1); X++) {

        for (int Y = 0; Y < (DexelArraySize - 1); Y++) {
            if (TriangleDexelArray[X][Y] == nullptr) {
                continue;
            }
            TriangleDexel* tempTriDexel = TriangleDexelArray[X][Y];
            TriangleRenderDexel* tempTriRenderDexel = TriangleRenderDexelArray[X][Y];
            while ((tempTriDexel != nullptr)) {
                if (tempTriDexel->TrianglePoints.size() == 6) {
                    break;
                }
                if (tempTriDexel->TrianglePoints.size() != 8) {
                    tempTriDexel = tempTriDexel->Next;
                    tempTriRenderDexel = tempTriRenderDexel->Next;
                    continue;
                }
                //记录渲染列表下标
                tempTriRenderDexel->TriangleIndex = CubeCenter.size();
                CubeCenter.push_back(tempTriDexel->TrianglePoints[0]);

                for (int i = 0; i < 8; i++) {
                    PointHeight.push_back(tempTriDexel->TrianglePoints[i].z);
                }

                Vector3Df SideNormal;
                if (CountTriDexelIsSide(X, Y, tempTriDexel)) { // 柱体未被完全遮掩，添加侧面
                    //记录是否有添加测面面片，用来计算Dexel节点在渲染列表该位置填入了多少顶点：顶面+底面+侧面=6+6+4*6=36
                    tempTriRenderDexel->IsAddSide = true;
                    Vector3Df Normal1 = (tempTriDexel->TrianglePoints[0] - tempTriDexel->TrianglePoints[2]).cross(tempTriDexel->TrianglePoints[0] - tempTriDexel->TrianglePoints[4]).normalize();
                    float dot1 = Normal1.dot(Vector3Df(0.0f, 0.0f, -1.0f));
                    if (dot1 < 0.0f) {
                        Normal1 = -Normal1;
                    }
                    Vector3Df Normal2 = (tempTriDexel->TrianglePoints[1] - tempTriDexel->TrianglePoints[3]).cross(tempTriDexel->TrianglePoints[1] - tempTriDexel->TrianglePoints[5]).normalize();
                    float dot2 = Normal2.dot(Vector3Df(0.0f, 0.0f, 1.0f));
                    if (dot2 < 0.0f) {
                        Normal2 = -Normal2;
                    }
                    Vector3Df MaskNormal = Vector3Df(0, 0, 1);

                    if (std::abs(Normal2.dot(MaskNormal)) > 0.95) {
                        if (std::abs(Normal1.dot(MaskNormal)) > 0.95) { //Normal2接近垂直XY平面
                            // SideTriIndex.push_back(X);
                            // SideTriIndex.push_back(Y);
                            SideNormal = Vector3Df(0.0f, 0.0f, 3.0f);
                        } else {
                            SideNormal = Normal1;
                            SideNormal.normalize();
                        }
                    } else {
                        SideNormal = Normal2;
                        SideNormal.normalize();
                    }
                }

                CubeNormals.push_back(SideNormal); //在着色器中通过法向量的模来判断

                tempTriDexel = tempTriDexel->Next;
                tempTriRenderDexel = tempTriRenderDexel->Next;
            }
            continue;
        }
    }

    Vector3Df MaskNormal = Vector3Df(0, 0, 1);
    Vector3Df faceNormal;

    for (int i = 0; i < SideTriIndex.size(); i = i + 2) {
        int X = SideTriIndex[i];
        int Y = SideTriIndex[i + 1];
        TriangleDexel* tempTriDexel = TriangleDexelArray[X][Y];
        TriangleRenderDexel* tempTriRenderDexel = TriangleRenderDexelArray[X][Y];
        Vector3Df CenterPoint = getDexelCenter(X, Y);
        //搜索25格内所有为addside的TriDexel
        while (tempTriDexel != nullptr) {
            faceNormal = DexelTrianglesNormals[tempTriRenderDexel->TriangleIndex + 13];
            Vector3Df LastNormal = Vector3Df(0.0f, 0.0f, 0.0f);
            Vector3Df SideNormal;
            Vector3Df NeigthborPoint;
            int OperatorSize = 2; //算子大小(OperatorSize * 2 + 1)
            //5 * 5 的int数组
            std::vector<std::vector<int>> AddSideArray = std::vector<std::vector<int>>(OperatorSize * 2 + 1, std::vector<int>(OperatorSize * 2 + 1, 0));
            for (int m = X - OperatorSize; m <= X + OperatorSize; m++) {
                for (int n = Y - OperatorSize; n <= Y + OperatorSize; n++) {
                    if ((m == X) && (n == Y)) {
                        continue;
                    }
                    if ((m < 0) || (m >= (DexelArraySize - 1)) || (n < 0) || (n >= (DexelArraySize - 1)) || (TriangleDexelArray[m][n] == nullptr)) {
                        continue;
                    }
                    TriangleRenderDexel* tempNeiTriRenderDexel = TriangleRenderDexelArray[m][n];
                    while (tempNeiTriRenderDexel != nullptr) {
                        if (tempNeiTriRenderDexel->IsAddSide) {
                            int ArrayIndex_X = m - X + OperatorSize;
                            int ArrayIndex_Y = n - Y + OperatorSize;
                            AddSideArray[ArrayIndex_X][ArrayIndex_Y] = 1;
                            break;
                        }
                        tempNeiTriRenderDexel = tempNeiTriRenderDexel->Next;
                    }
                }
            }
            for (int m = X - OperatorSize; m <= X + OperatorSize; m++) {
                for (int n = Y - OperatorSize; n <= Y + OperatorSize; n++) {
                    if ((m == X) && (n == Y)) {
                        continue;
                    }
                    int ArrayIndex_X = m - X + OperatorSize;
                    int ArrayIndex_Y = n - Y + OperatorSize;
                    if (AddSideArray[ArrayIndex_X][ArrayIndex_Y] == 0)
                        continue;
                    NeigthborPoint = getDexelCenter(m, n);
                    NeigthborPoint = NeigthborPoint - CenterPoint;
                    float SideLength = NeigthborPoint.length();
                    NeigthborPoint = NeigthborPoint.normalize();
                    SideNormal = NeigthborPoint.cross(MaskNormal);
                    //重要
                    if (m < X) {
                        SideNormal = SideNormal * (-1.0f);
                    } else if (m == X) {
                        if (n < Y) {
                            SideNormal = SideNormal * (-1.0f);
                        }
                    }
                    //判断该邻居对面的部分是否为addside，不是的话降低它的权重，是的话增加权重
                    //(OperatorSize,OperatorSize)为中心点，(0,0)为左下角点，(2 * OperatorSize,2 * OperatorSize)为右上角点
                    int deltaX = ArrayIndex_X - OperatorSize;
                    int deltaY = ArrayIndex_X - OperatorSize;
                    int CaterCornerX = OperatorSize - deltaX;
                    int CaterCornerY = OperatorSize - deltaY;
                    if (AddSideArray[CaterCornerX][CaterCornerY] == 0) {
                        SideLength = 0.30f * SideLength;
                    } else {
                        SideLength = 1.50f * SideLength;
                    }
                    //以邻居点和中心点的长度作为加权，它们之间的距离越远，权重越大
                    LastNormal = LastNormal + SideNormal * SideLength;
                }
            }
            LastNormal = LastNormal.normalize();
            int NormalIndex = tempTriRenderDexel->TriangleIndex + 1;
            NormalIndex = tempTriRenderDexel->TriangleIndex;
            LastNormal = LastNormal / (2.0f); //将该向量的模设为0.5，用于在shader中判断是否为需要处理的法向量
            CubeNormals[NormalIndex] = LastNormal;
            tempTriDexel = tempTriDexel->Next;
            tempTriRenderDexel = tempTriRenderDexel->Next;
        }
    }

    changeCoord();
    std::cout << "DexelTriangleInitialize_END" << std::endl;
}

double MRR_Dexel::CalculateWorkPieceVolume()
{
    double result = 0.0f;

    std::for_each(std::execution::seq, DexelArray.begin(), DexelArray.end(), [&](std::vector<PowerDexel*>& dexelArray) {
        const size_t i = &dexelArray - DexelArray.data();

        std::for_each(std::execution::seq, dexelArray.begin(), dexelArray.end(), [&](PowerDexel*& dexel) {
            const size_t j = &dexel - dexelArray.data();

            if (dexel == nullptr) {
                return;
            }
            PowerDexel* tempDexel = dexel;
            double DexelVolume = 0.0f;
            // std::cout << "i: " << i << " j: " << j << std::endl;
            while (tempDexel != nullptr) {
                DexelVolume = tempDexel->Second - tempDexel->First;

                tempDexel = tempDexel->Next;
            }
            // std::cout << "DexelVolume: " << DexelVolume << std::endl;
            result = result + DexelVolume;
            // std::cout << "result: " << result << std::endl;
        });
    });

    WorkpieceVolume = result;
    // std::cout << "WorkpieceVolume: " << WorkpieceVolume << std::endl;
    return WorkpieceVolume;
}

void MRR_Dexel::MillingWithCutter_5axis_SweptVolume(DexelCutterSweptVolume& SweptVolume)
{

    int MinX = std::ceil(float(SweptVolume.sweptVolmueParam.Min_X - ABBox.getMin().x) / UnitLength) - 1;
    int MaxX = std::ceil(float(SweptVolume.sweptVolmueParam.Max_X - ABBox.getMin().x) / UnitLength) + 1;

    int MinY = std::ceil(float(SweptVolume.sweptVolmueParam.Min_Y - ABBox.getMin().y) / UnitLength) - 1;
    int MaxY = std::ceil(float(SweptVolume.sweptVolmueParam.Max_Y - ABBox.getMin().y) / UnitLength) + 1;

    MinX = std::max(MinX - 1, 0);
    MinY = std::max(MinY - 1, 0);
    MinX = std::min(MinX, int(DexelArray.size()));
    MinY = std::min(MinY, int(DexelArray.size()));

    MaxX = std::min(MaxX + 1, int(DexelArray.size()));
    MaxY = std::min(MaxY + 1, int(DexelArray.size()));
    MaxX = std::max(MaxX, 0);
    MaxY = std::max(MaxY, 0);
    //输出范围面积：
    // std::cout << "ABBox_Min: " << ABBox.getMin().x << " " << ABBox.getMin().y << " " << ABBox.getMin().z << std::endl;
    // std::cout << "ABBox_Max: " << ABBox.getMax().x << " " << ABBox.getMax().y << " " << ABBox.getMax().z << std::endl;
    // std::cout << "CoordOffset: " << CoordOffset.x << " " << CoordOffset.y << " " << CoordOffset.z << std::endl<<std::endl;
    // CountXY = CountXY + (MaxY - MinY) * (MaxX - MinX);
    // std::cout << "(MaxY - MinY): " << (MaxY - MinY) << "(MaxX - MinX): " << (MaxX - MinX) << std::endl;
    // CountStep++;
    // if (CountStep == 100) {
    //     std::cout << "CountXY " << CountXY / 100 << std::endl;
    //     CountXY = 0;
    //     CountStep = 0;
    // }
    // std::cout << "Min_X" << SweptVolume.sweptVolmueParam.Min_X << " Min_Y" << SweptVolume.sweptVolmueParam.Min_Y << std::endl;
    // std::cout << "Max_X" << SweptVolume.sweptVolmueParam.Max_X << " Max_Y" << SweptVolume.sweptVolmueParam.Max_Y << std::endl;
    // std::cout << "MinX: " << MinX << " MinY: " << MinY << " MaxX: " << MaxX << " MaxY: " << MaxY << std::endl;
    //不知道为什么需要加1
    //Dexel模型与刀具进行布尔运算并更新
    auto DexelArray_0 = &DexelArray[0];
    std::for_each(std::execution::par, DexelArray.begin() + MinX, DexelArray.begin() + MaxX, [&](std::vector<PowerDexel*>& DexelArrayX) {
        // int X = &DexelArrayX - &DexelArray[0];
        int X = &DexelArrayX - DexelArray_0;
        auto DexelArrayX_0 = &DexelArrayX[0];
        // std::cout << "X: " << X << std::endl;
        std::for_each(std::execution::par, DexelArrayX.begin() + MinY, DexelArrayX.begin() + MaxY, [&](PowerDexel*& dexel) {
            if (dexel == nullptr) {
                return;
            }

            // int Y = &dexel - &DexelArrayX[0];
            int Y = &dexel - DexelArrayX_0;
            // std::cout << "Y: " << Y << std::endl;
            // dexel->IsChanged = false;
            Vector3Df Center = getDexelCenterWithCoordOffset(X, Y);

            float Heigth; //cutter->getCutterSurfaceHeight(Center.x, Center.y) + CoordOffset.z;//修正
            Vector3Df CutterPoint = Vector3Df(0, 0, 0); //cutter->CurrentPosture.center;//修正
            float PreVolume = dexel->getDexelVolume();

            if (SweptVolume.GetCutterDexelRange(Heigth, CutterPoint, Center.x, Center.y)) {
                if (Heigth > dexel->getMaxZ()) {
                    return;
                }
                Vector3Df cutterHeigth = Vector3Df(Heigth, 10000, 0);
                float Min_Z = dexel->getMinZ();
                //布尔运算函数中考虑到了多段Dexel的情况
                dexel = DexelBoolCalculation(dexel, cutterHeigth.x, cutterHeigth.y);
                if (dexel == nullptr) {
                    dexel = new PowerDexel(Min_Z, Min_Z + 0.000001);
                    // dexel->IsChanged = true;
                    // dexel->CuttePoint = CutterPoint;
                    // return;
                }
                dexel->IsChanged = true;
                dexel->CuttePoint = CutterPoint; //cutter->CurrentPosture.center;//修正
            }
            dexel->ChangedVolume = PreVolume - dexel->getDexelVolume();
        });
    });

    // // // 使用std并行获取Dexel的体积
    SweptVolume.sweptVolmueParam.MillingVolume = std::accumulate(DexelArray.begin() + MinX, DexelArray.begin() + MaxX, 0.0f, [MinY, MaxY](float currentSum, std::vector<PowerDexel*>& DexelArrayX) {
        return std::accumulate(DexelArrayX.begin() + MinY, DexelArrayX.begin() + MaxY, currentSum, [](float currentSum, PowerDexel* dexel) {
            // if (dexel == nullptr) {
            //     return currentSum;
            // }
            if ((dexel != nullptr) && (dexel->ChangedVolume > 0.0001)) {
                float ChangedVolume = dexel->ChangedVolume;
                dexel->ChangedVolume = 0.0f;
                return currentSum + ChangedVolume;
            }
            return currentSum;
        });
    });
    // std::cout << "SweptVolume.sweptVolmueParam.MillingVolume: " << SweptVolume.sweptVolmueParam.MillingVolume << std::endl;
    // float changedVolume = 0.0;
    // for (int i = MinX; i < MaxX; i++) {
    //     for (int j = MinY; j < MaxY; j++) {
    //         if (DexelArray[i][j] != nullptr) {
    //             if (DexelArray[i][j]->ChangedVolume > 0.0001) {
    //                 changedVolume += DexelArray[i][j]->ChangedVolume;
    //                 DexelArray[i][j]->ChangedVolume = 0.0f;
    //             }
    //         }
    //     }
    // }
    // SweptVolume.sweptVolmueParam.MillingVolume = changedVolume;
    // std::cout<<"changedVolume: "<<changedVolume<<std::endl;
    // std::cout << "changedVolume: " << changedVolume << std::endl;
}

void MRR_Dexel::updateDexelRenderList()
{
    // std::cout << "updateDexelRenderList" << std::endl;
    Vector3Df Min = ABBox.getMin();
    Vector3Df Max = ABBox.getMax();
    float MaxLength = std::max(std::max(Max.x - Min.x, Max.y - Min.y), Max.z - Min.z);

    std::for_each(std::execution::par, TriangleDexelArray.begin(), TriangleDexelArray.end(), [&](std::vector<TriangleDexel*>& TriangleDexelArrayX) {
        int X = &TriangleDexelArrayX - &TriangleDexelArray[0];
        std::for_each(std::execution::par, TriangleDexelArrayX.begin(), TriangleDexelArrayX.end(), [&](TriangleDexel*& TriDexel) {
            if (TriDexel == nullptr) {
                return;
            }
            int Y = &TriDexel - &TriangleDexelArrayX[0];

            bool Ischanged = false;
            bool SetToVoid = false;
            int DexelIndex_X, DexelIndex_Y;
            TriangleDexel* tempTriDexel = TriDexel;
            TriangleRenderDexel* tempTriRender = TriangleRenderDexelArray[X][Y];
            int count = 0;
            for (int i = 0; i < 8; i = i + 2) { //查看是否需要修改该三角面片
                DexelIndex_X = X + VertexMask[i];
                DexelIndex_Y = Y + VertexMask[i + 1];
                // //cornerDexel中存在空指针，后续无法生成面片，需要仔细判断原有面片
                // if (DexelArray[DexelIndex_X][DexelIndex_Y] == nullptr) {
                //     //将其设为changed,清空TriPoint,后续合成三角面片填入渲染列表时再将其TriDexel和TriRender删除
                //     //并将空出来的位置填入填入空闲列表以待备用
                //     tempTriRender->IsChanged = true;
                //     Ischanged = false; //助其跳过后续的计算
                //     break;
                // }
                // if (DexelArray[DexelIndex_X][DexelIndex_Y]->IsChanged) {
                //     Ischanged = true;
                //     break;
                // }

                if (DexelArray[DexelIndex_X][DexelIndex_Y]->getMaxZ() - DexelArray[DexelIndex_X][DexelIndex_Y]->getMinZ() < 0.0002f) {
                    count++;
                }
                if (DexelArray[DexelIndex_X][DexelIndex_Y]->IsChanged) {
                    Ischanged = true;
                }
            }

            if (!Ischanged) {
                return;
            }
            /******************通过DexelArray的四条边，计算DexelTriangleArray**********************/
            //删除原有的TriDexel三角面片顶点并回收内存,创建新的TriDexel，TriRender等到下一步再删除并创建新的TriRender，这里只是将其设为changed，
            tempTriRender->IsChanged = true;
            while (tempTriDexel != nullptr) {
                TriangleDexel* childTridexel = tempTriDexel->Next;
                tempTriDexel->Next = nullptr;
                delete tempTriDexel;
                tempTriDexel = childTridexel;
            }

            TriDexel = new TriangleDexel();
            tempTriDexel = TriDexel;
            /*********************************获取白域与黑域开始***********************************/
            PowerDexel* White = new PowerDexel(-1000.0f, 1000.0f);
            PowerDexel* Black = nullptr; //初始化白域和黑域
            PowerDexel* tempDexel = nullptr;
            //重要，避免生成已经被删去的三角面片
            if (count == 4) {
                return;
            }

            for (int i = 0; i < 8; i += 2) { //初始化黑域和白域
                DexelIndex_X = X + VertexMask[i];
                DexelIndex_Y = Y + VertexMask[i + 1];
                tempDexel = DexelArray[DexelIndex_X][DexelIndex_Y];
                if (tempDexel == nullptr) {
                    Black == nullptr;
                    break;
                }
                if (Black == nullptr && tempDexel != nullptr) {
                    Black = new PowerDexel(tempDexel); //初始化黑域
                }
                if (Black != nullptr && tempDexel != nullptr) {
                    White = DexelBoolGetWhiteZone(White, DexelArray[DexelIndex_X][DexelIndex_Y]); //获取白域
                    Black = DexelBoolGetBlackZone(Black, DexelArray[DexelIndex_X][DexelIndex_Y]); //获取黑域
                }
            }

            //重点关照处理
            if (Black == nullptr) {
                return; //四个边角点均为空，跳过
            }
            tempDexel = Black; //先对白域进行适当扩展，再对黑域进行适当缩小,再获取灰域
            float SegmentLength;
            while (tempDexel != nullptr) { //对初始黑域进行适当缩小,对白域进行适当扩展
                SegmentLength = tempDexel->Second - tempDexel->First;
                tempDexel->First += (SegmentLength * 0.1);
                tempDexel->Second -= (SegmentLength * 0.1);
                tempDexel = tempDexel->Next;
            }
            tempDexel = White; //对白域的空档进行适当扩张
            while (tempDexel->Next != nullptr) {
                SegmentLength = std::min(tempDexel->Next->First - tempDexel->Second, tempDexel->Next->Second - tempDexel->Next->First);
                SegmentLength = std::min(SegmentLength, tempDexel->Second - tempDexel->First);
                SegmentLength = std::abs(SegmentLength);
                tempDexel->Second -= (SegmentLength * 0.1);
                tempDexel->Next->First += (SegmentLength * 0.1);
                tempDexel = tempDexel->Next;
            }
            /*********************************获取白域与黑域结束***********************************/
            /*********************************计算灰域并获取三角面片顶点****************************/
            tempDexel = Black; //根据黑域和白域获取灰域并获取三角形
            float BottomGrayMin = White->Second; //获取底部面片的灰域
            float BottomGrayMax = tempDexel->First;
            float TopGrayMin, TopGrayMax;
            PowerDexel* tempCornerDexel = nullptr;
            Vector3Df CornerPoint;
            PowerDexel* tempWhite = White;
            std::vector<Vector3Df> TrianglePoint;
            while (tempDexel != nullptr) { //遍历黑域
                if (White->Next == nullptr) {
                    break;
                }
                if (tempDexel->Next != nullptr) {
                    if (tempDexel->Next->Second < White->Next->First) { //黑域还未结束，必然存在下一个白域
                        tempDexel = tempDexel->Next;
                        continue;
                    } //否则，tempDexel与tempDexel->Next存在白域，可以在灰域中获取三角形
                } //否则，tempDexel已经没有下一个顶点，它的下一个区域就是白域，可以在灰域中获取三角形顶点
                TopGrayMin = tempDexel->Second; //获取顶部面片的灰域
                TopGrayMax = White->Next->First;
                for (int i = 0; i < 8; i = i + 2) { //遍历4个顶点，获取三角形的顶点
                    DexelIndex_X = X + VertexMask[i];
                    DexelIndex_Y = Y + VertexMask[i + 1];
                    tempCornerDexel = DexelArray[DexelIndex_X][DexelIndex_Y];
                    if (tempCornerDexel == nullptr) {
                        break;
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
            /*********************************计算灰域并获取三角面片顶点结束****************************/
            /*********************************初始化TriDexel****************************/
            if ((TrianglePoint.size() >= 8) && (TrianglePoint.size() % 8) == 0) {
                for (int i = 0; i < TrianglePoint.size(); i = i + 8) {
                    for (int j = 0; j < 8; j++) {
                        TriDexel->TrianglePoints.push_back(TrianglePoint[i + j]);
                    }
                    if ((i + 8) < TrianglePoint.size()) {
                        TriDexel->Next = new TriangleDexel();
                        //TriRenderDexel->Next = new TriangleRenderDexel();只能等到修改渲染列表时再初始化
                        TriDexel = TriDexel->Next;
                    }
                }
            }
            DeleteDexel(Black);
            DeleteDexel(tempWhite);
            /*********************************初始化TriDexel结束***********************/
        });
    });
    // std::cout << "RenderListSizeChange_BEGIN" << std::endl;
    for (int X = 0; X < TriangleDexelArray.size(); X++) {
        for (int Y = 0; Y < TriangleDexelArray.size(); Y++) {
            if (TriangleDexelArray[X][Y] == nullptr) {
                continue;
            }

            for (int i = 0; i < 4; i++) {
                int x = std::min(X + i % 2, int(TriangleDexelArray.size() - 1));
                int y = std::min(Y + i / 2, int(TriangleDexelArray.size() - 1));
                if (DexelArray[x][y] != nullptr) {
                    DexelArray[x][y]->IsChanged = false;
                }
            }

            TriangleDexel* tempTriDexel = TriangleDexelArray[X][Y];
            TriangleRenderDexel* tempTriRenderDexel = TriangleRenderDexelArray[X][Y]; //用于回收旧RenderIndex
            // std::cout << "Begin_getRenderIndex" << std::endl;
            /*********************************回收渲染列表坐标***********************/
            if (tempTriRenderDexel->IsChanged) { //首先回收渲染列表坐标，后面再分配
                while (tempTriRenderDexel != nullptr) {

                    if (tempTriRenderDexel->TriangleIndex >= 0) {
                        Vector3Df vector = Vector3Df(-10000.0f, -10000.0f, -10000.0f);
                        VoidRenderIndexList.push(tempTriRenderDexel->TriangleIndex);
                        CubeCenter[tempTriRenderDexel->TriangleIndex] = vector;
                        CubeNormals[tempTriRenderDexel->TriangleIndex] = Vector3Df(0.0f, 0.0f, 0.0f);
                        //注意检查fill_n的使用是否正确
                        std::fill_n(PointHeight.begin() + tempTriRenderDexel->TriangleIndex * 8, 8, 0.0f);
                    }

                    TriangleRenderDexel* temp = tempTriRenderDexel->Next;
                    tempTriRenderDexel->Next = nullptr;
                    delete tempTriRenderDexel;
                    tempTriRenderDexel = temp;
                }

            } else {
                continue;
            }
            /***************************回收渲染列表坐标结束**************************/
            /****************根据TriDexel分段初始化TriRenderDexel********************/
            TriangleRenderDexelArray[X][Y] = new TriangleRenderDexel(); //放到循环里创建
            tempTriRenderDexel = TriangleRenderDexelArray[X][Y];

            while ((tempTriDexel != nullptr)) { //初始化TriRenderDexel
                if (tempTriDexel->Next != nullptr) {
                    tempTriRenderDexel->Next = new TriangleRenderDexel();
                }
                tempTriDexel = tempTriDexel->Next;
                tempTriRenderDexel = tempTriRenderDexel->Next;
            }
            tempTriDexel = TriangleDexelArray[X][Y];
            tempTriRenderDexel = TriangleRenderDexelArray[X][Y];
            // std::cout << "Init_TriRenderDexel_End" << std::endl;
            /**********************初始化TriRenderDexel结束*************************/
            /************************开始组合面片并压入渲染列表************************/
            float TopZ_0, TopZ_1, BotZ_0, BotZ_1;
            float Nerighbor_TopZ_0, Nerighbor_TopZ_1, Nerighbor_BotZ_0, Nerighbor_BotZ_1;
            int Neighbor_X, Neighbor_Y;
            int AddSide = 0; //判断是否添加侧面面片
            while ((tempTriDexel != nullptr)) {
                if (tempTriDexel->TrianglePoints.size() != 8) {
                    tempTriDexel = tempTriDexel->Next;
                    tempTriRenderDexel = tempTriRenderDexel->Next;
                    continue;
                }
                AddSide = 0; //遍历四个邻居Dexel，判断是否需要添加侧面面片
                for (int i = 0; i < 4; i++) {
                    Neighbor_X = X + GetNeighbor[i][0];
                    Neighbor_Y = Y + GetNeighbor[i][1];
                    if ((Neighbor_X < 0) || (Neighbor_X >= (DexelArraySize - 1)) || (Neighbor_Y < 0) || (Neighbor_Y >= (DexelArraySize - 1)) || (TriangleDexelArray[Neighbor_X][Neighbor_Y] == nullptr)) {
                        AddSide = 0;
                        break;
                    }
                    TriangleDexel* NeighborDexel = TriangleDexelArray[Neighbor_X][Neighbor_Y];
                    float length_0, length_1;
                    BotZ_0 = tempTriDexel->TrianglePoints[NeighToSide[i][0] * 2].z;
                    BotZ_1 = tempTriDexel->TrianglePoints[NeighToSide[i][1] * 2].z;
                    TopZ_0 = tempTriDexel->TrianglePoints[NeighToSide[i][0] * 2 + 1].z;
                    TopZ_1 = tempTriDexel->TrianglePoints[NeighToSide[i][1] * 2 + 1].z;
                    while (NeighborDexel != nullptr) { //判断所有柱体
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
                        Nerighbor_BotZ_0 = Nerighbor_BotZ_0 - length_0 * 0.01; //适当扩大遮掩范围
                        Nerighbor_BotZ_1 = Nerighbor_BotZ_1 - length_1 * 0.01;
                        Nerighbor_TopZ_0 = Nerighbor_TopZ_0 + length_0 * 0.01;
                        Nerighbor_TopZ_1 = Nerighbor_TopZ_1 + length_1 * 0.01;

                        bool IsAddSide = ((Nerighbor_TopZ_0 >= TopZ_0) && (Nerighbor_TopZ_1 >= TopZ_1) && (Nerighbor_BotZ_0 <= BotZ_0) && (Nerighbor_BotZ_1 <= BotZ_1));

                        if (IsAddSide) { //该侧面被遮掩
                            AddSide++;
                            break;
                        }
                        NeighborDexel = NeighborDexel->Next;
                    }
                }

                tempTriRenderDexel->IsAddSide = (AddSide < 4) ? true : false;
                // std::cout << "Begin_FillPoint" << std::endl;
                //添加顶部和底部面片
                Vector3Df SideNormal = Vector3Df(0.0f, 0.0f, 0.0f);
                if (VoidRenderIndexList.empty()) {

                    tempTriRenderDexel->TriangleIndex = CubeCenter.size();
                    CubeCenter.push_back((tempTriDexel->TrianglePoints[0])); // / MaxLength
                    for (int i = 0; i < 8; i++) {
                        PointHeight.push_back((tempTriDexel->TrianglePoints[i].z)); /// MaxLength
                    }
                    CubeNormals.push_back(SideNormal); //在着色器中通过法向量的模来判断是否需要添加面片
                } else {
                    tempTriRenderDexel->TriangleIndex = VoidRenderIndexList.top();
                    VoidRenderIndexList.pop();
                    CubeCenter[tempTriRenderDexel->TriangleIndex] = (tempTriDexel->TrianglePoints[0]); /// MaxLength
                    for (int i = 0; i < 8; i++) {
                        PointHeight[tempTriRenderDexel->TriangleIndex * 8 + i] = (tempTriDexel->TrianglePoints[i].z); /// MaxLength
                    }
                }

                //修改CubeNormal用于使用侧面面片
                if (tempTriRenderDexel->IsAddSide) {
                    // //记录是否有添加测面面片，用来计算Dexel节点在渲染列表该位置填入了多少顶点：顶面+底面+侧面=6+6+4*6=36

                    SideNormal = Vector3Df(0.0f, 0.0f, 3.0f);
                }
                CubeNormals[tempTriRenderDexel->TriangleIndex] = SideNormal; //添加侧面

                //判断是否为垂直面片，垂直面片的法向量需要根据相邻的垂直面片的拓扑信息进行计算
                //TopFace: 1357 ->15,13,17，先测试顶部面片的效果，底部面片稍后添加
                Vector3Df Line17 = (tempTriDexel->TrianglePoints[7] - tempTriDexel->TrianglePoints[1]).normalize();
                Vector3Df Line13 = (tempTriDexel->TrianglePoints[3] - tempTriDexel->TrianglePoints[1]).normalize();
                Vector3Df Line15 = (tempTriDexel->TrianglePoints[5] - tempTriDexel->TrianglePoints[1]).normalize();

                Vector3Df TopNormal1 = (Line15.cross(Line17)).normalize();
                Vector3Df TopNormal2 = (Line17.cross(Line13)).normalize();

                if ((abs(TopNormal1.z) < 0.05f) || (abs(TopNormal2.z) < 0.05f)) {
                    // tempTriRenderDexel->isDeleta = true; //面片接近垂直，对其进行标记
                    // SideTriIndex.push_back(X); //记录需要重置法向量的面片的索引
                    // SideTriIndex.push_back(Y);
                    // CubeNormals[tempTriRenderDexel->TriangleIndex] = TopNormal1;

                    Vector3Df LastNormal = Vector3Df(0.0f, 0.0f, 1.0f); //cutter->CurrentPosture.center - LastCutterPos;////修正
                    LastNormal.z = 0;
                    LastNormal = LastNormal.normalize();
                    LastNormal = LastNormal.cross(Vector3Df(0.0f, 0.0f, 1.0f));
                    LastNormal.normalize();
                    LastNormal = LastNormal * 5.0f; //将该向量的模设为5，用于在shader中判断是否为需要处理top面片
                    // LastNormal = LastNormal.normalize();
                    LastNormal = Vector3Df(0.0f, 0.0f, 3.0f); //使用侧面面片和自动生成法向量
                    CubeNormals[tempTriRenderDexel->TriangleIndex] = LastNormal;
                } else {
                    tempTriRenderDexel->isDeleta = false;
                }

                tempTriDexel = tempTriDexel->Next;
                tempTriRenderDexel = tempTriRenderDexel->Next;
            }
        }
    }
    // std::cout << "RenderListSizeChange_END" << std::endl;
}

void MRR_Dexel::fullDexelRenderList()
{
    Vector3Df Min = ABBox.getMin();
    Vector3Df Max = ABBox.getMax();
    float MaxLength = std::max(std::max(Max.x - Min.x, Max.y - Min.y), Max.z - Min.z);
    CubeCenter.clear();
    CubeNormals.clear();
    PointHeight.clear();

    for (int i = 0; i < TriangleDexelArray.size() - 1; i++) {
        TriangleDexelArray[i].clear();
        TriangleRenderDexelArray[i].clear();
    }
    TriangleDexelArray.clear();
    TriangleRenderDexelArray.clear();
    InitDexelTriangle_RenderType();
}
