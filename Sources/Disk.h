#pragma once

#include <string>

class Track
{
public:
   unsigned char* bitstream_;
   size_t size_;
};

class Side
{
public:
   Track track_[80];

};

class Disk
{
public:
   Disk();
   Disk(std::string filepath);
   virtual ~Disk();

   bool LoadFile(std::string filepath);
   bool LoadADF(unsigned char* buffer, size_t size);
   bool IsValid() { return valid_; }

private: 

   bool valid_;

   Side side_[2];

};
