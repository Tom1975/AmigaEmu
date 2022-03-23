#pragma once

#include <string>

class Track
{
public:
   Track();
   virtual ~Track();

   void Clear();


   unsigned char* bitstream_;
   size_t size_;
};

class Side
{
public:
   Side();
   virtual ~Side();

   void Clear();


   Track *track_;
   size_t nb_tracks_;

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
   void ClearStructure();
   size_t AddCylinderFromSectorList(Track* track, unsigned char track_number, size_t nb_sectors, size_t index, unsigned char* buffer_in, size_t buffer_size);
   int AddMFMByteToTrack(unsigned char* track, unsigned int index, unsigned short mfmbyte, int size = 16, unsigned char previous_bit = 0);
   int AddByteToTrack(unsigned char* track, unsigned int index, unsigned char byte, int size = 8, unsigned char previous_bit = 0);
   int AddLongToTrackOddEven(unsigned char* track, unsigned int index, unsigned long *data, size_t nb_data);
   template<typename T> int AddOddEven(unsigned char* track, unsigned int index, T *data, size_t nb_data);
   bool valid_;

   Side side_[2];
   size_t nb_sides_;

};
