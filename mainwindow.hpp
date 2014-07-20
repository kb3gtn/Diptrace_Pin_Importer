#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include "pinmodel.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    // call back from user clicking the load button
    void on_pushButton_load_clicked();
    // call back from user clicking the save button
    void on_pushButton_save_clicked();
    // export CSV actaion
    void on_action_export_csv_clicked();

private:
    Ui::MainWindow *ui;

    // process to load data from csv into local data model
    void load_csv_data();
    // process to save local data model into diptrace asc
    void save_asc_data();

    // data model for the pin table
    PinModel *model;

};

#endif // MAINWINDOW_HPP
