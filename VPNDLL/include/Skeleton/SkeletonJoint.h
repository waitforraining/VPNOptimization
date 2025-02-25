#ifndef __VPN_SKELETON_JOINT__
#define __VPN_SKELETON_JOINT__

#include "preCompiled.h"

#include "VPN/VPNTypeDefinition.h"

namespace ViewPointNetwork
{
	class VPN_API SkeletonJoint
	{
	public:
		SkeletonJoint() ;
		SkeletonJoint(int x, int y, JunctionType jt = JunctionType::JT_INIT);
		SkeletonJoint(const SkeletonJoint& rhv);
		~SkeletonJoint();
		int X() const;
		int Y() const;
		JunctionType getType() const { return m_type; };

	public:
		SkeletonJoint& operator=(const SkeletonJoint& rhv);
		bool operator ==(const SkeletonJoint& rhv) const;
		bool operator !=(const SkeletonJoint& rhv) const;
		bool operator <(const SkeletonJoint& other) const;
		void setJunctionType(JunctionType jt);

	private:
		int m_x;
		int m_y;
		JunctionType m_type;
	};
}

#endif
