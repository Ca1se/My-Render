# My-Render
This is a simple software rasterization written by C++17, It simulates the entire process when gpu renders a model.

### Blinn-Phong
![picture1](https://github.com/Ca1se/My-Render/blob/master/picture/picture1.png)  
![picture2](https://github.com/Ca1se/My-Render/blob/master/picture/picture2.png)
# Build
This project runs on linux, make sure that you have cmake and these libraries to build this project: z-lib, xcb, xcb-image.
```
git clone https://github.com/Ca1se/My-Render
cd My-Render
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```
# Usage
To use this render, you need prepare obj format model and corresponding png format texture.  
I have prepared some model in dir example.
```
./render --model ../example/cottage/cottage.obj --texture ../example/cottage/cottage_diffuse.png
```

# Reference
[SunXLei](https://github.com/SunXLei):  [计算机图形学系列笔记](https://www.zhihu.com/column/c_1249465121615204352), [SRender](https://github.com/SunXLei/SRender)  
[闫令琪](https://sites.cs.ucsb.edu/~lingqi/index.html): [GAMES101](https://sites.cs.ucsb.edu/~lingqi/teaching/games101.html)
