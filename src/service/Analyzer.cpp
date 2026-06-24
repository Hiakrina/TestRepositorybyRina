#include "service/Analyzer.h"

std::vector<CStudent> Analyzer::getInsufficientSolved(const std::vector<CStudent>& students,int totalProblems) const{
    std::vector<CStudent> result;
    for(int i = 0;i < students.size();i++){
        if(students[i].hasOJRecord()){
            double ratio = static_cast<double>(students[i].getOJSolved()) / totalProblems;
            if (ratio < 0.6){
                result.push_back(students[i]);
            }
        }
    }
    return result;
}
std::vector<CStudent> Analyzer::getStudentsNotInOJ(const std::vector<CStudent>& students) const{
    std::vector<CStudent> result;
    for(int i = 0;i<students.size();i++){
        if(!students[i].hasOJRecord()){
            //这里和上边不一样的是这里用了！hasOJrecord，本来我想和上边那个函数合并的，想了想太麻烦，算了（
            result.push_back(students[i]);
        }
    }
    return result;
}
std::vector<std::string> Analyzer::getOJOnlyIds(const std::vector<CStudent>& students,const std::vector<std::string> &allOJIds)const{
    std::vector<std::string> result;
    for(int i = 0; i < allOJIds.size(); i++){
        std::string OJID = allOJIds[i];
        bool found = false;
        for(int j = 0;j < students.size();j++){
            if(students[j].getStudentID()==allOJIds[i]){
                found = true;
                break;
            }
        }
        if (!found){
            result.push_back(OJID);
        }
    }
    return result;
}
CStudent* Analyzer::findByID(std::vector<CStudent>& students, const std::string& id){
    for(int i = 0;i<students.size();i++){
            if(students[i].getStudentID()==id){
                return &students[i];
            }
    }
    return nullptr;
}
std::vector<CStudent*> Analyzer::findByName(std::vector<CStudent>& students, const std::string& name){
    std::vector<CStudent*> result;
    for(int i = 0;i< students.size();i++){
        if(students[i].getName() == name){
            result.push_back(&students[i]);
        }
    }
    return result;
}
std::vector<CStudent*> Analyzer::findByClass(std::vector<CStudent>& students, const std::string &Name){
    std::vector<CStudent*> result;
    for(int i = 0;i<students.size();i++){
        if(students[i].getClassAfter() == Name || students[i].getClassBefore() == Name){
        //这里本来想直接用after的数据，（代码给ai润色过后）考虑到有可能分班之后班级没有就增加了原班级查找了  
            result.push_back(&students[i]);
        }
    }
    return result;
}
bool Analyzer::modifyScore(CStudent& student, const CScore& newScore, const AuthManager& auth){
        if (newScore.performance < 0 || newScore.performance > 100 ||
        newScore.exam < 0 || newScore.exam > 100 ||
        newScore.practice < 0 || newScore.practice > 100) {
        return false;
    }
        if (auth.ChangeOnesScore(student)){
        student.setScore(newScore);
        return true;
    }
    return false;
}
bool Analyzer::modifyClass(CStudent& student, const std::string& newClass, const AuthManager& auth){
        if(auth.CanChangeAll()){
            student.setClassAfter(newClass);
            return true;
        }
        return false;
}
std::vector<CStudent> Analyzer::getNormalStudents(const std::vector<CStudent>& all) const{
    std::vector<CStudent> result;
    for (int i = 0; i < all.size(); i++) {
        if (!all[i].isExcluded() && all[i].isNormal()) {
        /*isExcluded 是手动标记，可能漏掉缓考/旷考等未标记但状态异常的学生；
        isNormal 直接看 examStatus 是否为空*/
            result.push_back(all[i]);
        }
    }
    return result;
}      
