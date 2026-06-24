#ifndef EXAMSYSTEM_H
#define EXAMSYSTEM_H
#include "data/DataRepository.h"
#include "service/Analyzer.h"
#include "service/Auth.h"
#include "service/Expander.h"
#include "visualization/ScoreVisualizer.h"
#include "model/CStudent.h"

#include <string>
#include <vector>

class ExamStatisticsSystem {
public:
    ExamStatisticsSystem();

    //从指定目录加载Excel数据文件
    void loadData(const std::string& dirPath);

    //成绩计算：总评 = round(平时成绩*0.3 + 卷面成绩*0.7)
    void computeGrades();

    //对名单进行排序并输出到控制台
    //mode: 0=按学号, 1=按班级, 2=按姓名
    void sortAndDisplay(int mode);

    //OJ系统分析：输出过题不足、未在OJ两类名单
    //threshold:过题数阈值（学生用户使用默认值40，教师/管理员通过 canViewAll() 区分后可自定义）
    void analyzeOJ(int threshold);

    //交互式查找与修改循环
    //流程为：查找 → 显示 → 修改 → 重算总评 → 继续/退出
    //位于主流程的修改-可视化-扩充循环中，数据扩充后自动回到此处
    //若数据已扩充，进入修改前会先展示新增学生数据以供参考
    void findAndModify();

    //数据可视化：生成饼图(分数段分布)与柱状图(班级平均分对比)
    void visualize(const std::string& outputDir);

    //数据扩充：将学生列表扩充至目标人数(默认100人)，委托Expander执行
    //扩充后主流程自动返回修改界面，允许对新数据继续操作
    void augmentData(int targetCount = 100);

    //保存导出：将当前学生列表写入Excel
    void saveData(const std::string& outputPath);

    //完整运行流程
    void run(const std::string& dataDir, const std::string& outputDir);

    //获取学生列表（只读）
    const std::vector<CStudent>& getStudents() const;

private:
    //成绩计算
    static int calcTotal(int performance, int exam);

    //排序
    enum class SortMode { BY_ID, BY_CLASS, BY_NAME };
    static void sortStudents(std::vector<CStudent>& students, SortMode mode);

    //有关排序方法的调度
    static bool compareByID(const CStudent& a, const CStudent& b);
    static bool compareByClass(const CStudent& a, const CStudent& b);
    static bool compareByName(const CStudent& a, const CStudent& b);

    //终端交互
    void displayStudent(const CStudent& stu) const;
    void displayStudentList(const std::vector<CStudent*>& list) const;
    int readMenuChoice(int min, int max) const;

    //成员对象
    DataRepository repo;           //数据仓库
    Analyzer analyzer;             //OJ分析+查找+修改  
    AuthManager auth;              //身份认证（权限控制）
    Expander expander;             //数据扩充
    ScoreVisualizer visualizer;    //图表可视化
    std::vector<CStudent> students;//学生列表
};

#endif // EXAMSYSTEM_H
