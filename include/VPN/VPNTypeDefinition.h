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
		JT_INIT = 0,//��ʼ�Ǽܵ�
		JT_PATHMID = 1,//��Զ��
		JT_ADDOVERLAP = 2//Ϊ��֤�ص��ȵ����ӵ�
	};

	enum PositionType
	{
		PT_OUTSIDE = -1,//����
		PT_ONWALL = 0,//ǽ����
		PT_INSIDE = 1//����
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
