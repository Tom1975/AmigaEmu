#pragma once

#include <map>
#include <string>

class FunctionHelper
{
public:
   static std::string GetFunctionName(std::string lib_name, short index);


private:
   static std::map<std::string, std::map<short, std::string>> library_helper_;

};
