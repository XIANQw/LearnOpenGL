## 5_shadowmap log
做了一个下午加晚上总算完成了shadow mapping这一章, 也踩了几个坑, 记录一下

### 1. Debug 渲染 Depthmap 到屏幕上
这里用learnOpenGL教程上的quad能顺利渲染出来, 用自己的floor mesh就不行, 只有背景颜色.
后面发现原来是自己没做model变换, 是以floor的模型空间位置渲染的, floor与视线平行了.. 所以看不见.
教程给的quad的顶点坐标就已经覆盖了屏幕了, 所以不用做变换...

### 2. 掉帧
渲染出阴影后一开始很流畅, 随着运行几秒后掉帧严重. 原来我一开始是把所有的texture都加入mesh.textures数组里,
然后每帧都创建了一个depthmap Texture并加入mesh.textures. 导致数组爆炸性增长... 后来改用share_ptr就好了

### 3. floor发出红色高光, 还能看见depthmap的轮廓
Capture frame后发现depthmap不知道为什么被绑定到material.specularmap上了, 明明没有这一步操作...
导致从specularmap上采样出来的颜色只有r值, 所以计算光照后是红色的高光. 之后加入了一个bool值判断是否需要
采样specularmap 后解决.

### 思考
在渲染场景时, 不同的物体有不同的texture, 有的Obj有diff和spec tex, 有的只有diff. 那么是否渲染每一个obj
都要不同的shader呢 ? 按理来说是需要写不同的shader的. 但是GLSL又没有类似 import 的语句, 导致每个shader都
会有很多重复的函数, 比如光照计算函数. 这就很不方便. 之后得查查看能不能实现 import 提高代码复用率. 
