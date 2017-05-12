#include "mainwindow.h"
#include "ui_mainwindow.h"

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
            file.write("_h.ascii");
            file.write("\n");
            prefix1 = "_h.ascii";
            ui->lineEdit_prefix1->setText(prefix1);

            file.write("prefix2:");
            file.write("_n.ascii");
            file.write("\n");
            prefix2 = "_n.ascii";
            ui->lineEdit_prefix2->setText(prefix2);

            ui->checkBox_quelldateien_erhalten->setChecked(true);
            file.write("quelldateien_erhalten:ja");
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
    }
    file.close();
}

void MainWindow::on_actionInfo_triggered()
{
    QString tmp;
    tmp = "ASCII-Assistent / Version: 2017.05 / Lizenz:  GPL / Autor: Oliver Schuft\n";
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
void MainWindow::on_checkBox_quelldateien_erhalten_stateChanged(int arg1)
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
    dateien_erfassen();
    ui->plainTextEdit_Meldungsfenster->setPlainText(dateien_alle.get_text());
}

void MainWindow::on_pushButton_Start_clicked()
{

}





