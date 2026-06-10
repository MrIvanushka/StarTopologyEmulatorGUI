#include "EmulatorLogWidget.h"

#include <QVBoxLayout>

EmulatorLogWidget::EmulatorLogWidget(QWidget* parent) : QWidget(parent) {
	_progress = new QProgressBar(this);
	_status = new QLabel("—", this);
	_output = new QPlainTextEdit(/*this*/);
	_output->setReadOnly(true);
	_output->setMaximumBlockCount(2000);

	auto* lay = new QVBoxLayout(this);
	lay->setContentsMargins(0, 0, 0, 0);
	lay->addWidget(_progress);
	lay->addWidget(_status);
	//lay->addWidget(_output);
}

void EmulatorLogWidget::log(const QString& line)
{
	_output->appendPlainText(line);
}

void EmulatorLogWidget::setProgress(int val, int max) {
	_progress->setRange(0, max);
	_progress->setValue(val);
}

void EmulatorLogWidget::setStatus(const QString& txt)
{
	_status->setText(txt);
}

void EmulatorLogWidget::clear()
{
	_progress->setValue(0);
	_status->clear();
	_output->clear();
}