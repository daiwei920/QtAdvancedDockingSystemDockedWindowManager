#pragma once
#include "Core.h"
#include <list>
#include <string>
#include <vector>
#include <map>

class DockLayoutManager
{
public:

	struct DockAreaNode
	{
		DockAreaNode(CDockAreaWidget* area, std::string id = "") :
			m_Area(area),
			m_WindowId(id)
		{

		}
		std::string m_WindowId;
		CDockAreaWidget* m_Area = nullptr;
		DockAreaNode* parent = nullptr;
		std::vector<DockAreaNode*> m_Children;
	};

	// the instruction on how to draw,
	// draw the window on the 'position' of the 'area'
	struct DockDraw
	{
		std::string name;
		std::string parentName;
		Position pos; // the relative position of parent window
	};


	DockLayoutManager();
	~DockLayoutManager();


	void Load(std::list<DockAreaNode*> nodes);

	void Dump();
	void Dump(DockAreaNode* node, int recursiveLevel = 0);

	void Sort();
	void Sort(DockAreaNode* node);

	void Build();
	

	void Print();
	void PrintInstructions();

private:
	std::list<DockAreaNode*> m_Nodes;
	std::list<DockDraw> m_Instructions;
	std::vector<std::pair<std::string, std::list<DockAreaNode*>>> m_mapNodes;

	DockAreaNode* m_NodeRoot = nullptr;


	bool IsNodeExist(DockAreaNode* node);
	DockAreaNode* IsAreaExist(CDockAreaWidget* node);
	void Load(DockAreaNode* node);

	void BuildNodePath(DockAreaNode* node, std::list<DockAreaNode*>& path);

	void BuildInstructions();

	Position GetPostionOfTwoPoints(const QRect& first, const QRect& second);

	int GetLowestCommonAncestor(const std::list<DockAreaNode*>& path1, const std::list<DockAreaNode*>& path2);


};