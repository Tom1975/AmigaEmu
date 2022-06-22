#pragma once

class IDragNDropTarget
{
public:
   virtual void OpenFiles(const QStringList& pathList) = 0;
};