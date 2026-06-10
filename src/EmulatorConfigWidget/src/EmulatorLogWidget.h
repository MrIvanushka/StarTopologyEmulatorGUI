#pragma once

#include <QLabel>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QWidget>

class EmulatorLogWidget : public QWidget {
	Q_OBJECT
public:
	explicit EmulatorLogWidget(QWidget* parent = nullptr);

	void setProgress(int value, int max);
	void setStatus(const QString& text);
	void log(const QString& line);
	void clear();

	// Можно вынести сюда логику формирования итогового отчета
	//void showFinalResults(const SimulationResults& res, const EmulatorParamsWidget::Data& p);

private:
	QProgressBar* _progress;
	QLabel* _status;
	QPlainTextEdit* _output;
};