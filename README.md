This project uses OpenCV to capture a video from the default computer camera.

On a macOS, I used c++ compilier 'g++' along with Homebrew and CMake.

In terminal:
brew install opencv cmake

Ensure OpenCV is running:
pkg-config --modversion opencv4

Terminal commands to configure the project and build an executable:
mkdir build
cd build
cmake ..
make

Terminal Command to run the project:
./ComputerVis
(Or whatever the project name is)

After changes, rebuild the build directory in terminal:
rm -rf *
cmake ..
make