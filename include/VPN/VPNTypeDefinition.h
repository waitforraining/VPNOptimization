#ifndef __VPN_TYPE_DEFINITION__
#define __VPN_TYPE_DEFINITION__


#define NUMBER_H    "%c Number of points in this file : %d\n"

#define O_CONTENTS_H    "%c This file contains object points\n"
#define O_EXPL_H   "%c                       coordinates                                   variances                                    covariances\n\
%c point        x                y              z              x              y              z               xy             xz             yz\n"
#define O_LINE_H   "%c-----------------------------------------------------------------------------------------------------------------------------------------------\n"
#define O_FORMAT   " %4d%17.7lf%17.7lf%15.7lf%16.7le%15.7le%15.7le%16.7le%15.7le%15.7le\n"
#define O_FORMAT2   " %4d%16.7lf%16.7lf%15.7lf\n"

#define OP_EXPL_H   "%c                 coordinates                    variances                  covariances\n\
%cpoint    x           y           z        x        y        z        xy        xz        yz\n"
#define OP_LINE_H   "%c-----------------------------------------------------------------------------------------------\n"
#define OP_FORMAT  "%4d%11.5lf,%11.5lf,%11.5lf%9.2le%9.2le%9.2le%10.2le%10.2le%10.2le\n"

#define M_CONTENTS_H    "%c This file contains model points\n"
#define M_EXPL_H   "%c                       coordinates                                   variances                                    covariances\n\
%c point        x                y              z              x              y              z               xy             xz             yz\n"
#define M_LINE_H   "%c-----------------------------------------------------------------------------------------------------------------------------------------------\n"
#define M_FORMAT   " %4d%17.7lf%17.7lf%15.7lf%16.7le%15.7le%15.7le%16.7le%15.7le%15.7le\n"

#define MP_EXPL_H   "%c                 coordinates                    variances                  covariances\n\
%cpoint    x           y           z        x        y        z        xy        xz        yz\n"
#define MP_LINE_H   "%c-----------------------------------------------------------------------------------------------\n"
#define MP_FORMAT  "%4d%11.5lf,%11.5lf,%11.5lf%9.2le%9.2le%9.2le%10.2le%10.2le%10.2le\n"

#define C_CONTENTS_H   "%c This file contains control points\n"
#define C_STATUS_EXPL_H "%c The status field has the following meaning :\n\
%c\t0 -> Full control point (x, y and z known)\n\
%c\t1 -> Planimetric control point (x and y known, z is not to be used)\n\
%c\t2 -> Height control point (only z is known and can be used)\n\
%c\t3 -> No control point, all coordinates are unknown\n"
#define C_EXPL_H   "%c Pointnumber         x               y              z        status\n"
#define C_LINE_H   "%c------------- -------------- --------------- -------------- --------\n"
#define C_FORMAT  " %9d%16.3lf%16.3lf%15.3lf%9d\n"


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
		JT_INIT = 0,//初始骨架点
		JT_PATHMID = 1,//最远点
		JT_ADDOVERLAP = 2//为保证重叠度的连接点
	};

	enum PositionType
	{
		PT_OUTSIDE = -1,//室外
		PT_ONWALL = 0,//墙线上
		PT_INSIDE = 1//室内
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
