#include "DockLayoutManager.h"
#include "Core.h"


#define VIEW_NAME_LOBBY              "Lobby"
#define VIEW_NAME_PROJECT_LIST       "Project List"
#define VIEW_NAME_3D                 "3D View"
#define VIEW_NAME_IMAGE              "Image View"
#define VIEW_NAME_CROSSSECTION       "Cross Section View"
#define VIEW_NAME_LONG_PROFILE       "Long Profile View"
#define VIEW_NAME_EVENT_GRID         "Event Grid View"
#define VIEW_NAME_SETTINGS           "Settings"
#define VIEW_NAME_COVERAGE           "Coverage View"
#define VIEW_NAME_TOOLS              "Point Classification"
#define VIEW_NAME_HELP               "Help"

int test1()
{
	// simulate a complicated case (2x windows)
	//   |-----------------|---|        |-------------| 
	//   | Image           | P |        | 3D View     |
	//   |-------|---------| R |        |-------------|
	//   | Event |         | O |
	//   |-------|  Cross  | J |
	//   | Long  |         |   |
	//   |-------|---------|---|


	// 3D Window
	CDockAreaWidget* window2 = new CDockAreaWidget(QRect{ 0, 1, 728, 926 }, nullptr);
	CDockAreaWidget* area3D  = new CDockAreaWidget(QRect{ 0, 0, 728, 926 }, window2);

	// Main Window
	CDockAreaWidget* window1  = new CDockAreaWidget(QRect{   0, 1, 1280, 902 }, nullptr);
	CDockAreaWidget* areaProj = new CDockAreaWidget(QRect{ 920, 0,  360, 902 }, window1);
	CDockAreaWidget* areaLeft = new CDockAreaWidget(QRect{   0, 0,  915, 902 }, window1);
	CDockAreaWidget* areaImage = new CDockAreaWidget(QRect{  0, 0,  915, 370 }, areaLeft);
	CDockAreaWidget* areaLeft2 = new CDockAreaWidget(QRect{ 0, 375, 915, 527 }, areaLeft);
	CDockAreaWidget* areaLeft3 = new CDockAreaWidget(QRect{ 0,   0, 454, 527 }, areaLeft2);
	CDockAreaWidget* areaEvent = new CDockAreaWidget(QRect{ 0,   0, 454, 261 }, areaLeft3);
	CDockAreaWidget* areaCross = new CDockAreaWidget(QRect{ 459, 0, 456, 527 }, areaLeft2); 
	CDockAreaWidget* areaLong  = new CDockAreaWidget(QRect{ 0, 266, 454, 261 }, areaLeft3);


	// init dock nodes
	std::list<DockLayoutManager::DockAreaNode*> nodes;

	nodes.push_back(new DockLayoutManager::DockAreaNode(areaProj,  VIEW_NAME_PROJECT_LIST));
	//nodes.push_back(new DockLayoutManager::DockAreaNode(area3D,    VIEW_NAME_3D));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaImage, VIEW_NAME_IMAGE));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaCross, VIEW_NAME_CROSSSECTION));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaLong,  VIEW_NAME_LONG_PROFILE));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaEvent, VIEW_NAME_EVENT_GRID));
	//nodes.push_back(new DockLayoutManager::DockAreaNode(area3D, VIEW_NAME_SETTINGS));
	//nodes.push_back(new DockLayoutManager::DockAreaNode(area3D, VIEW_NAME_COVERAGE));
	//nodes.push_back(new DockLayoutManager::DockAreaNode(area3D, VIEW_NAME_TOOLS));
	//nodes.push_back(new DockLayoutManager::DockAreaNode(area3D, VIEW_NAME_HELP));


	DockLayoutManager mgr(nodes);
	//mgr.Dump();
	mgr.Sort();
	mgr.Dump();
	mgr.Draw();
	mgr.Print();
	mgr.PrintInstructions();
	
	return 0;
}




int test2()
{
	// simulate a complicated case
	//   |---|--------------------|
	//   | P | Image              |
	//   | R |-------|----|-------|
	//   | O | Event |    | Cross |
	//   | J |-------| 3D |-------|
	//   |   | Long  |    |  Cov  |
	//   |---|-------|----|-------|

	// Main Window
	CDockAreaWidget* window = new CDockAreaWidget(QRect{ 0, 1, 2560, 1303 }, nullptr);
	CDockAreaWidget* areaProj = new CDockAreaWidget(QRect{ 0,0,360,1303 }, window);
	CDockAreaWidget* areaRight = new CDockAreaWidget(QRect{ 365,0,2195,1303 }, window);
	CDockAreaWidget* areaRightBottom = new CDockAreaWidget(QRect{ 0,400,2195,903 }, areaRight);
	CDockAreaWidget* areaRightBottomRight = new CDockAreaWidget(QRect{ 1467,0,728,903 }, areaRightBottom);
	CDockAreaWidget* areaCoverage = new CDockAreaWidget(QRect{ 0,454,728,449 }, areaRightBottomRight);
	CDockAreaWidget* areaImage = new CDockAreaWidget(QRect{ 0,0,2195,395 }, areaRight);
	CDockAreaWidget* areaRightBottomLeft = new CDockAreaWidget(QRect{ 0,0,728,903 }, areaRightBottom);
	CDockAreaWidget* areaEvent = new CDockAreaWidget(QRect{ 0,0,728,449 }, areaRightBottomLeft);
	CDockAreaWidget* area3D = new CDockAreaWidget(QRect{ 733,0,729,903 }, areaRightBottom);
	CDockAreaWidget* areaCross = new CDockAreaWidget(QRect{ 0,0,728,449 }, areaRightBottomRight);
	CDockAreaWidget* areaLong = new CDockAreaWidget(QRect{ 0,454,728,449 }, areaRightBottomLeft);

	// init dock nodes
	std::list<DockLayoutManager::DockAreaNode*> nodes;

	nodes.push_back(new DockLayoutManager::DockAreaNode(areaProj, VIEW_NAME_PROJECT_LIST));
	nodes.push_back(new DockLayoutManager::DockAreaNode(area3D,    VIEW_NAME_3D));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaImage, VIEW_NAME_IMAGE));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaCross, VIEW_NAME_CROSSSECTION));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaLong, VIEW_NAME_LONG_PROFILE));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaEvent, VIEW_NAME_EVENT_GRID));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaCoverage, VIEW_NAME_COVERAGE));


	DockLayoutManager mgr(nodes);
	//mgr.Dump();
	mgr.Sort();
	mgr.Dump();
	mgr.Draw();
	mgr.Print();
	mgr.PrintInstructions();

	return 0;
}


int main()
{
	return test2();
}