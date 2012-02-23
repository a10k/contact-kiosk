#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QCoreApplication>
#include <QMessageFilter>
#include <QMessageAddress>
#include <QMessage>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createMessagingService();
    createMessageManager();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void MainWindow::showExpanded()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
}
void MainWindow::createMessagingService()
{
    m_service = new QMessageService(this);

}
void MainWindow::createMessageManager()
{
    m_manager = new QMessageManager(this);
    // Start receiving notifications when an SMS has been added to Inbox.
    m_manager->registerNotificationFilter(
                QMessageFilter::byType(QMessage::Sms) &
                QMessageFilter::byStandardFolder(QMessage::InboxFolder));

    // Call messageAdded() when a message has been added to the message
    // store.
    QObject::connect(m_manager,
        SIGNAL(messageAdded(const QMessageId&,
            const QMessageManager::NotificationFilterIdSet&)),
        this,
        SLOT(messageAdded(const QMessageId&,
            const QMessageManager::NotificationFilterIdSet&)));
}
void MainWindow::messageAdded(const QMessageId& id,
    const QMessageManager::NotificationFilterIdSet& matchingFilterIds)
{
    // Retrieve the message identified by id.
    QMessage message = m_manager->message(id);

    QMessageAddress from = message.from();
    QString text = message.textContent();

    //if part to make sure the password is true..
    if(text.contains(ui->lineEdit->text())){


    //natural language part
    QStringList parts = text.split(" ",QString::SkipEmptyParts);
    QStringList fils;
         fils = parts.filter("'s",Qt::CaseInsensitive);
         QString keyword = fils.at(0);
    keyword.remove("'s");//use keyword to search contacts..

   //if part to proceed only when keyword is found..
    if(!keyword.isEmpty())
    {
    //filter preperation
    QContactUnionFilter nameFilter;
    QStringList nameFields;
    nameFields << QContactName::FieldCustomLabel;
    nameFields << QContactName::FieldFirstName;
    nameFields << QContactName::FieldLastName;
    nameFields << QContactName::FieldMiddleName;
    nameFields << QContactName::FieldPrefix;
    nameFields << QContactName::FieldSuffix;
    foreach (const QString& fieldName, nameFields) {
        QContactDetailFilter subFilter;
        subFilter.setDetailDefinitionName(QContactName::DefinitionName, fieldName);
        subFilter.setValue(keyword);//using keyword to filter
        subFilter.setMatchFlags(QContactFilter::MatchContains);
        nameFilter.append(subFilter);
    }



    //intializations of contact managers and list making....
    m_cmanager =new QContactManager();
    m_contacts = m_cmanager->contacts(nameFilter);



    //to make the replysmsbody...
    QString replysmsbody;
    foreach (QContact contact, m_contacts) {
        replysmsbody.append("  ");
        replysmsbody.append(contact.detail(QContactPhoneNumber::DefinitionName).value(QContactPhoneNumber::FieldNumber));
    }

    //if part to ensure the reply is not blank
    if(!replysmsbody.isEmpty()){

    //reply
    QMessage reply;
    reply.setType(QMessage::Sms);
    reply.setTo(from);
    reply.setBody(replysmsbody);
    m_service->send(reply);
        }
    }
  }
}
