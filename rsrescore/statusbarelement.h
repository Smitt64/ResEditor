#ifndef STATUSBARELEMENT_H
#define STATUSBARELEMENT_H

#include <QWidget>

class QLabel;
class QHBoxLayout;
class StatusBarElement : public QWidget
{
    Q_OBJECT
public:
    explicit StatusBarElement(QWidget *parent = nullptr);
    ~StatusBarElement();

public slots:
    void setPixmap(const QPixmap &pix);
    void setText(const QString &str);

signals:

private:
    QLabel *m_pIcon, *m_pText;
    QHBoxLayout *pLayout;
};

#endif // STATUSBARELEMENT_H
