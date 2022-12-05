#pragma once

#include <map>
#include <string>

class FunctionHelper
{
public:
   static std::string GetFunctionName(std::string lib_name, short index);
   static std::string GetRegisterName(unsigned short);

private:
   static std::map<std::string, std::map<short, std::string>> library_helper_;
   static std::map<unsigned short, std::string> register_name_;

};
