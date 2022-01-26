//=== General Includes ===
#include "stdafx.h"
#include "EFiniteStateMachine.h"

Elite::FiniteStateMachine::FiniteStateMachine(Elite::FSMState* startState, Elite::Blackboard* pBlackboard)
    : m_pCurrentState(nullptr),
    m_pBlackboard(pBlackboard)
{
    ChangeState(startState);
}

Elite::FiniteStateMachine::~FiniteStateMachine()
{
    SAFE_DELETE(m_pBlackboard);
}

void Elite::FiniteStateMachine::AddTransition(Elite::FSMState* startState, Elite::FSMState* toState, Elite::FSMTransition* transition)
{
    auto it = m_Transitions.find(startState);
    if (it == m_Transitions.end())
    {
        m_Transitions[startState] = Transitions();
    }
   
    m_Transitions[startState].push_back(std::make_pair(transition, toState));
}

void Elite::FiniteStateMachine::Update(float deltaTime)
{
    //Look if 1 or more transition exists for the current state that we are in
    //Tip: Check the transitions map for a TransitionState pair
    auto it = m_Transitions.find(m_pCurrentState);
    //if a TransitionState exists
    if (it != m_Transitions.end())
    {
        // Loop over all the TransitionState pairs 
	    for (auto& transPair : it->second)
	    {
	        //If a ToTransition returns true => transition to the new corresponding state
		    if (transPair.first->ToTransition(m_pBlackboard))
		    {
                ChangeState(transPair.second);
                break;
		    }
	    }
    }

    //Update the current state (if one exists ;-))
    if (m_pCurrentState)
        m_pCurrentState->Update(m_pBlackboard, deltaTime);
}

Elite::Blackboard* Elite::FiniteStateMachine::GetBlackboard() const
{
    return m_pBlackboard;
}

void Elite::FiniteStateMachine::ChangeState(FSMState* newState)
{
    //If currently in a state => make sure the OnExit of that state gets called
    if (m_pCurrentState)
    {
        m_pCurrentState->OnExit(m_pBlackboard);
    }

    //Change the current state to the new state
    m_pCurrentState = newState;

    //Call the OnEnter of the new state
    if (m_pCurrentState)
    {
        m_pCurrentState->OnEnter(m_pBlackboard);
    }
        
}
