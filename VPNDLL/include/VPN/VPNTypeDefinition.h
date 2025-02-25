#ifndef __VPN_TYPE_DEFINITION__
#define __VPN_TYPE_DEFINITION__


namespace ViewPointNetwork
{
	enum HouseType
	{
		HT_Indoor = 0,
		HT_Outdoor = 1
	};


	enum RidgeType
	{
		RT_L1 = 1,
		RT_L2 = 2
	};

	enum JunctionType
	{
		JT_INIT = 0,// Initial skeleton point
		JT_PATHMID = 1,// The farthest point
		JT_ADDOVERLAP = 2// To ensure the overlap of the connection point
	};

	enum PositionType
	{
		PT_OUTSIDE = -1,// Outdoor
		PT_ONWALL = 0,// Wall line
		PT_INSIDE = 1// Indoor
	};

	enum OverlapScoreType
	{
		AVERAGE_LEN = 0,
		MIN_LEN = 1,
		UNION_LEN = 2,
		INTERSECTSUM_ANG = 3,
		INTERSECT_ANG = 4
	};
}

#endif
