#include "Workpiece.h"

Workpiece::Workpiece(float length, float width, float height, const Vector3Df& position)
{
    this->position = position;

    // this->scale = static_cast<float>(DEXEL_GRID_SIZE) / std::max(length, width);
    this->scale = 1.0f;
    // this->dexelGrid = DexelGrid<DEXEL_GRID_SIZE, DEXEL_GRID_SIZE>(
    //     static_cast<size_t>(length * this->scale),
    //     static_cast<size_t>(width * this->scale),
    //     height * this->scale);
    AABB3Df Dexelbbox;
    // mrrDexel.TestDexel();
    // std::cout << "tan(M_PI/2): " << tan(M_PI / 4.0f) << std::endl;
    Dexelbbox = AABB3Df(position, Vector3Df(position.x + length, position.y + width, position.z + height));
    mrrDexel.InitializeDexelArray(Dexelbbox);
    mrrDexel.InitDexelTriangle_RenderType();
}

void Workpiece::subtract(const DexelCutterSweptVolume& cutterSweptVolume)
{
    // this->dexelGrid.subtract(cutterSweptVolume.dexelGrid);
}

void Workpiece::subtract(std::vector<DexelCutterSweptVolume>& cutterSweptVolumes)
{
    // std::cout << "cutterSweptVolumess.size():" << cutterSweptVolumes.size() << std::endl;
    //获取当前时间
    // auto start = std::chrono::system_clock::now();

    this->CurrentVolume.clear();
    this->CurrentTime.clear();

    for (auto& cutterSweptVolume : cutterSweptVolumes) {
        // this->subtract(cutterSweptVolume);
        // std::function<bool(float&, Vector3Df&, const float&, const float&)> func = std::bind(&CutterSweptVolume::GetCutterDexelRange, cutterSweptVolume, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        //如果是用lamba表达式实现上述函数绑定,需要返回一个bool值
        // auto func = [&cutterSweptVolume](float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y) { return cutterSweptVolume.GetCutterDexelRange(Heigth, CutterPoint, Dexel_X, Dexel_Y); };
        // std::cout << "StartDexel: " << cutterSweptVolume.StartDexel << "EndDexel: " << cutterSweptVolume.EndDexel << std::endl;
        // std::cout<<"cutterSweptVolume.StartDexel: "<<cutterSweptVolume.sweptVolmueParam.StartDexel<<std::endl;
        // std::cout<<"cutterSweptVolume.EndDexel: "<<cutterSweptVolume.sweptVolmueParam.EndDexel<<std::endl;
        mrrDexel.MillingWithCutter_5axis_SweptVolume(cutterSweptVolume);

        float millingTime = cutterSweptVolume.sweptVolmueParam.StartDexel.distanceToPoint(cutterSweptVolume.sweptVolmueParam.EndDexel);

        this->MillingVolume.push_back(cutterSweptVolume.sweptVolmueParam.MillingVolume);
        this->MillingTime.push_back(millingTime);
        this->CurrentVolume.push_back(cutterSweptVolume.sweptVolmueParam.MillingVolume);
        this->CurrentTime.push_back(millingTime);
        // std::cout << "MillingVolume: " << cutterSweptVolume.sweptVolmueParam.MillingVolume << std::endl;
        // std::cout << "MillingTime: " << millingTime << std::endl;
        // this->CurrentVolume.push_back(cutterSweptVolume.sweptVolmueParam.MillingVolume);
        // this->CurrentTime.push_back(millingTime);

        // this->CurrentMRR.push_back(cutterSweptVolume.sweptVolmueParam.MillingVolume / millingTime);
    }
    // std::for_each(std::execution::seq,cutterSweptVolumes.begin(),cutterSweptVolumes.end(),[&](CutterSweptVolume& cutterSweptVolume){
    //     mrrDexel.MillingWithCutter_5axis_SweptVolume(cutterSweptVolume);
    // });
    // //获取当前时间
    // auto End = std::chrono::system_clock::now();
    // std::cout<<"Milling time: "<<std::chrono::duration_cast<std::chrono::milliseconds>(End - start).count()<<"ms"<<std::endl;

    mrrDexel.updateDexelRenderList();
    // mrrDexel.fullDexelRenderList();
}

void Workpiece::generateMesh(std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals) const
{
    // this->dexelGrid.generateMesh(vertices, normals);
}
