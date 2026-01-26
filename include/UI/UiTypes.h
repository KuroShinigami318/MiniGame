#pragma once
#include "types.h"
#include "lent_array.h"

namespace types
{
DeclareConsecutiveType(Coordinate, int, -1, UNDERLYING_TYPE_MAX(int), 0);
} // namespace types	

template <typename T, size_t _Size>
struct Vec
{
#define DefineVecOverloadOperator(_operator, _op) \
	Vec& _operator _op (const T& i_value) \
	{ \
		for (size_t i = 0; i < _Size; ++i) \
		{ \
			m_data[i] _op i_value; \
		} \
		return *this; \
	} \
	Vec& _operator _op (const Vec& i_value) \
	{ \
		for (size_t i = 0; i < _Size; ++i) \
		{ \
			m_data[i] _op i_value[i]; \
		} \
		return *this; \
	}

#define DefineGlobalVecOverloadOperator(_operator, _op) \
	template <typename T, size_t _Size> \
	Vec<T, _Size> _operator _op (Vec<T, _Size> i_lhs, const T& i_rhs) \
	{ \
		return i_lhs _op##= i_rhs; \
	} \
	template <typename T, size_t _Size> \
	Vec<T, _Size> _operator _op (Vec<T, _Size> i_lhs, const Vec<T, _Size>& i_rhs) \
	{ \
		return i_lhs _op##= i_rhs; \
	}

	constexpr Vec(T i_value) : m_data()
	{
		for (size_t i = 0; i < _Size; ++i)
		{
			m_data[i] = i_value;
		}
	}
	constexpr Vec(std::initializer_list<T> i_initializerList) : m_data(i_initializerList) {}
	Vec& operator=(std::initializer_list<T> i_initializerList)
	{
		if (i_initializerList.size() != _Size)
		{
			throw std::invalid_argument("Initializer list size does not match vector size");
		}
		auto it = i_initializerList.begin();
		for (size_t i = 0; i < _Size; ++i)
		{
			m_data[i] = *it++;
		}
		return *this;
	}
	Vec& operator=(const T& i_value)
	{
		for (size_t i = 0; i < _Size; ++i)
		{
			m_data[i] = i_value;
		}
		return *this;
	}
	T& operator[](size_t i_index)
	{
		return m_data[i_index];
	}
	const T& operator[](size_t i_index) const
	{
		return m_data[i_index];
	}
	T* data()
	{
		return m_data.data();
	}
	size_t size() const
	{
		return _Size;
   }
	Vec abs() const
	{
		Vec result = *this;
		for (size_t i = 0; i < _Size; ++i)
		{
			if (result[i] < 0)
			{
				result[i] = -result[i];
			}
		}
		return result;
   }
	bool operator==(const Vec& i_rhs)
	{
		for (size_t i = 0; i < _Size; i++)
		{
			if (m_data[i] != i_rhs[i])
			{
				return false;
			}
		}
		return true;
	}
	bool operator!=(const Vec& i_rhs)
	{
		return !(*this == i_rhs);
	}
	Vec& operator/=(const T& i_value)
	{
		for (size_t i = 0; i < _Size; ++i)
		{
			if (i_value == 0)
			{
				continue;
			}
			m_data[i] /= i_value;
		}
		return *this;
   }

	Vec& operator/=(const Vec& i_value)
	{
		for (size_t i = 0; i < _Size; ++i)
		{
			if (i_value[i] == 0)
			{
				continue;
			}
			m_data[i] /= i_value[i];
		}
		return *this;
	}

	DefineVecOverloadOperator(operator,+=);
	DefineVecOverloadOperator(operator,-=);
	DefineVecOverloadOperator(operator,*=);
	
private:
	std::array<T, _Size> m_data;
};

template <typename T, size_t _Size>
Vec<T, _Size> operator/(Vec<T, _Size> i_lhs, const T& i_rhs)
{
	return i_lhs /= i_rhs;
}
template <typename T, size_t _Size>
Vec<T, _Size> operator/(Vec<T, _Size> i_lhs, const Vec<T, _Size>& i_rhs)
{
	return i_lhs /= i_rhs;
}

DefineGlobalVecOverloadOperator(operator,+);
DefineGlobalVecOverloadOperator(operator,-);
DefineGlobalVecOverloadOperator(operator,*);

using Vec2i = Vec<int, 2>;
using Vec2l = Vec<long, 2>;
using Vec2f = Vec<float, 2>;
using Vec2d = Vec<double, 2>;

struct Position
{
#define DefineOverloadOperatorWithVec2(_operator, _op) \
	template <typename T> \
	Position& _operator _op (const Vec<T, 2>& i_value) \
	{ \
		x _op i_value[0]; \
		y _op i_value[1]; \
		return *this; \
	} \
	Position& operator _op (const Position& i_value) \
	{ \
		x _op i_value.x; \
		y _op i_value.y; \
		return *this; \
	}

#define DefineGlobalOverloadOperatorWithVec2(_operator, _op) \
	template <typename T> \
	Position _operator _op (Position i_lhs, const Vec<T, 2>& i_rhs) \
	{ \
		return i_lhs _op##= i_rhs; \
	} \
	static Position _operator _op (Position i_lhs, const Position& i_rhs) \
	{ \
		return i_lhs _op##= i_rhs; \
	}

public:
	bool operator==(const Position& i_other) const
	{
		return x == i_other.x && y == i_other.y;
	}
	bool operator!=(const Position& i_other) const
	{
		return !(*this == i_other);
	}

	Position() : Position(0, 0) {}
	Position(types::Coordinate i_x, types::Coordinate i_y) : x(i_x), y(i_y) {}
	bool IsValid() const
	{
		return x.is_valid() && y.is_valid();
	}
	void Invalidate()
	{
		x = types::Coordinate::k_invalid;
		y = types::Coordinate::k_invalid;
	}
	types::Coordinate x, y;

	DefineOverloadOperatorWithVec2(operator,+=);
	DefineOverloadOperatorWithVec2(operator,-=);
	DefineOverloadOperatorWithVec2(operator,*=);
	DefineOverloadOperatorWithVec2(operator,/=);
};

DefineGlobalOverloadOperatorWithVec2(operator,+);
DefineGlobalOverloadOperatorWithVec2(operator,-);
DefineGlobalOverloadOperatorWithVec2(operator,*);
DefineGlobalOverloadOperatorWithVec2(operator,/);

template <>
struct std::hash<Position>
{
	size_t operator()(const Position& i_position) const
	{
		size_t combinedHash = 0;
		hash_combine(combinedHash, i_position.x);
		hash_combine(combinedHash, i_position.y);
		return combinedHash;
	}
};

static std::string to_string(const Position point)
{
	std::ostringstream ss;
	ss << "x: " << point.x << ", y: " << point.y;
	return std::move(ss).str();
}

template <typename T, size_t _Size>
static std::string to_string(const Vec<T, _Size>& vec)
{
	return utils::Format("{}", utils::lent_array<T>(const_cast<Vec<T, _Size>&>(vec)));
}

MAKE_FORMATTABLE(Position, std::string, to_string);
MAKE_FORMATTABLE_TEMPLATE_TYPE(WRAP(Vec<T, _Size>), std::string, to_string, typename T, size_t _Size);
MAKE_FORMATTABLE_FOR_CONTAINER(utils::lent_array);