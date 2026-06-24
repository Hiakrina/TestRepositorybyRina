#include "visualization/LibxlsxChartRenderer.h"

#include <xlsxwriter.h>
#include <stdexcept>//异常检测
#include <sstream>//字符串流
#include <iomanip>//输入输出格式控制

void LibxlsxChartRenderer::drawBarChart(
    const std::vector<std::string>& categories,
    const std::vector<double>& values,
    const std::string& xLabel,
    const std::string& yLabel,
    const std::string& outputPath)
{
    //参数检查
    if (categories.size() != values.size()) {
        throw std::runtime_error(
            "LibxlsxChartRenderer::drawBarChart: categories 与 values 数量不匹配");
    }
    if (categories.empty()) {
        throw std::runtime_error(
            "LibxlsxChartRenderer::drawBarChart: 数据为空");
    }

    lxw_workbook* workbook = nullptr;
    lxw_worksheet* worksheet = nullptr;

    try{
        //尝试创建工作簿
        workbook = workbook_new(outputPath.c_str());
        if (!workbook) {
            throw std::runtime_error("无法创建工作簿: " + outputPath);
        }

        worksheet = workbook_add_worksheet(workbook, "班级平均分对比");

        //从第0行写入表头
        worksheet_write_string(worksheet, 0, 0, xLabel.c_str(), nullptr);
        worksheet_write_string(worksheet, 0, 1, yLabel.c_str(), nullptr);

        //从第1行开始写入数据
        for (size_t i = 0; i < categories.size(); ++i) {
            worksheet_write_string(worksheet, static_cast<lxw_row_t>(i + 1), 0,
                                   categories[i].c_str(), nullptr);
            worksheet_write_number(worksheet, static_cast<lxw_row_t>(i + 1), 1,
                                   values[i], nullptr);
        }

        //创建柱状图对象
        lxw_chart* chart = workbook_add_chart(workbook, LXW_CHART_COLUMN);

        //利用ostream构造Excel公式字符串
        //格式: =SheetName!$B$2:$B$N (值列：范围从B2到BN，N=数据行数+1)
        //       =SheetName!$A$2:$A$N (分类列：范围从A2到AN，N=数据行数+1)
        //$表示绝对引用，固定行列，确保图表引用正确
        //categories.size()返回数据行数，+1的原因在于数据从第1行开始写入
        std::ostringstream valuesRange, catsRange;
        valuesRange << "=班级平均分对比!$B$2:$B$" << (categories.size() + 1);
        catsRange   << "=班级平均分对比!$A$2:$A$" << (categories.size() + 1);

        //添加数据到图表，指定类别和数值范围
        lxw_chart_series* series = chart_add_series(
            chart, catsRange.str().c_str(), valuesRange.str().c_str());
        if (!series) {
            throw std::runtime_error("无法创建图表系列");
        }

        //设置轴标签
        chart_title_set_name(chart, "班级平均分对比");
        chart_axis_set_name(chart->x_axis, xLabel.c_str());
        chart_axis_set_name(chart->y_axis, yLabel.c_str());

        //将图表写入工作表
        //图表插入位置：D2（第1行，第3列），留出空间
        lxw_chart_options chartOpts = { 0 };
        worksheet_insert_chart_opt(worksheet, 1, 3, chart, &chartOpts);

        //保存和关闭
        lxw_error err = workbook_close(workbook);
        if (err != LXW_NO_ERROR) {
            throw std::runtime_error(
                std::string("保存工作簿失败, 错误码: ") + std::to_string(err));
        }
    }

    //捕获异常
    catch (...) {
        if (workbook) {
            workbook_close(workbook);
        }
        throw;
    }
}

void LibxlsxChartRenderer::drawPieChart(
    const std::vector<std::string>& labels,
    const std::vector<int>& values,
    const std::string& title,
    const std::string& outputPath)
{
    //参数检查
    if (labels.size() != values.size()) {
        throw std::runtime_error(
            "LibxlsxChartRenderer::drawPieChart: labels 与 values 数量不匹配");
    }
    if (labels.empty()) {
        throw std::runtime_error(
            "LibxlsxChartRenderer::drawPieChart: 数据为空");
    }

    lxw_workbook* workbook = nullptr;
    lxw_worksheet* worksheet = nullptr;

    try{
        //尝试创建工作簿
        workbook = workbook_new(outputPath.c_str());
        if (!workbook) {
            throw std::runtime_error("无法创建工作簿: " + outputPath);
        }

        worksheet = workbook_add_worksheet(workbook, "分数段分布");

        //从第0行写入表头
        worksheet_write_string(worksheet, 0, 0, "分数段", nullptr);
        worksheet_write_string(worksheet, 0, 1, "人数", nullptr);

        //从第1行开始写入数据
        for (size_t i = 0; i < labels.size(); ++i) {
            worksheet_write_string(worksheet, static_cast<lxw_row_t>(i + 1), 0,
                                   labels[i].c_str(), nullptr);
            worksheet_write_number(worksheet, static_cast<lxw_row_t>(i + 1), 1,
                                   static_cast<double>(values[i]), nullptr);
        }

        //创建饼状图对象
        lxw_chart* chart = workbook_add_chart(workbook, LXW_CHART_PIE);

        //利用ostream构造Excel公式字符串
        //格式为 =SheetName!$B$2:$B$N (值列：范围从B2到BN，N=数据行数+1)
        //        =SheetName!$A$2:$A$N (分类列：范围从A2到AN，N=数据行数+1)
        //$表示绝对引用，固定行列，确保图表引用正确
        //labels.size()返回数据行数，+1的原因在于数据从第1行开始写入
        std::ostringstream valuesRange, labelsRange;
        valuesRange << "=分数段分布!$B$2:$B$" << (labels.size() + 1);
        labelsRange << "=分数段分布!$A$2:$A$" << (labels.size() + 1);

        //添加数据到图表，指定类别和数值范围
        lxw_chart_series* series = chart_add_series(
            chart, labelsRange.str().c_str(), valuesRange.str().c_str());
        if (!series) {
            throw std::runtime_error("无法创建图表系列");
        }

        //设置标题
        chart_title_set_name(chart, title.c_str());

        //将图表写入工作表
        //图表插入位置为D2（第1行，第3列）是为了留出空间显示表格数据
        lxw_chart_options chartOpts = { 0 };
        worksheet_insert_chart_opt(worksheet, 1, 3, chart, &chartOpts);

        //保存和关闭
        lxw_error err = workbook_close(workbook);
        if (err != LXW_NO_ERROR) {
            throw std::runtime_error(
                std::string("保存工作簿失败, 错误码: ") + std::to_string(err));
        }
    }

    //捕获异常
    catch (...) {
        if (workbook) {
            workbook_close(workbook);
        }
        throw;
    }
}
