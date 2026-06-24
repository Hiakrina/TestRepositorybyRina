#ifndef AUTH_H
#define AUTH_H

#include "model/CStudent.h"
#include <string>
#include <vector>

/*
    权限体系基于继承与多态。
    StudentUser、TeacherUser、AdminUser 分别实现具体的权限逻辑，
    实现"学号第5位特征码决定身份"的设计思想。
    身份区分通过虚函数 canViewAll()/canChangeAll() 实现：
      学生:   canViewAll()=false, canChangeAll()=false
      教师:   canViewAll()=true,  canChangeAll()=false
      管理员: canViewAll()=true,  canChangeAll()=true
*/
class User {
public:
    std::string username;
    std::string password;
    std::string ID;
    std::string manageClass; //管理班级（教师用）

    User(const std::string& uname, const std::string& pwd,
         const std::string& id, const std::string& mcls = "")
        : username(uname), password(pwd), ID(id), manageClass(mcls) {}

    virtual ~User() {}

    virtual bool canViewAll() const = 0;
    virtual bool canChangeAll() const = 0;
    virtual bool canModifyScore(const CStudent& student) const = 0;
    virtual bool isOwner(const CStudent& student) const = 0;
};
class StudentUser : public User {
public:
    using User::User;
    bool canViewAll() const { 
        return false; 
    }
    bool canChangeAll() const { 
        return false; 
    }
    bool canModifyScore(const CStudent& student) const { 
        return false; 
    }
    bool isOwner(const CStudent& student) const {
        return student.getStudentID() == ID;
    }
};
class TeacherUser : public User {
public:
    using User::User;
    bool canViewAll() const {
         return true; 
        }
    bool canChangeAll() const { 
        return false; 
    }
    bool canModifyScore(const CStudent& student) const {

        if (manageClass.empty()) 
        return true;
        /*
           教师管辖逻辑：
           manageClass 为空时允许修改所有学生（super big teacher）
           否则只允许修改与 manageClass 匹配的班级学生。
        */
        return student.getClassAfter() == manageClass;
    }
    bool isOwner(const CStudent& student) const { 
        return false; 
    }
};
class AdminUser : public User {
public:
    using User::User;
    bool canViewAll() const { 
        return true; 
    }
    bool canChangeAll() const { 
        return true; 
    }
    bool canModifyScore(const CStudent& student) const { 
        return true; 
    }
    bool isOwner(const CStudent& student) const { 
        return false; 
    }
};

class AuthManager {
private:
    std::vector<User*> m_users; 
    const User* m_current;          // 当前登录用户 
    User* m_tempUser;               //暂时登录的假定用户，有点游客登录的意思？

public:
    AuthManager();
    ~AuthManager();

    /*
       登录验证：先在预设用户列表中查找，若未找到且学号第5位为 '0'（学生），
       则创建临时学生用户（游客）并登录。密码为固定的 123456。
    */
    bool login(const std::string& username, const std::string& passwd);

    const User* currentUser() const;

    // 权限查询
    bool CanViewAll() const;
    bool CanChangeAll() const;
    bool IsOwner(const CStudent& student) const;
    bool ChangeOnesScore(const CStudent& student) const;
};

#endif