#pragma once

#include "MathUtils.h"

namespace bs
{
	struct Bivector : public vec3
	{
		constexpr Bivector(vec3 v) noexcept : vec3(std::move(v))
		{	}

		//A bivector is the returned result from an outer product, contains area of enclose parallelogram

		//Is the "normal vector" to the plane, each component is the area of the projected basis plane

		// x val = (x ^ y)	| z plane
		// y val = (x ^ z)	| y-plane |	use (z^x) to make the signs same as cross product
		// z val = (y ^ z)	| x-plane
	};

	//Rotate 'V' across a plane created by surface normal N
	template<glm::length_t L, typename T, glm::qualifier Q>
	constexpr glm::vec<L, T, Q> reflect(glm::vec<L, T, Q> const& V, glm::vec<L, T, Q> const& N) noexcept
	{
		//According to https://marctenbosch.com/quaternions/

		/*if(dot(N, N) == 1)	//This is a possible impl for when N is not necessarily normalized
		{
			return V - (N * dot(N, V) * static_cast<T>(2)) / dot(N, N);
		}*/

		//THIS IMPL IS THE SAME AS GLM, in glm/geometric.hpp
		return V - N * dot(N, V) * static_cast<T>(2);
		// (n1*v1 + n2*v2 + n3*v3)*(n1, n2, n3)
		// = ||V||cos(a) * (n1, n2, n3)
		//Eh nah nvm, not gonna try to simplify this, probably already done maximally
	}

	// -ava = -av(a)^(-1) => a ~=~ a^-1 => a^-1 = 1/a => when a is unit vector, aa = aa^-1 = 1
	class Rotor
	{
	public:
		float scalar;
		Bivector plane;
	public:
		//Default constructor
		constexpr Rotor() noexcept	: scalar(1.0f), plane({0.0f, 0.0f, 0.0f})	{}
		//Construct from individual values
		constexpr Rotor(float scalar, float xy, float xz, float yz) noexcept : scalar(std::move(scalar)), plane({xy, xz, yz})	{}

		//Construct from angle + plane, plane must be normalized
		constexpr Rotor(Bivector plane, float radians) noexcept	: plane(std::move(plane))
		{
			//Only want half the angle
			radians /= -2.0f;
			//Scalar Part
			scalar = cos(radians);

			//Bivector part,  necessary when plane is normalized
			plane *= sin(radians);
			//Not -sin(radians), because sin is an odd function, and flipped the signs in [radians/=-2.0f]
			//cos is an even function, so the sign is irrelevant
		}
		//Construct from traversing from, and to, a vector
		constexpr Rotor(vec3 start, vec3 dest) noexcept : scalar(0.0f), plane({0.0f, 0.0f, 0.0f})
		{
			const auto&& crspdt = cross(start, dest);
			const auto rotPlane = Bivector({crspdt.z, -crspdt.y, crspdt.x});

			//Apparently a trick to get cos(a/2) from cos(a) => 1 + dot(start, dest)
			//Actual equation is cos(a/2) = +- sqrt( (1+cos(a)) / 2)
			//So when normalizing the rotor, the sqrt is removed bc s^2, so apparently it works?
			scalar = 1 + dot(start, dest);
			plane = rotPlane;
			normalize();

			//Idea:
			/**
			 * @brief Geometry Possibility
			 * 
			 * A rotor rotates by 2a
			 * dot(from, to) is cos(a)
			 * 
			 * vec A and B in the rotor creating the geometric product are unit/normalized, dot is cos(a/2)
			 * 
			 * So technically, normalizing them and applying it as a rotor will have it 
			 * rotate by twice the angle [2 * 2a = 4a] between [from] and [to], so the rotation needs to be halved
			 * 
			 * THIS IS WHAT IS USED (in theory)!!! I was right, and it is a thing, as shown here [different method]:
			 * http://physicsforgames.blogspot.com/2010/03/quaternion-tricks.html
			 * 
			 * Can take avg of from and to, normalize it, and use dot/cross on [from->avg] to achive [to].
			 * Thereby using that method it is possible.
			 * 
			 * THE TRICK DESCRIBED IN https://marctenbosch.com/quaternions/code.htm is shown here:
			 * http://marc-b-reynolds.github.io/quaternions/2016/05/30/QuatHDAngleCayley.html
			 * 
			 */

			/**
			 * @brief Another Idea
			 * Because it goes from A to B, and the angle is theta, must get theta/2.
			 * 
			 * If the lengths are 1, (so normalized), the vector between endpoints of the vectors,
			 * half the length of that vector, c, can find the midpoint of A & B (the triangle),
			 * so by angle bisector theorem, the resulting normalized midpoint vector, dot(A, midpoint vector),
			 * is cos(theta/2).
			 * Basically use dot(A, A+B) where both operands are normalized, to get cos(theta/2)
			 * 
			 * 
			 */
		}

		//Normalize the rotor
		constexpr void normalize() noexcept
		{
			const auto invlength = inversesqrt(lengthSquared());
			scalar *= invlength;
			plane *= invlength;
		}

		//Get the length squared of the rotor
		constexpr float lengthSquared() const noexcept
		{
			return (scalar * scalar) + bs::dot(plane, plane);
		}

		//Length of the rotor
		constexpr float length() const noexcept
		{
			return sqrt(lengthSquared());
		}

		//Basically conjugate, like in quaternion
		constexpr Rotor reverse() const noexcept
		{
			//Rotor AB => BA = AB.reverse()
			return Rotor(scalar, -plane.x, -plane.y, -plane.z);
		}

		//Rotor-Rotor product
		constexpr inline Rotor operator*(const Rotor& rhs) const noexcept
		{
			//Expansion of the geometric product rules
			const auto& r1 = *this;
			const auto& r2 = rhs;

			/*Rotor r;
			r.scalar = (r1.scalar * r2.scalar) - (r1.plane.x * r2.plane.x) - (r1.plane.y * r2.plane.y) - (r1.plane.z * r2.plane.z);
			r.plane = Bivector(vec3( //z, y, x
				(r1.plane.x * r2.scalar) + (r1.scalar * r2.plane.x) + (r1.plane.z * r2.plane.y) - (r1.plane.y * r2.plane.z), //z
				(r1.plane.y * r2.scalar) + (r1.scalar * r2.plane.y) - (r1.plane.z * r2.plane.x) + (r1.plane.x * r2.plane.z), //y
				(r1.plane.z * r2.scalar) + (r1.scalar * r2.plane.z) + (r1.plane.y * r2.plane.x) - (r1.plane.x * r2.plane.y)	 //x
			));
			return r;*/

			float&& sclr = (r1.scalar * r2.scalar) - (r1.plane.x * r2.plane.x) - (r1.plane.y * r2.plane.y) - (r1.plane.z * r2.plane.z);
			float&& xy = (r1.plane.x * r2.scalar) + (r1.scalar * r2.plane.x) + (r1.plane.z * r2.plane.y) - (r1.plane.y * r2.plane.z);
			float&& xz = (r1.plane.y * r2.scalar) + (r1.scalar * r2.plane.y) - (r1.plane.z * r2.plane.x) + (r1.plane.x * r2.plane.z);
			float&& yz = (r1.plane.z * r2.scalar) + (r1.scalar * r2.plane.z) + (r1.plane.y * r2.plane.x) - (r1.plane.x * r2.plane.y);

			return Rotor(sclr, xy, xz, yz); //This is for copy elision (I think this works?)
		}
	
		constexpr inline Rotor& operator*=(const Rotor& rhs) noexcept
		{
			(*this) = (*this) * rhs;
			return (*this);
		}

		constexpr vec3 rotate(vec3 v) const noexcept
		{
			const auto& r1 = *this;
			// ab v ba
			// q = P v
			// r = q P*

			// q = ab v
			const vec3 q(
				(r1.scalar * v.x) + (v.y * r1.plane.x) + (v.z * r1.plane.y),	//x
				(r1.scalar * v.y) - (v.x * r1.plane.x) + (v.z * r1.plane.z),	//y
				(r1.scalar * v.z) - (v.x * r1.plane.y) - (v.y * r1.plane.z) 	//z
			);

			const float q_xyz = (v.x * r1.plane.z) - (v.y * r1.plane.y) + (v.z * r1.plane.x); // trivector

			// r = q ba
			const vec3 r(
				(r1.scalar * q.x) + (q.y * r1.plane.x) + (q.z * r1.plane.y) + (q_xyz * r1.plane.z), //x
				(r1.scalar * q.y) - (q.x * r1.plane.x) - (q_xyz * r1.plane.y) + (q.z * r1.plane.z), //y
				(r1.scalar * q.z) + (q_xyz * r1.plane.x) - (q.x * r1.plane.y) - (q.y * r1.plane.z) //z
			);
			
			// trivector part of the result is always zero!

			return r;
		}
	};

	constexpr Rotor geometricProduct(vec3 const& a, vec3 const& b) noexcept
	{
		//ab = inner(a, b) + outer(a, b) = dot(a,b) + cross(a,b)
		// = 1/2 (ab + ab + ba - ba) = Above^
		// ab = |a|||b|(cos(a)+sin(a)B) ; where B is a bivector
		auto&& plane = Bivector(glm::cross(a, b));
		return Rotor(dot(a, b), plane.x, plane.y, plane.z); // maybe should be (z, -y, x)
	}


	/**
	 * For interpolation
	 * 
	 */
	// partially interpolate between 'src' and 'dst', as determined by 'alpha'
	Rotor interpolateRotor(const Rotor &src, const Rotor& dst, float alpha) 
	{
		// return src * exp(alpha * log(dst * inverse(src))); // <= math version
		//return Rotor(src * exp(Bivector(alpha * log(Rotor(inverse(src) * dst))))); // <= smth from internet

		//return Rotor(src * exp(alpha * Bivector(log(src.reverse() * dst).))); // <= I am not sure

		//Doing this with slerp, screw it

		// out = src*sin(angle*(1-t)) + dst*sin(angle*t)  all over sin(angle)
		// sin(angle - angle*t) = sin(angle)cos(angle*t) - cos(angle)sin(angle*t)

		auto out = (src * dst.reverse());
		auto cosangle = out.scalar;
		auto sinangle = out.plane.length();

		//@TODO!!! NOT FINISHED

		return dst;
	}

	/*Bivector log(const Rotor& r) 
	{
		// get the bivector/2-blade part of R
		Bivector B = r.plane;

		// compute the 'reverse norm' of the bivector part of R:
		float R2 = float(norm_r(B));

		// check to avoid divide-by-zero (and also below zero due to FP roundoff):
		if (R2 <= 0.0) {
			if (r.scalar < 0)  // this means the user ask for log(-1):
				return Bivector((float)glm::pi() * (e1 ^ e2)); // we return a 360 degree rotation in an arbitrary plane
			else
				return Bivector(vec3(0.0f, 0.0f, 0.0f));  // return log(1) = 0
		}

		// return the log:
		return Bivector(B * ((float)atan2(R2, float(R)) / R2));
	}*/

	// special exp for 3D Euclidean bivectors:
	/*Rotor exp(const Bivector &x)
	{
		// compute the square
		float x2 = _Float(x << x);

		// x2 must always be <= 0, but round off error can make it positive:
		if (x2 > 0.0f) x2 = 0.0f;

		// compute half angle:
		float ha = sqrt(-x2);

		if (ha == (float)0.0) return Rotor((float)1.0);

		// return rotor:
		return Rotor((float)cos(ha) + ((float)sin(ha) / ha) * x);
	}*/

	//Convert to Matrix
	constexpr inline mat3 convertToRotationMatrix(const Rotor& r)
	{
		mat3 m = mat3(1.0f);

		//Version 1: Operations = ()
		vec3 v1 = r.rotate(vec3(1, 0, 0));
		vec3 v2 = r.rotate(vec3(0, 1, 0));
		vec3 v3 = r.rotate(vec3(0, 0, 1));
		m = mat3(v1, v2, v3); // or just: return mat3(v1, v2, v3);

		//Version 2 (full per element): Operations = ()
		// glm::m


		return m;
	}

} // namespace bs
