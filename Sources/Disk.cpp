#include "Disk.h"

Disk::Disk() : valid_(false)
{

}
Disk::Disk(std::string filepath)
{
   valid_ = LoadFile(filepath);

}

Disk::~Disk()
{

}

bool Disk::LoadFile(std::string filepath)
{
   bool LoadOk = false;
   // todo : add various format support
   FILE *f;
   if (fopen_s(&f, filepath.c_str(), "rb") == 0)
   {
      fseek(f, 0, SEEK_END);
      const size_t size = ftell(f);
      rewind(f);

      auto* filebuffer = new unsigned char[size];
      if (fread(filebuffer, size, 1, f) == 1)
      {
         // Handle buffer
         LoadOk = LoadADF(filebuffer, size);
      }

      fclose(f);
   }

   return LoadOk;
}



bool Disk::LoadADF(unsigned char* buffer, size_t size)
{
   // Load ADF :
   // - Check size
   // - Size ok, load each track/side



   return true;
}