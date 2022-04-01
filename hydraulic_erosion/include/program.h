#pragma once

namespace he
{
	class Program
	{
	public:
		virtual void init() = 0;
		virtual void clear() = 0;
		virtual void update(float deltaTime) = 0;
		virtual void draw(float deltaTime) = 0;
	};
}

