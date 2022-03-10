#pragma once

#include <string>

class Disk
{
public:
   Disk();
   Disk(std::string filepath);
   virtual ~Disk();

   void LoadADF(const char* buffer, size_t size);
private: 


};
