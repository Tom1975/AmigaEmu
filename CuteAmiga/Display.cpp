#include "Display.h"

#include <QPainter>

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
   QWidget(parent)
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
   for (int i = 0; i < 1024; i++)
   {
      unsigned int * buffer = (unsigned int*)image_.scanLine(i);
      for (int j = 0; j < 1024; j++)
      {
         buffer[j] = (rand()<<15) + rand();
      }
   }
      
   

   pixmap_.convertFromImage(image_, Qt::ColorOnly);
   setUpdatesEnabled(true);
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
}

///////////////////////////////////////////////////////////////////////////
void Display::keyReleaseEvent(QKeyEvent *event_keyboard)
{
}

///////////////////////////////////////////////////////////////////////////
void Display::dragEnterEvent(QDragEnterEvent *event_drag)
{
}

///////////////////////////////////////////////////////////////////////////
void Display::dropEvent(QDropEvent* event)
{
}

void Display::AFrameIsReady()
{
   
}

void Display::resizeEvent(QResizeEvent* event)
{
}