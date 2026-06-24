#include "data/ExcelReader.h"

#include <OpenXLSX.hpp>//ExcelReader的具体实现依赖OpenXLSX库
#include <stdexcept>//异常处理
#include <string>

//实现ExcelReader类的readSheet，读取指定工作表中的数据，返回二维字符串表
std::vector<std::vector<std::string>> ExcelReader::readSheet(
    const std::string& filePath,
    int sheetIndex)
{
    std::vector<std::vector<std::string>> result;

    //使用try-catch捕获OpenXLSX库可能抛出的异常
    try {
        //打开工作簿
        OpenXLSX::XLDocument doc;
        doc.open(filePath);

        //获取工作表存储在wks中（OpenXLSX索引从1开始）
        OpenXLSX::XLWorksheet wks = doc.workbook().worksheet(sheetIndex);

        //获取已使用单元格范围
        OpenXLSX::XLCellRange range = wks.range();
        unsigned long long rows = range.numRows();
        //unsigned long long类型避免数据过大时溢出
        unsigned long long cols = range.numColumns();

        //预分配空间
        result.reserve(static_cast<size_t>(rows));

        //逐行逐列读取
        for (unsigned long long r = 1; r <= rows; ++r) {
            std::vector<std::string> row;
            row.reserve(static_cast<size_t>(cols));

            for (unsigned long long c = 1; c <= cols; ++c) {
                //获取单元格值并转换为字符串
                OpenXLSX::XLCell cell = wks.cell(r, c);
                std::string value;

                //根据单元格类型进行转换
                switch (cell.value().type()) {
                case OpenXLSX::XLValueType::String:
                    value = cell.value().get<std::string>();
                    //直接提取字符串
                    break;
                case OpenXLSX::XLValueType::Integer:
                    value = std::to_string(cell.value().get<int64_t>());
                    //整数转为字符串
                    break;
                case OpenXLSX::XLValueType::Float:
                    value = std::to_string(cell.value().get<double>());
                    //浮点数转为字符串，但可能包含小数部分
                    //用DataRepository::safeStoi解决
                    break;
                case OpenXLSX::XLValueType::Boolean:
                    value = cell.value().get<bool>() ? "TRUE" : "FALSE";
                    //布尔值转为大写字符串
                    break;
                case OpenXLSX::XLValueType::Empty:
                default:
                    value = "";
                    //空单元格和未知类型设为空字符串
                    break;
                }

                row.push_back(std::move(value));
            }

            result.push_back(std::move(row));
        }

        doc.close();
    }

    //捕获OpenXLSX库的异常并转换为运行时异常
    //统一异常类型，便于上层处理
    //防止底层的异常泄漏到上层
    catch (const OpenXLSX::XLException& e) {
        throw std::runtime_error(
            std::string("ExcelReader::readSheet 失败: ") + e.what() +
            " (文件: " + filePath + ", 工作表索引: " + std::to_string(sheetIndex) + ")");
    }

    //捕获其他标准异常并转换为运行时异常
    //防止未知异常导致程序崩溃
    catch (const std::exception& e) {
        throw std::runtime_error(
            std::string("ExcelReader::readSheet 异常: ") + e.what());
    }

    return result;
}
