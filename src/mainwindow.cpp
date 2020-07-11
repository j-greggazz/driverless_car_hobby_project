#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

std::string MainWindow::getVidPath() {
	return vidPath;
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pushButton_clicked()
{
	QString file_name = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath());
	QMessageBox::information(this, "..", file_name);
	vidPath = file_name.toUtf8().constData();
	if (file_name.size() > 0) {
		close();
	}
	
}
