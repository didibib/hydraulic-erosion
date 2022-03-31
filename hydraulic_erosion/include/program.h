#pragma once

namespace he
{
	class Program
	{
	public:
		virtual void update(float deltaTime) = 0;
		virtual void draw(float deltaTime) = 0;
	};
}

