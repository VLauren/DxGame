#pragma once
#include <algorithm>
#include <string>

class Resource
{
public:
    std::string m_name;
    Resource(const std::string& name)
    {
        m_name = name;
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), (int(*)(int))std::tolower);
    }
};
