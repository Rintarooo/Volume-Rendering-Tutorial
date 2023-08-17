Volume Rendering Example Code
Brandon Pelfrey - January 2011

This code is part of an ongoing tutorial on my blog @ brandonpelfrey.com/blog.
I will organize the lessons into separate folders and comment as much of the code as possible
in an attempt to supplement the tutorial text.

-- Building
This example requires ImageMagick's C++ bindings. 

Ubuntu/Debian: sudo apt-get install libmagick++-dev
Fedora: yum install ImageMagick-c++-devel

After that, all that is needed in order to build is to run make. The executable is placed in the
"bin" folder.

## Usage
## setup
```bash
# Install GLM
git clone https://github.com/g-truc/glm.git
mkdir include
mv ./glm/glm/ ./include
# Install stbi single header for writing image
wget -O Tutorial1/include/stb_image.h https://github.com/nothings/stb/blob/master/stb_image.h?raw=true 
wget -O Tutorial1/include/stb_image_write.h https://github.com/nothings/stb/blob/master/stb_image_write.h?raw=true
```

## GCP VM
```bash
# Dockerfileからビルド
docker-compose -f .devcontainer/docker-compose.yml build volume-render
# Xサーバーのアクセス権限を与える
xhost local:root
# コンテナ入る
docker-compose -f .devcontainer/docker-compose.yml run --rm volume-render /bin/bash
# GUI表示出来るか確認
xeyes
# C++ビルド
./build.sh
```

works on OpenGL 3.3 version