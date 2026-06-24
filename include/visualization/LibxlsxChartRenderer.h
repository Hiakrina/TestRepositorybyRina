#ifndef LIBXLSXCHARTRENDERER_H
#define LIBXLSXCHARTRENDERER_H

#include <string>
#include <vector>

//基于libxlsxwriter库实现的图表渲染器
//依赖 libxlsxwriter 库 (https://libxlsxwriter.github.io/)
class LibxlsxChartRenderer{
public:
    void drawBarChart(
        const std::vector<std::string>& categories,//分类标签，如班级名称等
        const std::vector<double>& values,//对应数值
        const std::string& xLabel,//X轴标签
        const std::string& yLabel,//Y轴标签
        const std::string& outputPath//输出路径
    );

    void drawPieChart(
        const std::vector<std::string>& labels,//扇型区域标签
        const std::vector<int>& values,//对应数值
        const std::string& title,//图表标题
        const std::string& outputPath//输出路径
    ) ;
};

#endif // LIBXLSXCHARTRENDERER_H
