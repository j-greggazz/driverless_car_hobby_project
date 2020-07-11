#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
	std::string getVidPath();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
	std::string vidPath;
};

#endif // MAINWINDOW_H
