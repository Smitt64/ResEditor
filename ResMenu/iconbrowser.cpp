#include "iconbrowser.h"
#include "resmenu.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPixmap>
#include <QTimer>
#include <QVBoxLayout>

IconBrowser::IconBrowser(QWidget *parent)
    : QWidget(parent)
{
    m_info = new QLabel(this);
    m_filter = new QLineEdit(this);
    m_filter->setPlaceholderText(tr("Фильтр по id или тегу (например: печать)..."));
    m_filter->setClearButtonEnabled(true);
    m_zoom = new QComboBox(this);
    m_zoom->addItems({QStringLiteral("1x"), QStringLiteral("2x"),
                      QStringLiteral("4x"), QStringLiteral("8x")});
    m_zoom->setCurrentIndex(1); // 2x — пиксельные иконки лучше видно

    QHBoxLayout *top = new QHBoxLayout;
    top->addWidget(m_filter, 1);
    top->addWidget(new QLabel(tr("Масштаб:"), this));
    top->addWidget(m_zoom);

    m_list = new QListWidget(this);
    m_list->setViewMode(QListView::IconMode);
    m_list->setResizeMode(QListView::Adjust);
    m_list->setUniformItemSizes(true);
    m_list->setSpacing(8);

    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->addWidget(m_info);
    lay->addLayout(top);
    lay->addWidget(m_list, 1);

    connect(m_filter, &QLineEdit::textChanged, this, &IconBrowser::applyFilter);
    connect(m_zoom, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &IconBrowser::applyZoom);
    connect(m_list, &QListWidget::itemActivated, this, [this](QListWidgetItem *item)
    {
        if (item)
            emit iconActivated(itemIconId(item));
    });
    connect(m_list, &QListWidget::currentItemChanged,
            this, [this](QListWidgetItem *current, QListWidgetItem *)
    {
        emit currentIconChanged(current ? itemIconId(current) : 0);
    });

    loadIcons();
    applyZoom(m_zoom->currentIndex());
}

quint32 IconBrowser::itemIconId(QListWidgetItem *item) const
{
    return item ? item->data(Qt::UserRole + 2).toUInt() : 0;
}

quint32 IconBrowser::selectedIconId() const
{
    return itemIconId(m_list->currentItem());
}

void IconBrowser::setCurrentIconId(const quint32 &id)
{
    if (!id)
        return;

    for (int i = 0; i < m_list->count(); ++i)
    {
        QListWidgetItem *item = m_list->item(i);
        if (itemIconId(item) == id)
        {
            m_list->setCurrentItem(item);

            // прокрутку к выделенному откладываем: до show() у списка
            // нет валидной геометрии, и scrollToItem не срабатывает
            const QModelIndex idx = m_list->model()->index(i, 0);
            QTimer::singleShot(0, m_list, [this, idx]()
            {
                m_list->scrollTo(idx, QAbstractItemView::PositionAtCenter);
            });
            break;
        }
    }
}

void IconBrowser::loadIcons()
{
    m_list->clear();

    ResMenu *res = ResMenu::inst();
    const QMap<quint32, QPixmap> pixmaps = res->resPixMaps();
    int loaded = 0;

    for (auto it = pixmaps.constBegin(); it != pixmaps.constEnd(); ++it)
    {
        const quint32 id = it.key();
        const QPixmap &pm16 = it.value();

        // это чей-то 24px-двойник — отдельно не показываем
        if (pm16.width() >= 20 && pixmaps.contains(id - 2000))
            continue;

        const bool has24 = pixmaps.contains(id + 2000);
        const QPixmap pm = has24 ? pixmaps.value(id + 2000) : pm16;

        const QStringList tags = res->resIconTags(id);
        const QStringList titles = res->resIconTitles(id);

        QListWidgetItem *item = new QListWidgetItem(QString::number(id), m_list);
        item->setData(Qt::UserRole, pm);
        // строка для фильтра: теги + названия пунктов, где иконка используется
        item->setData(Qt::UserRole + 1, (tags + titles).join(QLatin1Char(' ')));
        item->setData(Qt::UserRole + 2, id);

        QString tip = has24
            ? tr("ICON_%1 — вариант 24px (ресурс %2)").arg(id).arg(id + 2000)
            : tr("ICON_%1 — только 16px").arg(id);
        if (!tags.isEmpty())
            tip += tr("\nТеги: %1").arg(tags.join(tr(", ")));
        if (!titles.isEmpty())
            tip += tr("\n%1").arg(titles.mid(0, 5).join(tr("; ")));
        item->setToolTip(tip);

        ++loaded;
    }

    QString info = tr("Иконок: %1 (вариант 24px)").arg(loaded);
    const int tagsCount = res->resIconTagsCount();
    if (!tagsCount)
        info += tr(". IconTags.json не найден — поиск по тегам недоступен");
    else
        info += tr(". Тегов: %1").arg(tagsCount);
    m_info->setText(info);
}

void IconBrowser::applyFilter(const QString &text)
{
    for (int i = 0; i < m_list->count(); ++i)
    {
        QListWidgetItem *item = m_list->item(i);
        const QString tags = item->data(Qt::UserRole + 1).toString();
        const bool match = text.isEmpty()
            || item->text().contains(text, Qt::CaseInsensitive)
            || tags.contains(text, Qt::CaseInsensitive);
        item->setHidden(!match);
    }
}

void IconBrowser::applyZoom(int index)
{
    const int factor = 1 << index; // 1, 2, 4, 8
    for (int i = 0; i < m_list->count(); ++i)
    {
        QListWidgetItem *item = m_list->item(i);
        const QPixmap pm = item->data(Qt::UserRole).value<QPixmap>();
        if (pm.isNull())
            continue;

        // FastTransformation = без сглаживания, видны реальные пиксели
        const QPixmap scaled = factor == 1
            ? pm
            : pm.scaled(pm.size() * factor, Qt::KeepAspectRatio, Qt::FastTransformation);
        item->setIcon(QIcon(scaled));
    }
    m_list->setIconSize(QSize(24, 24) * factor);
}
