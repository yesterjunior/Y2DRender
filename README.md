# Y2DRender
基于OpenGL的简易2D渲染器
主要功能和特点：
1. 使用plist实现粒子效果；
2. 实现完整的pass和裁剪测试、Alpha 测试、模板测试、深度测试和混合模式的设置；
3. 自己实现了matrix运算的类：math.cpp与math.h中。
>OpenGL Version 330，OPENGL-ES Version 200

>用于之后的OPENGL-ES硬件移植工作所写的demo
>
>主要目的是程序化生成图形库，代替以往的存播模式，用来在车机等低带宽的硬件LED场景中渲染动画图形

![2D2](https://user-images.githubusercontent.com/41230077/163804480-aa7456e5-7612-4a23-958f-7a0ef61b2814.gif)

![2D](https://user-images.githubusercontent.com/41230077/163805781-a712aaed-bfa9-47cd-8429-2f6986d33ba0.gif)


## Y2DShader的主要原理

OpenGL顶点着色器着色之后经过正交投影矩阵投影变换到二维空间

## Shader 只是进行一些简单的封装，主要功能：
1. 编译着色程序
2. 绑定 Uniform 数据
3. 根据着色程序的顶点属性传递顶点数据到 GPU
    

## Pass设计
Y2DShader 的 Pass 设计比较简单，主要实现
1. 裁剪测试、Alpha 测试、模板测试、深度测试和混合模式的设置。
2. 填充模式、面剔除模式、顶点正面方向的设置。

```cpp
   void Pass::setOpenGLState()
    {
        /* 面剔除 */
        if ( bEnableCullFace ) {
            glEnable(GL_CULL_FACE);
            glCullFace(toEnum(cullMode));
            glFrontFace(toEnum(frontFace));
        }
        else {
            glDisable(GL_CULL_FACE);
        }

        /* 填充模式 */
        glPolygonMode(GL_FRONT_AND_BACK, toEnum(fillMode));

        /* 裁剪测试 */
        if ( bEnableScissor ) {
            glEnable(GL_SCISSOR_TEST);
            /* 左下角为坐标原点 */
            glScissor(nScissorX, nScissorY, nScissorW, nScissorH);
        }
        else {
            glDisable(GL_SCISSOR_TEST);
        }

        /* Alpha 测试 */
        if ( bEnableAlphaTest ) {
            glEnable(GL_ALPHA_TEST);
            glAlphaFunc(toEnum(alphaCompareFunction), fClampRef);
        }
        else {
            glDisable(GL_ALPHA_TEST);
        }

        /* 模板测试 */
        if ( bEnableStencilTest ) {
            glEnable(GL_STENCIL_TEST);
            glStencilFunc(toEnum(stencilCompareFunction), nStencilRef, nStencilMask);
            glStencilOp(toEnum(failStencilFailDepth), toEnum(passStencilFailDepth), toEnum(passStencilPassDepth));
        }
        else {
            glDisable(GL_STENCIL_TEST);
        }

        /* 深度测试 */
        if ( bEnableDepthTest ) {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(toEnum(depthCompareFunction));
        }
        else {
            glDisable(GL_DEPTH_TEST);
        }

        /* 混合模式 */
        if ( bEnableBlend ) {
            glEnable(GL_BLEND);
            glBlendEquation(toEnum(blendEquation));
            glBlendFuncSeparate(toEnum(blendSrc), toEnum(blendDst), toEnum(blendSrcAlpha), toEnum(blendDstAlpha));
        }
        else {
            glDisable(GL_BLEND);
        }
    }
```
