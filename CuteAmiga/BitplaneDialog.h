#pragma once


#include <QDialog>
#include <QListWidgetItem>
#include "AmigaEmulation.h"

namespace Ui {
class BitplaneDialog;
}

class BitplaneDialog : public QDialog, IUpdate
{
    Q_OBJECT

public:
    explicit BitplaneDialog(QWidget *parent = 0);
    ~BitplaneDialog();

    // Init dialog
    void SetEmulator(AmigaEmulation* emu_handler);
    virtual bool event(QEvent *event);

    void UpdateBitmapStructure();
    void UpdateDebug();
    void UpdateBitplaneOverview();
    void UpdateBitplaneList();

public slots:
   void on_add_rasterport__clicked();
   void on_add_rasterport__returnPressed();
   void on_bitmap_list__itemSelectionChanged();
   void on_bitplanes_list__itemSelectionChanged();

    // Update the view
    virtual void Update();



protected:
   // Menu action

private:

   class bitplane_struct
   {
   public:
      unsigned short byte_per_row;
      unsigned short rows;
      unsigned char depth;
      unsigned int bitplanes[8];
   };

   Ui::BitplaneDialog *ui;
   AmigaEmulation* emu_handler_;
   QListWidgetItem * bp_tree_;

   std::vector <QListWidgetItem*> list_items_;
   bitplane_struct current_bitmap_;
};
