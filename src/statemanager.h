#ifndef STATEMANAGER_H
#define STATEMANAGER_H

// Header-only class because it is relatively simple. Used to keep track of current emulator state as well as any changes to it

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
		numDebugMode,
	};


	void changeMainStateTo(const State nextState)
	{
		m_currentState = nextState;
	}

	void changeDebugModeTo(const DebugMode nextMode)
	{
		m_currentDebugMode = nextMode;
	}

	State getCurrentState() const { return m_currentState; }
	DebugMode getCurrentDebugMode() const { return m_currentDebugMode; }

private:
	State m_currentState{ running };

	// Only relevant if debug mode is on
	DebugMode m_currentDebugMode{ step };
};

#endif