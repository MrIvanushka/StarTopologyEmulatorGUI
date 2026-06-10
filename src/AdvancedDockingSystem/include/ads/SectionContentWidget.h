#ifndef SECTION_CONTENT_WIDGET_H
#define SECTION_CONTENT_WIDGET_H

#include <QFrame>

#include "ads/API.h"
#include "ads/SectionContent.h"


class ContainerWidget;
class SectionWidget;

class SectionContentWidget : public QFrame
{
	Q_OBJECT

	friend class ContainerWidget;

public:
	SectionContentWidget(SectionContent::RefPtr c, QWidget* parent = 0);
	virtual ~SectionContentWidget();

private:
	SectionContent::RefPtr _content;
};


#endif
