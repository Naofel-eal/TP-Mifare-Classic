#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_connect_clicked();

    void on_btn_disconnect_clicked();

    void on_btn_quitter_clicked();

    void on_btn_ledON_clicked();

    void on_btn_ledOFF_clicked();

    void on_btn_ledONyellow_clicked();

    void on_btn_ledONred_clicked();

    void on_btn_lire_clicked();


    void on_btn_buzzer_released();

    void on_btn_buzzer_pressed();

    void on_btn_ledON1_clicked();

    void on_writeBtn_clicked();

    void on_counterSpinBox_valueChanged(int arg1);

    void on_stepSpinBox_valueChanged(int arg1);

    void on_increment_clicked();

    void on_decrement_clicked();

    void on_counterLineEdit_textChanged(const QString &arg1);

    void on_counterLineEdit_editingFinished();

private:
    void notifUser();
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
