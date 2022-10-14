#ifndef _STRING_H
#define _STRING_H

#include <string>
#include <vector>

typedef std::vector<std::string> StringVector;

StringVector split(std::string str,std::string const& separator){
    StringVector contents;
    std::string::size_type location=str.find(separator);

    while(location != std::string::npos){
        contents.emplace_back(str.substr(0,location));
        str=str.substr(location+separator.length());
        location=str.find(separator);
    }
    if(!str.empty()) contents.emplace_back(str);

    return contents;
}
#endif