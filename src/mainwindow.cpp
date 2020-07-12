#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::string MainWindow::getVidPath()
{
	return vidPath;
}

Ui::MainWindow MainWindow::getUI()
{
	return *ui;
}

void MainWindow::on_selectVideo_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath());
    QMessageBox::information(this, "Chosen File", file_name);
    vidPath = file_name.toUtf8().constData();
	ui->path_valid_checkbox->setChecked(true);
	setStyleSheet("background-image: url(:../data/beach.jpg);");
}

void MainWindow::on_confirm_button_clicked()
{
    if ((vidPath.size()) > 0 & (ui->path_valid_checkbox->isChecked()) ) {
            close();
    }
    else{
        QMessageBox::information(this, "Status", "Video still not selected");
    }
}

void MainWindow::on_path_valid_checkbox_stateChanged(int arg1)
{

}

void MainWindow::on_cancel_button_clicked()
{
    QMessageBox::information(this, "Status", "Program aborted.");
	abort = true;
	close();
}

bool MainWindow::getAbort()
{
	return abort;
}
