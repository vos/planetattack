#ifndef QLOG_H
#define QLOG_H

#include <QPlainTextEdit>
#include <QTime>

static QPlainTextEdit *__logView = NULL;

static void qLog(const QString &msg, const QTextCharFormat &format)
{
    if (__logView == NULL) {
        qWarning("Log view is NULL, use qRegisterLogView before!");
        qDebug(qPrintable(msg));
        return;
    }
    __logView->setCurrentCharFormat(format);
    QString timeStr(QTime::currentTime().toString("hh:mm:ss:zzz"));
    __logView->appendPlainText(QString("%1  %2").arg(timeStr).arg(msg));
}

static void qLog(const QString &msg, const QColor &color = Qt::black)
{
    QTextCharFormat format;
    format.setForeground(color);
    qLog(msg, format);
}

static void qLogMsgHandler(QtMsgType type, const char *msg)
{
    QTextCharFormat format;
    switch (type) {
    case QtDebugMsg:
        format.setForeground(Qt::darkGray);
        break;
    case QtWarningMsg:
        format.setForeground(Qt::darkYellow);
        break;
    case QtCriticalMsg:
        format.setForeground(Qt::red);
        format.setFontWeight(QFont::Bold);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", msg);
        abort();
    }
    qLog(msg, format);
}

static void qRegisterLogView(QPlainTextEdit *logView, bool useLogMsgHandler = true)
{
    __logView = logView;
    if (useLogMsgHandler) {
        qInstallMsgHandler(qLogMsgHandler);
    }
}

#endif // QLOG_H
