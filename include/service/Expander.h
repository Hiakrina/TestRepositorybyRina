#ifndef EXPANDER_H
#define EXPANDER_H

#include <vector>
#include <string>
#include "model/CStudent.h"

class Expander {
public:
    // 主函数：扩充学生列表到 targetCount 人以上
    std::vector<CStudent> expand(const std::vector<CStudent>& existing, int targetCount);

private:
    std::string generateID(const std::vector<CStudent>& existing, int index);
    std::string generateName();
    CScore generateScore();
    std::string generateOJAccount(const std::string& studentID);
    int generateOJSolved();
    std::string generateExamStatus(const std::vector<CStudent>& existing);
    CStudent makeStudent(const std::vector<CStudent>& existing, int index);
};
/*  前五个函数用于生成ID，姓名，分数（各种包括实习，平时，卷面），OJ账号和过题数
最后来合成一个新学生
*/
#endif