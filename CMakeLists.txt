cmake_minimum_required(VERSION 3.16)
project(ExamStatisticsSystem VERSION 1.0.0 LANGUAGES CXX)

# CMake 3.27+: find_package 使用大写 <PACKAGENAME>_ROOT 变量作为搜索提示
if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
endif()

# C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# 第三方库配置
# ============================================================
# 本项目的第三方库通过 vcpkg 管理。
# 构建前请确保已安装 vcpkg 并配置所需包：
#
#   【MSVC 用户】
#     vcpkg install openxlsx:x64-windows libxlsxwriter:x64-windows
#     cmake -S . -B out/build/msvc-Release -G "Ninja" ^
#           -DCMAKE_BUILD_TYPE=Release ^
#           -DCMAKE_TOOLCHAIN_FILE=<vcpkg_root>/scripts/buildsystems/vcpkg.cmake
#
#   【MinGW 用户】
#     vcpkg install openxlsx:x64-mingw-dynamic libxlsxwriter:x64-mingw-dynamic
#     cmake -S . -B out/build/mingw-Release -G "Ninja" ^
#           -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++ ^
#           -DCMAKE_TOOLCHAIN_FILE=<vcpkg_root>/scripts/buildsystems/vcpkg.cmake ^
#           -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic
#
#   也可使用 CMakePresets.json 中的预设简化配置。
# ============================================================

# 自动检测编译器类型，推断 vcpkg triplet
if(NOT DEFINED VCPKG_TARGET_TRIPLET)
    if(MSVC)
        set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "vcpkg target triplet")
    elseif(MINGW)
        set(VCPKG_TARGET_TRIPLET "x64-mingw-dynamic" CACHE STRING "vcpkg target triplet")
    endif()
endif()

# OpenXLSX 库路径（请根据实际安装位置修改）
set(OPENXLSX_ROOT "${CMAKE_SOURCE_DIR}/third_party/OpenXLSX" CACHE PATH "OpenXLSX 安装根目录")

# 查找或手动指定 OpenXLSX
if(EXISTS "${OPENXLSX_ROOT}")
    set(OpenXLSX_DIR "${OPENXLSX_ROOT}/lib/cmake/OpenXLSX")
    find_package(OpenXLSX REQUIRED)
else()
    # 如果 OpenXLSX 未安装到 third_party，尝试 vcpkg / 系统查找
    find_package(OpenXLSX QUIET)
    if(NOT OpenXLSX_FOUND)
        message(WARNING "未找到 OpenXLSX 库，请将 OpenXLSX 安装到 third_party/OpenXLSX 或通过 vcpkg 安装")
    endif()
endif()

# ============================================================
# libxlsxwriter 图表库配置
# ============================================================
# libxlsxwriter 是一个C语言库，依赖 zlib。
# 提供以下三种安装方式（任选其一）：
#
# 方式一 [推荐]：用 vcpkg 安装（自动处理 zlib 依赖）
#   MSVC:  vcpkg install libxlsxwriter:x64-windows
#   MinGW: vcpkg install libxlsxwriter:x64-mingw-dynamic
#   CMake 命令行添加: -DCMAKE_TOOLCHAIN_FILE=<vcpkg_root>/scripts/buildsystems/vcpkg.cmake
#   注意：vcpkg 的 x64-mingw-dynamic 版 libxlsxwriter 可能缺少 third_party/ 头文件，
#         本脚本会在配置阶段自动从 x64-windows 版本补全。
#
# 方式二：手动编译后放入 third_party/libxlsxwriter/
#   1. git clone https://github.com/jmcnamara/libxlsxwriter.git
#   2. cd libxlsxwriter && cmake -B build -DBUILD_SHARED_LIBS=ON
#   3. cmake --build build --config Release
#   4. 将 include/ 和 build/Release/ 下的 .lib .dll 复制到 third_party/libxlsxwriter/
#   5. 同时需要 zlib 的 DLL (zlib1.dll) 放在可执行文件同目录或 PATH 中
#
# 方式三：系统包管理器（Linux）
#   sudo apt install libxlsxwriter-dev    # Debian/Ubuntu
#   sudo dnf install libxlsxwriter-devel  # Fedora

set(XLSXWRITER_ROOT "${CMAKE_SOURCE_DIR}/third_party/libxlsxwriter" CACHE PATH "libxlsxwriter 安装根目录")

if(EXISTS "${XLSXWRITER_ROOT}/include/xlsxwriter.h")
    message(STATUS "使用 third_party 中的 libxlsxwriter: ${XLSXWRITER_ROOT}")
    set(XLSXWRITER_INCLUDE_DIR "${XLSXWRITER_ROOT}/include")
    if(WIN32)
        # 尝试多个可能的库文件名
        if(EXISTS "${XLSXWRITER_ROOT}/lib/xlsxwriter.lib")
            set(XLSXWRITER_LIBRARY "${XLSXWRITER_ROOT}/lib/xlsxwriter.lib")
            set(XLSXWRITER_DLL     "${XLSXWRITER_ROOT}/lib/xlsxwriter.dll")
        elseif(EXISTS "${XLSXWRITER_ROOT}/lib/xlsxwriter_d.lib")
            set(XLSXWRITER_LIBRARY "${XLSXWRITER_ROOT}/lib/xlsxwriter_d.lib")
            set(XLSXWRITER_DLL     "${XLSXWRITER_ROOT}/lib/xlsxwriter_d.dll")
        else()
            message(WARNING "找到头文件但未找到 .lib，请确认 libxlsxwriter 已编译")
        endif()
    else()
        set(XLSXWRITER_LIBRARY "${XLSXWRITER_ROOT}/lib/libxlsxwriter.a")
    endif()
    set(XLSXWRITER_FOUND TRUE)
else()
    # 尝试用 pkg-config 查找（Linux / vcpkg）
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(XLSXWRITER QUIET xlsxwriter)
        # pkg_check_modules 设置的是 XLSXWRITER_INCLUDE_DIRS（复数），统一为单数
        if(XLSXWRITER_FOUND)
            if(NOT XLSXWRITER_INCLUDE_DIR AND XLSXWRITER_INCLUDE_DIRS)
                set(XLSXWRITER_INCLUDE_DIR "${XLSXWRITER_INCLUDE_DIRS}")
            endif()
            if(NOT XLSXWRITER_LIBRARY AND XLSXWRITER_LIBRARIES)
                # pkg_check_modules 返回库名而非完整路径，用 find_library 获取完整路径
                find_library(XLSXWRITER_LIBRARY ${XLSXWRITER_LIBRARIES}
                    PATHS ${XLSXWRITER_LIBRARY_DIRS}
                    NO_DEFAULT_PATH)
            endif()
        endif()
    endif()
    # 尝试用 CMake find_module 查找（备选方案）
    if(NOT XLSXWRITER_FOUND)
        find_library(XLSXWRITER_LIBRARY xlsxwriter)
        find_path(XLSXWRITER_INCLUDE_DIR xlsxwriter.h)
        if(XLSXWRITER_LIBRARY AND XLSXWRITER_INCLUDE_DIR)
            set(XLSXWRITER_FOUND TRUE)
        endif()
    endif()
    if(NOT XLSXWRITER_FOUND)
        message(WARNING "未找到 libxlsxwriter 库！图表功能将不可用。")
        message(WARNING "请选择以下方式之一安装：")
        message(WARNING "  1. MSVC:  vcpkg install libxlsxwriter:x64-windows")
        message(WARNING "     MinGW: vcpkg install libxlsxwriter:x64-mingw-dynamic")
        message(WARNING "  2. 手动编译放入 third_party/libxlsxwriter/")
        message(WARNING "  3. sudo apt install libxlsxwriter-dev (Linux)")
    endif()
endif()

# ============================================================
# 修复 vcpkg x64-mingw-dynamic 版 libxlsxwriter 缺少 third_party/ 头文件
# ============================================================
# 背景：vcpkg 的 x64-mingw-dynamic 版 libxlsxwriter 在安装时可能未包含
# third_party/queue.h、tree.h 等必需头文件，导致 MinGW 构建失败。
# 作为临时修复，从 x64-windows 版本复制缺失的头文件。
if(MINGW AND XLSXWRITER_FOUND AND XLSXWRITER_INCLUDE_DIR)
    set(_LXW_THIRD_PARTY_DST "${XLSXWRITER_INCLUDE_DIR}/third_party")
    if(NOT EXISTS "${_LXW_THIRD_PARTY_DST}/queue.h")
        # 尝试在 vcpkg 安装目录中寻找 x64-windows 版本的 third_party
        if(DEFINED VCPKG_INSTALLED_DIR)
            set(_LXW_MSVC_TP "${VCPKG_INSTALLED_DIR}/../x64-windows/include/xlsxwriter/third_party")
        else()
            # 从 CMAKE_TOOLCHAIN_FILE 推断 vcpkg 根目录
            get_filename_component(_VCPKG_SCRIPTS "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
            get_filename_component(_VCPKG_ROOT "${_VCPKG_SCRIPTS}" DIRECTORY)
            set(_LXW_MSVC_TP "${_VCPKG_ROOT}/installed/x64-windows/include/xlsxwriter/third_party")
        endif()
        if(EXISTS "${_LXW_MSVC_TP}/queue.h")
            message(STATUS "从 ${_LXW_MSVC_TP} 补全缺失的 libxlsxwriter third_party/ 头文件")
            file(COPY "${_LXW_MSVC_TP}" DESTINATION "${XLSXWRITER_INCLUDE_DIR}")
        else()
            # 最后尝试常见的 vcpkg 默认安装路径
            set(_LXW_MSVC_TP_FB "C:/vcpkg/installed/x64-windows/include/xlsxwriter/third_party")
            if(EXISTS "${_LXW_MSVC_TP_FB}/queue.h")
                message(STATUS "从 ${_LXW_MSVC_TP_FB} 补全缺失的 libxlsxwriter third_party/ 头文件")
                file(COPY "${_LXW_MSVC_TP_FB}" DESTINATION "${XLSXWRITER_INCLUDE_DIR}")
            else()
                message(WARNING "libxlsxwriter 缺少 third_party/queue.h 等头文件！")
                message(WARNING "请尝试: vcpkg install libxlsxwriter:x64-windows 后重新配置")
            endif()
        endif()
    endif()
endif()

# ============================================================
# 头文件路径
# ============================================================

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# ============================================================
# 源文件
# ============================================================

set(SOURCES
    # 数据模型层
    src/model/CStudent.cpp

    # 数据访问层
    src/data/ExcelReader.cpp
    src/data/ExcelWriter.cpp
    src/data/DataRepository.cpp

    # 业务逻辑层
    src/service/Auth.cpp
    src/service/Analyzer.cpp
    src/service/Expander.cpp

    # 系统控制层
    src/service/ExamStatisticsSystem.cpp

    # 可视化层
    src/visualization/LibxlsxChartRenderer.cpp
    src/visualization/ScoreVisualizer.cpp

    # 程序入口
    src/main.cpp
)

# ============================================================
# 可执行目标
# ============================================================

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})

# 链接 OpenXLSX
if(OpenXLSX_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenXLSX::OpenXLSX)
endif()

# 链接 libxlsxwriter
if(XLSXWRITER_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE ${XLSXWRITER_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${XLSXWRITER_LIBRARY})

    # Windows: 将 DLL 复制到可执行文件输出目录，确保运行时能找到
    if(WIN32 AND DEFINED XLSXWRITER_DLL AND EXISTS "${XLSXWRITER_DLL}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${XLSXWRITER_DLL}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMENT "复制 xlsxwriter.dll 到输出目录")
    endif()

    # 运行时可能还需要 zlib DLL，如已获取也一并复制
    if(WIN32 AND EXISTS "${XLSXWRITER_ROOT}/lib/zlib1.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${XLSXWRITER_ROOT}/lib/zlib1.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMENT "复制 zlib1.dll 到输出目录")
    endif()
endif()

# ============================================================
# 编译选项
# ============================================================

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /utf-8)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()
