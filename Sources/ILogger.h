#pragma once

class ILogger
{
public:
   enum class Severity
   {
      SEV_DEBUG = 0,
      SEV_INFO,
      SEV_WARNING,
      SEV_ERROR,
      SEV_CRITICAL
   };

   virtual void Log(Severity severity, const char* msg...) = 0;
};

