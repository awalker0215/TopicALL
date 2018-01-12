#ifndef SIGNTHREAD_H
#define SIGNTHREAD_H

#include <QThread>
#include <rpc/client.h>

class SignThread : public QThread
{
    Q_OBJECT
public:
    explicit SignThread(QObject *parent = 0);
    void run();
    bool stop;
signals:
    void SignChange(int);
};

#endif // SIGNTHREAD_H
