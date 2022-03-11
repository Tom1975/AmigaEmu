#pragma once

#include <qwidget.h>
#include <qevent.h>
#include "Monitor.h"

namespace Ui {
class Display;
}

class Display : public QWidget, public DisplayFrame
{
   Q_OBJECT

public:
   Display(QWidget* parent, const QPoint& position, const QSize& size, unsigned int frame_time = 0);
   Display(QWidget* parent);
    ~Display();

    virtual unsigned int * GetFrameBuffer(unsigned int line);

    virtual void HSync();
    virtual void VSync();
    virtual void Add16Pixels(unsigned int*);

    virtual void AFrameIsReady();
    void Init();

    // Display
    virtual void resizeEvent(QResizeEvent* event);

    // Keyboard
    virtual void keyPressEvent(QKeyEvent * event_keyboard);
    void keyReleaseEvent(QKeyEvent *event_keyboard);

    // Drag'n'drop
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

public slots:
    void paintEvent(QPaintEvent*);
    void ForceRefresh();

signals:
   void Update();
   

private:
   void OpenFiles(const QStringList& pathList);


   QPixmap pixmap_;
   QImage image_;

   // Pixel/line handling
   unsigned int index_current_line_;
   unsigned int * current_line_;
   unsigned int pixel_current_index_;
};
