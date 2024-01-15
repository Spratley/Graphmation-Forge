#pragma once

#define STATES_ROOT "states"
#define STATE_NAME "name"
#define STATE_ANIM_NAME "animationName"
#define STATE_LOOP "loop"
#define STATE_POS_X "positionX"
#define STATE_POS_Y "positionY"

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

#define OPERATOR_EQUAL "=="
#define OPERATOR_NOT_EQUAL "!="
#define OPERATOR_GREATER ">"
#define OPERATOR_GREATER_EQUAL ">="
#define OPERATOR_LESS "<"
#define OPERATOR_LESS_EQUAL "<="

#define CONDITION_OP_INT "OperatorInt"
#define CONDITION_OP_FLOAT "OperatorFloat"
#define CONDITION_OP_BOOL "OperatorBool"
#define CONDITION_BOOLEAN "Boolean"

#define SET_DATA(object, name, type, value) {type * outValue = new type ; \
outValue ->m_value = value ;\
object ->Set( name , outValue );}

#define ADD_DATA(object, name, type, value) {type * outValue = new type ;} \
outValue ->m_value = value ;\
object ->Add( name , outValue );}