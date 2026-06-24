#include "data/DataRepository.h"

#include <cctype>

DataRepository::DataRepository()
    : reader(new ExcelReader())
    , writer(new ExcelWriter())
{}

DataRepository::~DataRepository()
{
    delete reader;
    delete writer;
}

void DataRepository::loadAll(const std::string& examPath,
                             const std::string& totalPath,
                             const std::string& ojPath,
                             const std::string& performPath)
{
    students.clear();//清空之前的学生数据

    //1.读取并解析卷面成绩（包含学号、姓名、班级等基本信息）
    if (!examPath.empty()) {
        std::vector<std::vector<std::string>> rawExam = reader->readSheet(examPath, 1);
        students = parseExamSheet(rawExam);
    }

    //2.读取总评成绩单进行校验合并（校验总评成绩）
    if (!totalPath.empty()) {
        std::vector<std::vector<std::string>> rawTotal = reader->readSheet(totalPath, 1);
        parseTotalSheet(rawTotal, students);
    }

    //3.读取并合并OJ数据（OJ账号、过题数）
    if (!ojPath.empty()) {
        std::vector<std::vector<std::string>> rawOJ = reader->readSheet(ojPath, 1);
        parseOJSheet(rawOJ, students);
    }

    //4.读取并合并平时成绩（平时分）
    if (!performPath.empty()) {
        std::vector<std::vector<std::string>> rawPerform = reader->readSheet(performPath, 1);
        parsePerformanceSheet(rawPerform, students);
    }
}

void DataRepository::saveScores(const std::string& filePath,
                                const std::vector<CStudent>& students,
                                const std::string& sheetName)
{
    //构造二维字符串表：表头 + 数据行
    std::vector<std::vector<std::string>> table;

    //表头
    table.push_back({
        "学号", "姓名", "原班级", "现班级",
        "平时成绩", "卷面成绩", "OJ练习分", "总评成绩",
        "OJ账号", "OJ过题数", "考试状态", "是否排除"
    });

    //数据行
    for (    const CStudent& stu : students) {
        const CScore& sc = stu.getScore();
        table.push_back({
            stu.getStudentID(),
            stu.getName(),
            stu.getClassBefore(),
            stu.getClassAfter(),
            std::to_string(sc.performance),
            std::to_string(sc.exam),
            std::to_string(sc.practice),
            std::to_string(sc.total),
            stu.getOJAccount(),
            stu.hasOJRecord() ? std::to_string(stu.getOJSolved()) : "",
            stu.getExamStatus(),
            stu.isExcluded() ? "是" : "否"
        });
    }

    writer->writeSheet(filePath, table, sheetName);
}

//外部访问（可修改）
std::vector<CStudent>& DataRepository::getStudents()
{
    return students;
}

//外部访问（只读）
const std::vector<CStudent>& DataRepository::getStudents() const
{
    return students;
}

//去除字符串首尾空白字符
std::vector<CStudent> DataRepository::parseExamSheet(
    const std::vector<std::vector<std::string>>& raw)
{
    std::vector<CStudent> students;

    if (raw.empty()) return students;

    //实际列：P1=序号 P2=学号 P3=姓名 P4=班级
    //       P5-P9=各题型分 P10=总分 P11=备注
    //跳过表头（第0行），从第1行开始解析
    for (size_t i = 1; i < raw.size(); ++i) {
        const std::vector<std::string>& row = raw[i];
        if (row.empty()) continue;

        //至少需要学号、姓名、总分列
        if (row.size() < 10) continue;

        std::string id = trim(row[1]);  //P2: 学号
        if (id.empty()) continue;

        CStudent stu;
        stu.setStudentID(id);
        stu.setName(trim(row[2]));       //P3: 姓名
        stu.setClassBefore(trim(row[3])); //P4: 班级

        //卷面成绩（P10=总分, 索引9）
        int examScore = safeStoi(row[9], -1);
        CScore score;
        score.exam = (examScore >= 0) ? examScore : 0;
        stu.setScore(score);

        //备注（P11, 索引10）：仅重修/缓考/旷考/取消资格标记为排除
        if (row.size() > 10) {
            std::string status = trim(row[10]);
            if (!status.empty()) {
                stu.setExamStatus(status);
                if (status != "初修") {
                    stu.setExcluded(true);
                }
            }
        }

        students.push_back(std::move(stu));
    }

    return students;
}

void DataRepository::parsePerformanceSheet(
    const std::vector<std::vector<std::string>>& raw,
    std::vector<CStudent>& students)
{
    if (raw.empty() || students.empty()) return;


    //实际列：P1=学号 P2=姓名 P3=平时成绩
    //跳过表头，从第1行开始解析
    for (size_t i = 1; i < raw.size(); ++i) {
        const std::vector<std::string>& row = raw[i];
        if (row.size() < 3) continue;

        std::string id = trim(row[0]);  // P1: 学号
        if (id.empty()) continue;

        int performScore = safeStoi(row[2], -1);  // P3: 平时成绩

        for (size_t j = 0; j < students.size(); ++j) {
            if (students[j].getStudentID() == id) {
                students[j].getScore().performance = (performScore >= 0) ? performScore : 0;
                break;
            }
        }
    }
}

void DataRepository::parseOJSheet(
    const std::vector<std::vector<std::string>>& raw,
    std::vector<CStudent>& students)
{
    if (raw.empty() || students.empty()) return;


    // 实际列：P1=Username(学号) P2=RealName P3=AC(过题数)
    //          P4=TotalSubmission P5=TotalTime
    // 跳过表头，从第1行开始解析
    for (size_t i = 1; i < raw.size(); ++i) {
        const std::vector<std::string>& row = raw[i];
        if (row.size() < 3) continue;

        std::string id = trim(row[0]);  // P1: Username(学号)
        if (id.empty()) continue;

        for (size_t j = 0; j < students.size(); ++j) {
            if (students[j].getStudentID() == id) {
                students[j].setOJAccount(id);
                int solved = safeStoi(row[2], -1);
                if (solved >= 0) students[j].setOJSolved(solved);
                break;
            }
        }
    }
}

//解析总评成绩单
void DataRepository::parseTotalSheet(
    const std::vector<std::vector<std::string>>& raw,
    std::vector<CStudent>& students)
{
    if (raw.empty() || students.empty()) return;

    //实际结构：
    //   Row 1-4: 标题/元数据
    //   Row 5:   表头 (序号|学号|姓名|性别|专业/班级|学院|空|平时成绩|期末成绩|总成绩|备注)
    //   Row 6:   空行
    //   Row 7+:  数据行
    //列映射：P2=学号, P5=专业/班级, P8=平时成绩, P9=期末成绩, P10=总成绩, P11=备注
    const size_t HEADER_ROWS = 6;  //跳过前6行

    for (size_t i = HEADER_ROWS; i < raw.size(); ++i) {
        const std::vector<std::string>& row = raw[i];
        if (row.size() < 10) continue;

        std::string id = trim(row[1]);  //P2: 学号
        if (id.empty()) continue;

        for (size_t j = 0; j < students.size(); ++j) {
            if (students[j].getStudentID() != id) continue;

            CScore& sc = students[j].getScore();

            //专业/班级（P5, 索引4），更新现班级
            std::string cls = trim(row[4]);
            if (!cls.empty()) {
                students[j].setClassAfter(cls);
            }

            //平时成绩（P8, 索引7）
            int perf = safeStoi(row[7], -1);
            if (perf >= 0) sc.performance = perf;

            //期末成绩（P9, 索引8）
            int exam = safeStoi(row[8], -1);
            if (exam >= 0) sc.exam = exam;

            //总评成绩（P10, 索引9）
            int total = safeStoi(row[9], -1);
            if (total >= 0) sc.total = total;

            //备注（P11, 索引10）：仅重修/缓考/旷考/取消资格为异常
            if (row.size() > 10) {
                std::string status = trim(row[10]);
                if (!status.empty()) {
                    students[j].setExamStatus(status);
                    if (status != "初修") {
                        students[j].setExcluded(true);
                    }
                }
            }
            break;
        }
    }
}

//去除字符串两端的空白字符
std::string DataRepository::trim(const std::string& s)
{
    if (s.empty()) return s;

    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }

    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        --end;
    }

    return s.substr(start, end - start);
}

//安全地将字符串转换为整数
int DataRepository::safeStoi(const std::string& s, int defaultValue)
{
    if (s.empty()) return defaultValue;

    try {
        //处理可能包含小数点的数字字符串
        std::string cleaned;
        for (char c : s) {
            if (c == '.' || c == ',') break; //遇到小数点或逗号停止
            if (std::isdigit(static_cast<unsigned char>(c)) || c == '-') {
                cleaned += c;
            }
        }
        if (cleaned.empty()) return defaultValue;
        return std::stoi(cleaned);
    }
    catch (...) {
        return defaultValue;
    }
}
