#include "model/CStudent.h"
#include <string>
    CStudent::CStudent(){}
    CStudent::CStudent(const std::string& id, const std::string& name, const std::string& clsbefore, const std::string& clsafter){
        m_id = id;
        m_name = name;
        m_classBefore = clsbefore;
        m_classAfter = clsafter;
    }

    const std::string& CStudent::getStudentID() const{
        return m_id;
    }
    void CStudent::setStudentID(const std::string& id){
        m_id = id;
    }

    const std::string& CStudent::getName() const{
        return m_name;
    }
    void CStudent::setName(const std::string& name){
        m_name = name;
    }

    const std::string& CStudent::getClassBefore() const{
        return m_classBefore;
    }
    void CStudent::setClassBefore(const std::string& clsbefore){
        m_classBefore = clsbefore;
    }
    const std::string& CStudent::getClassAfter() const{
        return m_classAfter;
    }
    void CStudent::setClassAfter(const std::string& clsafter){
        m_classAfter = clsafter;
    }
    const std::string& CStudent::getExamStatus() const{
        return m_examStatus;
    }
    void CStudent::setExamStatus(const std::string& status){
        m_examStatus = status;
    }

    const CScore& CStudent::getScore() const{
        return m_score;
    }
    CScore& CStudent::getScore(){
        return m_score;
    }
    void CStudent::setScore(const CScore& score){
    if (score.performance < 0 || score.performance > 100 ||
        score.exam < 0 || score.exam > 100 ||
        score.practice < 0 || score.practice > 100) {
        return;
    }
        m_score = score;
    }

    const std::string& CStudent::getOJAccount() const{
        return m_ojAccount;
    }
    void CStudent::setOJAccount(const std::string& account){
        m_ojAccount = account;
    }

    int CStudent::getOJSolved() const{
        return m_ojSolved;
    }
    void CStudent::setOJSolved(int solved){
        m_ojSolved = solved;
    }

    bool CStudent::isExcluded() const{
        return m_excluded;
    }
    void CStudent::setExcluded(bool excluded){
        m_excluded = excluded;
    }

    bool CStudent::isNormal() const{
        return m_examStatus.empty();            //判断状态是否为空
    }
    bool CStudent::hasOJRecord() const{
        return m_ojSolved>=0;
    }