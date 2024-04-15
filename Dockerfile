FROM ubuntu:20.04

# Dependencies
RUN apt-get update -y
RUN apt-get install -y wget build-essential 
RUN apt-get install -y git clang cmake libvulkan-dev

RUN apt-get install -y g++-8 gcc-8
RUN apt-get install -y libx11-dev zlib1g-dev
RUN apt-get install -y libxext-dev

FROM ubuntu:20.04

# Dependencies
RUN apt-get update -y
RUN apt-get install -y wget curl gcc make gnupg 

# Install the Vulkan SDK to get access to glslc to compile the shaders
RUN wget -qO - https://packages.lunarg.com/lunarg-signing-key-pub.asc | apt-key add -
RUN wget -qO /etc/apt/sources.list.d/lunarg-vulkan-focal.list https://packages.lunarg.com/vulkan/lunarg-vulkan-focal.list
RUN apt update -y
RUN apt-get install -y vulkan-sdk

WORKDIR /app

COPY . .

RUN pwd
RUN ./build.sh

CMD ["./bin/testbed"]