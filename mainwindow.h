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
    void on_actionInfo_triggered();
    void on_checkBox_std_namen_stateChanged();
    void on_actionWerkzeug_anzeigen_triggered();
    void on_lineEdit_ziel_ganx_editingFinished();
    void on_pushButton_ziel_ganx_clicked();
    void on_checkBox_ascii_stateChanged();
    void on_checkBox_ganx_stateChanged();
    void on_radioButton_drehung_0_toggled(bool checked);
    void on_radioButton_drehung_90_toggled(bool checked);
    void on_radioButton_drehung_180_toggled(bool checked);
    void on_radioButton_drehung_270_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QString tz; //Trennzeichen für Pfade (Linux '/'  Windows '\')
    text_zeilenweise dateien_alle, postfixe;   //Alle Dateinamen mit Prefix und Dateiendung
    text_zeilenweise werkzeug;

    //QString bearbeitung_auf_die_Unterseite(QString dateitext, QString prefix);
    QString namen_durch_std_namen_tauschen(QString name);
    QString ascii_umwandeln_in_ganx(QString asciitext);
    text_zeilenweise ascii_optimieren(text_zeilenweise dateiinhalt);
    QString ascii_optimieren(QString dateiinhalt);
    text_zeilenweise bauteile_drehen(text_zeilenweise dateiinhalt);
    QString bauteile_drehen(QString dateiinhalt);
    text_zeilenweise bauteile_drehen_90Grad(text_zeilenweise dateiinhalt);
    void warnungen_ganx_in_mb_ausgeben(QString dateiinhalt_ascii);
    QString ima_makros_umwandeln(QString dateiinhalt);

    //Pfade:
    QString verzeichnis_quelle, verzeichnis_ziel, verzeichnis_ziel_ganx;
    //Prefixe:
    QString prefix1;

    //Checkboxen:
    QString quelldateien_erhalten;
    QString std_namen;
    QString erzeuge_ascii, erzeuge_ganx;

    //Radio Buttons:
    QString drehung_des_bauteils;


    //Variablen:
    double bezugsmass;

    //Funktionen:
    void setup();
    void schreibe_ini();
    void dateien_erfassen();
    QString get_wkz_nummer(QString wkz_typ, double dm = 0, double bearbeitungstiefe = 0);
    QString get_wkz_dm(QString wkz_nr);
    QString get_wkz_vorschub(QString wkz_nr);

};

#endif // MAINWINDOW_H
