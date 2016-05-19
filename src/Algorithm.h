#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <string>

class Algorithm
{
public:
    Algorithm() {};

    virtual bool Run();
    std::string GetName() const { return m_sName; }

protected:
    std::string m_sName;
};

#endif //ALGORITHM_H
