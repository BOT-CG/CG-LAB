## 实验目的

- 理解顶点和片段着色器的基础应用。
- 学习如何设置顶点数据和属性。
- 理解OpenGL的渲染流程。

## 实验任务和过程

1. **代码分析**：对以下代码段进行分析。
    - 着色器的编译和链接过程：`glCreateShader()`, `glShaderSource()`, `glCompileShader()`, `glCreateProgram()`, `glAttachShader()`, `glLinkProgram()`
    - 缓冲区对象（Buffer Object）和顶点数组对象（Vertex Array Object）的创建和绑定：`glGenVertexArrays()`, `glGenBuffers()`, `glBindVertexArray()`, `glBindBuffer()`, `glBufferData()`
    - `glVertexAttribPointer()` 和 `glEnableVertexAttribArray()` 的作用是什么？

2. **运行代码**：编译并运行提供的代码。注意观察程序运行结果，并记录下你的观察。

3. **答题与报告**：根据你的代码分析和实验观察，完成以下思考题，并将你的答案和分析整理成一份报告。

## 思考题

1. 解释顶点着色器（Vertex Shader）和片段着色器（Fragment Shader）的基础概念和作用。
2. `glBufferData()` 函数的第三个参数（`GL_STATIC_DRAW`）的作用是什么？有哪些其他选项？
3. 如果你不调用 `glEnableVertexAttribArray()`，会发生什么？请尝试并记录你的观察。

