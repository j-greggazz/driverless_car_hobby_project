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
	Ui::MainWindow getUI();
	bool getAbort();

private slots:
    void on_selectVideo_clicked();

    void on_confirm_button_clicked();

    void on_path_valid_checkbox_stateChanged(int arg1);

    void on_cancel_button_clicked();

	

private:
    Ui::MainWindow *ui;
    std::string vidPath;
	bool abort = false;
};

#endif // MAINWINDOW_H
