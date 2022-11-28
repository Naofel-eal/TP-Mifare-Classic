#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>

#include "MfErrNo.h"
#include "Core.h"
#include "Sw_Device.h"
#include "Sw_Mf_Classic.h"
#include "Sw_Poll.h"
#include "Sw_ISO14443A-3.h"
#include "TypeDefs.h"
#include "Tools.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

ReaderName MonLecteur;
BOOL bench = FALSE;
uint8_t sect_count = 0;


void MainWindow::on_btn_connect_clicked()
{
    int16_t status = MI_OK;
    MonLecteur.Type = ReaderCDC;
    MonLecteur.device = 0;

    status = OpenCOM(&MonLecteur);
    qDebug() << "OpenCOM" << status;
    RF_Power_Control(&MonLecteur, TRUE, 0);

    status = Version(&MonLecteur);
    ui->Affichage->setText(MonLecteur.version);
    ui->Affichage->update();

}

void MainWindow::on_btn_disconnect_clicked()
{
    int16_t status = MI_OK;
    RF_Power_Control(&MonLecteur, FALSE, 0);
    status = LEDBuzzer(&MonLecteur, LED_OFF);
    status = CloseCOM(&MonLecteur);
}


void MainWindow::on_btn_saisie_clicked()
{

}

void MainWindow::on_btn_quitter_clicked()
{
    int16_t status = MI_OK;
    RF_Power_Control(&MonLecteur, FALSE, 0);
    status = LEDBuzzer(&MonLecteur, LED_OFF);
    status = CloseCOM(&MonLecteur);
    qApp->quit();
}

void MainWindow::on_btn_ledON_clicked()
{
    int16_t status = MI_OK;
    status = LEDBuzzer(&MonLecteur, LED_GREEN_ON);
}

void MainWindow::on_btn_ledOFF_clicked()
{
    int16_t status = MI_OK;
    status = LEDBuzzer(&MonLecteur, LED_GREEN_OFF);
}

void MainWindow::on_btn_ledONyellow_clicked()
{
    int16_t status = MI_OK;
    status = LEDBuzzer(&MonLecteur, LED_YELLOW_ON);
}

void MainWindow::on_btn_ledONred_clicked()
{
    int16_t status = MI_OK;
    status = LEDBuzzer(&MonLecteur, LED_RED_ON);
}


void MainWindow::on_btn_lire_clicked()
{
    uint8_t data[240] = {0};
    int16_t status = 0;
    uint8_t offset;
    uint8_t atq[2];
    uint8_t sak[1];
    uint8_t uid[12];
    uint16_t uid_len = 12;
    int blockNom = 2;
    int blockPrenom = 1;

    status =ISO14443_3_A_PollCard(&MonLecteur, atq, sak, uid, &uid_len);

    if(status == MI_OK){
        qDebug() << "UID: " << uid;

        status = Mf_Classic_Read_Sector(&MonLecteur, TRUE, 2, data, AuthKeyA, 2);

        if(status == MI_OK){

            QString nom = "";

            qDebug() << "Status: " << status;

            for (offset = 0; offset < 16; offset++){
                if(data[16 * blockNom + offset] != 0)
                    nom += (char)data[16 * blockNom + offset];
            }
            qDebug() << "Nom: " << nom;
            ui->displayLastname->setText(nom);

            QString prenom = "";

            for (offset = 0; offset < 16; offset++){
                if(data[16 * blockPrenom + offset] != 0)
                    prenom += (char)data[16 * blockPrenom + offset];
            }
            qDebug() << "Prenom: " << prenom;
            ui->displayName->setText(prenom);

        }
        else{
            qDebug() << "Erreur lecture";
            qDebug() << status ;
        }


    }
    else{
        qDebug()<< "Erreur lecteur";
    }
}



void MainWindow::on_btn_buzzer_released()
{
    int16_t status = MI_OK;
    status = LEDBuzzer(&MonLecteur, LED_GREEN_ON);
}

void MainWindow::on_btn_buzzer_pressed()
{
    int16_t status = MI_OK;
    status = LEDBuzzer(&MonLecteur, LED_GREEN_ON+LED_GREEN_ON);
}

void MainWindow::on_btn_ledON1_clicked()
{
    int16_t status = MI_OK;
    status = LEDBuzzer(&MonLecteur, LED_ON);
}


void MainWindow::on_btn_update_clicked()
{
    uint8_t data[240] = {0};
    uint8_t data2[240] = {0};
    int16_t status = 0;
    uint8_t offset;
    uint8_t atq[2];
    uint8_t sak[1];
    uint8_t uid[12];
    uint16_t uid_len = 12;
    int blockNom = 2;
    int blockPrenom = 1;

    status = ISO14443_3_A_PollCard(&MonLecteur, atq, sak, uid, &uid_len);

    if(status == MI_OK){
        qDebug() << "UID: " << status;

       QString nom = ui->displayLastname->toPlainText();
       QString prenom = ui->displayName->toPlainText();

       QByteArray a = nom.toUtf8() ;
       QByteArray b = prenom.toUtf8() ;


       for(int i=0; i < 16; i++){
           data[i] = a[i];
           data2[i] = b[i];
       }

        status = Mf_Classic_Write_Block(&MonLecteur, TRUE, 10, data, AuthKeyB, 2);

        if(status == MI_OK){

            qDebug() << status ;
        }
        else{
            qDebug()<< "Erreur ecriture";
            qDebug() << status ;
        }

        status = Mf_Classic_Write_Block(&MonLecteur, TRUE, 9, data2, AuthKeyB, 2);

        if(status == MI_OK){

            qDebug() << status ;
        }
        else{
            qDebug()<< "Erreur ecriture";
            qDebug() << status ;
        }
    }
    else{
        qDebug()<< "Erreur lecteur";
    }
}
