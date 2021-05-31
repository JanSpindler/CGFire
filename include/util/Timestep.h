#pragma once

namespace util {

    //represents an amount of time passed, in Seconds
	class TimeStep
	{
	public:
         explicit TimeStep(float time)
			: m_Time(time)
		{
		}

		TimeStep& operator+=(TimeStep ts){
            m_Time+= ts;
            return *this; }

		operator float() const { return m_Time; }

		float GetSeconds() const { return m_Time; }
		float GetMilliseconds() const { return m_Time * 1000.0f; }
	private:
		float m_Time;
	};

}