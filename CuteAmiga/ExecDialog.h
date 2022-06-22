#pragma once


#include <QDialog>
#include <QTreeWidgetItem>
#include "AmigaEmulation.h"

namespace Ui {
class ExecDialog;
}

class ExecDialog : public QDialog, IUpdate
{
    Q_OBJECT

public:
    explicit ExecDialog(QWidget *parent = 0);
    ~ExecDialog();

    // Init dialog
    void SetEmulator(AmigaEmulation* emu_handler);
    virtual bool event(QEvent *event);
    void Break();

   public slots:

    // Update the view
    virtual void Update();

protected:
   // Menu action
   void UpdateDebug();
   void AddTask(unsigned long task_adress, QTreeWidgetItem * root_item, void(*list_handler)(unsigned long, QTreeWidgetItem*, std::vector<QTreeWidgetItem*> *));
   void UpdateList(unsigned long list_adress, QTreeWidgetItem * root_item, void(*list_handler)(unsigned long, QTreeWidgetItem*, std::vector<QTreeWidgetItem*> *) = nullptr);
   static void UpdateTask(unsigned long task_adress, QTreeWidgetItem* base_item, std::vector<QTreeWidgetItem*> *list_items);

private:

    Ui::ExecDialog *ui;
    AmigaEmulation* emu_handler_;

    QTreeWidgetItem * exec_base_item_;
    std::vector<QTreeWidgetItem*> list_items_;

};
