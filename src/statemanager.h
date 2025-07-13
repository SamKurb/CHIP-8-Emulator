#ifndef STATEMANAGER_H
#define STATEMANAGER_H

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

	// Only relevant if debug mode is one
	DebugMode m_currentDebugMode{ step };
};

#endif