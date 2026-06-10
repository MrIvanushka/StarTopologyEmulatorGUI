#pragma once

#include <QToolButton>
#include <QScrollArea>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

class CollapsibleSection : public QWidget
{
public:
	CollapsibleSection(const QString& title, QWidget* parent = nullptr);

	void setContentLayout(QLayout* contentLayout);

	QWidget* contentWidget() const;
private:
	QToolButton* _toggleButton;
	QWidget* _contentArea;
};