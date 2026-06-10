#include "CollapsibleSection.h"

#include <QVBoxLayout>

CollapsibleSection::CollapsibleSection(const QString& title, QWidget* parent)
	: QWidget(parent)
{
	auto* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSizeConstraint(QLayout::SetMinimumSize);
	layout->setSpacing(0);

	_toggleButton = new QToolButton(this);
	_toggleButton->setCheckable(true);
	_toggleButton->setChecked(false);
	_toggleButton->setText(title);
	_toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	_toggleButton->setArrowType(Qt::ArrowType::RightArrow);
	_toggleButton->setStyleSheet("QToolButton { border: none; font-weight: bold; }");
	_toggleButton->setFixedHeight(25);

	_contentArea = new QWidget(this);
	_contentArea->setVisible(false);

	layout->addWidget(_toggleButton);
	layout->addWidget(_contentArea);

	connect(_toggleButton, &QToolButton::toggled, [this](bool checked) {
		_toggleButton->setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
		_contentArea->setVisible(checked);
	});
}

void CollapsibleSection::setContentLayout(QLayout* contentLayout)
{
	_contentArea->setLayout(contentLayout);
}

QWidget* CollapsibleSection::contentWidget() const
{
	return _contentArea;
}