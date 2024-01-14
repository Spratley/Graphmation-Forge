#pragma once

#define STATES_ROOT "states"
#define STATE_NAME "name"
#define STATE_LOOP "loop"

#define STATE_TRANSITION_STATE "state"
#define STATE_TRANSITION_TRANSITION_ID "transitionID"

#define TRANSITIONS_ROOT "transitions"
#define TRANSITION_NAME "name"
#define TRANSITION_CONDITIONS "conditions"

#define CONDITION_TYPE "type"
#define CONDITION_OPERATION "operation"
#define CONDITION_VALUE "value"
#define CONDITION_VARIABLE_NAME "variableName"

#define TRY_PARSE(object, name, type, defaultValue) (object ->Has( name )) ? object ->Get< type >( name )->m_value : defaultValue ;
