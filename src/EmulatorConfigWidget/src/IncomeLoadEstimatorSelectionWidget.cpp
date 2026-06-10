#include "IncomeLoadEstimatorSelectionWidget.h"

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

#pragma push_macro("emit")
#undef emit
#include <StarTopologyEmulator/IncomeLoadEstimator/IncomeLoadEstimatorFactory.h>
#pragma pop_macro("emit")

using namespace starTopologyEmulator;

IncomeLoadEstimatorSelectionWidget::IncomeLoadEstimatorSelectionWidget(QWidget* parent) : QWidget(parent) {
        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSizeConstraint(QLayout::SetMinimumSize);

        _comboBox = new QComboBox(this);
        _comboBox->addItem(QString::fromLocal8Bit("Экспоненциальное среднее (EMA)"),
                static_cast<int>(IncomeLoadEstimatorType::Ema));
        _comboBox->addItem(QString::fromLocal8Bit("Фильтр Калмана"),
                static_cast<int>(IncomeLoadEstimatorType::Kalman));

        _settingsStack = new QStackedWidget(this);

        _emaPage = new EmaIncomeLoadEstimatorConfigWidget();
        _kalmanPage = new KalmanIncomeLoadEstimatorConfigWidget();

        _settingsStack->addWidget(_emaPage);
        _settingsStack->addWidget(_kalmanPage);

        mainLayout->addWidget(_comboBox);
        mainLayout->addWidget(_settingsStack);

        connect(_comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &IncomeLoadEstimatorSelectionWidget::onEstimatorIndexChanged);
}

void IncomeLoadEstimatorSelectionWidget::onEstimatorIndexChanged(int index)
{
        for (int i = 0; i < _settingsStack->count(); ++i) {
                QSizePolicy::Policy policy = (i == index) ? QSizePolicy::Minimum : QSizePolicy::Ignored;
                _settingsStack->widget(i)->setSizePolicy(policy, policy);
        }
        _settingsStack->widget(index)->updateGeometry();
        _settingsStack->updateGeometry();

        _settingsStack->setCurrentIndex(index);
        emit changed();
}

std::unique_ptr<IIncomeLoadEstimator> IncomeLoadEstimatorSelectionWidget::incomeLoadEstimator(
        MetricScope scope) const
{
        switch (_settingsStack->currentIndex())
        {
        case Ema:
                return IncomeLoadEstimatorFactory::make(_emaPage->getConfig(), std::move(scope));
        case Kalman:
                return IncomeLoadEstimatorFactory::make(_kalmanPage->getConfig(), std::move(scope));
        }
        return nullptr;
}