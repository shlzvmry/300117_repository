#include "delegate.h"
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>

SexDelegate::SexDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

QWidget *SexDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->addItem("男", 1);
    editor->addItem("女", 0);
    return editor;
}

void SexDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    int value = index.model()->data(index, Qt::EditRole).toInt();
    comboBox->setCurrentIndex(value == 1 ? 0 : 1);  // 1对应男，0对应女
}

void SexDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                               const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentData(), Qt::EditRole);
}

QString SexDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    if (value.isValid()) {
        int sexValue = value.toInt();
        return (sexValue == 1) ? "男" : "女";
    }
    return QStyledItemDelegate::displayText(value, locale);
}

DateDelegate::DateDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

QWidget *DateDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    QDateEdit *editor = new QDateEdit(parent);
    editor->setCalendarPopup(true);
    editor->setDisplayFormat("yyyy-MM-dd");
    return editor;
}

void DateDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QDateEdit *dateEdit = static_cast<QDateEdit*>(editor);
    QString dateStr = index.model()->data(index, Qt::EditRole).toString();
    QDate date = QDate::fromString(dateStr, "yyyy-MM-dd");
    if (!date.isValid()) {
        date = QDate::currentDate();
    }
    dateEdit->setDate(date);
}

void DateDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    QDateEdit *dateEdit = static_cast<QDateEdit*>(editor);
    model->setData(index, dateEdit->date().toString("yyyy-MM-dd"), Qt::EditRole);
}

NumberDelegate::NumberDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

QWidget *NumberDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setMinimum(0);
    editor->setMaximum(300);
    editor->setSingleStep(0.1);
    return editor;
}

void NumberDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    double value = index.model()->data(index, Qt::EditRole).toDouble();
    spinBox->setValue(value);
}

void NumberDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                  const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    model->setData(index, spinBox->value(), Qt::EditRole);
}
