#include "data/ExcelWriter.h"

#include <OpenXLSX.hpp>//ExcelWriter的具体实现依赖OpenXLSX库
#include <stdexcept>//异常处理
#include <string>

//实现ExcelWriter类的writeSheet，将二维字符串数据写入指定工作表
void ExcelWriter::writeSheet(
    const std::string& filePath,
    const std::vector<std::vector<std::string>>& data,
    const std::string& sheetName)
{
    //使用try-catch捕获OpenXLSX库可能抛出的异常
    try {
        //创建或打开工作簿（forceOverwrite=true 覆盖已存在文件）
        OpenXLSX::XLDocument doc;
        doc.create(filePath, true);

        //若工作表已存在则覆盖写入（先删后建）
        if (doc.workbook().worksheetExists(sheetName)) {
            doc.workbook().deleteSheet(sheetName);
        }
        doc.workbook().addWorksheet(sheetName);

        //删除新建工作簿默认的"Sheet1"，避免用户打开看到空表
        if (sheetName != "Sheet1" && doc.workbook().worksheetExists("Sheet1")) {
            doc.workbook().deleteSheet("Sheet1");
        }

        OpenXLSX::XLWorksheet wks = doc.workbook().worksheet(sheetName);

        //逐行逐列写入
        for (size_t r = 0; r < data.size(); ++r) {
            const std::vector<std::string>& row = data[r];
            for (size_t c = 0; c < row.size(); ++c) {
                //OpenXLSX 行/列索引从1开始（所以使用r+1和c+1）
                wks.cell(static_cast<unsigned long long>(r + 1),
                         static_cast<unsigned long long>(c + 1))
                    .value() = row[c];
            }
        }

        doc.save();
        doc.close();
    }
    
    //捕获OpenXLSX库的特定异常
    //统一异常类型，便于上层处理
    //防止底层的异常泄漏到上层
    catch (const OpenXLSX::XLException& e) {
        throw std::runtime_error(
            std::string("ExcelWriter::writeSheet 失败: ") + e.what() +
            " (文件: " + filePath + ", 工作表: " + sheetName + ")");
    }

    //捕获其他标准异常，添加上下文信息
    //防止未知异常导致程序崩溃
    catch (const std::exception& e) {
        throw std::runtime_error(
            std::string("ExcelWriter::writeSheet 异常: ") + e.what());
    }
}
