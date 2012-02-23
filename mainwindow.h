#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QMainWindow>
#include <QMessageService>
#include <QMessageManager>
#include <QMessageId>

#include "qtcontacts.h"
// QtMobility namespace
QTM_USE_NAMESPACE

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    // Note that this will only have an effect on Symbian and Fremantle.
    void setOrientation(ScreenOrientation orientation);

    void showExpanded();

private:
    Ui::MainWindow *ui;
    QMessageService* m_service;
    QMessageManager* m_manager;
    void createMessagingService();
    void createMessageManager();

    QContactManager *m_cmanager;
    QList<QContact> m_contacts;
   // QContactFilter m_currentFilter1;

public slots:
    void messageAdded(const QMessageId& id, const QMessageManager::NotificationFilterIdSet& matchingFilterIds);

};

#endif // MAINWINDOW_H
