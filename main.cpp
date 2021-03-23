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
#define VIEW_NAME_STATUS_TRACKER     "Status Tracker View"

bool test1()
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


	DockLayoutManager mgr;
	mgr.Load(nodes);
	mgr.Sort();
	mgr.Dump();
	mgr.Build();
	//mgr.PrintNodePath();
	std::list<DockLayoutManager::DockDraw> vActual = mgr.PrintInstructions();
	std::list<DockLayoutManager::DockDraw> vExpect;
	vExpect.push_back({ VIEW_NAME_IMAGE, "", LEFT });
	vExpect.push_back({ VIEW_NAME_PROJECT_LIST, VIEW_NAME_IMAGE, RIGHT });
	vExpect.push_back({ VIEW_NAME_EVENT_GRID,   VIEW_NAME_IMAGE, BOTTOM });
	vExpect.push_back({ VIEW_NAME_CROSSSECTION, VIEW_NAME_EVENT_GRID, RIGHT });
	vExpect.push_back({ VIEW_NAME_LONG_PROFILE, VIEW_NAME_EVENT_GRID, BOTTOM });
	printf("test1 \n");
	printf(vActual == vExpect ? "  PASS! \n" : "  FAIL!!!!! \n");
	return 0;
}




bool test2()
{
	// simulate a complicated case
	//   |---|--------------------|
	//   | P | Image              |
	//   | R |-------|----|-------|
	//   | O | Event |    | Cross |
	//   | J |-------| 3D |-------|
	//   |   | Long  |    |  Cov  |
	//   |---|-------|----|-------|
	// RESULTS
	/*
	==== Layout Dump ====
	ROOT
		* (0, 1  2560x1303):
			* (0, 0  360x1303): Project List
			* (365, 0  2195x1303):
				* (0, 0  2195x395): Image View
				* (0, 400  2195x903):
					* (0, 0  728x903):
						* (0, 0  728x449): Event Grid View
						* (0, 454  728x449): Long Profile View
					* (733, 0  729x903): 3D View
					* (1467, 0  728x903):
						* (0, 0  728x449): Cross Section View
						* (0, 454  728x449): Coverage View
	==== End of Layout Dump ====
			Project List   LEFT of the window:
			  Image View  RIGHT of the window: Project List
		 Event Grid View BOTTOM of the window: Image View
				 3D View  RIGHT of the window: Event Grid View
	  Cross Section View  RIGHT of the window: Event Grid View
	   Long Profile View BOTTOM of the window: Event Grid View
		   Coverage View BOTTOM of the window: Cross Section View
	*/

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


	DockLayoutManager mgr;
	mgr.Load(nodes);
	mgr.Sort();
	mgr.Dump();
	mgr.Build();
	//mgr.PrintNodePath();
	std::list<DockLayoutManager::DockDraw> vActual = mgr.PrintInstructions();
	std::list<DockLayoutManager::DockDraw> vExpect;
	vExpect.push_back({ VIEW_NAME_PROJECT_LIST, "", LEFT });
	vExpect.push_back({ VIEW_NAME_IMAGE,        VIEW_NAME_PROJECT_LIST, RIGHT });
	vExpect.push_back({ VIEW_NAME_EVENT_GRID,   VIEW_NAME_IMAGE,        BOTTOM });
	vExpect.push_back({ VIEW_NAME_3D,           VIEW_NAME_EVENT_GRID,   RIGHT });
	vExpect.push_back({ VIEW_NAME_CROSSSECTION, VIEW_NAME_3D,           RIGHT });
	vExpect.push_back({ VIEW_NAME_LONG_PROFILE, VIEW_NAME_EVENT_GRID,   BOTTOM });
	vExpect.push_back({ VIEW_NAME_COVERAGE,     VIEW_NAME_CROSSSECTION, BOTTOM });
	printf("test2 \n");
	printf(vActual == vExpect ? "  PASS! \n" : "  FAIL!!!!! \n");


	/*
			Project List   LEFT of the window:
		  Image View  RIGHT of the window: Project List
	 Event Grid View BOTTOM of the window: Image View
			 3D View  RIGHT of the window: Event Grid View
  Cross Section View  RIGHT of the window: 3D View
   Long Profile View BOTTOM of the window: Event Grid View
	   Coverage View BOTTOM of the window: Cross Section View
	   */

	return 0;
}

bool test3()
{
	// simulate a complicated case
	//   |---|-----------|--------|
	//   | P | Cross     |        |
	//   | R |-----------|   3D   |
	//   | O | Event     |        |
	//   | J |-----------|--------|
	//   |   | Image     |  Long  |
	//   |---|-----------|--------|


	CDockAreaWidget* window = new CDockAreaWidget(QRect{ 0, 1, 1280, 902 }, nullptr);
	CDockAreaWidget* areaProj = new CDockAreaWidget(QRect{ 0, 0, 360, 902 }, window);
	CDockAreaWidget* areaRight = new CDockAreaWidget(QRect{ 365, 0, 915, 902 }, window);
	CDockAreaWidget* areaRight2 = new CDockAreaWidget(QRect{ 0, 0, 915, 902 }, areaRight);
	CDockAreaWidget* areaRightUp = new CDockAreaWidget(QRect{ 0, 0, 915, 625 }, areaRight2);
	CDockAreaWidget* areaRightUpLeft = new CDockAreaWidget(QRect{ 0, 0, 598, 625 }, areaRightUp);
	CDockAreaWidget* areaCross = new CDockAreaWidget(QRect{ 0, 0, 598, 310 }, areaRightUpLeft);
	CDockAreaWidget* areaEvent = new CDockAreaWidget(QRect{ 0, 315, 598, 310 }, areaRightUpLeft);
	CDockAreaWidget* area3D = new CDockAreaWidget(QRect{ 603, 0, 312, 625 }, areaRightUp);
	CDockAreaWidget* areaRightDown = new CDockAreaWidget(QRect{ 0, 630, 915, 272 }, areaRight2);
	CDockAreaWidget* areaImage = new CDockAreaWidget(QRect{ 0, 0, 455, 272 }, areaRightDown);
	CDockAreaWidget* areaLong = new CDockAreaWidget(QRect{ 460, 0, 455, 272 }, areaRightDown);

	// init dock nodes
	std::list<DockLayoutManager::DockAreaNode*> nodes;
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaProj, VIEW_NAME_PROJECT_LIST));
	nodes.push_back(new DockLayoutManager::DockAreaNode(area3D, VIEW_NAME_3D));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaImage, VIEW_NAME_IMAGE));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaCross, VIEW_NAME_CROSSSECTION));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaLong, VIEW_NAME_LONG_PROFILE));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaEvent, VIEW_NAME_EVENT_GRID));

	DockLayoutManager mgr;
	mgr.Load(nodes);
	mgr.Sort();
	mgr.Dump();
	mgr.Build();
	//mgr.PrintNodePath();
	std::list<DockLayoutManager::DockDraw> vActual = mgr.PrintInstructions();
	std::list<DockLayoutManager::DockDraw> vExpect;
	vExpect.push_back({ VIEW_NAME_PROJECT_LIST, "", LEFT });
	vExpect.push_back({ VIEW_NAME_CROSSSECTION, VIEW_NAME_PROJECT_LIST, RIGHT  });
	vExpect.push_back({ VIEW_NAME_IMAGE,        VIEW_NAME_CROSSSECTION, BOTTOM });
	vExpect.push_back({ VIEW_NAME_3D,           VIEW_NAME_CROSSSECTION, RIGHT  });
	vExpect.push_back({ VIEW_NAME_LONG_PROFILE, VIEW_NAME_IMAGE,        RIGHT  });
	vExpect.push_back({ VIEW_NAME_EVENT_GRID,   VIEW_NAME_CROSSSECTION, BOTTOM });
	printf("test3 \n");
	printf(vActual == vExpect ? "  PASS! \n" : "  FAIL!!!!! \n");

	// RESULTS
	/*
	==== Layout Dump ====
	ROOT
		* (0, 1  1280x902):
			* (0, 0  360x902): Project List
			* (365, 0  915x902):
				* (0, 0  915x902):
					* (0, 0  915x625):
						* (0, 0  598x625):
							* (0, 0  598x310): Cross Section View
							* (0, 315  598x310): Event Grid View
						* (603, 0  312x625): 3D View
					* (0, 630  915x272):
						* (0, 0  455x272): Image View
						* (460, 0  455x272): Long Profile View
	==== End of Layout Dump ====
			Project List   LEFT of the window:
	  Cross Section View  RIGHT of the window: Project List
			  Image View BOTTOM of the window: Cross Section View
				 3D View  RIGHT of the window: Cross Section View
	   Long Profile View  RIGHT of the window: Image View
		 Event Grid View BOTTOM of the window: Cross Section View

	*/
	return 0;

}

bool test4()
{
	// simulate a complicated case
	//   |---|----------------------------|
	//   | P | Event                      |
	//   | R |----------------------------|
	//   | O |    | Tr | Image | Coverage |
	//   | J |    | ac |-------|----------|
	//   |   | 3D | k  |  Long            |
	//   |   |    |    |------------------|
	//   |   |    |    |  Cross           |
	//   |---|----|----|------------------|


	CDockAreaWidget* window = new CDockAreaWidget(QRect{ 0, 1, 2560, 1303 }, nullptr);
	CDockAreaWidget* areaProj = new CDockAreaWidget(QRect{ 0, 0, 360, 1303 }, window);
	CDockAreaWidget* areaRight = new CDockAreaWidget(QRect{ 365, 0, 2195, 1303 }, window);
	CDockAreaWidget* areaEvent = new CDockAreaWidget(QRect{ 0, 0, 2195, 389 }, areaRight);
	CDockAreaWidget* areaBottom = new CDockAreaWidget(QRect{ 0, 394, 2195, 909 }, areaRight);
	CDockAreaWidget* area3D = new CDockAreaWidget(QRect{ 0, 0, 787, 909 }, areaBottom);
	CDockAreaWidget* areaBottomRight = new CDockAreaWidget(QRect{ 1252, 0, 943, 909 }, areaBottom);
	CDockAreaWidget* areaBottomRightTop = new CDockAreaWidget(QRect{ 0, 0, 943, 200 }, areaBottomRight);
	CDockAreaWidget* areaImage = new CDockAreaWidget(QRect{ 0, 0, 319, 200 }, areaBottomRightTop);
	CDockAreaWidget* areaLong = new CDockAreaWidget(QRect{ 0, 205, 943, 200 }, areaBottomRight);
	CDockAreaWidget* areaCross = new CDockAreaWidget(QRect{ 0, 410, 943, 909 }, areaBottomRight);
	CDockAreaWidget* areaCoverage = new CDockAreaWidget(QRect{ 324, 0, 619, 200 }, areaBottomRightTop);
	CDockAreaWidget* areaTrack = new CDockAreaWidget(QRect{ 792, 0, 455, 909 }, areaBottom);

	// init dock nodes
	std::list<DockLayoutManager::DockAreaNode*> nodes;
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaProj, VIEW_NAME_PROJECT_LIST));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaEvent, VIEW_NAME_EVENT_GRID));
	nodes.push_back(new DockLayoutManager::DockAreaNode(area3D, VIEW_NAME_3D));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaImage, VIEW_NAME_IMAGE));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaCross, VIEW_NAME_CROSSSECTION));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaLong, VIEW_NAME_LONG_PROFILE));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaTrack, VIEW_NAME_STATUS_TRACKER));
	nodes.push_back(new DockLayoutManager::DockAreaNode(areaCoverage, VIEW_NAME_COVERAGE));

	DockLayoutManager mgr;
	mgr.Load(nodes);
	mgr.Sort();
	mgr.Dump();
	mgr.Build();
	mgr.PrintNodePath();

	std::list<DockLayoutManager::DockDraw> vActual = mgr.PrintInstructions();
	std::list<DockLayoutManager::DockDraw> vExpect;
	vExpect.push_back({ VIEW_NAME_PROJECT_LIST, "", LEFT });
	vExpect.push_back({ VIEW_NAME_EVENT_GRID,   VIEW_NAME_PROJECT_LIST, RIGHT });
	vExpect.push_back({ VIEW_NAME_3D,           VIEW_NAME_EVENT_GRID,   BOTTOM });
	vExpect.push_back({ VIEW_NAME_STATUS_TRACKER, VIEW_NAME_3D, RIGHT });
	vExpect.push_back({ VIEW_NAME_IMAGE,        VIEW_NAME_STATUS_TRACKER, RIGHT });
	vExpect.push_back({ VIEW_NAME_LONG_PROFILE, VIEW_NAME_IMAGE,        BOTTOM });
	vExpect.push_back({ VIEW_NAME_CROSSSECTION, VIEW_NAME_LONG_PROFILE, BOTTOM });
	vExpect.push_back({ VIEW_NAME_COVERAGE,     VIEW_NAME_IMAGE,        RIGHT });
	
	
	printf("test4 \n");
	printf(vActual == vExpect ? "  PASS! \n" : "  FAIL!!!!! \n");

	// RESULTS
	/*
	==== Layout Dump ====
	ROOT
		* (0, 1  2560x1303):
			* (0, 0  360x1303): Project List
			* (365, 0  2195x1303):
				* (0, 0  2195x389): Event Grid View
				* (0, 394  2195x909):
					* (0, 0  787x909): 3D View
					* (792, 0  455x909): Status Tracker View
					* (1252, 0  943x909):
						* (0, 0  943x200):
							* (0, 0  319x200): Image View
							* (324, 0  619x200): Coverage View
						* (0, 205  943x200): Long Profile View
						* (0, 410  943x909): Cross Section View
	==== End of Layout Dump ====
	Project List  --> (0, 1  2560x1303)  --> (0, 0  360x1303)
	Event Grid View  --> (0, 1  2560x1303)  --> (365, 0  2195x1303)  --> (0, 0  2195x389)
	3D View  --> (0, 1  2560x1303)  --> (365, 0  2195x1303)  --> (0, 394  2195x909)  --> (0, 0  787x909)
	Status Tracker View  --> (0, 1  2560x1303)  --> (365, 0  2195x1303)  --> (0, 394  2195x909)  --> (792, 0  455x909)
	Image View  --> (0, 1  2560x1303)  --> (365, 0  2195x1303)  --> (0, 394  2195x909)  --> (1252, 0  943x909)  --> (0, 0  943x200)  --> (0, 0  319x200)
	Coverage View  --> (0, 1  2560x1303)  --> (365, 0  2195x1303)  --> (0, 394  2195x909)  --> (1252, 0  943x909)  --> (0, 0  943x200)  --> (324, 0  619x200)
	Long Profile View  --> (0, 1  2560x1303)  --> (365, 0  2195x1303)  --> (0, 394  2195x909)  --> (1252, 0  943x909)  --> (0, 205  943x200)
	Cross Section View  --> (0, 1  2560x1303)  --> (365, 0  2195x1303)  --> (0, 394  2195x909)  --> (1252, 0  943x909)  --> (0, 410  943x909)
			Project List   LEFT of the window:
		 Event Grid View  RIGHT of the window: Project List
				 3D View BOTTOM of the window: Event Grid View
	 Status Tracker View  RIGHT of the window: 3D View
			  Image View  RIGHT of the window: Status Tracker View
	   Long Profile View BOTTOM of the window: Image View
	  Cross Section View BOTTOM of the window: Long Profile View
		   Coverage View  RIGHT of the window: Image View
	*/
	return 0;
}


int main()
{
	
	bool b1 = test1();
	bool b2 = test2();
	bool b3 = test3();
	bool b4 = test4();
	if (b1 || b2 || b3 || b4)
	{
		printf(" **** At least one case was failed! ****\n");
		return 1;
	}

	printf("==== ALL PASSED ====\n");
	return 0;
}