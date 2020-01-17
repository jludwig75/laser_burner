#pragma once

#include <string>
#include <sstream>
#include <vector>

std::vector<std::string> split(const std::string &str)
{
    std::stringstream ss(str);

    std::vector<std::string> v;
    while(ss)
    {
        std::string line;
        ss >> line;
        if (line[line.length() - 1] == '\n')
        {
            line.resize(line.length() - 1);
        }
        if (!line.empty())
        {
            v.push_back(line);
        }
    }

    return v;
}

template<typename _T>
bool from_string(const std::string &str, _T &t)
{
    std::stringstream ss(str);
    ss >> t;
    return ss.good() && ss.eof();
}

template<typename _T>
std::string to_string(_T &t)
{
    std::stringstream ss;

    ss << t;
    return ss.str();
}

