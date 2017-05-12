#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include "text.h"
#include "text_zeilenweise.h"

#ifndef INIFILE
#define INIFILE "ascii_assistent.ini"
#endif

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
    void on_pushButton_quelle_clicked();
    void on_pushButton_ziel_clicked();
    void on_pushButton_auflisten_clicked();
    void on_pushButton_Start_clicked();
    void on_checkBox_quelldateien_erhalten_stateChanged(int arg1);
    void on_lineEdit_quelle_editingFinished();
    void on_lineEdit_ziel_editingFinished();
    void on_lineEdit_prefix1_editingFinished();
    void on_lineEdit_prefix2_editingFinished();
    void on_actionInfo_triggered();

private:
    Ui::MainWindow *ui;
    QString tz; //Trennzeichen für Pfade (Linux '/'  Windows '\')
    text_zeilenweise dateien_haupt, dateien_neben, dateien_alle, postfixe;

    //Pfade:
    QString verzeichnis_quelle, verzeichnis_ziel;
    //Prefixe:
    QString prefix1, prefix2;

    //Checkboxen:
    QString quelldateien_erhalten;

    void setup();
    void schreibe_ini();
    void dateien_erfassen();

};

#endif // MAINWINDOW_H
