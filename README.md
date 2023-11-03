
>在开始实验项目之前，请尝试根据[这个教程](https://github.com/BOT-CG/Compter-Graphics/blob/main/vscode%E5%AE%89%E8%A3%85opengl/index.md)用opengl创建简单的窗口

## 项目结构



1. src：源代码目录，包含项目的主要代码。

2. CMakeLists.txt：这是一个CMake文件，用于配置和生成项目。



## Get Started

先将项目文件下载到本地
```bash
git clone https://github.com/BOT-CG/CG-LAB
```

切换到vcpkg.exe所在目录
```bash
cd path/to/vcpkg
```
执行以下命令安装依赖库
```bash
./vcpkg install glfw3:x64-mingw-static
./vcpkg install glm:x64-mingw-static
./vcpkg install assimp:x64-mingw-static
./vcpkg install glad:x64-mingw-static
./vcpkg install stb:x64-mingw-static
./vcpkg install yaml-cpp:x64-mingw-static
``` 

将CMakeLists.txt文件中vcpkg的配置路径修改为自己的路径
```txt
set(VCPKG_ROOT path/to/vcpkg/)

# 其余的设置会自动使用VCPKG_ROOT这个变量
set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake CACHE STRING "")
set(CMAKE_PREFIX_PATH ${VCPKG_ROOT}/installed/x64-mingw-static/share CACHE STRING "")
```

在vscode中选择mingw编译器，点击状态栏的齿轮按钮build整个项目。
### 一步之遥
现在直接build会报一个assimp中findpackge stb找不到的错误，去vcpkg/installed/x64-mingw-static/share/assimp/assimpConfig.cmake中把stb相关那一行删掉，就可以build这个项目了。
### 运行项目

1.可以点击cmake工具的的文件名，然后点击状态栏的三角形按钮运行。  
2.可以用资源管理器打开build文件夹子项目的exe文件运行。  
3.可以使用终端运行
```bash
# 切换到子项目文件夹
cd path/to/build/bin/basic/1.1xxxx

# 运行exe文件
./1.1xxxx.exe
```

> src/utils/中的 shader、mesh、model、camera来自[JoeyDeVries的LearnOpenGL](https://github.com/JoeyDeVries/LearnOpenGL)，该项目是非常好的学习资料。

