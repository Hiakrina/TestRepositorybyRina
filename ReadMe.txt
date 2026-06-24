================================================================================
                    期末成绩信息统计系统 v1.0
                    Exam Statistics System
================================================================================

面向高校的考试成绩管理与分析工具。
读取 Excel 数据，计算总评、分析 OJ 刷题、生成可视化图表，
支持学生/教师/管理员三级权限。

================================================================================
                              目录
================================================================================

  一、新手：5 分钟快速上手
  二、功能一览
  三、项目结构
  四、编译与构建
  五、运行说明
  六、数据文件格式
  七、系统架构
  八、技术选型与设计原则
  九、常见问题（FAQ）


================================================================================
一、新手：5 分钟快速上手
================================================================================

【第 1 步】获取程序
  从 GitHub Releases 页面下载最新版本：
    https://github.com/Hiakrina/TestRepositorybyRina

  下载后解压。程序首次运行时会自动创建 data/ 和 output/ 目录，
  你只需在 data/ 中放入以下 4 个 Excel 文件（格式参考"六、数据文件格式"）：

    data/
    +-- Gen_CPP_Final_Exam_Results.xlsx   卷面成绩
    +-- Gen_jxbcjd_zx.xlsx                总评成绩单
    +-- Gen_OJTEST.xlsx                   OJ 刷题数据
    +-- Gen_Performance_Scores.xlsx       平时成绩

【第 2 步】运行程序
  双击项目根目录下的 ExamStatisticsSystem.exe
  （程序会自动创建缺失的 data/ 和 output/ 目录）

【第 3 步】登录
  预置账号：

    身份       用户名         密码
    -----------------------------------
    管理员     admin21        123456
    教师       2021110001     123456
    学生       自己的学号      123456（学号第5位需为0）

【第 4 步】查看结果
  - 控制台打印所有学生名单和 OJ 分析报告
  - output/ 目录生成：
      成绩总表_导出.xlsx   完整成绩汇总
      分数段分布.xlsx      饼图（不及格/及格/中等/良好/优秀）
      班级平均分对比.xlsx  柱状图（各班总评均分）


================================================================================
二、功能一览
================================================================================

  0. 自动创建目录（NEW v1.0.1）
       首次运行自动创建 data/ 和 output/ 目录，无需手动 mkdir

  1. 用户登录与权限验证
       学生  -> 仅查看本人
       教师  -> 查看全部、修改本班成绩
       管理员 -> 全部权限

  2. 数据加载
       从四个 Excel 文件读取数据，合并为学生列表

  3. 成绩计算
       总评 = (平时成绩 * 0.3 + 卷面成绩 * 0.7)

  4. 名单排序与展示（按学号/班级/姓名，冒泡排序）

  5. OJ 系统分析
       过题不足的学生 / 不在 OJ 中的学生 / OJ 中的独立学号

  6. 交互式查找与修改
       按学号/姓名/班级查找 -> 修改成绩/班级 -> 自动重算总评

  7. 数据可视化（libxlsxwriter）
       分数段分布饼图 + 班级平均分对比柱状图

  8. 数据扩充
       基于现有学生统计分布生成模拟数据，扩充至 100 人
       考试状态（初修/重修/缓考）按比例随机分配

  9. 保存导出
       成绩总表导出为 Excel（只有"成绩总表"一个标签页）


================================================================================
三、项目结构
================================================================================

ExamStatisticsSystem_v1.0/
+-- data/                                输入数据目录
+-- output/                              输出结果目录
+-- include/                             头文件
|   +-- data/
|   |   +-- DataRepository.h             数据仓库（加载、解析、保存）
|   |   +-- ExcelReader.h                OpenXLSX 读取
|   |   +-- ExcelWriter.h                OpenXLSX 写入
|   +-- model/
|   |   +-- CStudent.h                   学生类
|   |   +-- CScore.h                     成绩结构体
|   +-- service/
|   |   +-- ExamStatisticsSystem.h       系统主控
|   |   +-- Analyzer.h                   查找与 OJ 分析
|   |   +-- Auth.h                       用户认证与权限
|   |   +-- Expander.h                   数据扩充
|   +-- visualization/
|       +-- LibxlsxChartRenderer.h       图表渲染器 (libxlsxwriter)
|       +-- ScoreVisualizer.h            分数可视化
+-- src/                                 源文件（与 include/ 对应）
+-- CMakeLists.txt                       CMake 构建脚本
+-- CMakePresets.json                    CMake 预设（MSVC / MinGW / Debug / Release）
+-- ExamStatisticsSystem.exe             可执行文件
+-- ReadMe.txt                           本文件


================================================================================
四、编译与构建
================================================================================

本项目代码是跨平台的（Windows API 已用 #ifdef _WIN32 隔离），
经测试支持以下编译器：

  编译器        最低版本    Windows            Linux
  ------------------------------------------------------------------
  MSVC          2022 (19)   Developer PS [推荐] 不支持
  MinGW-w64     GCC 11+    MSYS2 / cmd (需重编译依赖)  不支持
  GCC           11+        (通过 MinGW)        需要 apt install
  Clang         14+        LLVM / VS 附带      需要 apt install

【C++ 标准】C++17（OpenXLSX 库的要求）


【依赖库】

  库名            用途         vcpkg 安装命令
  ------------------------------------------------------------------
  OpenXLSX        读写 xlsx    vcpkg install openxlsx
  libxlsxwriter   生成图表     vcpkg install libxlsxwriter

  ⚠️ vcpkg 默认三元组 x64-windows 编译的库仅兼容 MSVC！
     MinGW 用户必须使用 x64-mingw-dynamic 三元组（见下方 MinGW 构建步骤），
     否则会出现 undefined reference to __imp__ZN8OpenXLSX... 链接错误。

  如果不使用 vcpkg 也可以手动编译上述库，放入 third_party/ 目录。
  Linux 用户可用系统包管理器：
    sudo apt install libxlsxwriter-dev    # Debian/Ubuntu
    sudo dnf install libxlsxwriter-devel  # Fedora


【编译工具】

  工具       最低版本   说明
  ---------------------------------------------------------------
  CMake      3.16+      构建配置
  Ninja      任意       构建后端（也可用 make / MSBuild）
  vcpkg      任意       包管理器（可选）


【Windows — MSVC 构建步骤】★ 推荐

  :: 0. 安装 Visual Studio 2022（勾选"使用 C++ 的桌面开发"）
  ::    安装 vcpkg 并集成：vcpkg integrate install
  :: 1. 打开 "Developer PowerShell for VS 2022"
  cd ExamStatisticsSystem_v1.0

  :: 2. 安装依赖（MSVC 默认三元组 x64-windows）
  vcpkg install openxlsx libxlsxwriter

  :: 3. 配置并构建 Release（分发用）

  :: 方式 A — 使用 CMakePresets（推荐）
  cmake --preset msvc-release
  cmake --build out/build/msvc-Release

  :: 方式 B — 手动指定参数
  cmake -B out/build/msvc-Release -S . -G Ninja `
    -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake `
    -DCMAKE_BUILD_TYPE=Release
  cmake --build out/build/msvc-Release --config Release

  :: 生成的 EXE 在 out/build/msvc-Release/ExamStatisticsSystem.exe
  :: 运行时 DLL 由 vcpkg 自动处理（或从 vcpkg_installed 目录复制）

  :: 不使用 vcpkg 时去掉 -DCMAKE_TOOLCHAIN_FILE 参数，手动指定库路径。


【Windows — MinGW-w64 构建步骤】

  ⚠️ 重要：默认 vcpkg 库是 MSVC ABI，MinGW 无法链接！
     必须使用 x64-mingw-dynamic 三元组重编译依赖。

  :: 方案 A — vcpkg + MinGW 三元组（推荐）

  :: 0. 安装 MSYS2 和 vcpkg
  :: 1. 在 MSYS2 MinGW64 终端中安装工具链：
  pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake \
    mingw-w64-x86_64-ninja

  :: 2. 为 MinGW 编译依赖（耗时约 10-30 分钟）
  vcpkg install openxlsx:x64-mingw-dynamic libxlsxwriter:x64-mingw-dynamic

  :: 3. 配置并构建
  cd ExamStatisticsSystem_v1.0

  :: 方式 A — 使用 CMakePresets（推荐，自动设置 triplet）
  cmake --preset mingw-release
  cmake --build out/build/mingw-Release

  :: 方式 B — 手动指定参数
  cmake -B out/build/mingw-Release -S . -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
  cmake --build out/build/mingw-Release

  :: 注意：CMakeLists.txt 会自动检测 MinGW 编译器并补全 vcpkg
  ::       x64-mingw-dynamic 版 libxlsxwriter 可能缺失的
  ::       third_party/ 头文件（从 x64-windows 版本复制）。

  :: 方案 B — 手动编译依赖放入 third_party/

  :: 1. 用 MinGW 编译器从 GitHub 克隆并编译 OpenXLSX 和 libxlsxwriter
  :: 2. 编译产物（.dll + .a / .lib）放入 third_party/ 对应目录
  :: 3. 构建
  cmake -B out/build/mingw-Release -S . -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENXLSX_ROOT=./third_party/OpenXLSX \
    -DXLSXWRITER_ROOT=./third_party/libxlsxwriter
  cmake --build out/build/mingw-Release


【Linux — GCC/Clang 构建步骤】

  sudo apt install cmake ninja-build g++ libxlsxwriter-dev

  cd ExamStatisticsSystem_v1.0

  :: 编译 OpenXLSX 放入 third_party/，然后：
  cmake -B out/build/linux-Release -S . -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENXLSX_ROOT=./third_party/OpenXLSX
  cmake --build out/build/linux-Release


================================================================================
五、运行说明
================================================================================

【命令行运行】

  :: 默认路径（./data -> ./output）
  ExamStatisticsSystem.exe

  :: 自定义路径
  ExamStatisticsSystem.exe D:\myData D:\myOutput

【分发给其他电脑】

  需要打包的文件：
    ExamStatisticsSystem.exe
    OpenXLSX.dll
    xlsxwriter.dll
    pugixml.dll
    miniz.dll
    minizip.dll
    z.dll
    boost_nowide-*.dll
    data/  （放入你的数据文件）

  目标电脑需要安装：
    Visual C++ Redistributable (https://aka.ms/vs/17/release/vc_redist.x64.exe)


================================================================================
六、数据文件格式（含列映射表）
================================================================================

【表 1】Gen_CPP_Final_Exam_Results.xlsx （卷面成绩）

  数据从第 2 行开始，列结构：
  P1=序号  P2=学号  P3=姓名  P4=班级
  P5=选择题  P6=填空题  P7=判断题  P8=简答题  P9=程序设计题
  P10=总分(卷面)  P11=备注(初修/重修/缓考/旷考/取消资格)

  程序读取的列：P2(学号) P3(姓名) P4(班级) P10(卷面分) P11(备注)

【表 2】Gen_jxbcjd_zx.xlsx （总评成绩单）

  前 4 行为标题/学年元数据，第 5 行为表头，第 6 行为空行，
  数据从第 7 行开始。列结构：
  P1=序号  P2=学号  P3=姓名  P4=性别  P5=专业/班级  P6=学院
  P7=(空)  P8=平时成绩  P9=期末成绩  P10=总成绩  P11=备注

  程序读取的列：P2(学号) P5(班级) P8(平时) P9(期末) P10(总评) P11(备注)

【表 3】Gen_OJTEST.xlsx （OJ 数据）

  数据从第 2 行开始，列结构：
  P1=Username(学号)  P2=Real Name  P3=AC(过题数)
  P4=Total Submission  P5=Total Time  P6..P21=各题得分

  程序读取的列：P1(学号) P3(过题数)

【表 4】Gen_Performance_Scores.xlsx （平时成绩）

  数据从第 2 行开始，列结构：
  P1=学号  P2=姓名  P3=平时成绩

  程序读取的列：P1(学号) P3(平时分数)

【数据合并逻辑】

  1. parseExamSheet    以卷面成绩表为主表，创建学生对象
  2. parseTotalSheet   按学号匹配，合并班级、平时、期末、总评、备注
  3. parseOJSheet      按学号匹配，合并 OJ 账号、过题数
  4. parsePerformance  按学号匹配，合并平时成绩（覆盖总评表中的值）


================================================================================
七、系统架构
================================================================================

【调用层次】

  main()
    |
    +-- ExamStatisticsSystem::run()
          |
          +-- Auth (登录验证)
          |     |
          |     +-- User 结构体数组（硬编码账号）
          |
          +-- DataRepository (数据层)
          |     +-- loadAll() -> ExcelReader::readSheet()
          |     |              -> parseExamSheet / parseTotalSheet
          |     |              -> parseOJSheet / parsePerformanceSheet
          |     +-- saveScores() -> ExcelWriter::writeSheet()
          |
          +-- computeGrades() (成绩计算)
          |
          +-- sortAndDisplay() -> 冒泡排序 + 控制台输出
          |
          +-- Analyzer (OJ 分析与查找)
          |     +-- getInsufficientSolved / getStudentsNotInOJ
          |     +-- findByID / findByName / findByClass
          |     +-- modifyScore / modifyClass
          |
          +-- ScoreVisualizer (可视化)
          |     +-- showScoreDistribution -> LibxlsxChartRenderer::drawPieChart
          |     +-- showClassComparison  -> LibxlsxChartRenderer::drawBarChart
          |
          +-- Expander (数据扩充)
          |     +-- generateID / generateName / generateScore
          |     +-- generateExamStatus (按比例随机 初修/重修/缓考)
          |     +-- generateOJAccount / generateOJSolved
          |
          +-- saveData() -> DataRepository::saveScores()


================================================================================
八、技术选型与设计原则
================================================================================

  类别          选择              说明
  ------------------------------------------------------------------
  语言          C++17            OpenXLSX库所需，项目本身代码为C++11标准
  编译器        MSVC 2022        Windows 原生
  构建系统      CMake + Ninja    快速增量编译，libxlsxwriter库所需
  包管理        vcpkg            微软官方
  Excel 读写    OpenXLSX         现代 C++ 库，直接操作 .xlsx
  图表生成      libxlsxwriter    C 库，生成 Excel 内嵌图表
  内存管理      裸指针new/delete 手动管理
  数据存储      std::vector      栈友好


================================================================================
九、常见问题（FAQ）
================================================================================

Q: 导出 Excel 是空表？
A: 导出后自动删除默认 Sheet1。若仍为空，检查 output/ 是否有写入权限。

Q: 总评成绩全为 0？
A: 确认备注列：正常="初修"，异常="重修"/"缓考"等。
   初修学生才参与总评计算。

Q: "无法打开包括文件: string"（仅 MSVC）
A: 请在开始菜单搜索并打开 Developer PowerShell for VS 2022 运行。
   MinGW / Linux 用户不受此问题影响。

Q: 在另一台电脑缺少 DLL？（仅 Windows MSVC）
A: 使用 Release 版本（Debug 的 *D.dll 不能分发）。
   目标电脑安装 VC++ Redistributable。

Q: 程序提示"无法打开文件"或找不到 data/ 目录？
A: v1.0.1 起程序会自动创建 data/ 和 output/ 目录。
   但 4 个 Excel 数据文件仍需手动放入 data/。
   若自动创建失败请确保程序在全英文文路径下运行，并检查程序给出的错误提示。
   

Q: MinGW 链接报 undefined reference to __imp__ZN8OpenXLSX...？
A: vcpkg 默认安装 MSVC ABI 版本的库，MinGW 无法链接。
   请为 MinGW 重编译依赖：
     vcpkg install openxlsx:x64-mingw-dynamic libxlsxwriter:x64-mingw-dynamic
   详见第四章 MinGW 构建步骤。

Q: 能用 GCC / MinGW / Clang 编译吗？
A: 可以。项目代码本身跨平台（#ifdef _WIN32 隔离）。
   需自行编译 OpenXLSX 和 libxlsxwriter 的对应版本，
   详见第四章各平台构建步骤。

================================================================================
                        许可证
  本项目为实习课程设计作品，由@Hiakrina与@mofazhumi0107-lab合作完成，仅供学习交流使用。

================================================================================
