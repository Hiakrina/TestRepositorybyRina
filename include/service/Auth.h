#ifndef AUTH_H
#define AUTH_H
#include "model/CStudent.h"
#include <string>
#include <vector>
/*
    这边我的思路是根据学号中间2位“20XX"01"XXXX”来区分各个操作系统的人的身份，include cstudent.h
    这边假定学生是01，老师是11，管理21
*/
enum class UserRole{
    Student,
    Teacher,
    Admin
};
struct User{
    std::string m_username;
    std::string m_password;
    UserRole role;
    std::string m_manageClass;        
    std::string m_ID;                 //学号或工号
};
class AuthManager{
private:
    std::vector<User> m_users;
    const User* m_current =nullptr;       //m_current来确定当前的用户状态
    User m_tempUser;
public:
    AuthManager();
    bool login(const std::string& username,const std::string& passwd);
    const User* currentUser() const;
    bool CanViewAll() const;    //(teacher/admin only)
    bool CanChangeAll() const;//能否全部改动（管理员）
    bool IsOwner(const CStudent& student) const;//是否为该student账号
    bool ChangeOnesScore(const CStudent& student) const;//是否能改动某学生的成绩
};
#endif