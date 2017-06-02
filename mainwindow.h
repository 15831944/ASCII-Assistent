#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include "text.h"
#include "text_zeilenweise.h"
#include "umwandeln.h"
#include "myfunktion.h"



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
    void on_checkBox_quelldateien_erhalten_stateChanged();
    void on_lineEdit_quelle_editingFinished();
    void on_lineEdit_ziel_editingFinished();
    void on_lineEdit_prefix1_editingFinished();
    void on_lineEdit_prefix2_editingFinished();
    void on_actionInfo_triggered();
    void on_checkBox_std_namen_stateChanged();
    void on_actionWerkzeug_anzeigen_triggered();
    void on_lineEdit_ziel_ganx_editingFinished();
    void on_pushButton_ziel_ganx_clicked();
    void on_checkBox_ascii_stateChanged();
    void on_checkBox_ganx_stateChanged();

private:
    Ui::MainWindow *ui;
    QString tz; //Trennzeichen für Pfade (Linux '/'  Windows '\')
    text_zeilenweise dateien_alle;   //Alle Dateinamen mit Prefix und Dateiendung
    text_zeilenweise postfixe;       //Alle Dateinamen ohne Prefix und ohne Dateiendung
    text_zeilenweise dateien_haupt;  //Alle Dateien der Hauptseite, leere Einträge = nur Name ohne Prefix
    text_zeilenweise dateien_neben;  //Alle Dateien der Nebentseite, leere Einträge = nur Name ohne Prefix
    text_zeilenweise werkzeug;

    QString bearbeitung_auf_die_Unterseite(QString dateitext, QString prefix);
    QString namen_durch_std_namen_tauschen(QString name);

    //Pfade:
    QString verzeichnis_quelle, verzeichnis_ziel, verzeichnis_ziel_ganx;
    //Prefixe:
    QString prefix1, prefix2;

    //Checkboxen:
    QString quelldateien_erhalten;
    QString std_namen;
    QString erzeuge_ascii, erzeuge_ganx;

    void setup();
    void schreibe_ini();
    void dateien_erfassen();

};

#endif // MAINWINDOW_H
