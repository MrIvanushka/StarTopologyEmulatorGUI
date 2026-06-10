#include "CustomPlot.h"

bool IsClose(double a, double b) {
	return (abs(a - b) < 0.0000001);
}

CustomPlot::CustomPlot(QWidget* parent=nullptr)
	: QCustomPlot(parent), mMousePressed(false), mPointSelected(false), current_hint_index_(-1), current_hint_(nullptr), current_hint_rect_(nullptr) {}

QByteArray CustomPlot::ToBytes() {
	QByteArray arr;

	return arr;
}

void CustomPlot::FromBytes(QByteArray bytes) {
	
}

void CustomPlot::setHorizontalAxisLimit(double limit) {
	horizontal_limit_ = limit;
}

void CustomPlot::setVerticalAxisLimit(double limit) {
	vertical_limit_ = limit;
}

void CustomPlot::movePlotRanges(double x_diff, double y_diff) {
	xAxis->setRange(xAxis->range() + x_diff);
	yAxis->setRange(yAxis->range() + y_diff);
}

void CustomPlot::setPointStep(int step) {
	auto old_step = point_step_;
	point_step_ = TimeUnit(step);
	if (old_step < point_step_ || old_step == TimeUnit::time_none) {
		MoveAllPointsToStep();
		replotAsync();
	}
}

void CustomPlot::MoveAllPointsToStep() {
	
}

void CustomPlot::replotAsync()
{
	std::lock_guard<std::mutex> lock(_lock);
	replot();
}

QPointF CustomPlot::PointToStep(const QPoint &pos, bool add = true) {
	QPointF axis_pos = { xAxis->pixelToCoord(pos.x()), yAxis->pixelToCoord(pos.y()) };
	int multiplier = 1000 / (int)point_step_;
	if (point_step_ != TimeUnit::time_none) {
		double new_x = std::round(axis_pos.x() * multiplier) / multiplier;
		axis_pos.setX(new_x);
	}
	return axis_pos;
}

QPointF CustomPlot::PointToStep(const QPointF& pos, bool add = true) {
	QPointF axis_pos = pos;
	int multiplier = 1000 / (int)point_step_;
	if (point_step_ != TimeUnit::time_none) {
		double new_x = std::round(axis_pos.x() * multiplier) / multiplier;
		axis_pos.setX(new_x);
	}
	return axis_pos;
}

void CustomPlot::mousePressEvent(QMouseEvent* event) {
	mMousePressed = true;
	prev_mouse_position_while_dragging_ = event->pos();
	QVariant nearest_point_variant;
	QCustomPlot::mousePressEvent(event);
}

void CustomPlot::mouseMoveEvent(QMouseEvent* event) {
	if (mMousePressed) {
		bool range_moved = false;
		double x_diff = xAxis->pixelToCoord(prev_mouse_position_while_dragging_.x())
			- xAxis->pixelToCoord(event->pos().x());
		if (!horizontal_limit_.has_value() || xAxis->range().lower + x_diff > horizontal_limit_.value()) {
			movePlotRanges(x_diff, 0);
			range_moved = true;
		}
		double y_diff = yAxis->pixelToCoord(prev_mouse_position_while_dragging_.y())
			- yAxis->pixelToCoord(event->pos().y());
		if (!vertical_limit_.has_value() || yAxis->range().lower + y_diff > vertical_limit_.value()) {
			movePlotRanges(0, y_diff);
			range_moved = true;
		}
		if (range_moved) {
			replotAsync();
		}
		prev_mouse_position_while_dragging_ = event->pos();
	}
	CheckHints(event->pos());
	QCustomPlot::mouseMoveEvent(event);
}

void CustomPlot::mouseReleaseEvent(QMouseEvent* event) {
	if (!mMouseHasMoved && event->button() == Qt::RightButton) {
		auto text_rect = new QCPItemText(this);
		text_rect->setText("aaaaaaaaaaaaa");
		text_rect->position->setType(QCPItemPosition::PositionType::ptPlotCoords);
		text_rect->position->setAxes(xAxis, yAxis);
		text_rect->position->setCoords(xAxis->pixelToCoord(event->pos().x()), yAxis->pixelToCoord(event->pos().y()));
	}
	mMousePressed = false;
	mPointSelected = false;
	QCustomPlot::mouseReleaseEvent(event);
}

void CustomPlot::wheelEvent(QWheelEvent* event) {
	const QPointF pos = event->position();
	const double delta = event->angleDelta().y();
	double wheelSteps = delta / 120.0;
	auto rect = axisRect()->rect();

	if (pos.x() >= rect.left() && pos.x() <= rect.right()) {
		double x_factor = qPow(axisRect()->rangeZoomFactor(Qt::Horizontal), wheelSteps);
		xAxis->scaleRange(x_factor, xAxis->pixelToCoord(pos.x()));
	}
	if (pos.y() >= rect.top() && pos.y() <= rect.bottom()) {
		double y_factor = qPow(axisRect()->rangeZoomFactor(Qt::Vertical), wheelSteps);
		yAxis->scaleRange(y_factor, yAxis->pixelToCoord(pos.y()));
	}

	double x_move = 0, y_move = 0;
	if (horizontal_limit_.has_value() && xAxis->range().lower < horizontal_limit_.value()) {
		x_move = horizontal_limit_.value() - xAxis->range().lower;
	}
	if (vertical_limit_.has_value() && yAxis->range().lower < vertical_limit_.value()) {
		y_move = vertical_limit_.value() - yAxis->range().lower;
	}
	movePlotRanges(x_move, y_move);
	replotAsync();
	QCustomPlot::wheelEvent(event);
}

void CustomPlot::CheckHints(QPoint pos) {/*
	QVariant nearest_point_variant;
	double res = graph(1)->selectTest(pos, false, &nearest_point_variant);
	if (res >= 0 && res < point_capture_distance_) {
		QCPDataSelection nearest_point_selection = qvariant_cast<QCPDataSelection>(nearest_point_variant);
		auto selected_point_index = nearest_point_selection.dataRange().begin();
		if (selected_point_index == current_hint_index_) {
			return;
		}
		if (current_hint_ != nullptr) {
			removeItem(current_hint_rect_);
			current_hint_rect_ = nullptr;
			removeItem(current_hint_);
			current_hint_ = nullptr;
		}
		current_hint_index_ = -1;
		if (hints_.size() < selected_point_index) {
			qDebug() << "hint is absent!";
			return;
		}
		auto nearest_point = graph(1)->data()->at(selected_point_index);
		current_hint_rect_ = new QCPItemRect(this);
		current_hint_rect_->setBrush(QBrush(Qt::white));
		current_hint_ = new QCPItemText(this);
		current_hint_->setText(hints_[selected_point_index]);
		current_hint_->position->setType(QCPItemPosition::PositionType::ptPlotCoords);
		current_hint_->position->setAxes(xAxis, yAxis);
		current_hint_->position->setCoords(nearest_point->mainKey(), nearest_point->mainValue());
		double offset_x = 0, offset_y = 0;
		if (horizontal_limit_.has_value() && xAxis->pixelToCoord(current_hint_->left->pixelPosition().x() - pixel_frame_width_) < horizontal_limit_.value()) {
			offset_x = horizontal_limit_.value() - xAxis->pixelToCoord(current_hint_->left->pixelPosition().x() - pixel_frame_width_);
		}
		if (vertical_limit_.has_value() && yAxis->pixelToCoord(current_hint_->bottom->pixelPosition().y() + pixel_frame_width_) < vertical_limit_.value()) {
			offset_y = vertical_limit_.value() - yAxis->pixelToCoord(current_hint_->bottom->pixelPosition().y() + pixel_frame_width_);
		}
		current_hint_->position->setCoords(nearest_point->mainKey() + offset_x, nearest_point->mainValue() + offset_y);
		current_hint_rect_->topLeft->setParentAnchor(current_hint_->topLeft);
		current_hint_rect_->topLeft->setCoords(-pixel_frame_width_, -pixel_frame_width_);
		current_hint_rect_->bottomRight->setParentAnchor(current_hint_->bottomRight);
		current_hint_rect_->bottomRight->setCoords(pixel_frame_width_, pixel_frame_width_);
		current_hint_index_ = selected_point_index;
	} else {
		if (current_hint_ != nullptr) {
			removeItem(current_hint_rect_);
			current_hint_rect_ = nullptr;
			removeItem(current_hint_);
			current_hint_ = nullptr;
		}
		current_hint_index_ = -1;
	}
	replotAsync();*/
}

void CustomPlot::returnToDefaultView() {/*
	double max_key, max_value;
	bool is_valid;
	max_key = graph(0)->getKeyRange(is_valid).upper;
	if (!is_valid) {
		return;
	}
	max_value = graph(0)->getValueRange(is_valid).upper;
	if (!is_valid) {
		return;
	}
	xAxis->setRange(0, max_key);
	yAxis->setRange(0, max_value * 1.1);
	replotAsync();*/
}
