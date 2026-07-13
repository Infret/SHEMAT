#ifndef BACKEND_H
#define BACKEND_H


#include <QObject>
#include <QString>

class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)

public:
    explicit Backend(QObject *parent = nullptr) : QObject(parent) {}

    QString message() const { return m_message; }
    void setMessage(const QString &msg) {
        if (m_message != msg) {
            m_message = msg;
            emit messageChanged();
        }
    }

signals:
    void messageChanged();

private:
    QString m_message = "Привет из C++";
};

#endif // BACKEND_H

