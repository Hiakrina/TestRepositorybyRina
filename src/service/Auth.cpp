#include "service/Auth.h"

AuthManager::AuthManager()
    : m_current(nullptr), m_tempUser(nullptr)
{
    m_users.push_back(new AdminUser("admin21", "123456", "", ""));
    m_users.push_back(new TeacherUser("2021110001", "123456", "2021110001", "智能1304"));
    //临时账户，包含老师*1，管理员*1；
}

AuthManager::~AuthManager() {
    for (size_t i = 0; i < m_users.size(); ++i) {
        delete m_users[i];
    }
    if (m_tempUser != nullptr) {
        delete m_tempUser;
        m_tempUser = nullptr;
    }
}

bool AuthManager::login(const std::string& username, const std::string& passwd) {
    // ".size"用于看用户账户数量，进行遍历
    for (int i = 0; i < m_users.size(); i++) {
        //验证账号密码 
        if (m_users[i]->username == username && m_users[i]->password == passwd) {
            m_current = m_users[i];
            return true;
        }
    }
    //用户名（学号）第5位为"0"，确认是学生,允许学生使用临时密码123456登录
    if (username.length() >= 5 && username[4] == '0' && passwd == "123456") {
        if (m_tempUser != nullptr) 
        delete m_tempUser;
        m_tempUser = new StudentUser(username, passwd, username, "");
        m_current = m_tempUser;
        return true;
    }
    return false;
}

const User* AuthManager::currentUser() const {
    return m_current;
}

bool AuthManager::CanViewAll() const {
    if (!m_current) 
    return false;  /* 未登录时无权限，下同
    排除掉学生（就学生权限低这一块），教师和管理员可以看全部 */
    return m_current->canViewAll();
}

bool AuthManager::CanChangeAll() const {
    if (!m_current) return false;
    return m_current->canChangeAll();
}

bool AuthManager::IsOwner(const CStudent& student) const {
    if (!m_current) 
    return false;
    //这个函数我是想留着最后改名字啥的时候用的，确认是自己的账号好改，不然让别人改了就不好了;）
    //(P.S.)好像bug有点小多来不及加了orz ,先这样凑活着用吧
    return m_current->isOwner(student);
}

bool AuthManager::ChangeOnesScore(const CStudent& student) const {
    if (!m_current) 
    return false;
    /* 其实乍一看有点晕，我写的时候也晕
       首先，如果老师管理班级没写的话是没有管理改分权限的
       其次，老师只能改自己班级的分数 */
    return m_current->canModifyScore(student);
}