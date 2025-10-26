#include "statemanager.h"
#include <string>

bool StateManager::canTransitionTo(const State newState)
{
    return m_currentState != newState;
}

bool StateManager::canTransitionTo(const DebugMode newDebugMode)
{
    return m_currentState == debug && m_currentDebugMode != newDebugMode;
}

bool StateManager::tryTransitionTo(const State newState)
{
    if (canTransitionTo(newState))
    {
        m_currentState = newState;
        return true;
    }
    return false;
}

bool StateManager::tryTransitionTo(const DebugMode debugMode)
{
    if (canTransitionTo(debugMode))
    {
        m_currentDebugMode = debugMode;
        return true;
    }
    return false;
}

bool StateManager::isInDebugMode()
{
    return m_currentState == debug;
}

StateManager::State StateManager::getCurrentState() const
{
    return m_currentState;
}

StateManager::DebugMode StateManager::getCurrentDebugMode() const
{
    return m_currentDebugMode;
}

std::string_view StateManager::getCurrentStateString() const
{
    return s_stateStrings[m_currentState];
}

std::string_view StateManager::getCurrentDebugModeString() const
{
    return s_debugModeStrings[m_currentDebugMode];
}
