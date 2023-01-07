#ifndef CHARACTERWIDGET_H
#define CHARACTERWIDGET_H

#include <QWidget>
#include <QMap>

class CharacterWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
public:
    explicit CharacterWidget(QWidget *parent = nullptr);
    virtual ~CharacterWidget();

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

    const int &columns() const;
    void setColumns(const int &value);

    const QFont &displayFont() const;

public slots:
    void updateFont(const QFont &font);
    void updateSize(const QString &fontSize);
    void updateStyle(const QString &fontStyle);
    void updateFontMerging(bool enable);

signals:
    void columnsChanged();
    void characterSelected(const QChar &character);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void calculateSquareSize();
    void fillGraphicsChar();
    int m_Columns, m_SquareSize;
    QFont m_DisplayFont;

    QList<QChar> m_Chars;
    QMap<int,QRect> m_CharsRects;

    int lastKey = -1;
};

#endif // CHARACTERWIDGET_H
