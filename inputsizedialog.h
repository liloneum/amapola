#ifndef INPUTSIZEDIALOG_H
#define INPUTSIZEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QSpinBox>

class InputSizeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InputSizeDialog(QWidget *parent = 0);
    static QSize getSize(QWidget *parent, const QString &title, QSize value = QSize(0, 0),
                      int minValue = 0, int maxValue = 2147483647,
                      int step = 1, bool *ok = 0);
signals:

public slots:

private:
    void setIntRange(int min, int max);
    void setDefaultValue(QSize value);
    void setIntStep(int step);
    QSize sizeValue();

    QLabel* mpLabel1;
    QLabel* mpLabel2;
    QVBoxLayout* mpMainLayout;
    QGridLayout* mpGridLayout;
    QDialogButtonBox* mpButtonBox;
    QSpinBox* mpWidthSpinBox;
    QSpinBox* mpHeightSpinBox;
};

#endif // INPUTSIZEDIALOG_H
