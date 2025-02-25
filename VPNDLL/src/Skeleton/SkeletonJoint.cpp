#include "Skeleton/SkeletonJoint.h"

namespace ViewPointNetwork
{
	SkeletonJoint::SkeletonJoint() :
		m_x(0), m_y(0), m_type(JunctionType::JT_INIT)
	{
	}

	SkeletonJoint::SkeletonJoint(int x, int y, JunctionType jt ) :
		m_x(x), m_y(y), m_type(jt)
	{
	}

	SkeletonJoint::SkeletonJoint(const SkeletonJoint& rhv) :
		m_x(rhv.m_x), m_y(rhv.m_y), m_type(rhv.m_type)
	{

	}

	SkeletonJoint::~SkeletonJoint()
	{

	}

	SkeletonJoint& SkeletonJoint::operator=(const SkeletonJoint& rhv)
	{
		if (this == &rhv)
			return *this;

		m_x = rhv.m_x;
		m_y = rhv.m_y;
		m_type = rhv.m_type;

		return *this;
	}

	bool SkeletonJoint::operator ==(const SkeletonJoint & rhv) const
	{
		//return m_x == rhv.m_x && m_y == rhv.m_y && m_type == rhv.m_type;
		return m_x == rhv.m_x && m_y == rhv.m_y;
	}

	bool SkeletonJoint::operator !=(const SkeletonJoint & rhv) const
	{
		return !(operator==(rhv));
	}

	bool SkeletonJoint::operator <(const SkeletonJoint& rhv) const {
		if (m_x != rhv.m_x) {
			return m_x < rhv.m_x;
		}
		return m_y < rhv.m_y;
	}

	int SkeletonJoint::X() const{
		return m_x;
	}

	int SkeletonJoint::Y() const{
		return m_y;
	}

	void SkeletonJoint::setJunctionType(JunctionType jt) {
		m_type = jt;
	}
}