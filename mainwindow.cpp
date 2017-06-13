#include "mainwindow.h"
#include "ui_mainwindow.h"

#define VERSION "2017.06.13"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    verzeichnis_quelle  = "";
    verzeichnis_ziel    = "";
    tz = QDir::separator(); //Systemspezifischer Separator (Linux: Ordner/Unterordner/...)
    setup();
    on_actionInfo_triggered();
    QPixmap bild("cnc.png");
    ui->label_bild->setPixmap(bild);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setup()
{
    bezugsmass = 40;
    bool inifile_gefunden = false;
    bool werkzeugdatei_gefunden = false;
    QDir programmordner("./");
    QStringList ordnerinhalt;
    ordnerinhalt = programmordner.entryList(QDir::Files);
    for(QStringList::iterator it = ordnerinhalt.begin() ; it!=ordnerinhalt.end() ; ++it)
    {
        QString name = *it;
        if(name.contains(INIFILE))
        {
            inifile_gefunden = true;
        }
        if(name.contains(WERKZEUGDATEI))
        {
            werkzeugdatei_gefunden = true;
        }
    }
    if(inifile_gefunden == false)
    {
        QFile file(INIFILE);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this,"Fehler","Fehler beim Dateizugriff!",QMessageBox::Ok);
        }else
        {
            file.write("verzeichnis_quelle:");
            file.write("\n");

            file.write("verzeichnis_ziel:");
            file.write("\n");

            file.write("prefix1:");
            file.write("_Haupt.ascii");
            file.write("\n");
            prefix1 = "_Haupt.ascii";
            ui->lineEdit_prefix1->setText(prefix1);

            file.write("prefix2:");
            file.write("_Neben.ascii");
            file.write("\n");
            prefix2 = "_Neben.ascii";
            ui->lineEdit_prefix2->setText(prefix2);

            ui->checkBox_quelldateien_erhalten->setChecked(true);
            file.write("quelldateien_erhalten:ja");
            file.write("\n");

            ui->checkBox_std_namen->setChecked(false);
            file.write("std_namen:nein");
            file.write("\n");

        }
        file.close();
    }else
    {
        QFile file(INIFILE);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this,"Fehler","Fehler beim Dateizugriff!",QMessageBox::Ok);
        }else
        {
            while(!file.atEnd())
            {
                QString zeile = QLatin1String(  file.readLine()  );
                if(zeile.contains("verzeichnis_quelle:"))
                {
                    verzeichnis_quelle = text_mitte(zeile, "verzeichnis_quelle:", "\n");
                    ui->lineEdit_quelle->setText(verzeichnis_quelle);
                }else if(zeile.contains("verzeichnis_ziel:"))
                {
                    verzeichnis_ziel = text_mitte(zeile, "verzeichnis_ziel:", "\n");
                    ui->lineEdit_ziel->setText(verzeichnis_ziel);
                }else if(zeile.contains("verzeichnis_ziel_ganx:"))
                {
                    verzeichnis_ziel_ganx = text_mitte(zeile, "verzeichnis_ziel_ganx:", "\n");
                    ui->lineEdit_ziel_ganx->setText(verzeichnis_ziel_ganx);
                }else if(zeile.contains("prefix1:"))
                {
                    prefix1 = text_mitte(zeile, "prefix1:", "\n");
                    ui->lineEdit_prefix1->setText(prefix1);
                }else if(zeile.contains("prefix2:"))
                {
                    prefix2 = text_mitte(zeile, "prefix2:", "\n") ;
                    ui->lineEdit_prefix2->setText(prefix2);
                }else if(zeile.contains("quelldateien_erhalten:"))
                {
                    quelldateien_erhalten = text_mitte(zeile, "quelldateien_erhalten:", "\n");
                    if(quelldateien_erhalten == "ja")
                    {
                        ui->checkBox_quelldateien_erhalten->setChecked(true);
                    }else
                    {
                        ui->checkBox_quelldateien_erhalten->setChecked(false);
                    }
                }else if(zeile.contains("std_namen:"))
                {
                    std_namen = text_mitte(zeile, "std_namen:", "\n");
                    if(std_namen == "ja")
                    {
                        ui->checkBox_std_namen->setChecked(true);
                    }else
                    {
                        ui->checkBox_std_namen->setChecked(false);
                    }
                }else if(zeile.contains("erzeuge_ascii:"))
                {
                    erzeuge_ascii = text_mitte(zeile, "erzeuge_ascii:", "\n");
                    if(erzeuge_ascii == "ja")
                    {
                        ui->checkBox_ascii->setChecked(true);
                    }else
                    {
                        ui->checkBox_ascii->setChecked(false);
                    }
                }else if(zeile.contains("erzeuge_ganx:"))
                {
                    erzeuge_ganx = text_mitte(zeile, "erzeuge_ganx:", "\n");
                    if(erzeuge_ganx == "ja")
                    {
                        ui->checkBox_ganx->setChecked(true);
                    }else
                    {
                        ui->checkBox_ganx->setChecked(false);
                    }
                }
            }
        }
        file.close();
    }

    if(werkzeugdatei_gefunden == false)
    {
        QFile file(WERKZEUGDATEI);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this,"Fehler","Fehler beim Dateizugriff!",QMessageBox::Ok);
        }else
        {
            //----------------------------------------------Tabellenkopf:
            file.write("Typ");
            file.write("\t");
            file.write("Nummer");
            file.write("\t");
            file.write("Durchmesser");
            file.write("\t");
            file.write("Nutzlaenge");
            file.write("\t");
            file.write("Vorschub");
            file.write("\t");
            file.write(" ");


            //file.write("\n");
        }
        file.close();
    }else
    {
        QFile file(WERKZEUGDATEI);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this,"Fehler","Fehler beim Dateizugriff!",QMessageBox::Ok);
        }else
        {
            werkzeug.set_text(file.readAll());
        }
        file.close();
    }

}

void MainWindow::schreibe_ini()
{
    QFile file(INIFILE);
    file.remove();
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,"Fehler","Fehler beim Dateizugriff!",QMessageBox::Ok);
    }else
    {
        //-------------------------------------------Verzeichnisse:
        file.write("verzeichnis_quelle:");
        file.write(verzeichnis_quelle.toUtf8());
        file.write("\n");

        file.write("verzeichnis_ziel:");
        file.write(verzeichnis_ziel.toUtf8());
        file.write("\n");

        file.write("verzeichnis_ziel_ganx:");
        file.write(verzeichnis_ziel_ganx.toUtf8());
        file.write("\n");

        //-------------------------------------------Prefixe:
        file.write("prefix1:");
        file.write(prefix1.toUtf8());
        file.write("\n");

        file.write("prefix2:");
        file.write(prefix2.toUtf8());
        file.write("\n");

        //-------------------------------------------Checkboxen:
        file.write("quelldateien_erhalten:");
        file.write(quelldateien_erhalten.toUtf8());
        file.write("\n");

        file.write("std_namen:");
        file.write(std_namen.toUtf8());
        file.write("\n");

        file.write("erzeuge_ascii:");
        file.write(erzeuge_ascii.toUtf8());
        file.write("\n");

        file.write("erzeuge_ganx:");
        file.write(erzeuge_ganx.toUtf8());
        file.write("\n");
    }
    file.close();
}

void MainWindow::on_actionInfo_triggered()
{
    QString tmp;
    tmp = "ASCII-Assistent / ";
    tmp += VERSION;
    tmp +=" / Lizenz:  GPL / Autor: Oliver Schuft\n";
    tmp += "\nBislang noch nicht unsterstuetzt wird:\n";
    tmp += "- Linie";
    tmp += "\n";
    tmp += "- Kreissegment";
    tmp += "\n";
    tmp += "- Kreissegment P2P";
    tmp += "\n";
    tmp += "- Ausklinkung";
    tmp += "\n";
    ui->plainTextEdit_Meldungsfenster->setPlainText(tmp);
}

//-----------------------------------------------------------------------Pfade:
void MainWindow::on_lineEdit_quelle_editingFinished()
{
    QString eingabe = ui->lineEdit_quelle->text();
    if(!QDir(eingabe).exists())
    {
        QMessageBox::warning(this,"Fehler","Verzeichniss \"" + eingabe + "\" nicht gefunden!",QMessageBox::Ok);
        ui->lineEdit_quelle->setText(verzeichnis_quelle);
    }else
    {
        verzeichnis_quelle = eingabe;
        schreibe_ini();
    }
}

void MainWindow::on_lineEdit_ziel_editingFinished()
{
    QString eingabe = ui->lineEdit_ziel->text();
    if(!QDir(eingabe).exists())
    {
        QMessageBox::warning(this,"Fehler","Verzeichniss \"" + eingabe + "\" nicht gefunden!",QMessageBox::Ok);
        ui->lineEdit_ziel->setText(verzeichnis_ziel);
    }else
    {
        verzeichnis_ziel = eingabe;
        schreibe_ini();
    }
}

void MainWindow::on_lineEdit_ziel_ganx_editingFinished()
{
    QString eingabe = ui->lineEdit_ziel_ganx->text();
    if(!QDir(eingabe).exists())
    {
        QMessageBox::warning(this,"Fehler","Verzeichniss \"" + eingabe + "\" nicht gefunden!",QMessageBox::Ok);
        ui->lineEdit_ziel_ganx->setText(verzeichnis_ziel_ganx);
    }else
    {
        verzeichnis_ziel_ganx = eingabe;
        schreibe_ini();
    }
}
//-----------------------------------------------------------------------Pfad-Buttons:
void MainWindow::on_pushButton_quelle_clicked()
{
    if(verzeichnis_quelle.isEmpty())
    {
        verzeichnis_quelle = "./";
    }
    QString tmp = QFileDialog::getExistingDirectory(this, tr("Quellverzeichniss"), verzeichnis_quelle);
    if(!tmp.isEmpty())
    {
        verzeichnis_quelle = tmp;
        ui->lineEdit_quelle->setText(verzeichnis_quelle);
        schreibe_ini();
    }
}

void MainWindow::on_pushButton_ziel_clicked()
{
    if(verzeichnis_ziel.isEmpty())
    {
        verzeichnis_ziel = "./";
    }
    QString tmp = QFileDialog::getExistingDirectory(this, tr("Zielverzeichniss ascii"), verzeichnis_ziel);
    if(!tmp.isEmpty())
    {
        verzeichnis_ziel = tmp;
        ui->lineEdit_ziel->setText(verzeichnis_ziel);
        schreibe_ini();
    }
}

void MainWindow::on_pushButton_ziel_ganx_clicked()
{
    if(verzeichnis_ziel_ganx.isEmpty())
    {
        verzeichnis_ziel_ganx = "./";
    }
    QString tmp = QFileDialog::getExistingDirectory(this, tr("Zielverzeichniss ganx"), verzeichnis_ziel_ganx);
    if(!tmp.isEmpty())
    {
        verzeichnis_ziel_ganx = tmp;
        ui->lineEdit_ziel_ganx->setText(verzeichnis_ziel_ganx);
        schreibe_ini();
    }
}
//-----------------------------------------------------------------------Prefixe:
void MainWindow::on_lineEdit_prefix1_editingFinished()
{
    QString eingabe = ui->lineEdit_prefix1->text();
    if(!eingabe.isEmpty()  &&   eingabe==prefix2)
    {
        QMessageBox::warning(this,"Fehler","Prefixe sind duerfen nicht gleich sein",QMessageBox::Ok);
        ui->lineEdit_prefix1->setText(prefix1);
    }else
    {
        prefix1 = eingabe;
        schreibe_ini();
    }
}

void MainWindow::on_lineEdit_prefix2_editingFinished()
{
    QString eingabe = ui->lineEdit_prefix2->text();
    if(!eingabe.isEmpty()  &&   eingabe==prefix1)
    {
        QMessageBox::warning(this,"Fehler","Prefixe sind duerfen nicht gleich sein",QMessageBox::Ok);
        ui->lineEdit_prefix2->setText(prefix2);
    }else
    {
        prefix2 = eingabe;
        schreibe_ini();
    }
}
//-----------------------------------------------------------------------Checkboxen:
void MainWindow::on_checkBox_quelldateien_erhalten_stateChanged()
{
    if(ui->checkBox_quelldateien_erhalten->isChecked() == true)
    {
        quelldateien_erhalten = "ja";
    }else
    {
        quelldateien_erhalten = "nein";
    }
    schreibe_ini();
}

void MainWindow::on_checkBox_std_namen_stateChanged()
{
    if(ui->checkBox_std_namen->isChecked() == true)
    {
        std_namen = "ja";
    }else
    {
        std_namen = "nein";
    }
    schreibe_ini();
}

void MainWindow::on_checkBox_ascii_stateChanged()
{
    if(ui->checkBox_ascii->isChecked() == true)
    {
        erzeuge_ascii = "ja";
    }else
    {
        erzeuge_ascii = "nein";
    }
    schreibe_ini();
}

void MainWindow::on_checkBox_ganx_stateChanged()
{
    if(ui->checkBox_ganx->isChecked() == true)
    {
        erzeuge_ganx = "ja";
    }else
    {
        erzeuge_ganx = "nein";
    }
    schreibe_ini();
}
//-----------------------------------------------------------------------
void MainWindow::dateien_erfassen()
{
    QDir ordner(verzeichnis_quelle);
    QStringList ordnerinhalt;
    ordnerinhalt = ordner.entryList(QDir::Files);
    text_zeilenweise tz;
    for(QStringList::iterator it = ordnerinhalt.begin() ; it!=ordnerinhalt.end() ; ++it)
    {
        QString name = *it;
        tz.zeile_anhaengen(name);
    }
    text_zeilenweise tz_postfixe;
    for(uint i = 1 ; i<=tz.zeilenanzahl() ; i++)
    {
        QString zeile = tz.zeile(i);
        if(zeile.contains(prefix1))
        {
            QString postfix = text_links(zeile, prefix1);
            if(tz_postfixe.finde_Zeile(postfix)==0)
            {
                tz_postfixe.zeilen_anhaengen(postfix);
            }
        }else if(zeile.contains(prefix2))
        {
            QString postfix = text_links(zeile, prefix2);
            if(tz_postfixe.finde_Zeile(postfix)==0)
            {
                tz_postfixe.zeilen_anhaengen(postfix);
            }
        }
    }
    text_zeilenweise tz1 = tz_postfixe;
    text_zeilenweise tz2 = tz_postfixe;
    for(uint i = 1 ; i<=tz.zeilenanzahl() ; i++)
    {
        QString zeile = tz.zeile(i);
        if(zeile.contains(prefix1))
        {
            QString postfix = text_links(zeile, prefix1);
            tz1.zeile_ersaetzen(tz_postfixe.finde_Zeile(postfix), zeile);
        }else if(zeile.contains(prefix2))
        {
            QString postfix = text_links(zeile, prefix2);
            tz2.zeile_ersaetzen(tz_postfixe.finde_Zeile(postfix), zeile);
        }
    }

    dateien_alle = tz;
    dateien_haupt = tz1;
    dateien_neben = tz2;
    postfixe = tz_postfixe;
}

void MainWindow::on_pushButton_auflisten_clicked()
{
    if(verzeichnis_quelle.isEmpty())
    {
        QMessageBox::warning(this,"Abbruch","Quellverzeichniss nicht angegeben!",QMessageBox::Ok);
        return;
    }
    if(verzeichnis_ziel.isEmpty())
    {
        QMessageBox::warning(this,"Abbruch","Zielverzeichniss nicht angegeben!",QMessageBox::Ok);
        return;
    }
    dateien_erfassen();
    QString vortext = "-----------------alle Dateien:\n";
    vortext += dateien_alle.get_text();
    vortext += "\n\n-----------------Dateien zusammengefasst:\n";
    QString nachtext;
    for(uint i=1;i<=dateien_haupt.zeilenanzahl();i++)
    {
        if(dateien_haupt.zeile(i).contains(prefix1) && dateien_neben.zeile(i).contains(prefix2))
        {
            nachtext += dateien_haupt.zeile(i);
            nachtext += "      /      ";
            nachtext += dateien_neben.zeile(i);
            nachtext += "\n";
        }else if(dateien_haupt.zeile(i).contains(prefix1))
        {
            nachtext += dateien_haupt.zeile(i);
            nachtext += "\n";
        }else if(dateien_neben.zeile(i).contains(prefix2))
        {
            nachtext += "                                              ";
            nachtext += dateien_neben.zeile(i);
            nachtext += "\n";
        }
    }
    if(std_namen == "ja")
    {
        nachtext += "\n-----------------Standard-Dateinamen:\n";
        for(uint i=1;i<=postfixe.zeilenanzahl();i++)
        {
            nachtext += postfixe.zeile(i);
            nachtext += "      -->      ";
            nachtext += namen_durch_std_namen_tauschen(postfixe.zeile(i));
            nachtext += "\n";
        }
    }

    ui->plainTextEdit_Meldungsfenster->setPlainText(vortext + nachtext);
    //ui->plainTextEdit_Meldungsfenster->setPlainText(dateien_neben.get_text());
}

void MainWindow::on_pushButton_Start_clicked()
{
    ui->plainTextEdit_Meldungsfenster->clear();
    if(verzeichnis_quelle.isEmpty())
    {
        QMessageBox::warning(this,"Abbruch","Quellverzeichniss nicht angegeben!",QMessageBox::Ok);
        return;
    }
    if(verzeichnis_ziel.isEmpty())
    {
        QMessageBox::warning(this,"Abbruch","Zielverzeichniss nicht angegeben!",QMessageBox::Ok);
        return;
    }
    dateien_erfassen();
    for(uint i=1;i<=dateien_haupt.zeilenanzahl();i++)
    {
        if(dateien_haupt.zeile(i).contains(prefix1) && dateien_neben.zeile(i).contains(prefix2))
        {
            //--------------------------------------------------------------------- ----------------------Bearbeitungen der Haupt-Seite:

            QString dateiinhalt_haupt;
            QFile datei_h(verzeichnis_quelle + QDir::separator() + dateien_haupt.zeile(i));
            if(!datei_h.exists())
            {
                QMessageBox::warning(this,"Abbruch","Datei " + dateien_haupt.zeile(i) + " nicht gefunden!",QMessageBox::Ok);
                ui->plainTextEdit_Meldungsfenster->setPlainText("Funktion mit Fehlermeldung abgebrochen!");
                return;
            }else
            {
                if(!datei_h.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    QMessageBox::warning(this,"Abbruch","Datei " + dateien_haupt.zeile(i) + " konnte nicht geoefnnet werden!",QMessageBox::Ok);
                    ui->plainTextEdit_Meldungsfenster->setPlainText("Funktion mit Fehlermeldung abgebruchen!");
                    return;
                }else
                {
                    dateiinhalt_haupt = datei_h.readAll();
                }
                datei_h.close();
            }
            dateiinhalt_haupt = bearbeitung_auf_die_Unterseite(dateiinhalt_haupt, prefix1);

            //--------------------------------------------------------------------- ----------------------Bearbeitungen der Neben-Seite:

            //Ich gehe davon aus, der VW alle HBE immer auf die Hauptseite exportiert, daher keine Berücksichtigung für HBEs
            QString dateiinhalt_neben;
            QFile datei_n(verzeichnis_quelle + QDir::separator() + dateien_neben.zeile(i));
            if(!datei_n.exists())
            {
                QMessageBox::warning(this,"Abbruch","Datei " + dateien_neben.zeile(i) + " nicht gefunden!",QMessageBox::Ok);
                ui->plainTextEdit_Meldungsfenster->setPlainText("Funktion mit Fehlermeldung abgebrochen!");
                return;
            }else
            {
                if(!datei_n.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    QMessageBox::warning(this,"Abbruch","Datei " + dateien_neben.zeile(i) + " konnte nicht geoefnnet werden!",QMessageBox::Ok);
                    ui->plainTextEdit_Meldungsfenster->setPlainText("Funktion mit Fehlermeldung abgebruchen!");
                    return;
                }else
                {
                    dateiinhalt_neben = datei_n.readAll();
                }
                datei_n.close();
            }

            //--------------------------------------------------------------------- ----------------------Inhalte zusammenführen:

            text_zeilenweise neue_Datei_tz;
            neue_Datei_tz.set_text(dateiinhalt_haupt);
            text_zeilenweise dateiinhalt_neben_tz;
            dateiinhalt_neben_tz.set_text(dateiinhalt_neben);

            for(uint i=4;i<=dateiinhalt_neben_tz.zeilenanzahl();i++)//erste 3 Zeile sind ja schon auf der Haupt-Seite einthalten
            {
                neue_Datei_tz.zeile_anhaengen(dateiinhalt_neben_tz.zeile(i));
            }

            //-------------------------------------------------------------------------------------------------
            neue_Datei_tz = ascii_optimieren(neue_Datei_tz);
            //-------------------------------------------------------------------------------------------------

            if(std_namen == "ja")
            {
                postfixe.zeile_ersaetzen(i,namen_durch_std_namen_tauschen(postfixe.zeile(i)));
            }

            if(erzeuge_ascii == "ja")
            {
                QFile datei_neu(verzeichnis_ziel + QDir::separator() + postfixe.zeile(i) + ASCII);
                if(!datei_neu.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    QMessageBox::warning(this,"Abbruch","Fehler beim Dateizugriff!",QMessageBox::Ok);
                    ui->plainTextEdit_Meldungsfenster->setPlainText("Funktion mit Fehlermeldung abgebrochen!");
                    return;
                }else
                {
                    datei_neu.write(neue_Datei_tz.get_text().toUtf8().constData());
                    datei_neu.close();
                    if(ui->plainTextEdit_Meldungsfenster->toPlainText().isEmpty() || ((i==1)&&(erzeuge_ganx != "ja"))  )
                    {
                        ui->plainTextEdit_Meldungsfenster->setPlainText(postfixe.zeile(i) + \
                                                                        ASCII + \
                                                                        " wurde angelegt.");
                    }else
                    {
                        ui->plainTextEdit_Meldungsfenster->setPlainText(ui->plainTextEdit_Meldungsfenster->toPlainText() +\
                                                                        "\n"+ \
                                                                        postfixe.zeile(i) + \
                                                                        ASCII + \
                                                                        " wurde angelegt.");
                    }
                    if(quelldateien_erhalten=="nein")
                    {
                        datei_h.remove();
                        datei_n.remove();
                    }
                }
            }
            if(erzeuge_ganx == "ja")
            {
                QFile datei_neu(verzeichnis_ziel_ganx + QDir::separator() + postfixe.zeile(i) + GANX);
                if(!datei_neu.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    QMessageBox::warning(this,"Abbruch","Fehler beim Dateizugriff!",QMessageBox::Ok);
                    ui->plainTextEdit_Meldungsfenster->setPlainText("Funktion mit Fehlermeldung abgebrochen!");
                    return;
                }else
                {
                    QString msg = ascii_umwandeln_in_ganx(neue_Datei_tz.get_text());
                    if(msg.contains("Fehler!"))
                    {
                        if(ui->plainTextEdit_Meldungsfenster->toPlainText().isEmpty() || ((i==1)&&(erzeuge_ascii != "ja")))
                        {
                            ui->plainTextEdit_Meldungsfenster->setPlainText(postfixe.zeile(i) + \
                                                                            GANX + \
                                                                            " ----> " + \
                                                                            msg);
                        }else
                        {
                            ui->plainTextEdit_Meldungsfenster->setPlainText(ui->plainTextEdit_Meldungsfenster->toPlainText() +\
                                                                            "\n"+ \
                                                                            postfixe.zeile(i) + \
                                                                            GANX + \
                                                                            " ----> " + \
                                                                            msg);
                        }
                    }else
                    {
                        datei_neu.write(msg.toUtf8().constData());
                        datei_neu.close();
                        if(ui->plainTextEdit_Meldungsfenster->toPlainText().isEmpty() || ((i==1)&&(erzeuge_ascii != "ja")))
                        {
                            ui->plainTextEdit_Meldungsfenster->setPlainText(postfixe.zeile(i) + \
                                                                            GANX + \
                                                                            " wurde angelegt.");
                        }else
                        {
                            ui->plainTextEdit_Meldungsfenster->setPlainText(ui->plainTextEdit_Meldungsfenster->toPlainText() +\
                                                                            "\n"+ \
                                                                            postfixe.zeile(i) + \
                                                                            GANX + \
                                                                            " wurde angelegt.");
                        }
                        if(quelldateien_erhalten=="nein")
                        {
                            datei_h.remove();
                            datei_n.remove();
                        }
                    }
                }
            }
        }else if(dateien_haupt.zeile(i).contains(prefix1))
        {
            QString dateiinhalt;
            QFile datei(verzeichnis_quelle + QDir::separator() + dateien_haupt.zeile(i));
            if(!datei.exists())
            {
                QMessageBox::warning(this,"Abbruch","Datei " + dateien_haupt.zeile(i) + " nicht gefunden!",QMessageBox::Ok);
                ui->plainTextEdit_Meldungsfenster->setPlainText("Funktion mit Fehlermeldung abgebrochen!");
                return;
            }else
            {
                if(!datei.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    QMessageBox::warning(this,"Abbruch","Datei " + dateien_haupt.zeile(i) + " konnte nicht geoefnnet werden!",QMessageBox::Ok);
                    ui->plainTextEdit_Meldungsfenster->setPlainText("Funktion mit Fehlermeldung abgebruchen!");
                    return;
                }else
                {
                    dateiinhalt = datei.readAll();
                }
                datei.close();
            }

            //-------------------------------------------------------------------------------------------------
            dateiinhalt = bearbeitung_auf_die_Unterseite(dateiinhalt, prefix1);
            //-------------------------------------------------------------------------------------------------
            dateiinhalt = ascii_optimieren(dateiinhalt);
            //-------------------------------------------------------------------------------------------------

            if(std_namen == "ja")
            {
                postfixe.zeile_ersaetzen(i,namen_durch_std_namen_tauschen(postfixe.zeile(i)));
            }

            if(erzeuge_ascii == "ja")
            {
                QFile datei_neu(verzeichnis_ziel + QDir::separator() + postfixe.zeile(i) + ASCII);
                if(!datei_neu.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    QMessageBox::warning(this,"Abbruch","Fehler beim Dateizugriff!",QMessageBox::Ok);
                    ui->plainTextEdit_Meldungsfenster->setPlainText("Funktion mit Fehlermeldung abgebrochen!");
                    return;
                }else
                {
                    datei_neu.write(dateiinhalt.toUtf8().constData());
                    datei_neu.close();
                    if(ui->plainTextEdit_Meldungsfenster->toPlainText().isEmpty() || ((i==1)&&(erzeuge_ganx != "ja"))  )
                    {
                        ui->plainTextEdit_Meldungsfenster->setPlainText(postfixe.zeile(i) + \
                                                                        ASCII + \
                                                                        " wurde angelegt.");
                    }else
                    {
                        ui->plainTextEdit_Meldungsfenster->setPlainText(ui->plainTextEdit_Meldungsfenster->toPlainText() +\
                                                                        "\n"+ \
                                                                        postfixe.zeile(i) + \
                                                                        ASCII + \
                                                                        " wurde angelegt.");
                    }
                    if(quelldateien_erhalten=="nein")
                    {
                        datei.remove();
                    }
                }
            }
            if(erzeuge_ganx == "ja")
            {
                QFile datei_neu(verzeichnis_ziel_ganx + QDir::separator() + postfixe.zeile(i) + GANX);
                if(!datei_neu.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    QMessageBox::warning(this,"Abbruch","Fehler beim Dateizugriff!",QMessageBox::Ok);
                    ui->plainTextEdit_Meldungsfenster->setPlainText("Funktion mit Fehlermeldung abgebrochen!");
                    return;
                }else
                {
                    QString msg = ascii_umwandeln_in_ganx(dateiinhalt);
                    if(msg.contains("Fehler!"))
                    {
                        if(ui->plainTextEdit_Meldungsfenster->toPlainText().isEmpty() || ((i==1)&&(erzeuge_ascii != "ja")))
                        {
                            ui->plainTextEdit_Meldungsfenster->setPlainText(postfixe.zeile(i) + \
                                                                            GANX + \
                                                                            " ----> " + \
                                                                            msg);
                        }else
                        {
                            ui->plainTextEdit_Meldungsfenster->setPlainText(ui->plainTextEdit_Meldungsfenster->toPlainText() +\
                                                                            "\n"+ \
                                                                            postfixe.zeile(i) + \
                                                                            GANX + \
                                                                            " ----> " + \
                                                                            msg);
                        }
                    }else
                    {
                        datei_neu.write(msg.toUtf8().constData());
                        datei_neu.close();
                        if(ui->plainTextEdit_Meldungsfenster->toPlainText().isEmpty() || ((i==1)&&(erzeuge_ascii != "ja")))
                        {
                            ui->plainTextEdit_Meldungsfenster->setPlainText(postfixe.zeile(i) + \
                                                                            GANX + \
                                                                            " wurde angelegt.");
                        }else
                        {
                            ui->plainTextEdit_Meldungsfenster->setPlainText(ui->plainTextEdit_Meldungsfenster->toPlainText() +\
                                                                            "\n"+ \
                                                                            postfixe.zeile(i) + \
                                                                            GANX + \
                                                                            " wurde angelegt.");
                        }
                        if(quelldateien_erhalten=="nein")
                        {
                            datei.remove();
                        }
                    }
                }
            }
        }else if(dateien_neben.zeile(i).contains(prefix2))
        {
            QString dateiinhalt;
            QFile datei(verzeichnis_quelle + QDir::separator() + dateien_neben.zeile(i));
            if(!datei.exists())
            {
                QMessageBox::warning(this,"Abbruch","Datei " + dateien_neben.zeile(i) + " nicht gefunden!",QMessageBox::Ok);
                ui->plainTextEdit_Meldungsfenster->setPlainText("Funktion mit Fehlermeldung abgebrochen!");
                return;
            }else
            {
                if(!datei.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    QMessageBox::warning(this,"Abbruch","Datei " + dateien_neben.zeile(i) + " konnte nicht geoefnnet werden!",QMessageBox::Ok);
                    ui->plainTextEdit_Meldungsfenster->setPlainText("Funktion mit Fehlermeldung abgebruchen!");
                    return;
                }else
                {
                    dateiinhalt = datei.readAll();
                }
                datei.close();
            }

            //-------------------------------------------------------------------------------------------------
            dateiinhalt = bearbeitung_auf_die_Unterseite(dateiinhalt, prefix2);
            //-------------------------------------------------------------------------------------------------
            dateiinhalt = ascii_optimieren(dateiinhalt);
            //-------------------------------------------------------------------------------------------------

            if(std_namen == "ja")
            {
                postfixe.zeile_ersaetzen(i,namen_durch_std_namen_tauschen(postfixe.zeile(i)));
            }

            if(erzeuge_ascii == "ja")
            {
                QFile datei_neu(verzeichnis_ziel + QDir::separator() + postfixe.zeile(i) + ASCII);
                if(!datei_neu.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    QMessageBox::warning(this,"Abbruch","Fehler beim Dateizugriff!",QMessageBox::Ok);
                    ui->plainTextEdit_Meldungsfenster->setPlainText("Funktion mit Fehlermeldung abgebrochen!");
                    return;
                }else
                {
                    datei_neu.write(dateiinhalt.toUtf8().constData());
                    datei_neu.close();
                    if(ui->plainTextEdit_Meldungsfenster->toPlainText().isEmpty() || ((i==1)&&(erzeuge_ganx != "ja"))  )
                    {
                        ui->plainTextEdit_Meldungsfenster->setPlainText(postfixe.zeile(i) + \
                                                                        ASCII + \
                                                                        " wurde angelegt.");
                    }else
                    {
                        ui->plainTextEdit_Meldungsfenster->setPlainText(ui->plainTextEdit_Meldungsfenster->toPlainText() +\
                                                                        "\n"+ \
                                                                        postfixe.zeile(i) + \
                                                                        ASCII + \
                                                                        " wurde angelegt.");
                    }
                    if(quelldateien_erhalten=="nein")
                    {
                        datei.remove();
                    }
                }
            }
            if(erzeuge_ganx == "ja")
            {
                QFile datei_neu(verzeichnis_ziel_ganx + QDir::separator() + postfixe.zeile(i) + GANX);
                if(!datei_neu.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    QMessageBox::warning(this,"Abbruch","Fehler beim Dateizugriff!",QMessageBox::Ok);
                    ui->plainTextEdit_Meldungsfenster->setPlainText("Funktion mit Fehlermeldung abgebrochen!");
                    return;
                }else
                {
                    QString msg = ascii_umwandeln_in_ganx(dateiinhalt);
                    if(msg.contains("Fehler!"))
                    {
                        if(ui->plainTextEdit_Meldungsfenster->toPlainText().isEmpty() || ((i==1)&&(erzeuge_ascii != "ja")) )
                        {
                            ui->plainTextEdit_Meldungsfenster->setPlainText(postfixe.zeile(i) + \
                                                                            GANX + \
                                                                            " ----> " + \
                                                                            msg);
                        }else
                        {
                            ui->plainTextEdit_Meldungsfenster->setPlainText(ui->plainTextEdit_Meldungsfenster->toPlainText() +\
                                                                            "\n"+ \
                                                                            postfixe.zeile(i) + \
                                                                            GANX + \
                                                                            " ----> " + \
                                                                            msg);
                        }
                    }else
                    {
                        datei_neu.write(msg.toUtf8().constData());
                        datei_neu.close();
                        if(ui->plainTextEdit_Meldungsfenster->toPlainText().isEmpty() || ((i==1)&&(erzeuge_ascii != "ja")) )
                        {
                            ui->plainTextEdit_Meldungsfenster->setPlainText(postfixe.zeile(i) + \
                                                                            GANX + \
                                                                            " wurde angelegt.");
                        }else
                        {
                            ui->plainTextEdit_Meldungsfenster->setPlainText(ui->plainTextEdit_Meldungsfenster->toPlainText() +\
                                                                            "\n"+ \
                                                                            postfixe.zeile(i) + \
                                                                            GANX + \
                                                                            " wurde angelegt.");
                        }
                        if(quelldateien_erhalten=="nein")
                        {
                            datei.remove();
                        }
                    }
                }
            }
        }
    }
}

QString MainWindow::bearbeitung_auf_die_Unterseite(QString dateitext, QString prefix)
{
    text_zeilenweise dateitext_tz;
    dateitext_tz.set_text(dateitext);

    text_zeilenweise programmkopf;
    programmkopf.set_trennzeichen(';');
    programmkopf.set_text(dateitext_tz.zeile(2));
    //Werkstückbreite prüfen (X-Maß):
    QString tmp = programmkopf.zeile(2);
    double breite = tmp.toDouble();
    if(breite < 50)
    {
        QString msg = "Fehler bei Bauteil ";
        text_zeilenweise tz;
        tz.set_trennzeichen(';');
        tz.set_text(dateitext_tz.zeile(3));
        msg+= tz.zeile(2);
        msg+= "!\n";
        msg+= "Breite < 50mm";
        QMessageBox mb;
        mb.setText(msg);
        mb.exec();
    }else if(breite > 1000)
    {
        QString msg = "Fehler bei Bauteil ";
        text_zeilenweise tz;
        tz.set_trennzeichen(';');
        tz.set_text(dateitext_tz.zeile(3));
        msg+= tz.zeile(2);
        msg+= "!\n";
        msg+= "Breite > 1000";
        QMessageBox mb;
        mb.setText(msg);
        mb.exec();
    }

    //Werkstücklänge merken für später (Y-Maß):
    tmp = programmkopf.zeile(3);
    double laenge = tmp.toDouble();
    if(laenge < 250)
    {
        QString msg = "Fehler bei Bauteil ";
        text_zeilenweise tz;
        tz.set_trennzeichen(';');
        tz.set_text(dateitext_tz.zeile(3));
        msg+= tz.zeile(2);
        msg+= "!\n";
        msg+= "Laenge < 250mm";
        QMessageBox mb;
        mb.setText(msg);
        mb.exec();
    }
    //Werkstückdicke merken für später (Z-Maß):
    tmp = programmkopf.zeile(4);
    double dicke = tmp.toDouble();
    if(dicke < 6)
    {
        QString msg = "Fehler bei Bauteil ";
        text_zeilenweise tz;
        tz.set_trennzeichen(';');
        tz.set_text(dateitext_tz.zeile(3));
        msg+= tz.zeile(2);
        msg+= "!\n";
        msg+= "Dicke < 6mm";
        QMessageBox mb;
        mb.setText(msg);
        mb.exec();
    }
    //Zeile 3 bearbeiten -->Bauteilnamen ändern:
    tmp = dateitext_tz.zeile(3);
    tmp = text_links(tmp, (text_links(prefix, ASCII)));
    dateitext_tz.zeile_ersaetzen(3, tmp);
    //weiter ab Zeile 4:
    for(uint i=4;i<=dateitext_tz.zeilenanzahl();i++)//Zeile 1+2 können übersprungen werden, Zeile 3 ist bereits geämdert
    {
        text_zeilenweise zeile;
        zeile.set_trennzeichen(';');
        zeile.set_text(dateitext_tz.zeile(i));
        if(zeile.zeile(1)==BEARBEITUNG_BOHRUNG)
        {
            if(zeile.zeile(2)==BEZUG_FLAECHE_OBEN_ASCII)
            {
                //Bezugskante:
                zeile.zeile_ersaetzen(2, BEZUG_FLAECHE_UNTEN_ASCII);
                //Y-Maß neu berechnen:
                QString mass_alt_qstring = zeile.zeile(5);
                double mass = mass_alt_qstring.toDouble();
                mass = laenge - mass;
                zeile.zeile_ersaetzen(5, double_to_qstring(mass));
            }else if(zeile.zeile(2)==BEZUG_FLAECHE_HINTEN_ASCII)//Hinten
            {
                //Bezugskante:
                zeile.zeile_ersaetzen(2, BEZUG_FLAECHE_VORNE_ASCII);//Vorne
                //Z-Höhe:
                QString mass_alt_qstring = zeile.zeile(6);
                double mass = mass_alt_qstring.toDouble();
                mass = dicke - mass;
                zeile.zeile_ersaetzen(6, double_to_qstring(mass));
            }else if(zeile.zeile(2)==BEZUG_FLAECHE_VORNE_ASCII)//Vorne
            {
                //Bezugskante:
                zeile.zeile_ersaetzen(2, BEZUG_FLAECHE_HINTEN_ASCII);//Vorne
                //Z-Höhe:
                QString mass_alt_qstring = zeile.zeile(6);
                double mass = mass_alt_qstring.toDouble();
                mass = dicke - mass;
                zeile.zeile_ersaetzen(6, double_to_qstring(mass));
            }else if(zeile.zeile(2)==BEZUG_FLAECHE_LINKS_ASCII ||  zeile.zeile(2)==BEZUG_FLAECHE_RECHTS_ASCII)//Links oder Rechts
            {
                //Y-Maß:
                QString mass_alt_qstring = zeile.zeile(5);
                double mass = mass_alt_qstring.toDouble();
                mass = laenge - mass;
                zeile.zeile_ersaetzen(5, double_to_qstring(mass));
                //Z-Höhe:
                mass_alt_qstring = zeile.zeile(6);
                mass = mass_alt_qstring.toDouble();
                mass = dicke - mass;
                zeile.zeile_ersaetzen(6, double_to_qstring(mass));
            }

        }else if(zeile.zeile(1)==BEARBEITUNG_NUT)
        {
            //Bezugskante:
            zeile.zeile_ersaetzen(2, BEZUG_FLAECHE_UNTEN_ASCII);
            //Angenommen wird, das nur Nuten in Y-Richtung ausgegeben werden (nur das kann die CNC)
            //Angenommen wird, dass nur durchgehende Nuten ausgegeben werden (Nur das kann VW derzeit)
        }else if(zeile.zeile(1)==BEARBEITUNG_FRAES)//Alle arten von Fräsarbeiten
        {
            if(zeile.zeile(3)==" 101")//Rechtecktaschen
            {
                zeile.zeile_ersaetzen(4, ausdruck_auswerten(zeile.zeile(4)));
                zeile.zeile_ersaetzen(5, ausdruck_auswerten(zeile.zeile(5)));
                zeile.zeile_ersaetzen(6, ausdruck_auswerten(zeile.zeile(6)));
                zeile.zeile_ersaetzen(7, ausdruck_auswerten(zeile.zeile(7)));
                zeile.zeile_ersaetzen(8, ausdruck_auswerten(zeile.zeile(8)));
                zeile.zeile_ersaetzen(9, ausdruck_auswerten(zeile.zeile(9)));
                //Bezugskante:
                zeile.zeile_ersaetzen(2, BEZUG_FLAECHE_UNTEN_ASCII);
                //Y-Maß neu berechnen:
                QString mass_alt_qstring = zeile.zeile(5);
                double mass = mass_alt_qstring.toDouble();
                mass = laenge - mass;
                zeile.zeile_ersaetzen(5, double_to_qstring(mass));
            }else if(zeile.zeile(3)==" 102")//Kreistaschen
            {
                zeile.zeile_ersaetzen(4, ausdruck_auswerten(zeile.zeile(4)));
                zeile.zeile_ersaetzen(5, ausdruck_auswerten(zeile.zeile(5)));
                zeile.zeile_ersaetzen(6, ausdruck_auswerten(zeile.zeile(6)));
                zeile.zeile_ersaetzen(7, ausdruck_auswerten(zeile.zeile(7)));
                //Bezugskante:
                zeile.zeile_ersaetzen(2, BEZUG_FLAECHE_UNTEN_ASCII);
                //Y-Maß neu berechnen:
                QString mass_alt_qstring = zeile.zeile(5);
                double mass = mass_alt_qstring.toDouble();
                mass = laenge - mass;
                zeile.zeile_ersaetzen(5, double_to_qstring(mass));
            }else if(zeile.zeile(3)==" 103")//Linie
            {
                //Noch programmieren!!!!
                //Vorerst sicherheitshalber raus nehmen:
                zeile.set_text("");
            }else if(zeile.zeile(3)==" 104")//Kreissegment
            {
                //Noch programmieren!!!!
                //Vorerst sicherheitshalber raus nehmen:
               zeile.set_text("");
            }else if(zeile.zeile(3)==" 105")//Ausklinkung
            {
                //Noch programmieren!!!!
                //Vorerst sicherheitshalber raus nehmen:
                zeile.set_text("");
            }else if(zeile.zeile(3)==" 106")//Kreissegment P2P
            {
                //Noch programmieren!!!!
                //Vorerst sicherheitshalber raus nehmen:
                zeile.set_text("");
            }
        }
        dateitext_tz.zeile_ersaetzen(i, zeile.get_text());
    }
    return dateitext_tz.get_text();
}

QString MainWindow::namen_durch_std_namen_tauschen(QString name)
{
    if(name.contains("Seite_Links"))
    {
        name = "Seite_li" + text_rechts(name, "Seite_Links");
        if(name == "Seite_li1")
        {
            name = "Seite_li";
        }
    }else if(name.contains("Seite_Rechts"))
    {
        name = "Seite_re" + text_rechts(name, "Seite_Rechts");
        if(name == "Seite_re1")
        {
            name = "Seite_re";
        }
    }else if(name.contains("Mittelseite"))
    {
        name = "MS" + text_rechts(name, "Mittelseite");
        if(name == "MS1")
        {
            name = "MS";
        }
    }else if(name.contains("Boden_Oben"))
    {
        name = "OB" + text_rechts(name, "Boden_Oben");
        if(name == "OB1")
        {
            name = "OB";
        }
    }else if(name.contains("Boden_Unten"))
    {
        name = "UB" + text_rechts(name, "Boden_Unten");
        if(name == "UB1")
        {
            name = "UB";
        }
    }else if(name.contains("Konstruktionsboden"))
    {
        name = "KB" + text_rechts(name, "Konstruktionsboden");
        if(name == "KB0")
        {
            name = "KB";
        }
    }else if(name.contains("Fachboden"))
    {
        name = "EB" + text_rechts(name, "Fachboden");
        if(name == "EB0")
        {
            name = "EB";
        }
    }else if(name.contains("Ruckwand"))
    {
        name = "RW" + text_rechts(name, "Ruckwand");
        if(name == "RW1")
        {
            name = "RW";
        }
    }else if(name.contains("Tur"))
    {
        name = "Tuer" + text_rechts(name, "Tur");
    }
    return name;
}

void MainWindow::on_actionWerkzeug_anzeigen_triggered()
{
    ui->plainTextEdit_Meldungsfenster->setPlainText(werkzeug.get_text());
}

QString MainWindow::ascii_umwandeln_in_ganx(QString asciitext)
{
    double laenge_y, breite_x, dicke_z;
    text_zeilenweise tz;
    tz.set_text(asciitext);
    text_zeilenweise zeile;
    zeile.set_trennzeichen(';');
    //-------------------------
    zeile.set_text(tz.zeile(2));
    laenge_y = zeile.zeile(3).toDouble();
    breite_x = zeile.zeile(2).toDouble();
    dicke_z = zeile.zeile(4).toDouble();


    //-------------------------
    QString returntext;
    //-------------------------1.Vorspann:
    {
    returntext  = "<?xml version=\"1.0\" standalone=\"yes\"?>";
    returntext += "\n";
    returntext += "<Programm xmlns=\"http://tempuri.org/Programm.xsd\">";
    returntext += "\n";
    }
    //-------------------------Programmkopf:
    {
    returntext += "  <PrgrSet>";
    returntext += "\n";
    returntext += "    <PrgrName>";
    zeile.set_text(tz.zeile(3));
    returntext += zeile.zeile(2);
    returntext += "</PrgrName>";
    returntext += "\n";
    returntext += "    <Description>";
    returntext += "";                       //Kommentar
    returntext += "</Description>";
    returntext += "\n";
    returntext += "    <wsX>";
    returntext += double_to_qstring(breite_x);            //Nur zum Testen
    returntext += "</wsX>";
    returntext += "\n";
    returntext += "    <wsY>";
    returntext += double_to_qstring(laenge_y);
    returntext += "</wsY>";
    returntext += "\n";
    returntext += "    <wsZ>";
    returntext += double_to_qstring(dicke_z);            //Nur zum Testen
    returntext += "</wsZ>";
    returntext += "\n";
    returntext += "    <DoMF>";
    returntext += "False";          //Maxitix
    returntext += "</DoMF>";
    returntext += "\n";
    returntext += "    <MFPos>";
    returntext += "0";              //Position Maxitix
    returntext += "</MFPos>";
    returntext += "\n";
    returntext += "    <MFTool>";
    returntext += "TO";             //Werkzeug für Maxitix
    returntext += "</MFTool>";
    returntext += "\n";
    returntext += "  </PrgrSet>";
    returntext += "\n";
    }
    //-------------------------2. Vorspann:
    {
    returntext += "  <Root>";
    returntext += "\n";
    returntext += "    <Version>1.1.0.26</Version>";
    returntext += "\n";
    returntext += "    <CreationDate>2017-06-02T09:26:14.9442359+02:00</CreationDate>";
    returntext += "\n";
    returntext += "    <CreationApp>ASCII Import - 1.1.0.26</CreationApp>";
    returntext += "\n";
    returntext += "    <ModificationApp />";
    returntext += "\n";
    returntext += "  </Root>";
    returntext += "\n";
    }
    //-------------------------Bearbeitungen <PrgrFileWork>:
    for(uint i=4 ; i<=tz.zeilenanzahl() ; i++)
    {
        zeile.set_text(tz.zeile(i));
        if(zeile.zeile(1)==BEARBEITUNG_BOHRUNG)
        {
            double x = zeile.zeile(4).toDouble();
            double y = zeile.zeile(5).toDouble();
            double z = zeile.zeile(6).toDouble();
            double dm = zeile.zeile(7).toDouble();

            if(zeile.zeile(2)==BEZUG_FLAECHE_OBEN_ASCII)
            {
                //x = Breite
                //y = Länge
                //z = Tiefe
                returntext += "  <PrgrFileWork>";
                returntext += "\n";
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                returntext += "    <Plane>Top</Plane>";
                returntext += "\n";
                //----------------------Bezugskante festlegen:
                QString bezug = BEZUG_REF_OBEN_LINKS;
                //y < 40 -> TL
                //Länge - y < 40 ->BL
                if(laenge_y - y < bezugsmass)
                {
                    bezug = BEZUG_REF_UNTEN_LINKS;
                }
                returntext += "    <Ref>";
                returntext += bezug;
                returntext += "</Ref>";
                returntext += "\n";
                //----------------------
                returntext += "    <Typ>B</Typ>";
                returntext += "\n";
                //----------------------
                returntext += "    <X>";
                returntext += double_to_qstring(x);
                returntext += "</X>";
                returntext += "\n";
                //----------------------
                returntext += "    <Y>";
                returntext += double_to_qstring(y);
                returntext += "</Y>";
                returntext += "\n";
                //----------------------
                returntext += "    <Z>";
                returntext += double_to_qstring(0);
                returntext += "</Z>";
                returntext += "\n";
                //----------------------
                returntext += "    <Diameter>";
                returntext += double_to_qstring(dm);
                returntext += "</Diameter>";
                returntext += "\n";
                //----------------------
                returntext += "   <Depth>";
                returntext += double_to_qstring(z);
                returntext += "</Depth>";
                returntext += "\n";
                //----------------------Werkzeugnummer:
                returntext += "    <Tool>";

                QString tnummer = get_wkz_nummer(WKZ_TYP_BOHRER, dm, z);
                if(tnummer.isEmpty())
                {
                    QString msg = "Fehler!\nKein Bohrer im Werkzeugmagazin für:\n" + zeile.get_text();
                    QMessageBox mb;
                    mb.setText(msg);
                    mb.exec();
                    return msg;
                }
                returntext += tnummer;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <OldID>";
                returntext += "Top";
                returntext += "\\";
                returntext += bezug;
                returntext += "\\";
                returntext += "B-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</OldID>";
                returntext += "\n";
                //----------------------
                returntext += "    <KleiGeTei>";    //Tiefe???
                returntext += double_to_qstring(z);
                returntext += "</KleiGeTei>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFileWork>";
                returntext += "\n";

            }else if(zeile.zeile(2)==BEZUG_FLAECHE_UNTEN_ASCII)
            {
                //x = Breite
                //y = Länge
                //z = Tiefe
                returntext += "  <PrgrFileWork>";
                returntext += "\n";
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                returntext += "    <Plane>Bottom</Plane>";
                returntext += "\n";
                //----------------------Bezugskante festlegen:
                QString bezug = BEZUG_REF_OBEN_LINKS;
                //y < 40 -> TL
                //Länge - y < 40 ->BL
                if(laenge_y - y < bezugsmass)
                {
                    bezug = BEZUG_REF_UNTEN_LINKS;
                }
                returntext += "    <Ref>";
                returntext += bezug;
                returntext += "</Ref>";
                returntext += "\n";
                //----------------------
                returntext += "    <Typ>B</Typ>";
                returntext += "\n";
                //----------------------
                returntext += "    <X>";
                returntext += double_to_qstring(x);
                returntext += "</X>";
                returntext += "\n";
                //----------------------
                returntext += "    <Y>";
                returntext += double_to_qstring(y);
                returntext += "</Y>";
                returntext += "\n";
                //----------------------
                returntext += "    <Z>";
                returntext += double_to_qstring(0);
                returntext += "</Z>";
                returntext += "\n";
                //----------------------
                returntext += "    <Diameter>";
                returntext += double_to_qstring(dm);
                returntext += "</Diameter>";
                returntext += "\n";
                //----------------------
                returntext += "   <Depth>";
                returntext += double_to_qstring(z);
                returntext += "</Depth>";
                returntext += "\n";
                //----------------------Werkzeugnummer:
                returntext += "    <Tool>";

                QString tnummer = get_wkz_nummer(WKZ_TYP_BOHRER, dm, z);
                if(tnummer.isEmpty())
                {
                    QString msg = "Fehler!\nKein Bohrer im Werkzeugmagazin für:\n" + zeile.get_text();
                    QMessageBox mb;
                    mb.setText(msg);
                    mb.exec();
                    return msg;
                }
                returntext += tnummer;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <OldID>";
                returntext += "Bottom";
                returntext += "\\";
                returntext += bezug;
                returntext += "\\";
                returntext += "B-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</OldID>";
                returntext += "\n";
                //----------------------
                returntext += "    <KleiGeTei>";    //Tiefe???
                returntext += double_to_qstring(z);
                returntext += "</KleiGeTei>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFileWork>";
                returntext += "\n";

            }else if(zeile.zeile(2)==BEZUG_FLAECHE_LINKS_ASCII)
            {
                //x = Tiefe
                //y = Y-Pos
                //z = Z-Pos
                returntext += "  <PrgrFileWork>";
                returntext += "\n";
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemen einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                returntext += "    <Plane>Left</Plane>";
                returntext += "\n";
                //----------------------Bezugskante festlegen:
                QString bezug = BEZUG_REF_OBEN_LINKS;
                returntext += "    <Ref>";
                returntext += bezug;
                returntext += "</Ref>";
                returntext += "\n";
                //----------------------
                returntext += "    <Typ>B</Typ>";
                returntext += "\n";
                //----------------------
                returntext += "    <X>";
                returntext += double_to_qstring(0);
                returntext += "</X>";
                returntext += "\n";
                //----------------------
                returntext += "    <Y>";
                returntext += double_to_qstring(y);
                returntext += "</Y>";
                returntext += "\n";
                //----------------------
                returntext += "    <Z>";
                returntext += double_to_qstring(z);
                returntext += "</Z>";
                returntext += "\n";
                //----------------------
                returntext += "    <Diameter>";
                returntext += double_to_qstring(dm);
                returntext += "</Diameter>";
                returntext += "\n";
                //----------------------
                returntext += "   <Depth>";
                returntext += double_to_qstring(x);
                returntext += "</Depth>";
                returntext += "\n";
                //----------------------Werkzeugnummer:
                returntext += "    <Tool>";

                QString tnummer = get_wkz_nummer(WKZ_TYP_BOHRER, dm, x);
                if(tnummer.isEmpty())
                {
                    QString msg = "Fehler!\nKein Bohrer im Werkzeugmagazin für:\n" + zeile.get_text();
                    QMessageBox mb;
                    mb.setText(msg);
                    mb.exec();
                    return msg;
                }
                returntext += tnummer;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <OldID>";
                returntext += "Left";
                returntext += "\\";
                returntext += bezug;
                returntext += "\\";
                returntext += "B-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</OldID>";
                returntext += "\n";
                //----------------------
                returntext += "    <KleiGeTei>";    //Tiefe???
                returntext += double_to_qstring(x);
                returntext += "</KleiGeTei>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFileWork>";
                returntext += "\n";

            }else if(zeile.zeile(2)==BEZUG_FLAECHE_RECHTS_ASCII)
            {
                //x = Tiefe
                //y = Y-Pos
                //z = Z-Pos
                returntext += "  <PrgrFileWork>";
                returntext += "\n";
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemen einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                returntext += "    <Plane>Right</Plane>";
                returntext += "\n";
                //----------------------Bezugskante festlegen:
                QString bezug = BEZUG_REF_OBEN_RECHTS;
                returntext += "    <Ref>";
                returntext += bezug;
                returntext += "</Ref>";
                returntext += "\n";
                //----------------------
                returntext += "    <Typ>B</Typ>";
                returntext += "\n";
                //----------------------
                returntext += "    <X>";
                returntext += double_to_qstring(0);
                returntext += "</X>";
                returntext += "\n";
                //----------------------
                returntext += "    <Y>";
                returntext += double_to_qstring(y);
                returntext += "</Y>";
                returntext += "\n";
                //----------------------
                returntext += "    <Z>";
                returntext += double_to_qstring(z);
                returntext += "</Z>";
                returntext += "\n";
                //----------------------
                returntext += "    <Diameter>";
                returntext += double_to_qstring(dm);
                returntext += "</Diameter>";
                returntext += "\n";
                //----------------------
                returntext += "   <Depth>";
                returntext += double_to_qstring(x);
                returntext += "</Depth>";
                returntext += "\n";
                //----------------------Werkzeugnummer:
                returntext += "    <Tool>";

                QString tnummer = get_wkz_nummer(WKZ_TYP_BOHRER, dm, x);
                if(tnummer.isEmpty())
                {
                    QString msg = "Fehler!\nKein Bohrer im Werkzeugmagazin für:\n" + zeile.get_text();
                    QMessageBox mb;
                    mb.setText(msg);
                    mb.exec();
                    return msg;
                }
                returntext += tnummer;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <OldID>";
                returntext += "Right";
                returntext += "\\";
                returntext += bezug;
                returntext += "\\";
                returntext += "B-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</OldID>";
                returntext += "\n";
                //----------------------
                returntext += "    <KleiGeTei>";    //Tiefe???
                returntext += double_to_qstring(x);
                returntext += "</KleiGeTei>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFileWork>";
                returntext += "\n";

            }else if(zeile.zeile(2)==BEZUG_FLAECHE_VORNE_ASCII)
            {
                //x = X-Pos
                //y = Tiefe
                //z = Z-Pos
                returntext += "  <PrgrFileWork>";
                returntext += "\n";
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemen einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                returntext += "    <Plane>Front</Plane>";
                returntext += "\n";
                //----------------------Bezugskante festlegen:
                QString bezug = BEZUG_REF_UNTEN_LINKS;
                returntext += "    <Ref>";
                returntext += bezug;
                returntext += "</Ref>";
                returntext += "\n";
                //----------------------
                returntext += "    <Typ>B</Typ>";
                returntext += "\n";
                //----------------------
                returntext += "    <X>";
                returntext += double_to_qstring(x);
                returntext += "</X>";
                returntext += "\n";
                //----------------------
                returntext += "    <Y>";
                returntext += double_to_qstring(0);
                returntext += "</Y>";
                returntext += "\n";
                //----------------------
                returntext += "    <Z>";
                returntext += double_to_qstring(z);
                returntext += "</Z>";
                returntext += "\n";
                //----------------------
                returntext += "    <Diameter>";
                returntext += double_to_qstring(dm);
                returntext += "</Diameter>";
                returntext += "\n";
                //----------------------
                returntext += "   <Depth>";
                returntext += double_to_qstring(y);
                returntext += "</Depth>";
                returntext += "\n";
                //----------------------Werkzeugnummer:
                returntext += "    <Tool>";

                QString tnummer = get_wkz_nummer(WKZ_TYP_BOHRER, dm, y);
                if(tnummer.isEmpty())
                {
                    QString msg = "Fehler!\nKein Bohrer im Werkzeugmagazin für:\n" + zeile.get_text();
                    QMessageBox mb;
                    mb.setText(msg);
                    mb.exec();
                    return msg;
                }
                returntext += tnummer;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <OldID>";
                returntext += "Front";
                returntext += "\\";
                returntext += bezug;
                returntext += "\\";
                returntext += "B-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</OldID>";
                returntext += "\n";
                //----------------------
                returntext += "    <KleiGeTei>";    //Tiefe???
                returntext += double_to_qstring(y);
                returntext += "</KleiGeTei>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFileWork>";
                returntext += "\n";
            }else if(zeile.zeile(2)==BEZUG_FLAECHE_HINTEN_ASCII)
            {
                //x = X-Pos
                //y = Tiefe
                //z = Z-Pos
                returntext += "  <PrgrFileWork>";
                returntext += "\n";
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemen einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                returntext += "    <Plane>Rear</Plane>";
                returntext += "\n";
                //----------------------Bezugskante festlegen:
                QString bezug = BEZUG_REF_OBEN_LINKS;
                returntext += "    <Ref>";
                returntext += bezug;
                returntext += "</Ref>";
                returntext += "\n";
                //----------------------
                returntext += "    <Typ>B</Typ>";
                returntext += "\n";
                //----------------------
                returntext += "    <X>";
                returntext += double_to_qstring(x);
                returntext += "</X>";
                returntext += "\n";
                //----------------------
                returntext += "    <Y>";
                returntext += double_to_qstring(0);
                returntext += "</Y>";
                returntext += "\n";
                //----------------------
                returntext += "    <Z>";
                returntext += double_to_qstring(z);
                returntext += "</Z>";
                returntext += "\n";
                //----------------------
                returntext += "    <Diameter>";
                returntext += double_to_qstring(dm);
                returntext += "</Diameter>";
                returntext += "\n";
                //----------------------
                returntext += "   <Depth>";
                returntext += double_to_qstring(y);
                returntext += "</Depth>";
                returntext += "\n";
                //----------------------Werkzeugnummer:
                returntext += "    <Tool>";

                QString tnummer = get_wkz_nummer(WKZ_TYP_BOHRER, dm, y);
                if(tnummer.isEmpty())
                {
                    QString msg = "Fehler!\nKein Bohrer im Werkzeugmagazin für:\n" + zeile.get_text();
                    QMessageBox mb;
                    mb.setText(msg);
                    mb.exec();
                    return msg;
                }
                returntext += tnummer;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <OldID>";
                returntext += "Rear";
                returntext += "\\";
                returntext += bezug;
                returntext += "\\";
                returntext += "B-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</OldID>";
                returntext += "\n";
                //----------------------
                returntext += "    <KleiGeTei>";    //Tiefe???
                returntext += double_to_qstring(y);
                returntext += "</KleiGeTei>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFileWork>";
                returntext += "\n";
            }
        }else if(zeile.zeile(1)==BEARBEITUNG_NUT)
        {
            double x = zeile.zeile(4).toDouble();
            double y = zeile.zeile(5).toDouble();
            double z = zeile.zeile(6).toDouble();
            double l = zeile.zeile(7).toDouble();
            QString wkztyp = WKZ_TYP_SAEGE;
            QString wkz_nr = get_wkz_nummer(wkztyp);
            if(wkz_nr.isEmpty())
            {
                QString msg = "Fehler!\nKeine Saege im Werkzeugmagazin für:\n" + zeile.get_text();
                QMessageBox mb;
                mb.setText(msg);
                mb.exec();
                return msg;
            }
            double wkz_dm = get_wkz_dm(wkz_nr).toDouble();
            double nutvariante = zeile.zeile(9).toDouble();
            QString nutvariante_qstring = "";
            if(nutvariante == 1)
            {
                nutvariante_qstring = "Var2";
            }else if(nutvariante == 2)
            {
                nutvariante_qstring = "Var1";
            }else
            {
                QString msg = "Fehler!\nKeine keine Nut-Variante angegeben:\n" + zeile.get_text();
                QMessageBox mb;
                mb.setText(msg);
                mb.exec();
                return msg;
            }
            QString nutrichtung;
            if (zeile.zeile(8).toDouble() == 1)
            {
                nutrichtung = "X";
            }else
            {
                nutrichtung = "Y";
            }

                returntext += "  <PrgrFileWork>";
                returntext += "\n";
                //----------------------
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemen einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                //----------------------
                returntext += "    <Plane>";
                if(zeile.zeile(2)==BEZUG_FLAECHE_OBEN_ASCII)
                {
                    returntext += "Top";
                }else
                {
                    returntext += "Bottom";
                }
                returntext += "</Plane>";
                returntext += "\n";
                //----------------------
                returntext += "    <Ref>";
                returntext += BEZUG_REF_OBEN_LINKS;
                returntext += "</Ref>";
                returntext += "\n";
                //----------------------
                returntext += "    <Typ>S</Typ>";
                returntext += "\n";
                //----------------------
                returntext += "    <X>";
                returntext += double_to_qstring(x);
                returntext += "</X>";
                returntext += "\n";
                //----------------------
                returntext += "    <Y>";
                returntext += double_to_qstring(y);
                returntext += "</Y>";
                returntext += "\n";
                //----------------------
                returntext += "    <Z>";
                returntext += double_to_qstring(0);
                returntext += "</Z>";
                returntext += "\n";
                //----------------------
                returntext += "    <Diameter>";
                returntext += double_to_qstring(wkz_dm);
                returntext += "</Diameter>";
                returntext += "\n";
                //----------------------
                returntext += "    <Depth>";
                returntext += double_to_qstring(z);
                returntext += "</Depth>";
                returntext += "\n";
                //----------------------
                returntext += "    <Tool>";
                returntext += wkz_nr;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <SVar>";
                returntext += nutvariante_qstring;
                returntext += "</SVar>";
                returntext += "\n";
                //----------------------
                returntext += "    <SParallel>";
                returntext += nutrichtung;
                returntext += "</SParallel>";
                returntext += "\n";
                //----------------------
                returntext += "    <SGrooveLength>";
                returntext += double_to_qstring(l);
                returntext += "</SGrooveLength>";
                returntext += "\n";
                //----------------------
                returntext += "    <OldID>";
                if(zeile.zeile(2)==BEZUG_FLAECHE_OBEN_ASCII)
                {
                    returntext += "Top";
                }else
                {
                    returntext += "Bottom";
                }
                returntext += "\\";
                returntext += BEZUG_REF_OBEN_LINKS;
                returntext += "\\";
                returntext += "S-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemen einfach die Zeilennummer in der ASCII-Datei
                returntext += "</OldID>";
                returntext += "\n";
                //----------------------
                returntext += "    <KleiGeTei>";
                returntext += "0";
                returntext += "</KleiGeTei>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFileWork>";
                returntext += "\n";

        }else if(zeile.zeile(1)==BEARBEITUNG_FRAES)//Alle arten von Fräsarbeiten
        {
            if(zeile.zeile(3).toDouble() == 101)//Rechtecktaschen
            {
                double x = zeile.zeile(4).toDouble();
                double y = zeile.zeile(5).toDouble();
                double z = zeile.zeile(6).toDouble();
                double lx = zeile.zeile(7).toDouble();
                double by = zeile.zeile(8).toDouble();
                double minmass = lx;
                if(by < minmass)
                {
                    minmass = by;
                }
                QString wkztyp = WKZ_TYP_FRAESER;
                QString wkz_nr = get_wkz_nummer(wkztyp, minmass, z);
                if(wkz_nr.isEmpty())
                {
                    QString msg = "Fehler!\nKein Fraeser im Werkzeugmagazin für:\n" + zeile.get_text();
                    QMessageBox mb;
                    mb.setText(msg);
                    mb.exec();
                    return msg;
                }
                double wkz_dm = get_wkz_dm(wkz_nr).toDouble();
                double wkz_vorschub = get_wkz_vorschub(wkz_nr).toDouble();
                int zustellungen = zeile.zeile(10).toInt();
                if(zustellungen <= 0)
                {
                    zustellungen = 1;
                }

                returntext += "  <PrgrFileWork>";
                returntext += "\n";
                //----------------------
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemen einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                //----------------------
                returntext += "    <Plane>";
                if(zeile.zeile(2)==BEZUG_FLAECHE_OBEN_ASCII)
                {
                    returntext += "Top";
                }else
                {
                    returntext += "Bottom";
                }
                returntext += "</Plane>";
                returntext += "\n";
                //----------------------Bezugskante festlegen:
                QString bezug = BEZUG_REF_OBEN_LINKS;
                //y < 40 -> TL
                //Länge - y < 40 ->BL
                if(laenge_y - y < bezugsmass)
                {
                    bezug = BEZUG_REF_UNTEN_LINKS;
                }
                returntext += "    <Ref>";
                returntext += bezug;
                returntext += "</Ref>";
                returntext += "\n";
                //----------------------
                returntext += "    <Typ>M</Typ>";
                returntext += "\n";
                //----------------------
                returntext += "    <X>";
                returntext += double_to_qstring(x);
                returntext += "</X>";
                returntext += "\n";
                //----------------------
                returntext += "    <Y>";
                returntext += double_to_qstring(y);
                returntext += "</Y>";
                returntext += "\n";
                //----------------------
                returntext += "    <Z>";
                returntext += double_to_qstring(0);
                returntext += "</Z>";
                returntext += "\n";
                //----------------------
                returntext += "    <Tool>";
                returntext += wkz_nr;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <Mill>";
                returntext += "3";                      //Rechtecktasche
                returntext += ";";
                returntext += double_to_qstring(by);    //TAB
                returntext += ";";
                returntext += double_to_qstring(lx);    //TAL
                returntext += ";";
                returntext += double_to_qstring(wkz_dm/2); //Eckenradius Tasche
                returntext += ";";
                returntext += double_to_qstring(z);     //TaTi
                returntext += ";";
                returntext += "1";                      //Variante der Rechtecktasche (1 = ausgeräumt)
                returntext += ";";
                returntext += "GL";                     //Gleichlauf (GL = Gleichlauf / GG = Gegenlauf)
                returntext += ";";
                returntext += double_to_qstring(wkz_vorschub);
                returntext += ";";
                returntext += int_to_qstring(zustellungen);
                returntext += "</Mill>";
                returntext += "\n";
                //----------------------
                returntext += "    <ImageKey>RE</ImageKey>";
                returntext += "\n";
                //----------------------
                returntext += "    <OldID>";
                if(zeile.zeile(2)==BEZUG_FLAECHE_OBEN_ASCII)
                {
                    returntext += "Top";
                }else
                {
                    returntext += "Bottom";
                }
                returntext += "\\";
                returntext += BEZUG_REF_OBEN_LINKS;
                returntext += "\\";
                returntext += "M-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemen einfach die Zeilennummer in der ASCII-Datei
                returntext += "</OldID>";
                returntext += "\n";
                //----------------------
                returntext += "    <KleiGeTei>";
                returntext += "0";
                returntext += "</KleiGeTei>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFileWork>";
                returntext += "\n";



            }else if(zeile.zeile(3).toDouble() == 102)//Kreistaschen
            {
                double x = zeile.zeile(4).toDouble();
                double y = zeile.zeile(5).toDouble();
                double z = zeile.zeile(6).toDouble();
                double dm = zeile.zeile(7).toDouble();
                QString wkztyp = WKZ_TYP_FRAESER;
                QString wkz_nr = get_wkz_nummer(wkztyp, dm, z);
                if(wkz_nr.isEmpty())
                {
                    QString msg = "Fehler!\nKein Fraeser im Werkzeugmagazin für:\n" + zeile.get_text();
                    QMessageBox mb;
                    mb.setText(msg);
                    mb.exec();
                    return msg;
                }
                //double wkz_dm = get_wkz_dm(wkz_nr).toDouble();
                double wkz_vorschub = get_wkz_vorschub(wkz_nr).toDouble();
                int zustellungen = zeile.zeile(8).toInt();
                if(zustellungen <= 0)
                {
                    zustellungen = 1;
                }


                returntext += "  <PrgrFileWork>";
                returntext += "\n";
                //----------------------
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemen einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                //----------------------
                returntext += "    <Plane>";
                if(zeile.zeile(2)==BEZUG_FLAECHE_OBEN_ASCII)
                {
                    returntext += "Top";
                }else
                {
                    returntext += "Bottom";
                }
                returntext += "</Plane>";
                returntext += "\n";
                //----------------------Bezugskante festlegen:
                QString bezug = BEZUG_REF_OBEN_LINKS;
                //y < 40 -> TL
                //Länge - y < 40 ->BL
                if(laenge_y - y < bezugsmass)
                {
                    bezug = BEZUG_REF_UNTEN_LINKS;
                }
                returntext += "    <Ref>";
                returntext += bezug;
                returntext += "</Ref>";
                returntext += "\n";
                //----------------------
                returntext += "    <Typ>M</Typ>";
                returntext += "\n";
                //----------------------
                returntext += "    <X>";
                returntext += double_to_qstring(x);
                returntext += "</X>";
                returntext += "\n";
                //----------------------
                returntext += "    <Y>";
                returntext += double_to_qstring(y);
                returntext += "</Y>";
                returntext += "\n";
                //----------------------
                returntext += "    <Z>";
                returntext += double_to_qstring(0);
                returntext += "</Z>";
                returntext += "\n";
                //----------------------
                returntext += "    <Tool>";
                returntext += wkz_nr;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <Mill>";
                returntext += "2";                      //Kreistasche
                returntext += ";";
                returntext += double_to_qstring(dm);
                returntext += ";";
                returntext += double_to_qstring(z);     //TaTi
                returntext += ";";
                returntext += "2";                      //Variante der Kreistasche (2 = ausgeräumt)
                returntext += ";";
                returntext += "GL";                     //Gleichlauf (GL = Gleichlauf / GG = Gegenlauf)
                returntext += ";";
                returntext += double_to_qstring(wkz_vorschub);
                returntext += ";";
                returntext += int_to_qstring(zustellungen);
                returntext += "</Mill>";
                returntext += "\n";
                //----------------------
                returntext += "    <ImageKey>KR</ImageKey>";
                returntext += "\n";
                //----------------------
                returntext += "    <OldID>";
                if(zeile.zeile(2)==BEZUG_FLAECHE_OBEN_ASCII)
                {
                    returntext += "Top";
                }else
                {
                    returntext += "Bottom";
                }
                returntext += "\\";
                returntext += BEZUG_REF_OBEN_LINKS;
                returntext += "\\";
                returntext += "M-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemen einfach die Zeilennummer in der ASCII-Datei
                returntext += "</OldID>";
                returntext += "\n";
                //----------------------
                returntext += "    <KleiGeTei>";
                returntext += "0";
                returntext += "</KleiGeTei>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFileWork>";
                returntext += "\n";
            }
        }
    }

    //-------------------------Bearbeitungen <PrgrFile>:
    for(uint i=4 ; i<=tz.zeilenanzahl() ; i++)
    {
        zeile.set_text(tz.zeile(i));
        if(zeile.zeile(1)==BEARBEITUNG_BOHRUNG)
        {
            double x = zeile.zeile(4).toDouble();
            double y = zeile.zeile(5).toDouble();
            double z = zeile.zeile(6).toDouble();
            double dm = zeile.zeile(7).toDouble();

            if(zeile.zeile(2)==BEZUG_FLAECHE_OBEN_ASCII)
            {
                //x = Breite
                //y = Länge
                //z = Tiefe
                returntext += "  <PrgrFile>";
                returntext += "\n";
                //----------------------
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                //----------------------
                QString bezug = BEZUG_REF_OBEN_LINKS;
                //y < 40 -> TL
                //Länge - y < 40 ->BL
                if(laenge_y - y < bezugsmass)
                {
                    bezug = BEZUG_REF_UNTEN_LINKS;
                    y = laenge_y - y;
                }
                //----------------------
                returntext += "    <ID>";
                returntext += "Top";
                returntext += "\\";
                returntext += bezug;
                returntext += "\\";
                returntext += "B-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</ID>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefVal1>";
                returntext += double_to_qstring(y).replace(".",",");
                returntext += "</RefVal1>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefVal2>";
                returntext += double_to_qstring(x).replace(".",",");
                returntext += "</RefVal2>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefLVL>";
                returntext += double_to_qstring(0).replace(".",",");
                returntext += "</RefLVL>";
                returntext += "\n";
                //----------------------
                returntext += "    <Diameter>";
                returntext += double_to_qstring(dm).replace(".",",");
                returntext += "</Diameter>";
                returntext += "\n";
                //----------------------
                returntext += "   <Depth>";
                returntext += double_to_qstring(z).replace(".",",");
                returntext += "</Depth>";
                returntext += "\n";
                //----------------------
                returntext += "    <DoDbl>false</DoDbl>";
                returntext += "\n";
                //----------------------
                returntext += "    <UsedDbl>ERR</UsedDbl>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblB>true</DblB>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblL>false</DblL>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblE>false</DblE>";
                returntext += "\n";
                //----------------------
                returntext += "    <DoMF>false</DoMF>";
                returntext += "\n";
                //----------------------
                returntext += "    <MFLage>B</MFLage>";
                returntext += "\n";
                //----------------------
                //----------------------Werkzeugnummer:
                returntext += "    <Tool>";

                QString tnummer = get_wkz_nummer(WKZ_TYP_BOHRER, dm, z);
                if(tnummer.isEmpty())
                {
                    QString msg = "Fehler!\nKein Bohrer im Werkzeugmagazin für:\n" + zeile.get_text();
                    QMessageBox mb;
                    mb.setText(msg);
                    mb.exec();
                    return msg;
                }
                returntext += tnummer;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <Cyclic>1</Cyclic>";
                returntext += "\n";
                //----------------------
                returntext += "    <ImageKey>B</ImageKey>>";
                returntext += "\n";
                //----------------------
                returntext += "    <Step>1</Step>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFile>";
                returntext += "\n";

            }else if(zeile.zeile(2)==BEZUG_FLAECHE_UNTEN_ASCII)
            {
                //x = Breite
                //y = Länge
                //z = Tiefe
                returntext += "  <PrgrFile>";
                returntext += "\n";
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                //----------------------
                QString bezug = BEZUG_REF_OBEN_LINKS;
                //y < 40 -> TL
                //Länge - y < 40 ->BL
                if(laenge_y - y < bezugsmass)
                {
                    bezug = BEZUG_REF_UNTEN_LINKS;
                    y = laenge_y - y;
                }
                //----------------------
                returntext += "    <ID>";
                returntext += "Bottom";
                returntext += "\\";
                returntext += bezug;
                returntext += "\\";
                returntext += "B-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</ID>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefVal1>";
                returntext += double_to_qstring(y).replace(".",",");
                returntext += "</RefVal1>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefVal2>";
                returntext += double_to_qstring(x).replace(".",",");
                returntext += "</RefVal2>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefLVL>";
                returntext += double_to_qstring(0).replace(".",",");
                returntext += "</RefLVL>";
                returntext += "\n";
                //----------------------
                returntext += "    <Diameter>";
                returntext += double_to_qstring(dm).replace(".",",");
                returntext += "</Diameter>";
                returntext += "\n";
                //----------------------
                returntext += "   <Depth>";
                returntext += double_to_qstring(z).replace(".",",");
                returntext += "</Depth>";
                returntext += "\n";
                //----------------------
                returntext += "    <DoDbl>false</DoDbl>";
                returntext += "\n";
                //----------------------
                returntext += "    <UsedDbl>ERR</UsedDbl>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblB>true</DblB>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblL>false</DblL>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblE>false</DblE>";
                returntext += "\n";
                //----------------------
                returntext += "    <DoMF>false</DoMF>";
                returntext += "\n";
                //----------------------
                returntext += "    <MFLage>B</MFLage>";
                returntext += "\n";
                //----------------------
                //----------------------Werkzeugnummer:
                returntext += "    <Tool>";

                QString tnummer = get_wkz_nummer(WKZ_TYP_BOHRER, dm, z);
                if(tnummer.isEmpty())
                {
                    QString msg = "Fehler!\nKein Bohrer im Werkzeugmagazin für:\n" + zeile.get_text();
                    QMessageBox mb;
                    mb.setText(msg);
                    mb.exec();
                    return msg;
                }
                returntext += tnummer;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <Cyclic>1</Cyclic>";
                returntext += "\n";
                //----------------------
                returntext += "    <ImageKey>B</ImageKey>>";
                returntext += "\n";
                //----------------------
                returntext += "    <Step>1</Step>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFile>";
                returntext += "\n";

            }else if(zeile.zeile(2)==BEZUG_FLAECHE_LINKS_ASCII)
            {
                //x = Tiefe
                //y = Y-Pos
                //z = Z-Pos
                returntext += "  <PrgrFile>";
                returntext += "\n";
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                //----------------------
                QString bezug = BEZUG_REF_OBEN_LINKS;
                //----------------------
                returntext += "    <ID>";
                returntext += "Left";
                returntext += "\\";
                returntext += bezug;
                returntext += "\\";
                returntext += "B-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</ID>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefVal1>";
                returntext += double_to_qstring(y).replace(".",",");
                returntext += "</RefVal1>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefVal2>";
                returntext += double_to_qstring(z).replace(".",",");
                returntext += "</RefVal2>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefLVL>";
                returntext += double_to_qstring(0).replace(".",",");
                returntext += "</RefLVL>";
                returntext += "\n";
                //----------------------
                returntext += "    <Diameter>";
                returntext += double_to_qstring(dm).replace(".",",");
                returntext += "</Diameter>";
                returntext += "\n";
                //----------------------
                returntext += "   <Depth>";
                returntext += double_to_qstring(x).replace(".",",");
                returntext += "</Depth>";
                returntext += "\n";
                //----------------------
                returntext += "    <DoDbl>false</DoDbl>";
                returntext += "\n";
                //----------------------
                returntext += "    <UsedDbl>ERR</UsedDbl>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblB>true</DblB>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblL>false</DblL>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblE>false</DblE>";
                returntext += "\n";
                //----------------------
                returntext += "    <DoMF>false</DoMF>";
                returntext += "\n";
                //----------------------
                returntext += "    <MFLage>B</MFLage>";
                returntext += "\n";
                //----------------------
                //----------------------Werkzeugnummer:
                returntext += "    <Tool>";

                QString tnummer = get_wkz_nummer(WKZ_TYP_BOHRER, dm, x);
                if(tnummer.isEmpty())
                {
                    QString msg = "Fehler!\nKein Bohrer im Werkzeugmagazin für:\n" + zeile.get_text();
                    QMessageBox mb;
                    mb.setText(msg);
                    mb.exec();
                    return msg;
                }
                returntext += tnummer;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <Cyclic>1</Cyclic>";
                returntext += "\n";
                //----------------------
                returntext += "    <ImageKey>B</ImageKey>>";
                returntext += "\n";
                //----------------------
                returntext += "    <Step>1</Step>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFile>";
                returntext += "\n";

            }else if(zeile.zeile(2)==BEZUG_FLAECHE_RECHTS_ASCII)
            {
                //x = Tiefe
                //y = Y-Pos
                //z = Z-Pos
                returntext += "  <PrgrFile>";
                returntext += "\n";
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                //----------------------
                QString bezug = BEZUG_REF_OBEN_RECHTS;
                //----------------------
                returntext += "    <ID>";
                returntext += "Right";
                returntext += "\\";
                returntext += bezug;
                returntext += "\\";
                returntext += "B-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</ID>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefVal1>";
                returntext += double_to_qstring(y).replace(".",",");
                returntext += "</RefVal1>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefVal2>";
                returntext += double_to_qstring(z).replace(".",",");
                returntext += "</RefVal2>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefLVL>";
                returntext += double_to_qstring(0).replace(".",",");
                returntext += "</RefLVL>";
                returntext += "\n";
                //----------------------
                returntext += "    <Diameter>";
                returntext += double_to_qstring(dm).replace(".",",");
                returntext += "</Diameter>";
                returntext += "\n";
                //----------------------
                returntext += "   <Depth>";
                returntext += double_to_qstring(x).replace(".",",");
                returntext += "</Depth>";
                returntext += "\n";
                //----------------------
                returntext += "    <DoDbl>false</DoDbl>";
                returntext += "\n";
                //----------------------
                returntext += "    <UsedDbl>ERR</UsedDbl>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblB>true</DblB>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblL>false</DblL>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblE>false</DblE>";
                returntext += "\n";
                //----------------------
                returntext += "    <DoMF>false</DoMF>";
                returntext += "\n";
                //----------------------
                returntext += "    <MFLage>B</MFLage>";
                returntext += "\n";
                //----------------------
                //----------------------Werkzeugnummer:
                returntext += "    <Tool>";

                QString tnummer = get_wkz_nummer(WKZ_TYP_BOHRER, dm, x);
                if(tnummer.isEmpty())
                {
                    QString msg = "Fehler!\nKein Bohrer im Werkzeugmagazin für:\n" + zeile.get_text();
                    QMessageBox mb;
                    mb.setText(msg);
                    mb.exec();
                    return msg;
                }
                returntext += tnummer;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <Cyclic>1</Cyclic>";
                returntext += "\n";
                //----------------------
                returntext += "    <ImageKey>B</ImageKey>>";
                returntext += "\n";
                //----------------------
                returntext += "    <Step>1</Step>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFile>";
                returntext += "\n";

            }else if(zeile.zeile(2)==BEZUG_FLAECHE_VORNE_ASCII)
            {
                //x = x-Pos
                //y = Tiefe
                //z = Z-Pos
                returntext += "  <PrgrFile>";
                returntext += "\n";
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                //----------------------
                QString bezug = BEZUG_REF_UNTEN_LINKS;
                //----------------------
                returntext += "    <ID>";
                returntext += "Front";
                returntext += "\\";
                returntext += bezug;
                returntext += "\\";
                returntext += "B-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</ID>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefVal1>";
                returntext += double_to_qstring(z).replace(".",",");
                returntext += "</RefVal1>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefVal2>";
                returntext += double_to_qstring(x).replace(".",",");
                returntext += "</RefVal2>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefLVL>";
                returntext += double_to_qstring(0).replace(".",",");
                returntext += "</RefLVL>";
                returntext += "\n";
                //----------------------
                returntext += "    <Diameter>";
                returntext += double_to_qstring(dm).replace(".",",");
                returntext += "</Diameter>";
                returntext += "\n";
                //----------------------
                returntext += "   <Depth>";
                returntext += double_to_qstring(y).replace(".",",");
                returntext += "</Depth>";
                returntext += "\n";
                //----------------------
                returntext += "    <DoDbl>false</DoDbl>";
                returntext += "\n";
                //----------------------
                returntext += "    <UsedDbl>ERR</UsedDbl>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblB>true</DblB>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblL>false</DblL>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblE>false</DblE>";
                returntext += "\n";
                //----------------------
                returntext += "    <DoMF>false</DoMF>";
                returntext += "\n";
                //----------------------
                returntext += "    <MFLage>B</MFLage>";
                returntext += "\n";
                //----------------------
                //----------------------Werkzeugnummer:
                returntext += "    <Tool>";

                QString tnummer = get_wkz_nummer(WKZ_TYP_BOHRER, dm, y);
                if(tnummer.isEmpty())
                {
                    QString msg = "Fehler!\nKein Bohrer im Werkzeugmagazin für:\n" + zeile.get_text();
                    QMessageBox mb;
                    mb.setText(msg);
                    mb.exec();
                    return msg;
                }
                returntext += tnummer;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <Cyclic>1</Cyclic>";
                returntext += "\n";
                //----------------------
                returntext += "    <ImageKey>B</ImageKey>>";
                returntext += "\n";
                //----------------------
                returntext += "    <Step>1</Step>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFile>";
                returntext += "\n";
            }else if(zeile.zeile(2)==BEZUG_FLAECHE_HINTEN_ASCII)
            {
                //x = x-Pos
                //y = Tiefe
                //z = Z-Pos
                returntext += "  <PrgrFile>";
                returntext += "\n";
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                //----------------------
                QString bezug = BEZUG_REF_OBEN_LINKS;
                //----------------------
                returntext += "    <ID>";
                returntext += "Rear";
                returntext += "\\";
                returntext += bezug;
                returntext += "\\";
                returntext += "B-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</ID>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefVal1>";
                returntext += double_to_qstring(z).replace(".",",");
                returntext += "</RefVal1>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefVal2>";
                returntext += double_to_qstring(x).replace(".",",");
                returntext += "</RefVal2>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefLVL>";
                returntext += double_to_qstring(0).replace(".",",");
                returntext += "</RefLVL>";
                returntext += "\n";
                //----------------------
                returntext += "    <Diameter>";
                returntext += double_to_qstring(dm).replace(".",",");
                returntext += "</Diameter>";
                returntext += "\n";
                //----------------------
                returntext += "   <Depth>";
                returntext += double_to_qstring(y).replace(".",",");
                returntext += "</Depth>";
                returntext += "\n";
                //----------------------
                returntext += "    <DoDbl>false</DoDbl>";
                returntext += "\n";
                //----------------------
                returntext += "    <UsedDbl>ERR</UsedDbl>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblB>true</DblB>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblL>false</DblL>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblE>false</DblE>";
                returntext += "\n";
                //----------------------
                returntext += "    <DoMF>false</DoMF>";
                returntext += "\n";
                //----------------------
                returntext += "    <MFLage>B</MFLage>";
                returntext += "\n";
                //----------------------
                //----------------------Werkzeugnummer:
                returntext += "    <Tool>";

                QString tnummer = get_wkz_nummer(WKZ_TYP_BOHRER, dm, y);
                if(tnummer.isEmpty())
                {
                    QString msg = "Fehler!\nKein Bohrer im Werkzeugmagazin für:\n" + zeile.get_text();
                    QMessageBox mb;
                    mb.setText(msg);
                    mb.exec();
                    return msg;
                }
                returntext += tnummer;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <Cyclic>1</Cyclic>";
                returntext += "\n";
                //----------------------
                returntext += "    <ImageKey>B</ImageKey>>";
                returntext += "\n";
                //----------------------
                returntext += "    <Step>1</Step>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFile>";
                returntext += "\n";
            }
        }else if(zeile.zeile(1)==BEARBEITUNG_NUT)
        {
            double x = zeile.zeile(4).toDouble();
            double y = zeile.zeile(5).toDouble();
            double z = zeile.zeile(6).toDouble();
            double l = zeile.zeile(7).toDouble();
            QString wkztyp = WKZ_TYP_SAEGE;
            QString wkz_nr = get_wkz_nummer(wkztyp);
            //double wkz_dm = get_wkz_dm(wkz_nr).toDouble();
            double nutvariante = zeile.zeile(9).toDouble();
            QString nutvariante_qstring = "";
            if(nutvariante == 1)
            {
                nutvariante_qstring = "Var2";
            }else if(nutvariante == 2)
            {
                nutvariante_qstring = "Var1";
            }else
            {
                QString msg = "Fehler!\nKeine keine Nut-Variante angegeben:\n" + zeile.get_text();
                QMessageBox mb;
                mb.setText(msg);
                mb.exec();
                return msg;
            }
            QString nutrichtung;
            if (zeile.zeile(8).toDouble() == 1)
            {
                nutrichtung = "X";
            }else
            {
                nutrichtung = "Y";
            }

            returntext += "  <PrgrFile>";
            returntext += "\n";
            //----------------------
            returntext += "    <CntID>";
            returntext += int_to_qstring(i);               //ID-Nummer, wir nehemen einfach die Zeilennummer in der ASCII-Datei
            returntext += "</CntID>";
            returntext += "\n";
            //----------------------
            returntext += "    <ID>";
            if(zeile.zeile(2)==BEZUG_FLAECHE_OBEN_ASCII)
            {
                returntext += "Top";
            }else
            {
                returntext += "Bottom";
            }
            returntext += "\\";
            returntext += BEZUG_REF_OBEN_LINKS;
            returntext += "\\";
            returntext += "S-";
            returntext += int_to_qstring(i);               //ID-Nummer, wir nehemen einfach die Zeilennummer in der ASCII-Datei
            returntext += "</ID>";
            returntext += "\n";
            //----------------------
            returntext += "    <RefVal1>";
            returntext += double_to_qstring(y).replace(".",",");
            returntext += "</RefVal1>";
            returntext += "\n";
            //----------------------
            returntext += "    <RefVal2>";
            returntext += double_to_qstring(x).replace(".",",");
            returntext += "</RefVal2>";
            returntext += "\n";
            //----------------------
            returntext += "    <Diameter>";
            returntext += "0";
            returntext += "</Diameter>";
            returntext += "\n";
            //----------------------
            returntext += "    <Depth>";
            returntext += double_to_qstring(z).replace(".",",");
            returntext += "</Depth>";
            returntext += "\n";
            //----------------------
            returntext += "    <DoDbl>false</DoDbl>";
            returntext += "\n";
            //----------------------
            returntext += "    <DblB>false</DblB>";
            returntext += "\n";
            //----------------------
            returntext += "    <DblL>false</DblL>";
            returntext += "\n";
            //----------------------
            returntext += "    <DblE>false</DblE>";
            returntext += "\n";
            //----------------------
            returntext += "    <DoMF>false</DoMF>";
            returntext += "\n";
            //----------------------
            returntext += "    <Tool>";
            returntext += wkz_nr;
            returntext += "</Tool>";
            returntext += "\n";
            //----------------------
            returntext += "    <SVar>";
            returntext += nutvariante_qstring;
            returntext += "</SVar>";
            returntext += "\n";
            //----------------------
            returntext += "    <SParallel>";
            returntext += nutrichtung;
            returntext += "</SParallel>";
            returntext += "\n";
            //----------------------
            returntext += "    <SGrooveLength>";
            returntext += double_to_qstring(l);
            returntext += "</SGrooveLength>";
            returntext += "\n";
            //----------------------
            returntext += "    <Cyclic>0</Cyclic>";
            returntext += "\n";
            //----------------------
            returntext += "    <ImageKey>S</ImageKey>";
            returntext += "\n";
            //----------------------
            returntext += "    <Step>1</Step>";
            returntext += "\n";
            //----------------------
            returntext += "  </PrgrFile>";
            returntext += "\n";
            //----------------------

        }else if(zeile.zeile(1)==BEARBEITUNG_FRAES)//Alle arten von Fräsarbeiten
        {
            if(zeile.zeile(3).toDouble() == 101)//Rechtecktaschen
            {
                double x = zeile.zeile(4).toDouble();
                double y = zeile.zeile(5).toDouble();
                double z = zeile.zeile(6).toDouble();
                double lx = zeile.zeile(7).toDouble();
                double by = zeile.zeile(8).toDouble();
                double minmass = lx;
                if(by < minmass)
                {
                    minmass = by;
                }
                QString wkztyp = WKZ_TYP_FRAESER;
                QString wkz_nr = get_wkz_nummer(wkztyp, minmass, z);
                double wkz_dm = get_wkz_dm(wkz_nr).toDouble();
                double wkz_vorschub = get_wkz_vorschub(wkz_nr).toDouble();
                int zustellungen = zeile.zeile(10).toInt();
                if(zustellungen <= 0)
                {
                    zustellungen = 1;
                }

                returntext += "  <PrgrFile>";
                returntext += "\n";
                //----------------------
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemen einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                //----------------------
                QString bezug = BEZUG_REF_OBEN_LINKS;
                //y < 40 -> TL
                //Länge - y < 40 ->BL
                if(laenge_y - y < bezugsmass)
                {
                    bezug = BEZUG_REF_UNTEN_LINKS;
                    y = laenge_y - y;
                }
                //----------------------
                returntext += "    <ID>";
                if(zeile.zeile(2)==BEZUG_FLAECHE_OBEN_ASCII)
                {
                    returntext += "Top";
                }else
                {
                    returntext += "Bottom";
                }
                returntext += "\\";
                returntext += bezug;
                returntext += "\\";
                returntext += "M-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</ID>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefVal1>";
                returntext += double_to_qstring(y).replace(".",",");
                returntext += "</RefVal1>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefVal2>";
                returntext += double_to_qstring(x).replace(".",",");
                returntext += "</RefVal2>";
                returntext += "\n";
                //----------------------
                //----------------------
                returntext += "    <Diameter>";
                returntext += "0";
                returntext += "</Diameter>";
                returntext += "\n";
                //----------------------
                returntext += "    <Depth>";
                returntext += double_to_qstring(z).replace(".",",");
                returntext += "</Depth>";
                returntext += "\n";
                //----------------------
                returntext += "    <DoDbl>false</DoDbl>";
                returntext += "\n";
                //----------------------
                returntext += "    <UsedDbl>ERR</UsedDbl>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblB>false</DblB>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblL>false</DblL>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblE>false</DblE>";
                returntext += "\n";
                //----------------------
                returntext += "    <DoMF>false</DoMF>";
                returntext += "\n";
                //----------------------
                returntext += "    <Tool>";
                returntext += wkz_nr;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <Cyclic>0</Cyclic>";
                returntext += "\n";
                //----------------------
                returntext += "    <Mill>";
                returntext += "3";                      //Rechtecktasche
                returntext += ";";
                returntext += double_to_qstring(by);    //TAB
                returntext += ";";
                returntext += double_to_qstring(lx);    //TAL
                returntext += ";";
                returntext += double_to_qstring(wkz_dm/2); //Eckenradius Tasche
                returntext += ";";
                returntext += double_to_qstring(z);     //TaTi
                returntext += ";";
                returntext += "1";                      //Variante der Rechtecktasche (1 = ausgeräumt)
                returntext += ";";
                returntext += "GL";                     //Gleichlauf (GL = Gleichlauf / GG = Gegenlauf)
                returntext += ";";
                returntext += double_to_qstring(wkz_vorschub);
                returntext += ";";
                returntext += int_to_qstring(zustellungen);
                returntext += "</Mill>";
                returntext += "\n";
                //----------------------
                returntext += "    <ImageKey>RE</ImageKey>";
                returntext += "\n";
                //----------------------
                returntext += "    <Step>1</Step>";
                returntext += "\n";
                //----------------------
                returntext += "    <Clause>0</Clause>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFile>";
                returntext += "\n";
            }else if(zeile.zeile(3).toDouble() == 102)//Kreistaschen
            {
                double x = zeile.zeile(4).toDouble();
                double y = zeile.zeile(5).toDouble();
                double z = zeile.zeile(6).toDouble();
                double dm = zeile.zeile(7).toDouble();
                QString wkztyp = WKZ_TYP_FRAESER;
                QString wkz_nr = get_wkz_nummer(wkztyp, dm, z);
                //double wkz_dm = get_wkz_dm(wkz_nr).toDouble();
                double wkz_vorschub = get_wkz_vorschub(wkz_nr).toDouble();
                int zustellungen = zeile.zeile(8).toInt();
                if(zustellungen <= 0)
                {
                    zustellungen = 1;
                }

                returntext += "  <PrgrFile>";
                returntext += "\n";
                //----------------------
                returntext += "    <CntID>";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemen einfach die Zeilennummer in der ASCII-Datei
                returntext += "</CntID>";
                returntext += "\n";
                //----------------------
                QString bezug = BEZUG_REF_OBEN_LINKS;
                //y < 40 -> TL
                //Länge - y < 40 ->BL
                if(laenge_y - y < bezugsmass)
                {
                    bezug = BEZUG_REF_UNTEN_LINKS;
                    y = laenge_y - y;
                }
                //----------------------
                returntext += "    <ID>";
                if(zeile.zeile(2)==BEZUG_FLAECHE_OBEN_ASCII)
                {
                    returntext += "Top";
                }else
                {
                    returntext += "Bottom";
                }
                returntext += "\\";
                returntext += bezug;
                returntext += "\\";
                returntext += "M-";
                returntext += int_to_qstring(i);               //ID-Nummer, wir nehemn einfach die Zeilennummer in der ASCII-Datei
                returntext += "</ID>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefVal1>";
                returntext += double_to_qstring(y).replace(".",",");
                returntext += "</RefVal1>";
                returntext += "\n";
                //----------------------
                returntext += "    <RefVal2>";
                returntext += double_to_qstring(x).replace(".",",");
                returntext += "</RefVal2>";
                returntext += "\n";
                //----------------------
                returntext += "    <Diameter>";
                returntext += "0";
                returntext += "</Diameter>";
                returntext += "\n";
                //----------------------
                returntext += "    <Depth>";
                returntext += double_to_qstring(z).replace(".",",");
                returntext += "</Depth>";
                returntext += "\n";
                //----------------------
                returntext += "    <DoDbl>false</DoDbl>";
                returntext += "\n";
                //----------------------
                returntext += "    <UsedDbl>ERR</UsedDbl>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblB>false</DblB>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblL>false</DblL>";
                returntext += "\n";
                //----------------------
                returntext += "    <DblE>false</DblE>";
                returntext += "\n";
                //----------------------
                returntext += "    <DoMF>false</DoMF>";
                returntext += "\n";
                //----------------------
                returntext += "    <Tool>";
                returntext += wkz_nr;
                returntext += "</Tool>";
                returntext += "\n";
                //----------------------
                returntext += "    <Cyclic>0</Cyclic>";
                returntext += "\n";
                //----------------------
                returntext += "    <Mill>";
                returntext += "2";                      //Kreistasche
                returntext += ";";
                returntext += double_to_qstring(dm);
                returntext += ";";
                returntext += double_to_qstring(z);     //TaTi
                returntext += ";";
                returntext += "2";                      //Variante der Kreistasche (2 = ausgeräumt)
                returntext += ";";
                returntext += "GL";                     //Gleichlauf (GL = Gleichlauf / GG = Gegenlauf)
                returntext += ";";
                returntext += double_to_qstring(wkz_vorschub);
                returntext += ";";
                returntext += int_to_qstring(zustellungen);
                returntext += "</Mill>";
                returntext += "\n";
                //----------------------
                returntext += "    <ImageKey>KR</ImageKey>";
                returntext += "\n";
                //----------------------
                returntext += "    <Step>1</Step>";
                returntext += "\n";
                //----------------------
                returntext += "    <Clause>0</Clause>";
                returntext += "\n";
                //----------------------
                returntext += "  </PrgrFile>";
                returntext += "\n";
            }
        }
    }

    //-------------------------Programmende:
    returntext += "</Programm>";

    return returntext;
}

QString MainWindow::get_wkz_nummer(QString wkz_typ, double dm, double bearbeitungstiefe)
{
    QString returntext = "";
    text_zeilenweise zeile;
    zeile.set_trennzeichen('\t');
    double wkz_dm_tmp = 0;

    for(uint i = 2; i<=werkzeug.zeilenanzahl() ;i++)
    {
        zeile.set_text(werkzeug.zeile(i));

        if(  (zeile.zeile(1) == wkz_typ)  &&  (wkz_typ == WKZ_TYP_BOHRER)  )
        {
            if(zeile.zeile(3).toDouble() == dm)
            {
                if(zeile.zeile(4).toDouble() > bearbeitungstiefe)
                {
                    returntext = zeile.zeile(2);
                }
            }            
        }else if(  (zeile.zeile(1) == wkz_typ)  &&  (wkz_typ == WKZ_TYP_SAEGE)  )
        {
            returntext = zeile.zeile(2);
        }else if(  (zeile.zeile(1) == wkz_typ)  &&  (wkz_typ == WKZ_TYP_FRAESER)  )
        {
            double wkz_dm = zeile.zeile(3).toDouble();
            if(  (wkz_dm <= dm-4)  &&  (wkz_dm > wkz_dm_tmp)  )
            {
                if(zeile.zeile(4).toDouble() > bearbeitungstiefe)
                {
                    wkz_dm_tmp = wkz_dm;
                    returntext = zeile.zeile(2);
                }
            }
        }
    }   
    return returntext;
}

QString MainWindow::get_wkz_dm(QString wkz_nr)
{
    QString returntext = "";
    text_zeilenweise zeile;
    zeile.set_trennzeichen('\t');

    for(uint i = 2; i<=werkzeug.zeilenanzahl() ;i++)
    {
        zeile.set_text(werkzeug.zeile(i));

        if(zeile.zeile(2) == wkz_nr)
        {
            returntext = zeile.zeile(3);
        }

    }
    return returntext;
}

QString MainWindow::get_wkz_vorschub(QString wkz_nr)
{
    QString returntext = "";
    text_zeilenweise zeile;
    zeile.set_trennzeichen('\t');

    for(uint i = 2; i<=werkzeug.zeilenanzahl() ;i++)
    {
        zeile.set_text(werkzeug.zeile(i));

        if(zeile.zeile(2) == wkz_nr)
        {
            returntext = zeile.zeile(5);
        }

    }
    return returntext;
}

text_zeilenweise MainWindow::ascii_optimieren(text_zeilenweise dateiinhalt)
{
    for(uint i=1; i<=dateiinhalt.zeilenanzahl() ;i++)
    {
        text_zeilenweise zeile;
        zeile.set_trennzeichen(';');
        zeile.set_text(dateiinhalt.zeile(i));

        if(zeile.zeile(1)==BEARBEITUNG_FRAES)//Alle arten von Fräsarbeiten
        {
            if(zeile.zeile(3).toDouble() == 101)//Rechtecktaschen
            {
                if(zeile.zeilenanzahl() >= 12)//12er Eintrag ist der Drehwinkel ->nicht ganner-ascii-konform, daher prüfen ob vorhanden
                {
                    double drewi = zeile.zeile(12).toDouble();
                    if(  (drewi == 90)  ||
                         (drewi == -90) ||
                         (drewi == 270) ||
                         (drewi == -270)  )
                    {
                        QString l = zeile.zeile(7);
                        QString b = zeile.zeile(8);
                        zeile.zeile_ersaetzen(7, b);
                        zeile.zeile_ersaetzen(8, l);
                    }
                }
            }
        }
        dateiinhalt.zeile_ersaetzen(i, zeile.get_text());

    }
    return dateiinhalt;
}

QString MainWindow::ascii_optimieren(QString dateiinhalt)
{
    text_zeilenweise tz;
    tz.set_text(dateiinhalt);
    tz = ascii_optimieren(tz);
    return tz.get_text();
}
