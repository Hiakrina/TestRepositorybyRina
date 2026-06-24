#include "visualization/ScoreVisualizer.h"
#include "visualization/LibxlsxChartRenderer.h"

#include <string>
#include <vector>

//ScoreVisualizer类用于生成成绩分布的可视化图表
ScoreVisualizer::ScoreVisualizer()
    : renderer(new LibxlsxChartRenderer())
{}

ScoreVisualizer::~ScoreVisualizer()
{
    delete renderer;
}

//生成展示分数段分布的饼状图
void ScoreVisualizer::showScoreDistribution(
    const std::vector<CStudent>& students,
    const std::string& outputPath)
{
    //过滤有效学生
    std::vector<const CStudent*> valid = filterValid(students);

    //统计分数段分布
    std::vector<std::string> labels;
    std::vector<int> counts;
    countScoreRanges(valid, labels, counts);

    //生成饼状图
    renderer->drawPieChart(labels, counts,
                             "总评成绩分数段分布", outputPath);
}

//生成展示班级成绩对比的柱状图
void ScoreVisualizer::showClassComparison(
    const std::vector<CStudent>& students,
    const std::string& outputPath)
{
    //过滤有效学生
    std::vector<const CStudent*> valid = filterValid(students);

    //按班级计算平均分
    std::vector<std::string> classes;
    std::vector<double> averages;
    calcClassAverages(valid, classes, averages);

    //生成柱状图
    renderer->drawBarChart(classes, averages, 
                            "班级", "总评平均分", outputPath);
}

//过滤有效学生，排除重修、缓考、旷考、取消资格的学生
std::vector<const CStudent*> ScoreVisualizer::filterValid(
    const std::vector<CStudent>& students)
{
    std::vector<const CStudent*> valid;
    for (const CStudent& stu : students) {
        if (!stu.isExcluded() && stu.isNormal()) {
            valid.push_back(&stu);
        }
    }
    return valid;
}

//统计分数段人数
//其中分数段定义为：0-59(不及格)，60-69(及格)，
//                 70-79(中等)，80-89(良好)，90-100(优秀)
void ScoreVisualizer::countScoreRanges(
    const std::vector<const CStudent*>& validStudents,
    std::vector<std::string>& labels,
    std::vector<int>& counts)
{
    labels = {"0-59(不及格)", "60-69(及格)", "70-79(中等)",
              "80-89(良好)", "90-100(优秀)"};
    
    //初始化人数统计为0
    counts.assign(5, 0);

    //遍历有效学生，根据总评成绩统计分数段人数
    for (const CStudent* stu : validStudents) {
        int total = stu->getScore().total;

        if (total < 60) {
            counts[0]++;
        } else if (total < 70) {
            counts[1]++;
        } else if (total < 80) {
            counts[2]++;
        } else if (total < 90) {
            counts[3]++;
        } else {
            counts[4]++;
        }
    }
}

//按班级计算总评平均分
void ScoreVisualizer::calcClassAverages(
    const std::vector<const CStudent*>& validStudents,
    std::vector<std::string>& classes,
    std::vector<double>& averages)
{
    //遍历第一遍：收集所有不重复的班级名
    for (size_t i = 0; i < validStudents.size(); ++i) {
        std::string cls = validStudents[i]->getClassAfter();
        if (cls.empty()) cls = validStudents[i]->getClassBefore();
        if (cls.empty()) cls = "未知班级";

        bool found = false;
        for (size_t j = 0; j < classes.size(); ++j) {
            if (classes[j] == cls) { found = true; break; }
        }
        if (!found) classes.push_back(cls);
    }

    //遍历第二遍：对每个班级，遍历所有学生累加分数
    for (size_t c = 0; c < classes.size(); ++c) {
        double sum = 0;
        int count = 0;
        for (size_t i = 0; i < validStudents.size(); ++i) {
            std::string cls = validStudents[i]->getClassAfter();
            if (cls.empty()) cls = validStudents[i]->getClassBefore();
            if (cls.empty()) cls = "未知班级";

            if (cls == classes[c]) {
                sum += validStudents[i]->getScore().total;
                ++count;
            }
        }
        averages.push_back(count > 0 ? sum / count : 0);
    }
}
