#pragma once

#include <QGraphicsItem>

class TestDistance;

/**
 * @brief ��ͼ�㣬Ŀǰֻ�в��
 * @note ���ڿ�����Ҫ�����ͼ������ӱ�湦��
 */
class DrawItemLayer : public QGraphicsObject
{
	//Q_OBJECT
public:
	DrawItemLayer(QGraphicsItem * parent);
	~DrawItemLayer();

	// �������
	void clearData();

	//����/�˳���� true:�����࣬false:�˳����
	void setMeasureMode(bool mode);

	//���ò�������
	void setMeasureVisible(bool show);

	QRectF boundingRect() const;

	//��ͼ���� ���µȼ�
	void updateLevel();

	//GisLayer���������¼�
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);//����ͷ�							
	void keyReleaseEvent(QKeyEvent *event);					//���ռ����¼� ESC �˳�����

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

	//ʵʱ��ȡ���λ��
	void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

private:
	//�Ƿ��ǲ��ģʽ
	bool measureMode;

	//���item
	TestDistance * currentTestDist;
	//����ƶ�ʱ��λ�� �����ʱʹ�ã�
	QPointF mouseMoveLonlatPos;

};
