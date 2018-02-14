#pragma once


namespace ChaosGameApp
{
	// Generates vertex data for the chaos game.
	class ChaosGameGenerator
	{
	public:
		ChaosGameGenerator();

		virtual ~ChaosGameGenerator();

	private:
		// Random number engine.
		std::mt19937 m_randomEngine;
	};
}
