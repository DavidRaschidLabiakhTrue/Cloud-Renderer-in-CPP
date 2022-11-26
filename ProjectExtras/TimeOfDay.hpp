#pragma once
#ifndef TimeOfDay_HPP
#define TimeOfDay_HPP


#include "../Include/glm/glm.hpp"
#include "../Include/glm/gtx/string_cast.hpp"
#include <iostream>
#include <string>
#include <unordered_map> // this will map time ranges to colors;
#include "../imgui/imgui.h"
using glm::clamp;
using glm::vec3;
using glm::to_string;

#define ToScale / 255.0f

struct TimeOfDay 
{
	TimeOfDay(int _hour = 0, int _min = 0);
	~TimeOfDay();

	int hour = 0;
	int minute = 0;
	void clampTime();

	float normalizeMinute()
	{
		return float(minute) / float(60);
	}
	float normalizeHour()
	{
		return float(hour) / float(24);
	}

	friend std::ostream& operator << (std::ostream& os, const TimeOfDay& timeOfDay);
	
};


extern bool operator == (const TimeOfDay& lhs, const TimeOfDay& rhs);





struct AtmosphereColors
{
	AtmosphereColors()
	{

	};
	AtmosphereColors(vec3 upper, vec3 lower)
	{
		upperAtmosphereColor = upper;
		lowerAtmosphereColor = lower;
	}
	vec3 upperAtmosphereColor = vec3(0);
	vec3 lowerAtmosphereColor = vec3(0);
	friend std::ostream& operator << (std::ostream& os, const AtmosphereColors& atmosphereColor);

};



struct TimeHasher
{
	size_t operator()(const TimeOfDay& timeOfDay) const noexcept;
};

using std::unordered_map;
typedef unordered_map<TimeOfDay, AtmosphereColors, TimeHasher> AtmosphereColorMap;
class TimeOfDayOperator // runs on 24 hour intervals,  not 12 & 12 due to complexity.
{
	
	public:
		TimeOfDayOperator()
		{
			generate();
		}

		~TimeOfDayOperator()
		{

		}

		void setGui()
		{
			using std::cout;
			auto lastCol = col;
			ImGui::SliderInt("Hour", &timeOfDay.hour, 0, 23);
			ImGui::SliderInt("Minute", &timeOfDay.minute, 0, 59);
			timeOfDay.clampTime();

			col = access(timeOfDay);

			//if (col.upperAtmosphereColor != lastCol.upperAtmosphereColor || col.lowerAtmosphereColor != lastCol.lowerAtmosphereColor)
			//{
			//	cout << "new color" << to_string(col.upperAtmosphereColor) << " " << to_string(col.lowerAtmosphereColor) << std::endl;
			//}
		}

		void generate() // brute force method, precomputes to associate time and minute color gradients
		{
			int totalElements = 24 * 60; // 24 hours, 60 minutes each
	


			vec3 midNightUpper = vec3(13, 19, 35) ToScale; 
			vec3 midNightLower = vec3(26, 31, 50) ToScale;

			vec3 deepMorningUpper = vec3(13, 14, 58) ToScale;
			vec3 deepMorningLower = vec3(41, 43, 144) ToScale;

			vec3 earlyMorningUpper = deepMorningUpper;
			vec3 earlyMorningLower = deepMorningLower;


			vec3 dawnUpper = vec3(58, 71, 113) ToScale; 
			vec3 dawnLower = vec3(251, 144, 53) ToScale;

			vec3 noonUpper = vec3(64.0f, 96.0f, 153.0f) ToScale; 
			vec3 noonLower = vec3(143, 174, 203) ToScale;

			vec3 afternoonUpper = vec3(34, 130, 209) ToScale;
			vec3 afternoonLower = vec3(213, 184, 148) ToScale; 

			vec3 sunsetUpper = vec3(255, 170, 139) ToScale; 
			vec3 sunsetLower = vec3(255, 86, 97) ToScale;


			vec3 duskUpper = vec3(79, 78, 138) ToScale; 
			vec3 duskLower = vec3(216, 74, 54) ToScale;

			vec3 nightUpper = vec3(9,14,28) ToScale; 
			vec3 nightLower = vec3(62, 46, 39) ToScale;



			
			vec3 fromUpper, fromLower;
			vec3 toUpper, toLower;
			

			map.reserve(totalElements);

			float mappedIndex;
			float mappedMultiplier;

			for (int i = 0; i < 23; i++) // 24 hours;
			{

				mappedIndex = float((i % 3)) + 1.0f; // we don't want this at 0 so + 1 to bring this into range of 1-3;
				
				for (int j = 0; j < 59; j++) // 60 minutes
				{
					// very much a hard coded solution

					mappedMultiplier = float(j) / 60.0f;

					switch (i)
					{
						case 0: // midnight
						case 1:
						case 2:

							fromUpper = midNightUpper;
							fromLower = midNightLower;

							toUpper = deepMorningUpper;
							toLower = deepMorningLower;

							
							
							
							break;
						case 3: // deep morning
						case 4:
						case 5:
							fromUpper = deepMorningUpper;
							fromLower = deepMorningLower;

							toUpper = earlyMorningUpper;
							toLower = earlyMorningLower;

							
							break;
						case 6: // early morning
						case 7:
						case 8:
							fromUpper = earlyMorningUpper;
							fromLower = earlyMorningLower;

							toUpper = dawnUpper;
							toLower = dawnLower;

						
							break;
						case 9: // dawn
						case 10:
						case 11:
							fromUpper = dawnUpper;
							fromLower = dawnLower;

							toUpper = noonUpper;
							toLower = noonLower;

							
							break;
						case 12: // noon
						case 13:
						case 14:
							fromUpper = noonUpper;
							fromLower = noonLower;

							toUpper = afternoonUpper;
							toLower = afternoonLower;

							
							break;
						case 15: // afternoon
						case 16:
						case 17:
							fromUpper = afternoonUpper;
							fromLower = afternoonLower;

							toUpper = sunsetUpper;
							toLower = sunsetLower;

							
							break;
						case 18: // evening
						case 19:
						case 20:
							fromUpper = sunsetUpper;
							toLower = sunsetLower;

							toUpper = nightUpper;
							toLower = nightLower;

							
							break;

						case 21: // night
						case 22:
						case 23:
							fromUpper = nightUpper;
							fromUpper = nightLower;

							toUpper = midNightUpper;
							toLower = midNightLower;
							
							
							break;
					}
					
					map[TimeOfDay(i, j)] = AtmosphereColors(glm::mix(fromUpper, toUpper, mappedIndex * mappedMultiplier), glm::mix(fromLower, toLower, mappedIndex * mappedMultiplier));
					
				}
			}
			print();
		}
		void print()
		{
			//if (map.size() > 0)
			//{
			//	using std::cout;
			//	for (const auto& item : map)
			//	{
			//		cout << item.first;
			//		cout << item.second;
			//	}
			//}
		}

		AtmosphereColors access(const TimeOfDay& timeOfDay)
		{
			return map[timeOfDay];
		}
		



		void setGUI()
		{

		}
		TimeOfDay timeOfDay;
		AtmosphereColors col;
		AtmosphereColorMap map;





};

#endif