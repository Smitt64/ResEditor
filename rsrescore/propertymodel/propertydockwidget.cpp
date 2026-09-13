#include "propertydockwidget.h"
#include "propertytreeview.h"
#include "propertymodel.h"
#include "customrectitem.h"
#include <QGraphicsScene>
#include <QMainWindow>
#include <QDebug>
#include <QHeaderView>
#include <QSplitter>
#include <QStyleOptionViewItem>
#include <QPainter>
#include <QFontDatabase>
#include <QMouseEvent>
#include <propertytreeitem.h>
// https://stackoverflow.com/questions/27149733/qtreeview-merge-some-cells

class Office2013PropertyTreeView : public PropertyTreeView
{
    int m_hoveredRow;
    QModelIndex m_hoveredIndex;

public:
    Office2013PropertyTreeView(QWidget *parent = nullptr) :
        PropertyTreeView(parent)
    {
        setUniformRowHeights(true);

        setMouseTracking(true); // ВАЖНО: включаем отслеживание мыши
        setAttribute(Qt::WA_Hover); // Включаем события наведения
        viewport()->setMouseTracking(true);
    }

    virtual ~Office2013PropertyTreeView()
    {

    }

protected:
    virtual void drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE
    {
        // Создаем копию option для модификации
        QStyleOptionViewItem opt = option;

        // Ручное определение hover состояния
        bool isHovered = false;
        const Office2013PropertyTreeView* treeView = dynamic_cast<const Office2013PropertyTreeView*>(this);
        if (treeView && treeView->m_hoveredRow == index.row())
        {
            isHovered = true;
            // Устанавливаем флаг State_MouseOver для корректной работы стандартной отрисовки
            opt.state |= QStyle::State_MouseOver;
        }

        int type = index.data(PropertyTreeItem::RoleTypeItem).value<PropertyTreeItem::TypeItem>();

        if (type == PropertyTreeItem::TypeItem_Group)
        {
            // Полная ширина строки группы
            int firstSection = header()->logicalIndex(0);
            int lastSection = header()->logicalIndex(header()->count() - 1);
            int left = header()->sectionViewportPosition(firstSection);
            int right = header()->sectionViewportPosition(lastSection) + header()->sectionSize(lastSection);

            opt.rect.setX(left);
            opt.rect.setWidth(right - left);

            QString groupName = index.data(Qt::DisplayRole).toString();
            int groupIndex = index.data(PropertyTreeItem::RoleGroup).toInt();
            int childCount = model()->rowCount(index);

            // ========== ФОН С ЗАМЕТНЫМ ЦВЕТОМ ==========
            QColor bgColor("#f8f8f8");

            // Если наведен курсор - делаем фон немного светлее
            if (isHovered)
            {
                bgColor = bgColor.lighter(105);
            }

            // Мягкий градиент для объема
            QLinearGradient gradient(opt.rect.topLeft(), opt.rect.bottomLeft());
            gradient.setColorAt(0.0, bgColor.lighter(105)); // Светлее вверху
            gradient.setColorAt(0.5, bgColor);               // Основной цвет
            gradient.setColorAt(1.0, bgColor.darker(105));   // Темнее внизу

            painter->fillRect(opt.rect, gradient);

            // ========== АКЦЕНТНАЯ ЛИНИЯ СЛЕВА ==========
            QColor accentLineColor;

            if (isHovered)
            {
                // При наведении - синий цвет #0072C6
                accentLineColor = QColor("#0072C6");
            }
            else
            {
                // Обычно - цвет группы
                accentLineColor = PropertyTreeItem::groupColor(groupIndex);

                // Делаем линию заметнее
                if (accentLineColor.lightness() > 200)
                    accentLineColor = accentLineColor.darker(130);
                else
                    accentLineColor = accentLineColor.darker(115);
            }

            // Рисуем акцентную линию
            QRect accentRect(opt.rect.left(), opt.rect.top(), 3, opt.rect.height());
            painter->fillRect(accentRect, accentLineColor);

            // Белая линия справа от акцентной для объема
            painter->setPen(QColor(255, 255, 255, 180));
            painter->drawLine(opt.rect.left() + 3, opt.rect.top(),
                              opt.rect.left() + 3, opt.rect.bottom());

            // ========== ГРАНИЦЫ В СТИЛЕ OFFICE 2013 ==========
            // Верхняя тонкая серая линия
            QColor topBorderColor = isHovered ? QColor("#c5d2e0") : QColor(220, 220, 220);
            painter->setPen(topBorderColor);
            painter->drawLine(opt.rect.topLeft(), opt.rect.topRight());

            // Нижняя линия
            painter->setPen(QColor(210, 210, 210));
            painter->drawLine(opt.rect.bottomLeft(), opt.rect.bottomRight());

            // ========== ТЕКСТ ГРУППЫ ==========
            QFont font = painter->font();
            font.setBold(true);
            font.setPointSize(font.pointSize());

            // Используем Segoe UI если доступен
            if (QFontDatabase().families().contains("Segoe UI"))
                font.setFamily("Segoe UI");

            painter->setFont(font);

            // Цвет текста зависит от наведения
            QColor textColor;
            if (isHovered)
            {
                textColor = QColor("#0072C6"); // Синий при наведении
            }
            else
            {
                textColor = QColor(102, 102, 102); // Серый обычно (#666666)
            }

            // Легкая тень текста для объема (только если не наведен курсор)
            if (!isHovered)
            {
                painter->setPen(QColor(255, 255, 255, 150));
                painter->drawText(opt.rect.adjusted(13, 1, -7, 1),
                                  Qt::AlignLeft | Qt::AlignVCenter, groupName);
            }

            // Основной текст
            painter->setPen(textColor);

            // Текст с отступом от акцентной линии
            QRect textRect = opt.rect.adjusted(12, 0, -40, 0);
            painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, groupName);

            // ========== СЧЕТЧИК ЭЛЕМЕНТОВ (опционально) ==========
            if (childCount > 0)
            {
                QFont countFont = font;
                countFont.setBold(false);
                painter->setFont(countFont);

                QString countText = QString("(%1)").arg(childCount);

                // Цвет счетчика также меняется при наведении
                QColor countColor = isHovered ?
                                        QColor("#0072C6") :        // Синий при наведении
                                        QColor(128, 128, 160);     // Серо-синий обычно

                painter->setPen(countColor);
                painter->drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, countText);
            }

            // Светлая подсветка верхнего края
            painter->setPen(QColor(255, 255, 255, 120));
            painter->drawLine(opt.rect.topLeft() + QPoint(0, 1),
                              opt.rect.topRight() + QPoint(0, 1));

            // ========== ДОПОЛНИТЕЛЬНЫЙ ЭФФЕКТ ПРИ НАВЕДЕНИИ ==========
            if (isHovered)
            {
                // Легкая голубая подсветка всей строки
                painter->setPen(Qt::NoPen);
                painter->setBrush(QColor(230, 242, 252, 30)); // Очень прозрачный голубой
                painter->drawRect(opt.rect);

                // Тонкая синяя линия снизу при наведении
                painter->setPen(QPen(QColor("#0072C6"), 1));
                painter->drawLine(opt.rect.bottomLeft(), opt.rect.bottomRight());
            }

        }
        else
        {
            // Для обычных строк тоже устанавливаем hover состояние
            if (isHovered)
            {
                opt.state |= QStyle::State_MouseOver;
            }

            // Обычная строка свойства - стандартная отрисовка
            QTreeView::drawRow(painter, opt, index);
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        QModelIndex index = indexAt(event->pos());

        // Обновляем hovered строку только если она изменилась
        if (index.isValid() && index != m_hoveredIndex)
        {
            m_hoveredIndex = index;
            m_hoveredRow = index.row();

            // Перерисовываем предыдущую и текущую строки
            viewport()->update();
        }
        else if (!index.isValid() && m_hoveredRow != -1)
        {
            // Мышь ушла со строки
            m_hoveredRow = -1;
            m_hoveredIndex = QModelIndex();
            viewport()->update();
        }

        QTreeView::mouseMoveEvent(event);
    }

    void leaveEvent(QEvent *event) override
    {
        if (m_hoveredRow != -1)
        {
            m_hoveredRow = -1;
            m_hoveredIndex = QModelIndex();
            viewport()->update();
        }
        QTreeView::leaveEvent(event);
    }

private:
    // Добавляем mutable для доступа в const методе
    mutable int m_lastHoveredRow;
};

// -------------------------------------------------------------------------------------------------------------------

PropertyDockWidget::PropertyDockWidget(QWidget *paretn) :
    QDockWidget(paretn)
{
    m_pTreeView = new Office2013PropertyTreeView(this);
    m_pTreeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_pContainer = new QMainWindow();
    m_pContainer->setWindowFlags(m_pContainer->windowFlags() | Qt::FramelessWindowHint);
    m_pContainer->setMinimumWidth(350);

    m_pStructView = new QTreeView(this);
    m_pStructView->setRootIsDecorated(false);
    m_pStructView->setIndentation(10);
    m_pStructView->setItemsExpandable(false);
    m_pStructView->header()->setStretchLastSection(false);

    m_pStructView->setMaximumHeight(200);
    m_pStructView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_pSplitter = new QSplitter(Qt::Vertical, this);
    m_pSplitter->setObjectName("PropertyDockSplitter");
    m_pSplitter->addWidget(m_pTreeView);
    m_pSplitter->addWidget(m_pStructView);

    m_pContainer->setCentralWidget(m_pSplitter);
    setWidget(m_pContainer);

    setWindowTitle(tr("Свойства"));

    connect(m_pTreeView, &QTreeView::expanded, [=](const QModelIndex &index)
    {
        m_Expanded.insert(index.data().toString());
    });

    connect(m_pTreeView, &QTreeView::collapsed, [=](const QModelIndex &index)
    {
        m_Expanded.remove(index.data().toString());
    });

    connect(m_pStructView, &QTreeView::clicked, [=](const QModelIndex &index)
    {
        CustomRectItem *item = index.data(Qt::UserRole + 1).value<CustomRectItem*>();

        if (item)
        {
            item->scene()->clearSelection();
            item->setSelected(true);
        }
    });
}

PropertyDockWidget::~PropertyDockWidget()
{

}

void PropertyDockWidget::setStructModel(QAbstractItemModel *model)
{
    m_pStructView->setModel(model);

    if (!model)
        return;

    m_pStructView->expandAll();

    m_pStructView->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    connect(model, &QAbstractItemModel::modelReset, [=]()
    {
        m_pStructView->expandAll();
    });
}

void PropertyDockWidget::setPropertyModel(QAbstractItemModel *model)
{
    m_pTreeView->setModel(model);

    if (!model)
        return;

    for (const auto &str : qAsConst(m_Expanded))
    {
        QModelIndexList lst = model->match(model->index(0, 0), Qt::DisplayRole, str, Qt::MatchFixedString);

        if (!lst.empty())
            m_pTreeView->expand(lst.first());
    };

    m_pTreeView->header()->resizeSection(0, 150);
}
