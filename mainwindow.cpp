#include "mainwindow.h"
#include "ui_mainwindow.h"

#define VERSION "2017.05.22"

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setup()
{
    bool inifile_gefunden = false;
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
                }
            }
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
        file.write("verzeichnis_quelle:");
        file.write(verzeichnis_quelle.toUtf8());
        file.write("\n");

        file.write("verzeichnis_ziel:");
        file.write(verzeichnis_ziel.toUtf8());
        file.write("\n");

        file.write("prefix1:");
        file.write(prefix1.toUtf8());
        file.write("\n");

        file.write("prefix2:");
        file.write(prefix2.toUtf8());
        file.write("\n");

        file.write("quelldateien_erhalten:");
        file.write(quelldateien_erhalten.toUtf8());
        file.write("\n");

        file.write("std_namen:");
        file.write(std_namen.toUtf8());
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
    QString tmp = QFileDialog::getExistingDirectory(this, tr("Zielverzeichniss"), verzeichnis_ziel);
    if(!tmp.isEmpty())
    {
        verzeichnis_ziel = tmp;
        ui->lineEdit_ziel->setText(verzeichnis_ziel);
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

            if(std_namen == "ja")
            {
                postfixe.zeile_ersaetzen(i,namen_durch_std_namen_tauschen(postfixe.zeile(i)));
            }

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
                if(ui->plainTextEdit_Meldungsfenster->toPlainText().isEmpty() || i==1)
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

            dateiinhalt = bearbeitung_auf_die_Unterseite(dateiinhalt, prefix1);

            if(std_namen == "ja")
            {
                postfixe.zeile_ersaetzen(i,namen_durch_std_namen_tauschen(postfixe.zeile(i)));
            }

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
                if(ui->plainTextEdit_Meldungsfenster->toPlainText().isEmpty() || i==1)
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

            dateiinhalt = bearbeitung_auf_die_Unterseite(dateiinhalt, prefix2);

            if(std_namen == "ja")
            {
                postfixe.zeile_ersaetzen(i,namen_durch_std_namen_tauschen(postfixe.zeile(i)));
            }

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
                if(ui->plainTextEdit_Meldungsfenster->toPlainText().isEmpty() || i==1)
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
    }
}

QString MainWindow::bearbeitung_auf_die_Unterseite(QString dateitext, QString prefix)
{
    text_zeilenweise dateitext_tz;
    dateitext_tz.set_text(dateitext);

    text_zeilenweise programmkopf;
    programmkopf.set_trennzeichen(';');
    programmkopf.set_text(dateitext_tz.zeile(2));
    //Werkstücklänge merken für später (Y-Maß):
    QString tmp = programmkopf.zeile(3);
    double laenge = tmp.toDouble();
    //Werkstückdicke merken für später (Z-Maß):
    tmp = programmkopf.zeile(4);
    double dicke = tmp.toDouble();
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
        if(zeile.zeile(1)=="H")//Bohrungen
        {
            if(zeile.zeile(2)==" TOP")
            {
                //Bezugskante:
                zeile.zeile_ersaetzen(2, " BOT");
                //Y-Maß neu berechnen:
                QString mass_alt_qstring = zeile.zeile(5);
                double mass = mass_alt_qstring.toDouble();
                mass = laenge - mass;
                zeile.zeile_ersaetzen(5, double_to_qstring(mass));
            }else if(zeile.zeile(2)==" REA")//Hinten
            {
                //Bezugskante:
                zeile.zeile_ersaetzen(2, " FRO");//Vorne
                //Z-Höhe:
                QString mass_alt_qstring = zeile.zeile(6);
                double mass = mass_alt_qstring.toDouble();
                mass = dicke - mass;
                zeile.zeile_ersaetzen(6, double_to_qstring(mass));
            }else if(zeile.zeile(2)==" FRO")//Vorne
            {
                //Bezugskante:
                zeile.zeile_ersaetzen(2, " REA");//Vorne
                //Z-Höhe:
                QString mass_alt_qstring = zeile.zeile(6);
                double mass = mass_alt_qstring.toDouble();
                mass = dicke - mass;
                zeile.zeile_ersaetzen(6, double_to_qstring(mass));
            }else if(zeile.zeile(2)==" LEF" ||  zeile.zeile(2)==" RIG")//Links oder Rechts
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

        }else if(zeile.zeile(1)=="S")//Nuten
        {
            //Bezugskante:
            zeile.zeile_ersaetzen(2, " BOT");
            //Angenommen wird, das nur Nuten in Y-Richtung ausgegeben werden (nur das kann die CNC)
            //Angenommen wird, dass nur durchgehende Nuten ausgegeben werden (Nur das kann VW derzeit)
        }else if(zeile.zeile(1)=="M")//Alle arten von Fräsarbeiten
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
                zeile.zeile_ersaetzen(2, " BOT");
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
                zeile.zeile_ersaetzen(2, " BOT");
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



