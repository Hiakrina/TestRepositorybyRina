/* 期末成绩信息统计系统 —— 程序入口

    技术路线：
        main() 仅负责参数解析与启动程序。
        创建 ExamStatisticsSystem 实例，传入数据目录及输出目录路径，
        调用系统主控类 run() 执行全部流程。
        程序结束前输出日志或错误提示，确保资源正常释放。

    用法:
        ExamStatisticsSystem <数据目录> <输出目录>
    示例:
        ExamStatisticsSystem ./data ./output

    默认值:
        数据目录 = ./data
        输出目录 = ./output
*/
#include "service/ExamStatisticsSystem.h"

#include <iostream>
#include <string>
#include <clocale>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char* argv[])
{
#ifdef _WIN32
    // 控制台 I/O 使用 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // C 运行时文件 I/O 使用 UTF-8（修复含中文路径的文件创建失败问题）
    // 需要 Windows 10 1903+，UCRT 支持 .utf8 locale
    setlocale(LC_ALL, ".utf8");
#endif

    //参数解析
    std::string dataDir   = "./data";
    std::string outputDir = "./output";

    if (argc >= 2) {
        dataDir = argv[1];
    }
    if (argc >= 3) {
        outputDir = argv[2];
    }

    //输出启动信息
    std::cout << "期末成绩信息统计系统 启动" << std::endl;
    std::cout << "数据目录: " << dataDir << std::endl;
    std::cout << "输出目录: " << outputDir << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    //创建系统实例并运行
    ExamStatisticsSystem system;

    try {
        system.run(dataDir, outputDir);
    }
    catch (const std::exception& e) {
        std::cerr << "\n[致命错误] 程序无法继续运行: " << e.what() << std::endl;
        std::cerr << "请确认 data 目录存在且包含以下文件:" << std::endl;
        std::cerr << "  Gen_CPP_Final_Exam_Results.xlsx" << std::endl;
        std::cerr << "  Gen_jxbcjd_zx.xlsx" << std::endl;
        std::cerr << "  Gen_OJTEST.xlsx" << std::endl;
        std::cerr << "  Gen_Performance_Scores.xlsx" << std::endl;
        std::cerr << "\n按 Enter 键退出..." << std::endl;
        std::cin.get();
        return 1;
    }
    catch (...) {
        std::cerr << "\n[致命错误] 发生未知异常" << std::endl;
        std::cerr << "\n按 Enter 键退出..." << std::endl;
        std::cin.get();
        return 1;
    }

    // 技术路线: 程序结束前输出日志或错误提示，确保资源正常释放
    // system 析构时自动释放所有资源（RAII）
    std::cout << "程序正常退出，按 Enter 键关闭..." << std::endl;
    std::cin.get();
    return 0;
}
