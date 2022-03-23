#include "Disk.h"


///////////////////////////////////////////////////////////
// Track
Track::Track():bitstream_(nullptr), size_(0)
{

}

Track::~Track()
{
   Clear();
}

void Track::Clear()
{
   delete[]bitstream_;
   bitstream_ = nullptr;
   size_ = 0;
}


///////////////////////////////////////////////////////////
// Side
Side::Side():nb_tracks_(0), track_(nullptr)
{

}

Side::~Side()
{
   Clear();
}

void Side::Clear()
{
   for (size_t i = 0; i < nb_tracks_; i++)
   {
      track_[i].Clear();
   }
   delete[]track_;
}


///////////////////////////////////////////////////////////
// Disk
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

void Disk::ClearStructure()
{
   side_[0].Clear();
   side_[1].Clear();
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


int Disk::AddMFMByteToTrack(unsigned char* track, unsigned int index, unsigned short mfmbyte, int size,
   unsigned char previous_bit)
{
   if (track == nullptr)
      return index + (size );

   if (index > 0)
   {
      previous_bit = track[index - 1];
   }

   for (int i = 0; i < size; i++)
   {
      // Get bit
      unsigned char c = ((mfmbyte >> (size - 1 - i)) & 0x01);

      track[index++] = c;
      previous_bit = c;
   }
   return index;
}

int Disk::AddByteToTrack(unsigned char* track, unsigned int index, unsigned char byte, int size,
   unsigned char previous_bit)
{
   if (track == nullptr)
      return index + (size * 2);

   if (index > 0)
   {
      previous_bit = track[index - 1];
   }

   for (int i = 0; i < size; i++)
   {
      // Get bit
      unsigned char c = ((byte >> (7 - i)) & 0x01);

      if (c == 1)
      {
         track[index++] = 0;
         track[index] = 1;
      }
      else
      {
         track[index++] = (previous_bit == 0) ? 1 : 0;
         track[index] = 0;
      }

      previous_bit = track[index++];
   }
   return index;
}

int Disk::AddLongToTrackOddEven(unsigned char* track, unsigned int index, unsigned long *data, size_t nb_data)
{
   if (track == nullptr)
      return index + 64;

   unsigned char previous_bit = 0;
   if (index > 0)
   {
      previous_bit = track[index - 1];
   }

   for (int j = 0; j < 2; j++)
   {
      for (size_t k = 0; k < nb_data; k++)
      {
         for (int i = 0; i < 16; i++)
         {
            // Get bit
            unsigned char c = ((data[k] >> (31 - (i * 2) - j)) & 0x01);

            if (c == 1)
            {
               track[index++] = 0;
               track[index] = 1;
            }
            else
            {
               track[index++] = (previous_bit == 0) ? 1 : 0;
               track[index] = 0;
            }

            previous_bit = track[index++];
         }
      }
   }
   return index;
}

template<typename T> int Disk::AddOddEven(unsigned char* track, unsigned int index, T *data, size_t nb_data)
{
   if (track == nullptr)
      return index + sizeof(T)*2* nb_data;

   unsigned char previous_bit = 0;
   if (index > 0)
   {
      previous_bit = track[index - 1];
   }

   for (int j = 0; j < 2; j++)
   {
      for (size_t k = 0; k < nb_data; k++)
      {
         for (int i = 0; i < sizeof(T)*4; i++)
         {
            // Get bit
            unsigned char c = ((data[k] >> ((sizeof(T)*8-1) - (i * 2) - j)) & 0x01);

            if (c == 1)
            {
               track[index++] = 0;
               track[index] = 1;
            }
            else
            {
               track[index++] = (previous_bit == 0) ? 1 : 0;
               track[index] = 0;
            }

            previous_bit = track[index++];
         }
      }
   }
   return index;
}

size_t Disk::AddCylinderFromSectorList(Track* track, unsigned char track_number, size_t nb_sectors, size_t index, unsigned char* buffer_in, size_t buffer_size)
{
   track->size_ = 0x19000;
   track->bitstream_ = new unsigned char[track->size_];

   // Fill track with GAP
   AddMFMByteToTrack(track->bitstream_, 0, 0xE5E5);
 
   // add sectors
   size_t stream_index = 0;
   size_t stream_in = 0;
   for (size_t s = 0; s < nb_sectors; s++)
   {
      // SYNC bits : 0xAAAAAAAA
      stream_index = AddMFMByteToTrack(track->bitstream_, stream_index, 0xAAAA);
      stream_index = AddMFMByteToTrack(track->bitstream_, stream_index, 0xAAAA);
      // SYNC Word : 0x4489 0x4489
      stream_index = AddMFMByteToTrack(track->bitstream_, stream_index, 0x4489);
      stream_index = AddMFMByteToTrack(track->bitstream_, stream_index, 0x4489);

      // HEADER : 
      size_t stream_header = stream_index;
      unsigned long header_info = 0xFF0000 | (track_number << 16) | ((s & 0xFF) << 8) | ((nb_sectors - s) & 0xFF);
      stream_index = AddOddEven<unsigned long>(track->bitstream_, stream_index, &header_info, 1);

      // Label
      unsigned long label[] = { 0x0, 0x0 , 0x0 , 0x0 };
      stream_index = AddOddEven<unsigned long>(track->bitstream_, stream_index, label, 4);

      // Checksum
      unsigned long checksum = 0;

      for (size_t i = 0; i < 10; i++) 
      {
         unsigned long value = 0;
         for (int b = 0; b < 32; b++)
         {
            value |= (track->bitstream_[stream_header + i * 32 + b])<<b;
         }
         checksum ^= value;
      }
      stream_index = AddOddEven<unsigned long>(track->bitstream_, stream_index, &checksum, 1);

      // Data checksum
      size_t data_checksum_index = stream_index;
      checksum = 0;
      stream_index = AddOddEven<unsigned long>(track->bitstream_, stream_index, &checksum, 1);

      // Data
      stream_index = AddOddEven<unsigned char>(track->bitstream_, stream_index, &buffer_in[stream_in], 512);
      stream_in += 512;

      // Compute Data checksum
      checksum = 0;
      for (size_t i = 0; i < 1024/4; i++)
      {
         unsigned long value = 0;
         for (int b = 0; b < 32; b++)
         {
            value |= (track->bitstream_[stream_header + i * 32 + b]) << b;
         }
         checksum ^= value;
      }
      AddOddEven<unsigned long>(track->bitstream_, data_checksum_index, &checksum, 1);
   }

   return stream_index;
}



bool Disk::LoadADF(unsigned char* buffer, size_t size)
{
   // Load ADF :
   // - Check size
   size_t nb_sectors_total = size / 512;

   // - Size ok, load each track/side
   // todo : handle HD
   size_t nb_sectors = 11;
   size_t nb_tracks = nb_sectors_total / 11;
   nb_sides_ = (nb_tracks > 80) ? 2 : 1;
   size_t nb_cylinders;
   if (nb_sides_ == 1)
   {
      nb_cylinders = nb_tracks;
   }
   else
   {
      nb_cylinders = nb_tracks / 2;
   }

   // Create structure.
   ClearStructure();

   for (size_t s = 0; s < nb_sides_; s++)
   {
      side_[s].nb_tracks_ = nb_cylinders;
      side_[s].track_ = new Track[nb_cylinders];
   }

   size_t index = 0;
   for (size_t c = 0; c < nb_cylinders; c++)
   {
      for (size_t s = 0; s < nb_sides_; s++)
      {
         index = AddCylinderFromSectorList(&side_[s].track_[c], (c*2+s), nb_sectors, index, buffer, size);
         side_[s].track_[c].bitstream_ = new unsigned char[512];
         side_[s].track_[c].size_ = 512;
      }
   }


   return true;
}
