#ifndef CSTUDENT_H
#define CSTUDENT_H

#include <string>
#include "CScore.h"

class CStudent {
public:
    CStudent();
    CStudent(const std::string& id, const std::string& name, const std::string& clsbefore,const std::string& clsafter);

    const std::string& getStudentID() const;
    void setStudentID(const std::string& id);

    const std::string& getName() const;
    void setName(const std::string& name);

    const std::string& getClassBefore() const;
    void setClassBefore(const std::string& clsbefore);

    const std::string& getClassAfter() const;
    void setClassAfter(const std::string& clsafter);

    const std::string& getExamStatus() const;
    void setExamStatus(const std::string& status);

    const CScore& getScore() const;
    CScore& getScore();
    void setScore(const CScore& score);

    const std::string& getOJAccount() const;
    void setOJAccount(const std::string& account);

    int getOJSolved() const;
    void setOJSolved(int solved);

    bool isExcluded() const;
    void setExcluded(bool excluded);

    bool isNormal() const;
    bool hasOJRecord() const;

private:
    std::string m_name;
    std::string m_id;
    std::string m_classBefore;
    std::string m_classAfter;
    std::string m_examStatus;
    CScore m_score;
    std::string m_ojAccount;
    int m_ojSolved = -1;
    bool m_excluded = false;
    /*      
    成员变量之间添加了"m_"表示member（成员变量），方便后续用“=”赋值；
    OJSolved设为-1用于区分缺考和全错；
    excluded用于区分是否因（重修、缓考、旷考、取消资格）来额外考量        
    */
};

#endif
