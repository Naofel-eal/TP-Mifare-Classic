#include "mainwindow.h"
#include <QMainWindow>
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
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    ui->counterLineEdit->setValidator( new QIntValidator(0, 100, this) );
    setWindowTitle("HACK MIFARE");
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
    if(status == MI_OK) {
         ui->error->setText("");
        this->notifUser();
        qDebug() << "OpenCOM" << status;
        status = Version(&MonLecteur);
        ui->Affichage->setText(MonLecteur.version);
        ui->Affichage->update();
        RF_Power_Control(&MonLecteur, TRUE, 0);
    }
    else {
        ui->error->setText("Erreur connexion lecteur.");
    }
}

void MainWindow::on_btn_disconnect_clicked()
{
    ui->error->setText("");
    this->notifUser();
    RF_Power_Control(&MonLecteur, FALSE, 0);
    LEDBuzzer(&MonLecteur, LED_OFF);
    CloseCOM(&MonLecteur);
    RF_Power_Control(&MonLecteur, FALSE, 0);
}

void MainWindow::on_btn_quitter_clicked()
{
    ui->error->setText("");
    RF_Power_Control(&MonLecteur, FALSE, 0);
    LEDBuzzer(&MonLecteur, LED_OFF);
    CloseCOM(&MonLecteur);
    qApp->quit();
}

void MainWindow::on_btn_ledON_clicked()
{
    ui->error->setText("");
    LEDBuzzer(&MonLecteur, LED_RED_ON);
}

void MainWindow::on_btn_ledOFF_clicked()
{
    ui->error->setText("");
    LEDBuzzer(&MonLecteur, LED_GREEN_OFF);
}

void MainWindow::on_btn_ledONyellow_clicked()
{
    ui->error->setText("");
    LEDBuzzer(&MonLecteur, LED_YELLOW_ON);
}

void MainWindow::on_btn_ledONred_clicked()
{
    ui->error->setText("");
    LEDBuzzer(&MonLecteur, LED_GREEN_ON);
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
        ui->error->setText("");
        this->notifUser();
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
        ui->error->setText("");
        ui->counterLineEdit->setText(QString::number(value));
    }
    else
        ui->error->setText("Erreur lecture : " + QString::number(status));
}

void MainWindow::on_btn_buzzer_released()
{
    LEDBuzzer(&MonLecteur, LED_GREEN_ON);
}

void MainWindow::on_btn_buzzer_pressed()
{
    LEDBuzzer(&MonLecteur, LED_GREEN_ON+LED_GREEN_ON);
}

void MainWindow::on_btn_ledON1_clicked()
{
    LEDBuzzer(&MonLecteur, LED_ON);
}

void MainWindow::on_writeBtn_clicked()
{
    uint8_t nameArray[240] = {0};
    uint8_t firstnameArray[240] = {0};
    int16_t status = 0;
    uint8_t offset;
    uint8_t atq[2];
    uint8_t sak[1];
    uint8_t uid[12];
    uint16_t uid_len = 12;
    int blockNom = 2;
    int blockPrenom = 1;

    QString nom = ui->nameText->toPlainText();
    QString prenom = ui->firstnameText->toPlainText();

    if(nom !="" && prenom != "") {
        status = ISO14443_3_A_PollCard(&MonLecteur, atq, sak, uid, &uid_len);
        if(status == MI_OK){
            ui->error->setText("");
            this->notifUser();
           QByteArray nameByte = nom.toUtf8() ;
           QByteArray firstnameByte = prenom.toUtf8() ;

           for(int i=0; i < 16; i++){
               nameArray[i] = nameByte[i];
               firstnameArray[i] = firstnameByte[i];
           }

            status = Mf_Classic_Write_Block(&MonLecteur, TRUE, 10, nameArray, AuthKeyB, 2);

            if(status != MI_OK){
                ui->error->setText("Erreur ecriture nom: " + QString::number(status));
            }
            else {
                ui->error->setText("");
            }

            status = Mf_Classic_Write_Block(&MonLecteur, TRUE, 9, firstnameArray, AuthKeyB, 2);

            if (status != MI_OK){
                ui->error->setText("Erreur ecriture prenom: " + QString::number(status));
            }
            else {
                ui->error->setText("");
            }
        }
        else{
            ui->error->setText("Erreur lecteur");
        }
        ui->error->setText("");
        ui->firstnameText->setText("");
        ui->nameText->setText("");
    }
    else {
        ui->error->setText("Le nom et le prénom ne doivent pas être vide !");
    }
}


void MainWindow::on_counterSpinBox_valueChanged(int arg1)
{
    uint32_t value = arg1;

    int status = Mf_Classic_Write_Value(&MonLecteur, TRUE, 14, value, AuthKeyB,3);
    if(status != MI_OK)
        ui->error->setText("Erreur ecriture compteur: " + QString::number(status));
    else {
        ui->error->setText("");
    }
}

void MainWindow::notifUser() {
    LEDBuzzer(&MonLecteur, LED_RED_ON);
    LEDBuzzer(&MonLecteur, LED_GREEN_ON+LED_GREEN_ON);
    QThread::msleep(100);
    LEDBuzzer(&MonLecteur, LED_GREEN_OFF);
}

void MainWindow::on_increment_clicked()
{
    int step = ui->stepSpinBox->value();

    int status = Mf_Classic_Increment_Value(&MonLecteur, true, 14, step, 13, AuthKeyB, 3);
    if(status != MI_OK)
        ui->error->setText("Erreur incrémentation compteur: " + QString::number(status));
    else
        ui->error->setText("");

    status = Mf_Classic_Restore_Value(&MonLecteur, true, 13, 14, AuthKeyB, 3);
    if(status != MI_OK)
        ui->error->setText("Erreur restore lors de l'incrémentation du compteur: " + QString::number(status));
    else {
        this->on_btn_lire_clicked();
        ui->error->setText("");
    }
}


void MainWindow::on_decrement_clicked()
{
    int step = ui->stepSpinBox->value();

    int status = Mf_Classic_Decrement_Value(&MonLecteur, true, 14, step, 13, AuthKeyA, 3);
    if(status != MI_OK)
        ui->error->setText("Erreur décrémentation compteur: " + QString::number(status));
    else
        ui->error->setText("");

    status = Mf_Classic_Restore_Value(&MonLecteur, true, 13, 14, AuthKeyA, 3);
    if(status != MI_OK)
        ui->error->setText("Erreur restore lors de la décrémentation du compteur: " + QString::number(status));
    else {
        this->on_btn_lire_clicked();
        ui->error->setText("");
    }
}


void MainWindow::on_counterLineEdit_editingFinished()
{
    uint32_t value = ui->counterLineEdit->text().toInt();

    int status = Mf_Classic_Write_Value(&MonLecteur, TRUE, 14, value, AuthKeyB,3);
    if(status != MI_OK)
        ui->error->setText("Erreur ecriture compteur: " + QString::number(status));
    else {
        this->on_btn_lire_clicked();
        ui->error->setText("");
    }
}

