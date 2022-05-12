#include "Display.h"

#include <QPainter>
#include <QMimeData>


Display::Display(QWidget* parent) :
   QWidget(parent), pixmap_(1024, 1024), image_(1024, 1024, QImage::Format_RGB32)
{
   setAttribute(Qt::WA_PaintOnScreen);
   setAttribute(Qt::WA_OpaquePaintEvent);
   setAttribute(Qt::WA_NoSystemBackground);
   setFocusPolicy(Qt::StrongFocus);
   setEnabled(true);
   setAcceptDrops(true);
   Init();

   //connect(this, SIGNAL(SendRepaint(QPaintEvent*)), parent, SLOT(paintEvent(QPaintEvent*)));
   //connect(this, SIGNAL(VSync()), this, SLOT(Update()), Qt::QueuedConnection);
   connect(this, SIGNAL(Update()), this, SLOT(ForceRefresh()), Qt::QueuedConnection);
}

Display::Display(QWidget* parent, const QPoint& position, const QSize& size, unsigned int frame_time) :
   QWidget(parent), hardware_io_(nullptr)
{
   setAttribute(Qt::WA_PaintOnScreen);
   setAttribute(Qt::WA_OpaquePaintEvent);
   setAttribute(Qt::WA_NoSystemBackground);
   setFocusPolicy(Qt::StrongFocus);
   setEnabled(true);
   setAcceptDrops(true);
   move(position);
   resize(size);

   setFixedSize(1024, 1024);

   connect(this, SIGNAL(SendRepaint(QPaintEvent*)),
      this, SLOT(paintEvent(QPaintEvent*)), Qt::QueuedConnection);

   Init();
}

Display::~Display()
{
}

void Display::Init()
{
  
   pixmap_.convertFromImage(image_, Qt::ColorOnly);
   setUpdatesEnabled(true);

   index_current_line_ = 0;
   current_line_ = GetFrameBuffer(index_current_line_++);
   next_line_ = GetFrameBuffer(index_current_line_++);
   pixel_current_index_ = 0;
}

void Display::SetDragnDropTarget(IDragNDropTarget* target)
{
   target_ = target;
}

void Display::SetHardwareIO(HardwareInterface * hardware_io)
{
   hardware_io_ = hardware_io;
}

unsigned int * Display::GetFrameBuffer(unsigned int line)
{
   // extract framebuffer from pixmap
   return (unsigned int*)image_.scanLine(line);   
}



void Display::ForceRefresh()
{
   QWidget::repaint();
}

void Display::VSync()
{
   emit Update();
   AFrameIsReady();

   index_current_line_ = 0;
   current_line_ = GetFrameBuffer(index_current_line_++);
   next_line_ = GetFrameBuffer(index_current_line_++);
   pixel_current_index_ = 0;
}

void Display::HSync()
{
   pixel_current_index_ = 0;
   memcpy(next_line_, current_line_, 1024 * sizeof(unsigned int));
   current_line_ = GetFrameBuffer(index_current_line_++);
   next_line_ = GetFrameBuffer(index_current_line_++);   
}

void Display::Add16Pixels(unsigned int* pixels)
{
   memcpy(&current_line_[pixel_current_index_], pixels, 16 * sizeof(unsigned int));
   pixel_current_index_ += 16;
}

void Display::paintEvent(QPaintEvent*)
{
   QPainter painter;
   painter.begin(this);
   pixmap_.convertFromImage(image_, Qt::ColorOnly);
   painter.drawPixmap(0, 0, pixmap_);
   painter.end();
}

///////////////////////////////////////////////////////////////////////////
void Display::keyPressEvent(QKeyEvent * event_keyboard)
{
   // todo : add key 
   hardware_io_->KeyAction(0, true);
}

///////////////////////////////////////////////////////////////////////////
void Display::keyReleaseEvent(QKeyEvent *event_keyboard)
{
   // todo : add key 
   hardware_io_->KeyAction(0, false);
}

///////////////////////////////////////////////////////////////////////////
void Display::mousePressEvent(QMouseEvent* event)
{
   
   if (event->button() == Qt::LeftButton)
   {
      hardware_io_->MouseClick(0, true);
      // Capture the window
   }
   else
   {
      hardware_io_->MouseClick(1, true);
      // Capture the window

   }
}

void Display::mouseReleaseEvent(QMouseEvent* event)
{
   if (event->button() == Qt::LeftButton)
   {
      hardware_io_->MouseClick(0, false);
      // Capture the window
   }
   else
   {
      hardware_io_->MouseClick(1, false);
      // Capture the window

   }
}

///////////////////////////////////////////////////////////////////////////
void Display::dragEnterEvent(QDragEnterEvent *event_drag)
{
   event_drag->acceptProposedAction();

}

///////////////////////////////////////////////////////////////////////////
void Display::dropEvent(QDropEvent* event)
{
   // If it's a file, try to load it as a disk
   const QMimeData* mimeData = event->mimeData();

   // check for our needed mime type, here a file or a list of files
   if (mimeData->hasUrls())
   {
      QStringList pathList;
      QList<QUrl> urlList = mimeData->urls();

      // extract the local paths of the files
      for (int i = 0; i < urlList.size() && i < 32; ++i)
      {
         pathList.append(urlList.at(i).toLocalFile());
      }

      // call a function to open the files
      target_->OpenFiles(pathList);
   }

}

void Display::AFrameIsReady()
{
   
}

void Display::resizeEvent(QResizeEvent* event)
{
}
