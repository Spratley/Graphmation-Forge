#include "Transition.h"

#include "GraphmationForgeApp.h"
#include "Node.h"

#include "JParse/JParse.h"
#include "ParsingDefines.h"
#include "StringConvert.h"

#define TRANSITION_HALF_WIDTH 2.5f

#define ARROW_THICKNESS 10.0f
#define ARROW_HALF_HEIGHT 10.0f
#define ARROW_HALF_WIDTH 20.0f

#define TWO_WAY_TRANSITION_OFFSET 25.0f

Transition::Transition(HWND const parentWindowHandle)
: ISelectable(parentWindowHandle)
{}

bool Transition::IsMouseOverlapping(POINT mousePos)
{
    return PtInRegion(m_paintRegion, mousePos.x, mousePos.y);
}

HRGN const & Transition::GetPaintRegion()
{
    UpdateRegion();
    return m_paintRegion;
}

void Transition::Paint(HDC hdc, HBRUSH fillColor)
{
    FillRgn(hdc, GetPaintRegion(), fillColor);
}

bool const Transition::HasSameConditions(Transition * transition) const
{
    for (TransitionCondition const& condition : m_conditions)
    {
        bool hasMatchingCondition = false;
        for (TransitionCondition const& otherCondition : transition->GetConditions())
        {
            if (condition.IsEqual(otherCondition))
            {
                hasMatchingCondition = true;
                break;
            }
        }
        if (!hasMatchingCondition)
        {
            return false;
        }
    }
    return true;
}

void Transition::UpdateRegion()
{
    DeleteObject(m_paintRegion);

    POINT fromPoint = m_fromNode->GetNodePosition();
    POINT toPoint = m_toNode->GetNodePosition();

    fromPoint.x += NODE_WIDTH / 2;
    fromPoint.y += NODE_HEIGHT / 2;
    toPoint.x += NODE_WIDTH / 2;
    toPoint.y += NODE_HEIGHT / 2;

    // Normalize direction vector
    POINT vector;
    vector.x = toPoint.x - fromPoint.x;
    vector.y = toPoint.y - fromPoint.y;
    float denom = sqrtf(powf((float)vector.x, 2.0f) + powf((float)vector.y, 2.0f));
    float normalVectorX = (float)vector.x / denom;
    float normalVectorY = (float)vector.y / denom;
    
    // Offset if we have a two way connection
    if (GraphmationForgeApp::GetInstance()->AreNodesTwoWayConnected(m_fromNode, m_toNode))
    {
        fromPoint.x += static_cast<LONG>(normalVectorY * TWO_WAY_TRANSITION_OFFSET);
        fromPoint.y += static_cast<LONG>(-normalVectorX * TWO_WAY_TRANSITION_OFFSET);
        toPoint.x += static_cast<LONG>(normalVectorY * TWO_WAY_TRANSITION_OFFSET);
        toPoint.y += static_cast<LONG>(-normalVectorX * TWO_WAY_TRANSITION_OFFSET);
    }

    POINT widthVector;
    widthVector.x = static_cast<LONG>(normalVectorY * TRANSITION_HALF_WIDTH);
    widthVector.y = static_cast<LONG>(-normalVectorX * TRANSITION_HALF_WIDTH);

    int const numPoints = 4;
    POINT points[numPoints] =
    {
        {fromPoint.x + widthVector.x, fromPoint.y + widthVector.y},
        {fromPoint.x - widthVector.x, fromPoint.y - widthVector.y},
        {toPoint.x - widthVector.x, toPoint.y - widthVector.y},
        {toPoint.x + widthVector.x, toPoint.y + widthVector.y}
    };

    HRGN line = CreatePolygonRgn(points, numPoints, WINDING);

    // TODO: Create multiple arrows if there exists multiple transitions to the same state
    // Create the arrow in the middle

    float midPointX = (fromPoint.x + toPoint.x) * 0.5f;
    float midPointY = (fromPoint.y + toPoint.y) * 0.5f;

    int const numArrowPoints = 6;
    POINT arrowPoints[numArrowPoints];
    BuildPointArrow(arrowPoints, normalVectorX, normalVectorY);
    
    // Offset to middle
    for (int i = 0; i < 6; i++)
    {
        arrowPoints[i].x += static_cast<LONG>(midPointX);
        arrowPoints[i].y += static_cast<LONG>(midPointY);
    }

    HRGN arrow = CreatePolygonRgn(arrowPoints, numArrowPoints, WINDING);

    m_paintRegion = CreateRectRgn(0, 0, 0, 0);
    CombineRgn(m_paintRegion, line, arrow, RGN_OR);

    DeleteObject(line);
    DeleteObject(arrow);
}

void Transition::BuildPointArrow(POINT * outPoints, float directionX, float directionY)
{
    LONG arrowTipX = static_cast<LONG>(directionX * ARROW_HALF_HEIGHT);
    LONG arrowTipY = static_cast<LONG>(directionY * ARROW_HALF_HEIGHT);

    LONG arrowEndX = static_cast<LONG>(directionX * -ARROW_HALF_HEIGHT);
    LONG arrowEndY = static_cast<LONG>(directionY * -ARROW_HALF_HEIGHT);

    POINT arrowWidthVector;
    arrowWidthVector.x = static_cast<LONG>(directionY * ARROW_HALF_WIDTH);
    arrowWidthVector.y = static_cast<LONG>(-directionX * ARROW_HALF_WIDTH);

    POINT arrowHeightVector;
    arrowHeightVector.x = static_cast<LONG>(directionX * ARROW_THICKNESS);
    arrowHeightVector.y = static_cast<LONG>(directionY * ARROW_THICKNESS);

    outPoints[0] = { arrowEndX - arrowWidthVector.x + arrowHeightVector.x   , arrowEndY - arrowWidthVector.y    + arrowHeightVector.y };
    outPoints[1] = { arrowTipX                      + arrowHeightVector.x   , arrowTipY                         + arrowHeightVector.y };
    outPoints[2] = { arrowEndX + arrowWidthVector.x + arrowHeightVector.x   , arrowEndY + arrowWidthVector.y    + arrowHeightVector.y };
    outPoints[3] = { arrowEndX + arrowWidthVector.x - arrowHeightVector.x   , arrowEndY + arrowWidthVector.y    - arrowHeightVector.y };
    outPoints[4] = { arrowTipX                      - arrowHeightVector.x   , arrowTipY                         - arrowHeightVector.y };
    outPoints[5] = { arrowEndX - arrowWidthVector.x - arrowHeightVector.x   , arrowEndY - arrowWidthVector.y    - arrowHeightVector.y };
}

void TransitionCondition::SetOperatorFromString(std::string const & op)
{
    const char* str = op.c_str();
    if (0 == strcmp(str, OPERATOR_EQUAL))
    {
        m_conditionType = EQUAL;
    }
    else if (0 == strcmp(str, OPERATOR_NOT_EQUAL))
    {
        m_conditionType = NOT_EQUAL;
    }
    else if (0 == strcmp(str, OPERATOR_GREATER))
    {
        m_conditionType = GREATER;
    }
    else if (0 == strcmp(str, OPERATOR_GREATER_EQUAL))
    {
        m_conditionType = GREATER_EQUAL;
    }
    else if (0 == strcmp(str, OPERATOR_LESS))
    {
        m_conditionType = LESS;
    }
    else if (0 == strcmp(str, OPERATOR_LESS_EQUAL))
    {
        m_conditionType = LESS_EQUAL;
    }
}

void TransitionCondition::SetVariableTypeFromString(std::string const & type)
{
    const char* str = type.c_str();
    if (0 == strcmp(str, CONDITION_OP_INT))
    {
        m_expectedType = TYPE_INT;
    }
    else if (0 == strcmp(str, CONDITION_OP_FLOAT))
    {
        m_expectedType = TYPE_FLOAT;
    }
    else if (0 == strcmp(str, CONDITION_OP_BOOL) || 0 == strcmp(str, CONDITION_BOOLEAN))
    {
        m_expectedType = TYPE_BOOL;
    }
}

bool const TransitionCondition::IsEqual(TransitionCondition const & other) const
{
    bool sameName = StrCmpW(m_variableName.c_str(), other.m_variableName.c_str()) == 0;
    bool sameType = m_expectedType == other.m_expectedType;
    bool sameCondition = m_conditionType == other.m_conditionType;

    if (!sameName || !sameType || !sameCondition)
    {
        return false;
    }

    // Check variable
    switch (m_expectedType)
    {
    default:
    case TYPE_BOOL:
        return m_value.m_bool == other.m_value.m_bool;
    case TYPE_FLOAT:
        return m_value.m_float == other.m_value.m_float;
    case TYPE_INT:
        return m_value.m_int == other.m_value.m_int;
    }
}

void TransitionCondition::BuildJSON(JParse::Object * container) const
{
    SET_DATA(container, CONDITION_VARIABLE_NAME, JParse::String, GetVariableNameStr());
    SET_DATA(container, CONDITION_TYPE, JParse::String, GetConditionTypeStr());

    if (ShouldSerializeOperation())
    {
        SET_DATA(container, CONDITION_OPERATION, JParse::String, GetOperatorTypeStr());
    }

    if (ShouldSerializeValue())
    {
        switch (m_expectedType)
        {
        case TYPE_BOOL:
            SET_DATA(container, CONDITION_VALUE, JParse::Boolean, m_value.m_bool);
            break;
        case TYPE_FLOAT:
            SET_DATA(container, CONDITION_VALUE, JParse::Float, m_value.m_float);
            break;
        default:
        case TYPE_INT:
            SET_DATA(container, CONDITION_VALUE, JParse::Integer, m_value.m_int);
            break;
        }
    }
}

bool const TransitionCondition::ShouldSerializeOperation() const
{
    if (m_expectedType == TYPE_BOOL && m_conditionType == EQUAL && m_value.m_bool == true)
    {
        // We are just a Boolean IsTrue, so recording the operation is redundant
        return false;
    }

    return true;
}

bool const TransitionCondition::ShouldSerializeValue() const
{
    if (m_expectedType == TYPE_BOOL && m_conditionType == EQUAL && m_value.m_bool == true)
    {
        // We are just a Boolean IsTrue, so recording the value is redundant
        return false;
    }

    return true;
}

std::string const TransitionCondition::GetVariableNameStr() const
{
    return StringConvert::ToStr(m_variableName);
}

std::string const TransitionCondition::GetConditionTypeStr() const
{
    switch (m_expectedType)
    {
    case TYPE_BOOL:
        if (m_conditionType == EQUAL && m_value.m_bool == true)
        {
            return CONDITION_BOOLEAN;
        }
        return CONDITION_OP_BOOL;
    case TYPE_FLOAT:
        return CONDITION_OP_FLOAT;
    default:
    case TYPE_INT:
        return CONDITION_OP_INT;
    }
}

std::string const TransitionCondition::GetOperatorTypeStr() const
{
    switch (m_conditionType)
    {
    default:
    case EQUAL:
        return OPERATOR_EQUAL;
    case NOT_EQUAL:
        return OPERATOR_NOT_EQUAL;
    case LESS:
        return OPERATOR_LESS;
    case LESS_EQUAL:
        return OPERATOR_LESS_EQUAL;
    case GREATER:
        return OPERATOR_GREATER;
    case GREATER_EQUAL:
        return OPERATOR_GREATER_EQUAL;
    }
}
