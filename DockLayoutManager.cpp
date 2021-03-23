#include "DockLayoutManager.h"
#include "stdio.h"
#include <algorithm>

DockLayoutManager::DockLayoutManager()
{

}

DockLayoutManager::~DockLayoutManager()
{

}

void DockLayoutManager::Load(std::list<DockAreaNode*> nodes)
{
	m_NodeRoot = new DockAreaNode(nullptr, "Root");
	for (auto node : nodes)
	{
		Load(node);
	}
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
		return first.x > second.x;
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

void DockLayoutManager::Build()
{
	std::list<DockAreaNode*> path;
	path.push_back(m_NodeRoot);
	for (auto root : m_NodeRoot->m_Children)
	{
		m_mapNodes.clear();

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

bool sortNode(std::pair<std::string, int> i, std::pair<std::string, int> j)
{
	return i.second < j.second;
}

void DockLayoutManager::BuildInstructions()
{
	// restore the depth of the node, and sort it - it is also the order when building the layout
	std::vector<std::pair<std::string, int>> depthOrder;
	for (auto node : m_mapNodes)
	{
		std::string name = node.first;
		std::list<DockAreaNode*> path = node.second;
		int depth = path.size();
		for (auto iter = path.rbegin(); iter != path.rend(); iter++)
		{
			if (!(*iter)->m_Area)
			{
				break;
			}
			if ((*iter)->m_Area->geometry().x != 0 || (*iter)->m_Area->geometry().y != 0)
			{
				break;
			}
			if ((*iter)->parent->m_WindowId != "")
			{
				break;
			}
			depth -= 1;
		}
		depthOrder.push_back({ name, depth });
	}

	// sort the order as building order
	std::sort(depthOrder.begin(), depthOrder.end(), sortNode);

	// restores the global position of the windows
	struct NodeData
	{
		std::string name;
		QRect area = { 0, 0, 0, 0 };
		std::list<DockAreaNode*> path;
	};
	std::vector<NodeData> nodes((int)depthOrder.size(), NodeData());
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

		// Find the node in ordered list
		for (int i = 0; i < (int)depthOrder.size(); i++)
		{
			if (depthOrder[i].first == nodeName)
			{
				nodes[i].name = nodeName;
				nodes[i].area = point;
				nodes[i].path = node.second;
				break;
			}
		}
	}

	// build the layout instructions
	for (int index = 0; index < (int)nodes.size(); index++)
	{
		const QRect currentPoint = nodes[index].area;
		const std::string currentPointName = depthOrder[index].first;
		if (index == 0)
		{
			// for the first window, we add it to the LEFT
			m_Instructions.push_back({ currentPointName, "", LEFT });
		}
		else
		{
			int maxNumOfCommonAncestors = -1;
			int indexOfMaxCommonAncestor = -1;

			// find the relative node with either x the same or y
			for (int searchIndex = 0; searchIndex < index; searchIndex++)
			{
				const QRect relativePoint = nodes[searchIndex].area;

				// if either x or y is the same, it means we can find the relative position,
				if (relativePoint.x == currentPoint.x || relativePoint.y == currentPoint.y)
				{
					int lca = GetLowestCommonAncestor(nodes[index].path, nodes[searchIndex].path);
					if (lca >= maxNumOfCommonAncestors)
					{
						maxNumOfCommonAncestors = lca;
						indexOfMaxCommonAncestor = searchIndex;
					}
				}
			}

			// we have found the parent node
			if (indexOfMaxCommonAncestor != -1)
			{
				Position posRelative = GetPostionOfTwoPoints(nodes[indexOfMaxCommonAncestor].area, currentPoint);
				m_Instructions.push_back({ currentPointName, nodes[indexOfMaxCommonAncestor].name, posRelative });
			}
		}
	}
}

int DockLayoutManager::GetLowestCommonAncestor(const std::list<DockAreaNode*>& path1, const std::list<DockAreaNode*>& path2)
{
	auto iter1 = path1.begin();
	auto iter2 = path2.begin();
	int num = 0;
	while (iter1 != path1.end() && iter2 != path2.end())
	{
		if ((*iter1)->m_Area && (*iter2)->m_Area && (*iter1)->m_Area->geometry() != (*iter2)->m_Area->geometry())
		{
			break;
		}
		num++;
		iter1++;
		iter2++;
	}
	return num;
}


// This function is to identify the related position of two rectangles. 
Position DockLayoutManager::GetPostionOfTwoPoints(const QRect& first, const QRect& second)
{
	if (first.x == second.x)
	{
		if (first.y == second.y)
		{
			return CENTER;
		}
		if (first.y < second.y)
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
		if (first.y < second.y)
		{
			if (first.x + first.width < second.x)
			{
				return RIGHT;
			}
			return BOTTOM;
		}
		if (first.y > second.y + second.height)
		{
			return TOP;
		}
		return RIGHT; 
	}
	else
	{
		if (first.y == second.y)
		{
			return LEFT;
		}
		if (first.y < second.y)
		{
			if (first.y + first.height < second.y)
			{
				return BOTTOM;
			}
			return LEFT;
		}
		if (first.y > second.y + second.height)
		{
			return TOP;
		}
		return LEFT;
	}
}

void DockLayoutManager::PrintNodePath()
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

std::list<DockLayoutManager::DockDraw> DockLayoutManager::PrintInstructions()
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
	return m_Instructions;
}