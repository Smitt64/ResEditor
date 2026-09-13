#include "commandkeydialog.h"
#include "reskeymap.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QKeySequenceEdit>
#include <QLabel>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

CommandKeyDialog::CommandKeyDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Идентификатор команды"));
    setWindowIcon(QIcon::fromTheme(QStringLiteral("Shortcut")));

    m_keyEdit = new QKeySequenceEdit(this);

    QToolButton *clearBtn = new QToolButton(this);
    clearBtn->setIcon(QIcon::fromTheme(QStringLiteral("CleanData")));
    clearBtn->setToolTip(tr("Очистить сочетание"));
    connect(clearBtn, &QToolButton::clicked, this, [this]()
    {
        m_keyEdit->clear();
        syncFromSequence();
    });

    QHBoxLayout *seqLay = new QHBoxLayout();
    seqLay->setContentsMargins(0, 0, 0, 0);
    seqLay->addWidget(m_keyEdit, 1);
    seqLay->addWidget(clearBtn);

    m_seqHint = new QLabel(this);
    m_seqHint->setWordWrap(true);
    m_seqHint->setStyleSheet(QStringLiteral("color: #a06000;"));
    m_seqHint->hide();

    m_codeSpin = new QSpinBox(this);
    m_codeSpin->setRange(0, 999999);

    m_codeHint = new QLabel(this);
    m_codeHint->setWordWrap(true);
    m_codeHint->setStyleSheet(QStringLiteral("color: #606060;"));

    m_knownCombo = new QComboBox(this);
    m_knownCombo->addItem(tr("Выбрать известный код..."), -1);
    for (const ResKeyMap::Entry &e : ResKeyMap::entries())
    {
        // В список — только канонические записи (первые для кода)
        if (m_knownCombo->findData(e.code) >= 0)
            continue;

        QString text;
        if (!e.seq.isEmpty())
        {
            const QKeySequence ks = QKeySequence::fromString(e.seq, QKeySequence::PortableText);
            text = QStringLiteral("%1 (%2)").arg(ks.toString(QKeySequence::NativeText)).arg(e.code);
        }
        else if (!e.name.isEmpty())
            text = QStringLiteral("%1 (%2)").arg(e.name).arg(e.code);
        else
            continue;

        m_knownCombo->addItem(text, e.code);
    }

    m_showShortcut = new QCheckBox(tr("Показывать сочетание в названии пункта"), this);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QFormLayout *form = new QFormLayout();
    form->addRow(tr("Сочетание:"), seqLay);
    form->addRow(QString(), m_seqHint);
    form->addRow(tr("Код команды:"), m_codeSpin);
    form->addRow(QString(), m_codeHint);
    form->addRow(tr("Известные:"), m_knownCombo);

    QVBoxLayout *mainLay = new QVBoxLayout(this);
    mainLay->addLayout(form);
    mainLay->addWidget(m_showShortcut);
    mainLay->addWidget(buttons);

    connect(m_keyEdit, &QKeySequenceEdit::keySequenceChanged,
            this, [this](const QKeySequence &) { syncFromSequence(); });

    connect(m_codeSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int) { syncFromCode(); });

    connect(m_knownCombo, QOverload<int>::of(&QComboBox::activated),
            this, [this](int index)
    {
        const int code = m_knownCombo->itemData(index).toInt();
        if (code > 0)
            m_codeSpin->setValue(code); // синхронизация — через syncFromCode
        m_knownCombo->setCurrentIndex(0);
    });
}

CommandKeyDialog::~CommandKeyDialog()
{
}

void CommandKeyDialog::setCommand(const int &code, const QString &shortcutText)
{
    m_updating = true;

    m_codeSpin->setValue(code);

    QKeySequence seq;
    if (!shortcutText.isEmpty())
        seq = QKeySequence::fromString(shortcutText, QKeySequence::PortableText);
    if (seq.isEmpty())
        seq = ResKeyMap::keySequenceFromCode(code);

    m_keyEdit->setKeySequence(seq);
    m_showShortcut->setChecked(!shortcutText.isEmpty());

    m_updating = false;
    updateHints();
}

int CommandKeyDialog::commandCode() const
{
    return m_codeSpin->value();
}

QString CommandKeyDialog::shortcutText() const
{
    const QKeySequence seq = m_keyEdit->keySequence();
    if (!m_showShortcut->isChecked() || seq.isEmpty())
        return QString();

    // Сочетание совпадает с деривируемым из кода — хранить не нужно
    if (ResKeyMap::codeFromKeySequence(seq) == m_codeSpin->value())
        return QString();

    return seq.toString(QKeySequence::PortableText);
}

bool CommandKeyDialog::getCommand(QWidget *parent, int &code, QString &shortcutText)
{
    CommandKeyDialog dlg(parent);
    dlg.setCommand(code, shortcutText);

    if (dlg.exec() != QDialog::Accepted)
        return false;

    code = dlg.commandCode();
    shortcutText = dlg.shortcutText();
    return true;
}

void CommandKeyDialog::syncFromSequence()
{
    if (m_updating)
        return;

    const QKeySequence seq = m_keyEdit->keySequence();
    if (seq.isEmpty())
    {
        updateHints();
        return;
    }

    const int seqCode = ResKeyMap::codeFromKeySequence(seq);
    const int curCode = m_codeSpin->value();

    m_updating = true;
    if (ResKeyMap::isUserCode(curCode))
    {
        // Пользовательский код «липкий»: не трогаем его, даже если
        // сочетание есть в константах — оно уходит в shortcutText
        m_showShortcut->setChecked(true);
    }
    else if (seqCode >= 0)
    {
        // Сочетание имеет код — код следует за сочетанием,
        // shortcutText не нужен (деривируется)
        m_codeSpin->setValue(seqCode);
        m_showShortcut->setChecked(false);
    }
    else
    {
        // Кода нет — переходим в пользовательский диапазон,
        // сочетание пойдёт в shortcutText
        m_codeSpin->setValue(ResKeyMap::UserCodeBase);
        m_showShortcut->setChecked(true);
    }
    m_updating = false;

    updateHints();
}

void CommandKeyDialog::syncFromCode()
{
    if (m_updating)
        return;

    const int code = m_codeSpin->value();
    const QKeySequence derived = ResKeyMap::keySequenceFromCode(code);

    m_updating = true;
    if (!derived.isEmpty())
    {
        // Код клавиши — сочетание деривируется из кода
        m_keyEdit->setKeySequence(derived);
        m_showShortcut->setChecked(false);
    }
    else if (ResKeyMap::isUserCode(code))
    {
        // Пользовательский код — захваченное ранее сочетание
        // (в т.ч. сложное, которого нет в константах) сохраняем,
        // оно пойдёт в shortcutText
        if (!m_keyEdit->keySequence().isEmpty())
            m_showShortcut->setChecked(true);
    }
    else
    {
        // Служебный/неизвестный код — сочетания нет
        m_keyEdit->clear();
    }
    m_updating = false;

    updateHints();
}

void CommandKeyDialog::updateHints()
{
    const int code = m_codeSpin->value();
    const QKeySequence seq = m_keyEdit->keySequence();
    const int seqCode = seq.isEmpty() ? -1 : ResKeyMap::codeFromKeySequence(seq);

    if (!seq.isEmpty() && ResKeyMap::isUserCode(code) && !m_showShortcut->isChecked())
    {
        m_seqHint->setText(tr("Сочетание не будет показано в названии пункта меню "
                              "(включите флажок ниже)."));
        m_seqHint->show();
    }
    else if (!seq.isEmpty() && seqCode < 0 && !ResKeyMap::isUserCode(code))
    {
        m_seqHint->setText(tr("Этой комбинации нет в кодах клавиш — она будет "
                              "сохранена как сочетание пользовательской команды."));
        m_seqHint->show();
    }
    else
        m_seqHint->hide();

    if (ResKeyMap::isUserCode(code))
        m_codeHint->setText(tr("Пользовательский диапазон (>= %1)").arg(ResKeyMap::UserCodeBase));
    else if (code > 0 && ResKeyMap::keySequenceFromCode(code).isEmpty())
    {
        const QString name = ResKeyMap::nameFromCode(code);
        m_codeHint->setText(name.isEmpty() ? tr("Код без сочетания клавиш")
                                           : tr("Служебный код (%1)").arg(name));
    }
    else
        m_codeHint->clear();
}
