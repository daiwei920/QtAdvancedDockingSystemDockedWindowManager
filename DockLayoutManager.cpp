#include "DockLayoutManager.h"
#include "stdio.h"
#include <algorithm>

DockLayoutManager::DockLayoutManager(std::list<DockLayoutManager::DockAreaNode*> nodes)
{
	m_NodeRoot = new DockAreaNode(nullptr, "Root");
	for (auto node : nodes)
	{
		Load(node);
	}
}

DockLayoutManager::~DockLayoutManager()
{

}

void DockLayoutManager::Load(DockAreaNode* node)
{
	// check if the node is already in our list
	if (!IsNodeExist(node))
	{
		m_Nodes.push_back(node);
	}

	if (node->m_Area->parentWidget() == nullptr ||
		(node->m_Area->geometry().x == 0 && node->m_Area->geometry().y == 1))
	{
		// if the parent is the main frame position (0, 1), we also stop it
		// if not add into root node, add it now
		if (std::find(m_NodeRoot->m_Children.begin(), m_NodeRoot->m_Children.end(), node) == m_NodeRoot->m_Children.end())
		{
			m_NodeRoot->m_Children.push_back(node);
			node->parent = m_NodeRoot;
		}
		return;
	}

	CDockAreaWidget* parent = node->m_Area->parentWidget();
	if (parent)
	{
		DockAreaNode* parentNode = IsAreaExist(parent);
		if (!parentNode)
		{
			parentNode = new DockAreaNode(parent);
			// now recursive to check parents node
			Load(parentNode);
		}
		parentNode->m_Children.push_back(node);
		node->parent = parentNode;
	}
}

DockLayoutManager::DockAreaNode* DockLayoutManager::IsAreaExist(CDockAreaWidget* n)
{
	for (auto node : m_Nodes)
	{
		if (node->m_Area == n)
		{
			return node;
		}
	}
	return nullptr;
}

bool DockLayoutManager::IsNodeExist(DockAreaNode* n)
{
	for (auto node : m_Nodes)
	{
		if (node == n)
		{
			return true;
		}
	}
	return false;
}

void DockLayoutManager::Dump()
{
	printf("==== Layout Dump ====\n");
	Dump(m_NodeRoot);
	printf("==== End of Layout Dump ====\n");
}

void DockLayoutManager::Dump(DockAreaNode* node, int recursiveLevel)
{
	if (node->m_Area == nullptr)
	{
		printf("ROOT\n");
	}
	else
	{
		for (int j = 0; j < recursiveLevel; j++)
		{
			printf("    ");
		}		
		printf("* %s: %s\n", node->m_Area->geometry().ToString().c_str(), node->m_WindowId.c_str());
	}

	for (int i = 0; i < (int)node->m_Children.size(); i++)
	{
		Dump(node->m_Children[i], recursiveLevel + 1);
	}
}

bool compareRect(const QRect& first, const QRect& second)
{
	if (first.y == second.y)
	{
		return first.x < second.x;
	}
	return first.y < second.y;
}

bool comparePairNodes(const std::pair<QRect, std::string>& p1, const std::pair<QRect, std::string>& p2)
{
	return compareRect(p1.first, p2.first);
}

bool compareNodes(const DockLayoutManager::DockAreaNode* first, const DockLayoutManager::DockAreaNode* second)
{
	return compareRect(first->m_Area->geometry(), second->m_Area->geometry());
}

void DockLayoutManager::Sort()
{
	return Sort(m_NodeRoot);

}
void DockLayoutManager::Sort(DockLayoutManager::DockAreaNode* node)
{
	if (node && node->m_Children.size() > 0)
	{
		std::sort(node->m_Children.begin(), node->m_Children.end(), compareNodes);
		for (int i = 0; i < (int)node->m_Children.size(); i++)
		{
			Sort(node->m_Children[i]);
		}
	}
}

void DockLayoutManager::Draw()
{
	std::list<DockAreaNode*> path;
	path.push_back(m_NodeRoot);
	for (auto root : m_NodeRoot->m_Children)
	{
		// iterate the tree and build the path for LCA (Lowest Common Ancestor)
		BuildNodePath(root, path);

		// now build layout instructions
		BuildInstructions();
	}
}

void DockLayoutManager::BuildNodePath(DockAreaNode* node, std::list<DockAreaNode*>& path)
{
	path.push_back(node);
	if (node->m_WindowId != "Root" && node->m_WindowId != "")
	{
		std::pair<std::string, std::list<DockAreaNode*>> p{ node->m_WindowId , path };
		m_mapNodes.push_back(p);
	}
	else
	{
		for (auto n : node->m_Children)
		{
			BuildNodePath(n, path);
		}
	}
	path.pop_back();
}

void DockLayoutManager::BuildInstructions()
{
	// build the global absolute position
	std::list<std::pair<QRect, std::string>> nodes;
	for (auto node : m_mapNodes)
	{
		std::string nodeName = node.first;
		std::list<DockAreaNode*> paths = node.second;
		QRect point = { 0,0,0,0 };
		for (auto path : paths)
		{
			// accumulate x,y value to get global position
			if (path->m_Area)
			{
				QRect rect = path->m_Area->geometry();
				point.x += rect.x;
				point.y += rect.y;
			}
			// get height and width from the last node
			if (path->m_Children.empty())
			{
				point.height = path->m_Area->geometry().height;
				point.width = path->m_Area->geometry().width;
			}
		}
		
		nodes.push_back({ point, nodeName });
	}

	// sort the points
	nodes.sort(comparePairNodes);

	// build instructions
	for (auto iter = nodes.begin(); iter != nodes.end(); iter++)
	{
		const QRect currentPoint = iter->first;
		const std::string currentPointName = iter->second;
		// add the instruction
		if (m_Instructions.empty())
		{
			// for the first window, we add it to the LEFT
			m_Instructions.push_back({ currentPointName, "", LEFT });
		}
		else
		{
			std::string parentNodeName = "";
			Position posRelative;
			// find the relative node with either x the same or y
			auto iterFindRelative = iter;
			while (iterFindRelative != nodes.begin())
			{
				iterFindRelative--;
				const QRect relativePoint = iterFindRelative->first;
				const std::string relativePointName = iterFindRelative->second;

				// if either x or y is the same, it means we can find the relative position,
				// Also considering the width/height
				const int FRAME_WIDTH = 10;
				if ((relativePoint.x == currentPoint.x && currentPoint.y - relativePoint.y - relativePoint.height < FRAME_WIDTH) ||
					(relativePoint.y == currentPoint.y && currentPoint.x - relativePoint.x - relativePoint.width < FRAME_WIDTH))
				{
					parentNodeName = relativePointName;
					posRelative = GetPostionOfTwoPoints(relativePoint, currentPoint);
					break;
				}
			}
			// we have found the parent node
			if (parentNodeName != "")
			{
				m_Instructions.push_back({ currentPointName, parentNodeName, posRelative });
			}
		}
	}
}

Position DockLayoutManager::GetPostionOfTwoPoints(const QRect& first, const QRect& second)
{
	if (first.x == second.x)
	{
		if (first.y == second.y)
		{
			return CENTER;
		}
		else if (first.y < second.y)
		{
			return BOTTOM;
		}
		return TOP;
	}
	else if (first.x < second.x)
	{
		if (first.y == second.y)
		{
			return RIGHT;
		}
		else if (first.y < second.y)
		{
			// bottom right
			return BOTTOM; // should not happen, we have sorted already!
		}
		// top right
		return RIGHT; // should not happen, we have sorted already!
	}
	else
	{
		if (first.y == second.y)
		{
			return LEFT; // should not happen, we have sorted already!
		}
		else if (first.y < second.y)
		{
			// bottom left
			return LEFT; // should not happen, we have sorted already!
		}
		// top left
		return TOP;  // should not happen, we have sorted already!
	}
}

void DockLayoutManager::Print()
{
	for (auto iter = m_mapNodes.begin(); iter != m_mapNodes.end(); iter++)
	{
		std::pair<std::string, std::list<DockAreaNode*>> item = *iter;
		printf("%s ", item.first.c_str());
		std::list<DockAreaNode*> path = item.second;
		for (auto p : path)
		{
			if (p->m_Area)
			{
				printf(" --> %s ", p->m_Area->geometry().ToString().c_str());
			}
		}
		printf("\n");
	}
}

void DockLayoutManager::PrintInstructions()
{
	for (auto iter = m_Instructions.begin(); iter != m_Instructions.end(); iter++)
	{
		std::string pos;
		switch (iter->pos)
		{
		case LEFT:  pos = "LEFT"; break;
		case RIGHT: pos = "RIGHT"; break;
		case TOP:   pos = "TOP"; break;
		case BOTTOM:pos = "BOTTOM"; break;
		}
		printf("%20s %6s of the window: %s\n", iter->name.c_str(), pos.c_str(), iter->parentName.c_str());
	}
}