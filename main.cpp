#include <QApplication>
#include <QtCore>
#include <QtWidgets>
#include <QWidget>
#include <QFileDialog>
#include <QStyleFactory>

#include "src/EditorWidget/src/EditorWidget.h"

using namespace editor;

static void initStyleSheet(QApplication& a)
{
	Q_INIT_RESOURCE(ads); // If static linked.
	QFile f(":ads/stylesheets/default-windows.css");
	if (f.open(QFile::ReadOnly))
	{
		const QByteArray ba = f.readAll();
		f.close();
		a.setStyleSheet(QString(ba));
	}
}

int main(int argc, char** argv)
{
	//Q_INIT_RESOURCE(resources);
	QApplication app(argc, argv);
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	app.setWindowIcon(QIcon(":/EditorWidgets/Icons/icon.ico"));

	//auto keys = QStyleFactory::keys();
	app.setStyle(QStyleFactory::create("Fusion"));//"windowsvista"));
	initStyleSheet(app);

	EditorWidget* gui = new EditorWidget;
	gui->setWindowTitle("StarTopologyEmulatorGUI");
	gui->show();

	app.exec();

	delete gui;

	return 0;
}