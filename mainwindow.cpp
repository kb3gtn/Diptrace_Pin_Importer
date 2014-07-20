#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new PinModel(this);
    // connect model data changed to view update data..
    QObject::connect( model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                      ui->tableView, SLOT(dataChanged(QModelIndex,QModelIndex)));
    ui->tableView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Throw up a qfiledialog and get a .csv file from the user.
// perform .csv to data model conversion.
void MainWindow::on_pushButton_load_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open CSV File"), "", tr("CSV (*.csv)"));
    PinModel *newmodel = new PinModel(this);
    PinModel *oldmodel = model;
    newmodel->load_from_csv(fileName);
    // now we can set the model to use the datamodel we have loaded
    ui->lineEdit->setText(fileName);
    model = newmodel;
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
    delete oldmodel;
}

// Throw up a qfiledialog and get a .asc file to save our data too.
// perform data model to Diptrace ASC format conversion.
void MainWindow::on_pushButton_save_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("SAVE ASC File"), "", tr("ASC (*.asc)"));
    model->save_to_asc(fileName);
}

void MainWindow::on_action_export_csv_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save CSV File"), "", tr("CSV (*.csv)"));
    model->save_to_csv(fileName);
}

