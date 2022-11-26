#include "TimeOfDay.hpp"

TimeOfDay::TimeOfDay(int _hour, int _min) : hour(_hour), minute(_min)
{
}

TimeOfDay::~TimeOfDay()
{
}

void TimeOfDay::clampTime() // recursive
{
	this->hour = glm::clamp(this->hour, 0, 23);
	this->minute = glm::clamp(this->minute, 0, 59);
}

std::ostream& operator<<(std::ostream& os, const TimeOfDay& timeOfDay)
{
	os << "Hour: " << timeOfDay.hour << "Minute: " << timeOfDay.minute << std::endl;

	return os;
}

bool operator==(const TimeOfDay& lhs, const TimeOfDay& rhs)
{

	return (lhs.minute == rhs.minute) && (lhs.hour == rhs.hour);
}

std::ostream& operator<<(std::ostream& os, const AtmosphereColors& atmosphereColor)
{
	os << "Lower Color: " << to_string(atmosphereColor.lowerAtmosphereColor) << " Upper Color: " << to_string(atmosphereColor.lowerAtmosphereColor) << std::endl;
	return os;

}

size_t TimeHasher::operator()(const TimeOfDay& timeOfDay) const noexcept
{
		auto h1 = std::hash<int>{}(timeOfDay.hour);
		auto h2 = std::hash<int>{}(timeOfDay.minute);
		return h1 ^ h2 << 1;
};
