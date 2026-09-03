#ifndef COMMANDKEYDIALOG_H
#define COMMANDKEYDIALOG_H

#include <QDialog>

class QCheckBox;
class QComboBox;
class QKeySequenceEdit;
class QLabel;
class QSpinBox;

// Диалог редактирования идентификатора команды (MenuAction::commandId)
// и сочетания клавиш (MenuAction::shortcutText).
//
// Коды < K_USER (ResKeyMap::UserCodeBase) — клавиши из TAllKeys, сочетание
// для них деривируется из кода. Коды >= K_USER — пользовательские команды,
// сочетание (в т.ч. не имеющее кода в TAllKeys) хранится в shortcutText.
class CommandKeyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CommandKeyDialog(QWidget *parent = nullptr);
    virtual ~CommandKeyDialog();

    void setCommand(const int &code, const QString &shortcutText);

    int commandCode() const;
    // Что записывать в MenuAction::shortcutText (может быть пустым —
    // тогда сочетание деривируется из кода либо не показывается вовсе)
    QString shortcutText() const;

    static bool getCommand(QWidget *parent, int &code, QString &shortcutText);

private:
    void syncFromSequence();
    void syncFromCode();
    void updateHints();

    bool m_updating = false;

    QKeySequenceEdit *m_keyEdit;
    QSpinBox *m_codeSpin;
    QComboBox *m_knownCombo;
    QCheckBox *m_showShortcut;
    QLabel *m_seqHint;
    QLabel *m_codeHint;
};

#endif // COMMANDKEYDIALOG_H
