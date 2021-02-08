#pragma once
#include <sstream>
struct QPoint
{
	int x = 0;
	int y = 0;
	friend bool operator==(const QPoint& lhs, const QPoint& rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y;
	}
};

struct QRect
{
public:
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;

	std::string ToString()
	{
		std::stringstream ss;
		ss << "(" << x << ", " << y << "  " << width << "x" << height << ")";
		return ss.str();
	}

	friend bool operator==(const QRect& lhs, const QRect& rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y &&
			lhs.width == rhs.width && lhs.height == rhs.height;
	}
};

class CDockAreaWidget
{
public:
	CDockAreaWidget(QRect rect, CDockAreaWidget* parent)
	{
		m_Rect = rect;
		m_Parent = parent;
	}

	QRect geometry() const { return m_Rect;  }
	CDockAreaWidget* parentWidget() { return m_Parent; }

private:
	QRect m_Rect;
	CDockAreaWidget* m_Parent;
};

enum Position
{
	CENTER,
	LEFT,
	RIGHT,
	TOP,
	BOTTOM,	
};