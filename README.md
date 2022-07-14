# Renderer
This is a simple software rasterization written by C++17.

### Blinn-Phong
![picture1](https://github.com/Ca1se/My-Render/blob/master/picture/picture1.png)  
![picture2](https://github.com/Ca1se/My-Render/blob/master/picture/picture2.png)
# Build
## Arch Linux
```
# install dependency
sudo pacman -S libxcb xcb-util-image zlib

# build
git clone https://github.com/caseshot/Renderer
cd Renderer
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
```
# Usage
```
./renderer [--window-size <width>x<height>] --model <model-path>... --texture (<texture-path> | null)...
```
Model and texture should correspond to each other, set the texture path to 'null' means no texture assigned to the model, only support png format texture file
```
# example
./renderer --model ../example/cottage/cottage.obj --texture ../example/cottage/cottage_diffuse.png
```

# Reference
[SunXLei](https://github.com/SunXLei):  [计算机图形学系列笔记](https://www.zhihu.com/column/c_1249465121615204352), [SRender](https://github.com/SunXLei/SRender)  
[闫令琪](https://sites.cs.ucsb.edu/~lingqi/index.html): [GAMES101](https://sites.cs.ucsb.edu/~lingqi/teaching/games101.html)
