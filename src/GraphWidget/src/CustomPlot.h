#pragma once
#include "qcustomplot_mod.h"
#include <optional>
#include <mutex>

enum class TimeUnit : int {
	time_none = -1,
	second = 1000,
	ms100 = 100,
	ms50 = 50,
};


class CustomPlot : public QCustomPlot
{
public:
    // этот график всегда можно перетаскивать и зумить
    CustomPlot(QWidget* parent);
    QByteArray ToBytes();
    void FromBytes(QByteArray bytes);

    void setHorizontalAxisLimit(double limit);
    void setVerticalAxisLimit(double limit);
    void setPointStep(int step);
    QPointF PointToStep(const QPoint& pos, bool add);
    QPointF PointToStep(const QPointF& pos, bool add);
    void MoveAllPointsToStep();
    void SetEditable(bool editable) { editable_ = editable; }
    void AddHint(QString str) { hints_.append(str); }

    void replotAsync();
public slots:
    void returnToDefaultView();

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

private:
    std::mutex _lock;

    bool mMousePressed;
    bool mPointSelected;
    QPoint prev_mouse_position_while_dragging_;
    // нижние пределы для вертикальной и горизонтальной оси
    std::optional<double> horizontal_limit_, vertical_limit_;
    double point_capture_distance_ = 20;
    double border_add_if_default_ = 5;
    double selected_point_x_;
    double selected_point_y_;
    TimeUnit point_step_ = TimeUnit::time_none;
    bool editable_ = true;

    QVector<QString> hints_;
    int current_hint_index_ = -1;
    QCPItemText* current_hint_ = nullptr;
    QCPItemRect* current_hint_rect_ = nullptr;
    int pixel_frame_width_ = 3;

    void movePlotRanges(double x_diff, double y_diff);
    void CheckHints(QPoint pos);
};

