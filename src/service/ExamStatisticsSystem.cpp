#include "service/ExamStatisticsSystem.h"

#include <cmath>
#include <filesystem>
#include <iostream>
#include <stdexcept>

ExamStatisticsSystem::ExamStatisticsSystem(){}

void ExamStatisticsSystem::loadData(const std::string& dirPath)
{
    //从指定目录读取四个Excel文件（使用实际数据文件名）
    std::string examPath    = dirPath + "/Gen_CPP_Final_Exam_Results.xlsx";
    std::string totalPath   = dirPath + "/Gen_jxbcjd_zx.xlsx";
    std::string ojPath      = dirPath + "/Gen_OJTEST.xlsx";
    std::string performPath = dirPath + "/Gen_Performance_Scores.xlsx";

    std::cout << "[加载数据] 开始从目录读取数据: " << dirPath << std::endl;

    //尝试加载数据和报告加载结果
    try {
        repo.loadAll(examPath, totalPath, ojPath, performPath);
        students = repo.getStudents();
        std::cout << "[加载数据] 成功加载 " << students.size() 
                    << " 条学生记录" << std::endl;
    }
    //异常捕获和报告
    catch (const std::exception& e) {
        std::cerr << "[加载数据] 错误: " << e.what() << std::endl;
        throw;
    }
}

//计算总评成绩
int ExamStatisticsSystem::calcTotal(int performance, int exam)
{
    //总评 = round(平时*0.3 + 卷面*0.7)
    double raw = performance * 0.3 + exam * 0.7;
    return static_cast<int>(std::round(raw));
}

void ExamStatisticsSystem::computeGrades()
{
    std::cout << "[成绩计算] 开始计算总评成绩..." << std::endl;

    for (CStudent& stu : students) {
        CScore& sc = stu.getScore();
        //只对正常参加考试的学生计算总评
        if (!stu.isExcluded() && stu.isNormal()) {
            sc.total = calcTotal(sc.performance, sc.exam);
        }
    }

    std::cout << "[成绩计算] 总评成绩计算完成"
                 "(公式: 总评 = round(平时*0.3 + 卷面*0.7))" << std::endl;
}

//按学号排序
bool ExamStatisticsSystem::compareByID(const CStudent& a, const CStudent& b)
{
    return a.getStudentID() < b.getStudentID();
}

//按班级排序（优先现班级，若相同或现班级为空则按学号）
bool ExamStatisticsSystem::compareByClass(const CStudent& a, const CStudent& b)
{
    std::string ca = a.getClassAfter().empty() ? a.getClassBefore() : a.getClassAfter();
    std::string cb = b.getClassAfter().empty() ? b.getClassBefore() : b.getClassAfter();
    if (ca != cb) return ca < cb;
    return a.getStudentID() < b.getStudentID();
}

//按姓名排序
bool ExamStatisticsSystem::compareByName(const CStudent& a, const CStudent& b)
{
    return a.getName() < b.getName();
}

//三种排序模式的选择：按学号、班级（优先现班级，相同班级再按学号）、姓名。
void ExamStatisticsSystem::sortStudents(std::vector<CStudent>& students, SortMode mode)
{
    bool (*comp)(const CStudent&, const CStudent&) = compareByID;
    if (mode == SortMode::BY_CLASS) comp = compareByClass;
    if (mode == SortMode::BY_NAME)  comp = compareByName;

    size_t n = students.size();
    for (size_t i = 0; i + 1 < n; ++i) {
        for (size_t j = 0; j + 1 < n - i; ++j) {
            if (comp(students[j + 1], students[j])) {
                CStudent tmp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = tmp;
            }
        }
    }
}

void ExamStatisticsSystem::sortAndDisplay(int mode)
{
    //将mode转换为SortMode的枚举类型排序模式
    SortMode sortMode;
    switch (mode) {
    case 0: sortMode = SortMode::BY_ID;    break;
    case 1: sortMode = SortMode::BY_CLASS; break;
    case 2: sortMode = SortMode::BY_NAME;  break;
    default:
        //std::cerr用于输出错误提示
        std::cerr << "[排序] 未知排序模式: " << mode << "，默认按学号排序" << std::endl;
        sortMode = SortMode::BY_ID;
    }

    sortStudents(students, sortMode);

    //输出排序后的名单（使用经过验证的 sortMode 索引，避免原始 mode 越界）
    const char* modeNames[] = {"学号", "班级", "姓名"};
    int displayMode = static_cast<int>(sortMode);
    std::cout << "\n========== 学生名单（按" << modeNames[displayMode] << "排序）共 "
              << students.size() << " 人 ==========" << std::endl;

    for (const CStudent& stu : students) {
        displayStudent(stu);
    }
    std::cout << "========================================" << std::endl;
}

void ExamStatisticsSystem::analyzeOJ(int threshold)
{
    // threshold: 过题数阈值，低于此值为不足
    std::cout << "\n[OJ分析] 开始分析OJ数据 (过题阈值: " << threshold << ")..." << std::endl;

    //1.过题数不足的学生
    std::vector<CStudent> insufficient = analyzer.getInsufficientSolved(students, threshold);
    std::cout << "\n--- 过题数不足(< " << threshold << ")的学生 (" << insufficient.size() << " 人) ---" << std::endl;
    for (const CStudent& stu : insufficient) {
        std::cout << "  学号: " << stu.getStudentID()
                  << "  姓名: " << stu.getName()
                  << "  过题数: " << stu.getOJSolved() << std::endl;
    }

    //2.不在OJ系统中的学生
    std::vector<CStudent> notInOJ = analyzer.getStudentsNotInOJ(students);
    std::cout << "\n--- 不在OJ系统中的学生 (" << notInOJ.size() << " 人) ---" << std::endl;
    for (const CStudent& stu : notInOJ) {
        std::cout << "  学号: " << stu.getStudentID()
                  << "  姓名: " << stu.getName() << std::endl;
    }

    std::cout << "\n[OJ分析] 完成" << std::endl;
}

//显示单个学生信息
void ExamStatisticsSystem::displayStudent(const CStudent& stu) const
{
    const CScore& sc = stu.getScore();
    std::cout << "  学号: " << stu.getStudentID()
              << "  姓名: " << stu.getName()
              << "  原班级: " << stu.getClassBefore()
              << "  现班级: " << stu.getClassAfter()
              << "  平时: " << sc.performance
              << "  卷面: " << sc.exam
              << "  总评: " << sc.total
              << "  状态: " << (stu.getExamStatus().empty() ? "正常" : stu.getExamStatus())
              << "  OJ账号: " << stu.getOJAccount()
              << "  过题: " << (stu.hasOJRecord() ? std::to_string(stu.getOJSolved()) : "无")
              << std::endl;
}

//显示学生的信息列表
void ExamStatisticsSystem::displayStudentList(const std::vector<CStudent*>& list) const
{
    if (list.empty()) {
        std::cout << "  (无匹配结果)" << std::endl;
        return;
    }
    for (size_t i = 0; i < list.size(); ++i) {
        std::cout << "  [" << (i + 1) << "] ";
        displayStudent(*list[i]);
    }
}

//读取用户输入，确定菜单选项
int ExamStatisticsSystem::readMenuChoice(int min, int max) const
{
    int choice = -1;
    while (true) {
        std::cout << "  请选择 [" << min << "-" << max << "]: ";
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "  输入无效，请输入数字" << std::endl;
            continue;
        }
        std::cin.ignore(10000, '\n');
        if (choice >= min && choice <= max) {
            return choice;
        }
        std::cout << "  超出范围，请重新输入" << std::endl;
    }
}

//交互式查找与修改学生信息
void ExamStatisticsSystem::findAndModify()
{
    std::cout << "\n========== 查找与修改（交互模式） ==========" << std::endl;

    bool continueLoop = true;
    while (continueLoop) {
        //1.选择查找方式
        std::cout << "\n请选择查找方式:" << std::endl;
        std::cout << "  1. 按学号查找" << std::endl;
        std::cout << "  2. 按姓名查找" << std::endl;
        std::cout << "  3. 按班级查找" << std::endl;
        std::cout << "  0. 退出修改模式" << std::endl;

        int findMode = readMenuChoice(0, 3);
        if (findMode == 0) {
            std::cout << "[查找修改] 退出交互模式" << std::endl;
            break;
        }

        //2.输入关键字
        std::string keyword;
        std::cout << "  请输入关键字: ";
        std::getline(std::cin, keyword);
        if (keyword.empty()) continue;

        //3.执行查找
        std::vector<CStudent*> results;
        switch (findMode) {
        case 1: {
            CStudent* found = analyzer.findByID(students, keyword);
            if (found) results.push_back(found);
            break;
        }
        case 2:
            results = analyzer.findByName(students, keyword);
            break;
        case 3:
            results = analyzer.findByClass(students, keyword);
            break;
        }

        //4.显示结果
        std::cout << "\n  查找结果 (" << results.size() << " 条):" << std::endl;
        displayStudentList(results);

        if (results.empty()) continue;

        //5.选择要修改的学生（多条结果时）
        CStudent* target = nullptr;
        if (results.size() == 1) {
            target = results[0];
        } else {
            std::cout << "  请选择要修改的学生序号 (0=跳过): ";
            int idx = readMenuChoice(0, static_cast<int>(results.size()));
            if (idx == 0) continue;
            target = results[idx - 1];
        }

        //6.选择修改项
        std::cout << "\n  当前学生信息:" << std::endl;
        displayStudent(*target);

        std::cout << "\n  请选择修改项:" << std::endl;
        std::cout << "    1. 修改平时成绩" << std::endl;
        std::cout << "    2. 修改卷面成绩" << std::endl;
        std::cout << "    3. 修改班级(仅管理员)" << std::endl;
        std::cout << "    0. 跳过" << std::endl;

        int modChoice = readMenuChoice(0, 3);
        if (modChoice == 0) continue;

        //7.输入新值并修改
        std::cout << "  请输入新值: ";
        std::string newValue;
        std::getline(std::cin, newValue);
        if (modChoice != 3 && newValue.empty()) {
            std::cout << "  [取消] 未输入有效值" << std::endl;
            continue;
        }

        bool success = false;
        try {
            switch (modChoice) {
            case 1: {
                //修改平时成绩
                CScore newScore = target->getScore();
                newScore.performance = std::stoi(newValue);
                success = analyzer.modifyScore(*target, newScore, auth);
                break;
            }
            case 2: {
                //修改卷面成绩
                CScore newScore = target->getScore();
                newScore.exam = std::stoi(newValue);
                success = analyzer.modifyScore(*target, newScore, auth);
                break;
            }
            case 3:
                success = analyzer.modifyClass(*target, newValue, auth);
                break;
            }
        }
        catch (const std::exception&) {
            std::cout << "  [输入无效] 请输入数字" << std::endl;
            continue;
        }

        if (success) {
            //修改成功后重算总评
            CScore& sc = target->getScore();
            sc.total = calcTotal(sc.performance, sc.exam);
            std::cout << "  [修改成功] 总评已自动重算。修改后信息:" << std::endl;
            displayStudent(*target);
        } else {
            std::cout << "  [修改失败] 权限不足或操作无效" << std::endl;
        }

        //8.是否继续
        std::cout << "\n  是否继续修改? (1=继续, 0=退出): ";
        continueLoop = (readMenuChoice(0, 1) == 1);
    }
}

//数据可视化：
//生成展示分数段分布的饼图和展示班级平均分对比的柱状图，并保存为Excel文件
void ExamStatisticsSystem::visualize(const std::string& outputDir)
{
    std::cout << "\n[可视化] 开始生成图表..." << std::endl;

    std::string piePath = outputDir + "/分数段分布.xlsx";
    std::string barPath = outputDir + "/班级平均分对比.xlsx";

    try {
        visualizer.showScoreDistribution(students, piePath);
        std::cout << "[可视化] 饼图已生成: " << piePath << std::endl;

        visualizer.showClassComparison(students, barPath);
        std::cout << "[可视化] 柱状图已生成: " << barPath << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[可视化] 错误: " << e.what() << std::endl;
    }
}

//数据扩充：委托Expander将学生列表扩充至目标人数
void ExamStatisticsSystem::augmentData(int targetCount)
{
    std::cout << "\n[数据扩充] 当前人数: " << students.size()
              << "，目标人数: " << targetCount << std::endl;
    students = expander.expand(students, targetCount);
    std::cout << "[数据扩充] 完成，当前总人数: " << students.size() << std::endl;
}

//保存导出当前学生列表，输出Excel文件
void ExamStatisticsSystem::saveData(const std::string& outputPath)
{
    std::cout << "\n[保存导出] 正在导出数据到: " << outputPath << std::endl;
    try {
        repo.saveScores(outputPath, students);
        std::cout << "[保存导出] 成功导出 " << students.size() << " 条记录" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[保存导出] 错误: " << e.what() << std::endl;
        throw;
    }
}

//系统主流程：按照拟定的顺序执行各个步骤
//流程: 自动创建目录 → 用户登录 → 加载数据 → 成绩计算 → 名单排序 → OJ分析
//      → [循环: 交互修改(若已扩充则先展示新数据) → 可视化 → 数据扩充? → (若扩充则回到修改)] → 保存导出
//学生用户OJ分析使用默认阈值40，教师和管理员可自定义阈值
void ExamStatisticsSystem::run(const std::string& dataDir, const std::string& outputDir)
{
    //0.自动创建所需目录（如不存在）
    namespace fs = std::filesystem;
    try {
        if (!fs::exists(dataDir)) {
            fs::create_directories(dataDir);
            std::cout << "[初始化] 已创建数据目录: " << dataDir << std::endl;
        }
        if (!fs::exists(outputDir)) {
            fs::create_directories(outputDir);
            std::cout << "[初始化] 已创建输出目录: " << outputDir << std::endl;
        }
    }
    catch (const fs::filesystem_error& e) {
        throw std::runtime_error(
            std::string("无法创建所需目录: ") + e.what());
    }

    std::cout << "========================================" << std::endl;
    std::cout << "  期末成绩信息统计系统 v1.0" << std::endl;
    std::cout << "========================================" << std::endl;

    //1.用户登录与权限验证
    bool loggedIn = false;
    while (!loggedIn) {
        std::string username, passwd;
        std::cout << "\n请输入用户名(学号/工号): ";
        std::getline(std::cin, username);
        std::cout << "请输入密码: ";
        std::getline(std::cin, passwd);

        if (auth.login(username, passwd)) {
            loggedIn = true;
            const User* user = auth.currentUser();
            // 通过虚函数区分身份：学生 canViewAll=false，教师 canChangeAll=false，管理员均为 true
            const char* roleName = "学生";
            if (auth.CanChangeAll())      roleName = "管理员";
            else if (auth.CanViewAll())   roleName = "教师";
            std::cout << "[登录成功] 欢迎 " << user->username
                      << " (身份: " << roleName << ")" << std::endl;
        } else {
            std::cout << "[登录失败] 用户名或密码错误，请重试" << std::endl;
        }
    }

    try {
        //2.加载数据
        loadData(dataDir);

        //3.成绩计算
        computeGrades();

        //4.名单排序
        //默认按学号排序显示
        sortAndDisplay(0);

        //5.OJ分析
        //学生用户（canViewAll=false）直接使用默认阈值，教师和管理员可自定义阈值
        int threshold = 40;   // 默认过题数阈值
        if (!auth.CanViewAll()) {
            std::cout << "\n[OJ分析] 使用默认过题阈值: " << threshold << std::endl;
        } else {
            std::cout << "\n请输入OJ过题数阈值(低于此值为不足，默认" << threshold << "): ";
            std::string thresholdInput;
            std::getline(std::cin, thresholdInput);
            if (!thresholdInput.empty()) {
                try { threshold = std::stoi(thresholdInput); }
                catch (...) { threshold = 40; }
            }
        }
        analyzeOJ(threshold);

        //6.交互修改（可循环：修改 → 可视化 → 扩充 → 返回修改）
        //若数据已扩充，进入修改前先展示新增学生以供参考
        bool continueMainLoop = true;
        size_t countBeforeExpand = students.size();
        while (continueMainLoop) {
            std::cout << "\n是否进入交互修改模式? (1=是, 0=跳过): ";
            std::string input;
            std::getline(std::cin, input);
            if (input == "1") {
                // 数据扩充后首次进入修改时，先输出新增学生数据
                if (students.size() > countBeforeExpand) {
                    size_t newCount = students.size() - countBeforeExpand;
                    std::cout << "\n--- 新增学生数据（供修改参考，共 "
                              << newCount << " 人） ---" << std::endl;
                    for (size_t i = countBeforeExpand; i < students.size(); ++i) {
                        std::cout << "  [" << (i + 1) << "] ";
                        displayStudent(students[i]);
                    }
                    std::cout << "----------------------------------------------" << std::endl;
                    countBeforeExpand = students.size();   // 更新基准，避免重复显示
                }
                findAndModify();
            }

            //7.数据可视化
            visualize(outputDir);

            //8.数据扩充（扩充后返回修改界面，允许对新数据继续操作）
            std::cout << "\n是否进行数据扩充? (1=是, 0=跳过, 直接保存): ";
            std::string expandInput;
            std::getline(std::cin, expandInput);
            if (expandInput == "1") {
                countBeforeExpand = students.size();   // 记录扩充前基准
                augmentData(100);
                std::cout << "\n[流程] 数据已扩充，新增 "
                          << (students.size() - countBeforeExpand)
                          << " 人，返回修改界面..." << std::endl;
            } else {
                continueMainLoop = false;   // 退出循环，进入保存
            }
        }

        //9.保存导出
        std::string outputPath = outputDir + "/成绩总表_导出.xlsx";
        saveData(outputPath);

        std::cout << "\n========================================" << std::endl;
        std::cout << "  流程执行完毕，程序正常结束" << std::endl;
        std::cout << "========================================" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "\n[严重错误] 程序异常终止: " << e.what() << std::endl;
        std::cerr << "请检查数据文件路径是否正确，以及第三方库是否已正确安装" << std::endl;
        std::cerr.flush();
        std::cout << "\n按 Enter 键退出..." << std::endl;
        std::cin.get();
    }
    catch (...) {
        std::cerr << "\n[严重错误] 发生未知异常，程序异常终止" << std::endl;
        std::cerr.flush();
        std::cout << "\n按 Enter 键退出..." << std::endl;
        std::cin.get();
    }
}

//提供对外接口，获取当前学生列表（只读）
const std::vector<CStudent>& ExamStatisticsSystem::getStudents() const
{
    return students;
}
