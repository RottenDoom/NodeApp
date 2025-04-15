import os
import subprocess
import sys
import shutil

OPENCV_VERSION = "4.5.5"
OPENCV_URL = f"https://github.com/opencv/opencv/archive/{OPENCV_VERSION}.zip"
OPENCV_DIR = "opencv-src"
BUILD_DIR = "opencv-build"
INSTALL_DIR = "opencv-install"

def run(cmd, cwd=None):
    print(f"Running: {cmd}")
    subprocess.check_call(cmd, shell=True, cwd=cwd)

def main():
    if not os.path.exists(OPENCV_DIR):
        run(f"curl -L {OPENCV_URL} -o opencv.zip")
        run("tar -xf opencv.zip")
        os.rename(f"opencv-{OPENCV_VERSION}", OPENCV_DIR)

    os.makedirs(BUILD_DIR, exist_ok=True)
    cmake_cmd = f"""
    cmake -S ../{OPENCV_DIR} -B . ^
        -DCMAKE_INSTALL_PREFIX=../{INSTALL_DIR} ^
        -DBUILD_SHARED_LIBS=OFF ^
        -DBUILD_TESTS=OFF ^
        -DBUILD_PERF_TESTS=OFF ^
        -DBUILD_EXAMPLES=OFF ^
        -G "MinGW Makefiles"
    """
    run(cmake_cmd, cwd=BUILD_DIR)

    run("mingw32-make -j4", cwd=BUILD_DIR)
    run("mingw32-make install", cwd=BUILD_DIR)

    print("\nOpenCV installed successfully!")

if __name__ == "__main__":
    main()