#ifndef DATAREPOSITORY_H
#define DATAREPOSITORY_H

#include "ExcelReader.h"
#include "ExcelWriter.h"
#include "model/CStudent.h"

#include <string>
#include <vector>

class DataRepository {
public:
    DataRepository();
    ~DataRepository();

    //加载数据
    void loadAll(const std::string& examPath,
                 const std::string& totalPath,
                 const std::string& ojPath,
                 const std::string& performPath);

    //保存成绩到Excel
    void saveScores(const std::string& filePath,
                    const std::vector<CStudent>& students,
                    const std::string& sheetName = "成绩总表");
    //外部访问（可修改）
    std::vector<CStudent>& getStudents();
    //外部访问（只读）
    const std::vector<CStudent>& getStudents() const;

private:
    //解析考试成绩表
    std::vector<CStudent> parseExamSheet(
        const std::vector<std::vector<std::string>>& raw);

    //解析平时成绩表
    void parsePerformanceSheet(
        const std::vector<std::vector<std::string>>& raw,
        std::vector<CStudent>& students);

    //解析OJ成绩表
    void parseOJSheet(
        const std::vector<std::vector<std::string>>& raw,
        std::vector<CStudent>& students);

    //解析总评成绩表
    void parseTotalSheet(
        const std::vector<std::vector<std::string>>& raw,
        std::vector<CStudent>& students);

    //去除字符串两端的空白字符
    static std::string trim(const std::string& s);

    //安全地将字符串转换为整数
    static int safeStoi(const std::string& s, int defaultValue = 0);

    ExcelReader* reader;
    ExcelWriter* writer;
    std::vector<CStudent> students;
};

#endif // DATAREPOSITORY_H
