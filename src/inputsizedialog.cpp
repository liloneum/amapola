#include "inputsizedialog.h"
#include <QLayout>

InputSizeDialog::InputSizeDialog(QWidget *parent) :
    QDialog(parent)
{
    mpLabel1 = new QLabel(tr("Witdh : "), this);
    mpLabel2 = new QLabel(tr("Height : "), this);
    mpLabel1->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    mpLabel2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    mpWidthSpinBox = new QSpinBox(this);
    mpWidthSpinBox->setMinimum(0);

    mpHeightSpinBox = new QSpinBox(this);
    mpHeightSpinBox->setMinimum(0);

    mpButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    QObject::connect(mpButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(mpButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    mpMainLayout = new QVBoxLayout(this);
    mpMainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    mpGridLayout = new QGridLayout();
    mpGridLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    mpGridLayout->addWidget(mpLabel1, 0, 0);
    mpGridLayout->addWidget(mpWidthSpinBox, 0, 1);
    mpGridLayout->addWidget(mpLabel2, 1, 0);
    mpGridLayout->addWidget(mpHeightSpinBox, 1, 1);

    mpMainLayout->addLayout(mpGridLayout);
    mpMainLayout->addWidget(mpButtonBox);
}

QSize InputSizeDialog::getSize(QWidget *parent, const QString &title,
                               QSize value, int min, int max, int step, bool *ok)
{
    InputSizeDialog dialog(parent);
    dialog.setWindowTitle(title);
    dialog.setIntRange(min, max);
    dialog.setDefaultValue(value);
    dialog.setIntStep(step);

    int ret = dialog.exec();
    if (ok)
        *ok = !!ret;
    if (ret) {
        return dialog.sizeValue();
    } else {
        return value;
    }
}

void InputSizeDialog::setIntRange(int min, int max) {

    mpWidthSpinBox->setRange(min, max);
    mpHeightSpinBox->setRange(min, max);
}

void InputSizeDialog::setDefaultValue(QSize value) {

    mpWidthSpinBox->setValue(value.width());
    mpHeightSpinBox->setValue(value.height());
}

void InputSizeDialog::setIntStep(int step) {

    mpWidthSpinBox->setSingleStep(step);
    mpHeightSpinBox->setSingleStep(step);
}

QSize InputSizeDialog::sizeValue() {

    QSize size;
    size.setWidth(mpWidthSpinBox->value());
    size.setHeight(mpHeightSpinBox->value());

    return size;
}
