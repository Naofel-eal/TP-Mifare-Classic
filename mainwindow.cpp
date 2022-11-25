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

    status = Version(&MonLecteur);
    ui->Affichage->setText(MonLecteur.version);
    ui->Affichage->update();
    RF_Power_Control(&MonLecteur, TRUE, 0);
}

void MainWindow::on_btn_disconnect_clicked()
{
    RF_Power_Control(&MonLecteur, FALSE, 0);
    LEDBuzzer(&MonLecteur, LED_OFF);
    CloseCOM(&MonLecteur);
    RF_Power_Control(&MonLecteur, FALSE, 0);
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
    status = LEDBuzzer(&MonLecteur, LED_RED_ON);
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
    status = LEDBuzzer(&MonLecteur, LED_GREEN_ON);
}


void MainWindow::on_btn_lire_clicked() {
    uint8_t data[240] = {0};
    uint8_t offset;
    int16_t status = 0;
    uint8_t atq[2];
    uint8_t sak[1];
    uint8_t uid[12];
    uint16_t uid_len = 12;

    int sectIdentity = 2;
    int sectCounter = 3;
    int blocPrenom = 1; //BLOC DU PRENOM
    int blocNom = 2; //BLOC DU NOM

    ISO14443_3_A_PollCard(&MonLecteur, atq, sak, uid, &uid_len);
    status = Mf_Classic_Read_Sector(&MonLecteur, TRUE, sectIdentity, data, AuthKeyA, 2);
    if (status == MI_OK){
        QString prenom = "";
        QString nom = "";

        for (offset = 0; offset < 16; offset++){
            if(data[16 * blocPrenom + offset] != 0)
                prenom += (char)data[16 * blocPrenom + offset];
            if(data[16 * blocNom + offset] != 0)
                nom += (char)data[16 * blocNom + offset];
        }
        ui->nameLabel->setText("Nom: " + nom);
        ui->firstnameLabel->setText("Prénom: " + prenom);
    }

    int sectorCounter = 3;
    int blocCounter = 14;
    uint32_t value;

    status = Mf_Classic_Read_Value(&MonLecteur, TRUE, blocCounter, &value, AuthKeyA, 3);
    if(status == MI_OK) {
        ui->lcdCounter->display((int)value);
    }
    else
        qDebug() << "Erreur COMPTEUR status: " << status;
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
