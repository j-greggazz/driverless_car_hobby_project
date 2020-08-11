#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
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
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "Confirm quit", "Are you sure you want to quit?",
		QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes) {
		qDebug() << "Yes was clicked";
		QMessageBox::information(this, "Status", "Program aborted.");
		abort = true;
		close();
		//QApplication::quit();
	}
	else {
		qDebug() << "Yes was *not* clicked";
	}
    
}

bool MainWindow::getAbort()
{
	return abort;
}
