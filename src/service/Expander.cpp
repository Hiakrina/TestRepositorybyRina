#include "service/Expander.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sstream>
#include <iomanip>

std::vector<CStudent> Expander::expand(const std::vector<CStudent>& existing, int targetCount) {
    //初始化随机数
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(0)));
        seeded = true;
    }

    if (existing.size() >= targetCount) {
        return existing;
    }
    int need = targetCount - existing.size();
    // 这里考虑过用for循环，但是好像vector的构造用赋值号拷贝也是深拷贝就没用
    std::vector<CStudent> result = existing;
    for (int i = 0; i < need; i++) {
        result.push_back(makeStudent(existing, i));
    }
    return result;
}

std::string Expander::generateID(const std::vector<CStudent>& existing, int index) {
    int maxCode = 0;
    for (int i = 0; i < existing.size(); i++) {
        // 只处理后四位流水号
        std::string id = existing[i].getStudentID();
        int code = std::stoi(id.substr(6, 4));
        if (code > maxCode) {
            maxCode = code;
        }
    }
    int newcode = maxCode + index + 1;
    std::ostringstream out;
    out << "202101" << std::setw(4) << std::setfill('0') << newcode;
    std::string newID = out.str();
    return newID;
}

std::string Expander::generateName() {
    static const std::vector<std::string> surnames = {
        "陈", "吕", "赵", "王", "赵", "刘", "陈", "杨", "黄",
        "周", "吴", "徐", "孙", "马", "朱", "胡", "郭", "连",
        "何", "高", "林", "郑"
    };
    static const std::vector<std::string> givenNames = {
        "伟", "芳", "娜", "飞", "敏", "强", "丽", "静", "超", "明",
        "建国", "志强", "秀英", "海燕", "小龙", "婷婷", "浩然", "雨涵",
        "子轩", "一鸣", "晖洁"
    };
    int si = rand() % surnames.size();
    int gi = rand() % givenNames.size();
    return surnames[si] + givenNames[gi];
}

CScore Expander::generateScore() {
    CScore sc;
    sc.performance = rand() % 41 + 60;   // 平时 60–100 ，卷面同理
    sc.exam = rand() % 61 + 40; 
    if (rand() % 2 == 0) {
        sc.practice = 0;
    } else {
        sc.practice = rand() % 41 + 60;
    }
    // 总评 = round(平时*0.3 + 卷面*0.7)
    sc.total = static_cast<int>(std::round(sc.performance * 0.3 + sc.exam * 0.7));
    return sc;
}

std::string Expander::generateOJAccount(const std::string& studentID) {
    if (rand() % 2 == 0) {
        return studentID;
    } else {
        return studentID + "_oj";
    }
}

int Expander::generateOJSolved() {
    if (rand() % 100 < 80) {
        return rand() % 101;    // 有 OJ 记录：0~100 题
    } else {
        return -1;              // 无 OJ 记录
    }
}

std::string Expander::generateExamStatus(const std::vector<CStudent>& existing) {
    int countChuxiu = 0;
    int countChongxiu = 0;
    int countHuankao = 0;
    int countOther = 0;

    for (int i = 0; i < existing.size(); i++) {
        std::string s = existing[i].getExamStatus();
        if (s.empty() || s == "初修") countChuxiu++;
        else if (s == "重修") countChongxiu++;
        else if (s == "缓考") countHuankao++;
        else countOther++;
    }

    int total = countChuxiu + countChongxiu + countHuankao + countOther;
    if (total == 0) return "";

    int r = rand() % total;
    if (r < countChuxiu)                return "初修";
    r -= countChuxiu;
    if (r < countChongxiu)              return "重修";
    r -= countChongxiu;
    if (r < countHuankao)               return "缓考";
    return "";
}

CStudent Expander::makeStudent(const std::vector<CStudent>& existing, int index) {
    std::string id   = generateID(existing, index);
    std::string name = generateName();   // 普通中文姓名
    static const std::vector<std::string> easterEggNames = {
        "Susie Glitter", "Falcons_NiKo", "Kal'tsit", "Donald Trump",
        "Finn Anderson", "Ilya Osipov", "Maxim Lukin", "Rene Madsen"
    };
    //彩蛋环节（
    bool isEasterEgg = false;
    if (rand() % 100 < 2) {                      // 震惊！触发彩蛋概率竟然高达2%！
        int eggIdx = rand() % easterEggNames.size();
        name = easterEggNames[eggIdx];
        isEasterEgg = true;
    }

    CScore sc;
    std::string ojAccount;
    int ojSolved;
    std::string examStatus;
    if (isEasterEgg) {
        // 彩蛋学生：全满分，OJ 过题数100，账号使用名字本身
        sc.performance  = 100;
        sc.exam = 100;
        sc.practice = 100;
        sc.total = 100;
        ojAccount = name;
        ojSolved = 100;
        examStatus = "初修";
    } else {
        // 普通学生：随机生成
        sc = generateScore();
        ojAccount = generateOJAccount(id);
        ojSolved = generateOJSolved();
        examStatus = generateExamStatus(existing);
    }

    CStudent stu(id, name, "计算机类1311", "智能1304");
    stu.setScore(sc);
    stu.setOJAccount(ojAccount);
    stu.setOJSolved(ojSolved);
    stu.setExamStatus(examStatus);
    stu.setExcluded(examStatus != "初修" && !examStatus.empty());

    return stu;
}