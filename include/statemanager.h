#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <string>
#include <array>
#include <sys/stat.h>

class StateManager
{
public:
	enum State
	{
		running,
		debug,
		numMainStates,
	};

	enum DebugMode
	{
		step,
		manual,
		numDebugModes,
	};

	bool tryTransitionTo(const State newState);
	bool tryTransitionTo(const DebugMode newDebugMode);

	bool isInDebugMode();

	State getCurrentState() const;
	DebugMode getCurrentDebugMode() const;

	std::string_view getCurrentStateString() const;
	std::string_view getCurrentDebugModeString() const;

private:
	static constexpr std::array<std::string_view, numMainStates> s_stateStrings {
		"Running",
		"Debug"
	};

	static_assert(s_stateStrings.size() == numMainStates);

	static constexpr std::array<std::string_view, numMainStates> s_debugModeStrings {
		"Step",
		"Manual"
	};

	static_assert(s_debugModeStrings.size() == numDebugModes);

	bool canTransitionTo(const State newState);
	bool canTransitionTo(const DebugMode newDebugMode);

	State m_currentState{ running };

	// Only relevant if debug mode is on
	DebugMode m_currentDebugMode{ step };
};

#endif