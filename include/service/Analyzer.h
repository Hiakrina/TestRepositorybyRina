#ifndef ANALYZER_H
#define ANALYZER_H
#include "Auth.h"
#include "model/CStudent.h"
#include <vector>
#include <string>

class Analyzer{
public:
        std::vector<CStudent> getInsufficientSolved(const std::vector<CStudent>& students,int minSolved) const;            
        //这里打算设计一个函数是把过题的人单独列一个vector,这样方便调用，下同。
        std::vector<CStudent> getStudentsNotInOJ(const std::vector<CStudent>& students) const;
        std::vector<std::string> getOJOnlyIds(const std::vector<CStudent>& students,const std::vector<std::string> &allOJIds)const;
        //考虑到这里只需要用到字符串来判断OJ多余出的学号，因此返回vector<string>而不是student类
        CStudent* findByID(std::vector<CStudent>& students, const std::string& id);
        std::vector<CStudent*> findByName(std::vector<CStudent>& students, const std::string& name);
        std::vector<CStudent*> findByClass(std::vector<CStudent>& students,const std::string &Classname);
        //这仨就是按不同方式来找人的
        bool modifyScore(CStudent& student, const CScore& newScore, const AuthManager& auth);
        bool modifyClass(CStudent& student, const std::string& newClass, const AuthManager& auth);
        //是否有权限改
        std::vector<CStudent> getNormalStudents(const std::vector<CStudent>& all) const;                        
        //把特殊学生群体剔除出去，满足“进行成绩分析时不需要考虑重修、缓考、旷考或取消资格的学生信息。”
};      

#endif