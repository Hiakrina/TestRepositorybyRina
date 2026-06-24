#ifndef SCOREVISUALIZER_H
#define SCOREVISUALIZER_H

#include "LibxlsxChartRenderer.h"
#include "model/CStudent.h"

#include <string>
#include <vector>

class ScoreVisualizer {
public:
    ScoreVisualizer();
    ~ScoreVisualizer();

    void showScoreDistribution(
        const std::vector<CStudent>& students,
        const std::string& outputPath);

    void showClassComparison(
        const std::vector<CStudent>& students,
        const std::string& outputPath);

private:
    //过滤出有效学生（排除重修、缓考、旷考、取消资格）
    static std::vector<const CStudent*> filterValid(
        const std::vector<CStudent>& students);

    //统计分数段人数
    static void countScoreRanges(
        const std::vector<const CStudent*>& validStudents,
        std::vector<std::string>& labels,
        std::vector<int>& counts);

    //计算班级平均分
    static void calcClassAverages(
        const std::vector<const CStudent*>& validStudents,
        std::vector<std::string>& classes,
        std::vector<double>& averages);

    LibxlsxChartRenderer* renderer;
};

#endif // SCOREVISUALIZER_H
