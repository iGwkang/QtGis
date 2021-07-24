#pragma once

#include <QGraphicsItem>

class TestDistance;

/**
 * @brief 绘图层，目前只有测距
 * @note 后期可能需要在这个图层上添加标绘功能
 */
class DrawItemLayer : public QGraphicsObject
{
	//Q_OBJECT
public:
	DrawItemLayer(QGraphicsItem * parent);
	~DrawItemLayer();

	// 清空数据
	void clearData();

	//进入/退出测距 true:进入测距，false:退出测距
	void setMeasureMode(bool mode);

	//设置测距的显隐
	void setMeasureVisible(bool show);

	QRectF boundingRect() const;

	//地图缩放 更新等级
	void updateLevel();

	//GisLayer传过来的事件
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);//鼠标释放							
	void keyReleaseEvent(QKeyEvent *event);					//接收键盘事件 ESC 退出绘制

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

	//实时获取鼠标位置
	void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

private:
	//是否是测距模式
	bool measureMode;

	//测距item
	TestDistance * currentTestDist;
	//鼠标移动时的位置 （测距时使用）
	QPointF mouseMoveLonlatPos;

};
