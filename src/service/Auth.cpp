#include "service/Auth.h"

AuthManager::AuthManager() {
    m_users.push_back(User{"admin21", "123456", UserRole::Admin, "", ""});
    m_users.push_back(User{"2021110001", "123456", UserRole::Teacher, "智能1304", "2021110001"});
}
bool AuthManager::login(const std::string& username, const std::string& passwd) {
    // .size用于看用户账户数量，进行遍历
    for (int i = 0; i < m_users.size(); i++) {                      //遍历验证账密
        if (m_users[i].m_username == username && m_users[i].m_password == passwd) {
            m_current = &m_users[i];
            return true;
        }
    }
    // 用户名（学号）第5位为"0"，确认是学生,允许学生使用临时密码123456登录。
    if (username.length() >= 5 && username[4] == '0' && passwd == "123456") {
        m_tempUser = User{username, passwd, UserRole::Student, "", username};
        m_current = &m_tempUser;
        return true;
    }
    return false;
}

const User* AuthManager::currentUser() const {
    return m_current;
}

bool AuthManager::CanViewAll() const {
    if (!m_current) return false;  // 未登录时无权限，下同
    // 排除掉学生（就学生权限低这一块），教师和管理员可以看全部
    if (m_current->role != UserRole::Student) {
        return true;
    }
    return false;
}

bool AuthManager::CanChangeAll() const {
    if (!m_current) return false;
    if (m_current->role == UserRole::Admin) {
        return true;
    }
    return false;
}

bool AuthManager::IsOwner(const CStudent& student) const {
    if (!m_current) return false;
    if (m_current->role == UserRole::Student) {
        // 这个函数我是想留着最后改名字啥的时候用的，确认是自己的账号好改，不然让别人改了就不好了;）
        if (student.getStudentID() == m_current->m_ID) {
            return true;
        }
    }
    return false;
}

bool AuthManager::ChangeOnesScore(const CStudent& student) const {
    if (!m_current) return false;

    if (m_current->role == UserRole::Admin) {
        return true;
    }
    else if (m_current->role == UserRole::Teacher) {
        /* 其实乍一看有点晕，我写的时候也晕
           首先，如果老师管理班级没写的话是没有管理改分权限的
           其次，老师只能改自己班级的分数 */
        return (!m_current->m_manageClass.empty()) &&
               (m_current->m_manageClass == student.getClassAfter());
    }
    return false;
}