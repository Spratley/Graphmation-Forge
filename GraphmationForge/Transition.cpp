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
{
    InitProperties();
}

void Transition::SetName(std::wstring const & name)
{
    m_properties.GetPropertyPtr<StringProperty>(PropertyID_TransitionName)->m_value = name;
}

std::wstring const & Transition::GetName() const
{
    return m_properties.GetPropertyPtr<StringProperty>(PropertyID_TransitionName)->m_value;
}

std::vector<TransitionCondition>& Transition::GetConditions()
{
    return m_properties.GetPropertyPtr<VectorProperty<TransitionCondition>>(PropertyID_Conditions)->m_value;
}

std::vector<TransitionCondition> const & Transition::GetConditions() const
{
    return m_properties.GetPropertyPtr<VectorProperty<TransitionCondition>>(PropertyID_Conditions)->m_value;
}

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
    std::vector<TransitionCondition> const& myConditions = GetConditions();
    for (TransitionCondition const& condition : myConditions)
    {
        bool hasMatchingCondition = false;
        std::vector<TransitionCondition> const& otherConditions = transition->GetConditions();
        for (TransitionCondition const& otherCondition : otherConditions)
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

void Transition::AddNewCondition()
{
    std::vector<TransitionCondition>& conditions = GetConditions();
    conditions.emplace_back();
}

void Transition::DeleteCondition(int conditionIndex)
{
    std::vector<TransitionCondition>& conditions = GetConditions();
    if (conditionIndex >= conditions.size())
    {
        return;
    }
    conditions.erase(conditions.begin() + conditionIndex);
}

void Transition::UpdateRegion()
{
    DeleteObject(m_paintRegion);

    POINT fromPoint = m_fromNode->GetNodePosition();
    POINT toPoint = m_toNode->GetNodePosition();

    fromPoint.x += NODE_MIN_WIDTH / 2;
    fromPoint.y += NODE_HEIGHT / 2;
    toPoint.x += NODE_MIN_WIDTH / 2;
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

void Transition::InitProperties()
{
    m_properties.RegisterProperty(std::make_unique<StringProperty>(L"New Transition"), PropertyID_TransitionName);
    m_properties.RegisterProperty(std::make_unique<VectorProperty<TransitionCondition>>(), PropertyID_Conditions);
}

TransitionCondition::TransitionCondition()
{
    InitProperties();
}

TransitionCondition::TransitionCondition(TransitionCondition const& other)
: m_properties(other.m_properties)
{ }

void TransitionCondition::SetOperatorFromString(std::string const & op)
{
    std::wstring str = StringConvert::ToWStr(op);
    OperatorType::Enum value = OperatorType::FromString(str);
    m_properties.GetPropertyPtr<EnumProperty<OperatorType, OperatorType::Enum>>(PropertyID_ConditionType)->m_value = value;
}

void TransitionCondition::SetVariableTypeFromString(std::string const & type)
{
    std::wstring str = StringConvert::ToWStr(type);
    VariableType::Enum value = VariableType::FromString(str);
    m_properties.GetPropertyPtr<EnumProperty<VariableType, VariableType::Enum>>(PropertyID_ExpectedType)->m_value = value;
}

bool const TransitionCondition::IsEqual(TransitionCondition const & other) const
{
    bool sameName = StrCmpW(GetVariableName().c_str(), other.GetVariableName().c_str()) == 0;
    bool sameType = GetVariableType() == other.GetVariableType();
    bool sameCondition = GetConditionType() == other.GetConditionType();

    if (!sameName || !sameType || !sameCondition)
    {
        return false;
    }

    // Check variable
    switch (GetVariableType())
    {
    default:
    case VariableType::TYPE_BOOL:
        return GetVariableConst().m_bool == other.GetVariableConst().m_bool;
    case VariableType::TYPE_FLOAT:
        return GetVariableConst().m_float == other.GetVariableConst().m_float;
    case VariableType::TYPE_INT:
        return GetVariableConst().m_int == other.GetVariableConst().m_int;
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
        switch (GetVariableType())
        {
        case VariableType::Enum::TYPE_BOOL:
            SET_DATA(container, CONDITION_VALUE, JParse::Boolean, GetVariableConst().m_bool);
            break;
        case VariableType::Enum::TYPE_FLOAT:
            SET_DATA(container, CONDITION_VALUE, JParse::Float, GetVariableConst().m_float);
            break;
        default:
        case VariableType::Enum::TYPE_INT:
            SET_DATA(container, CONDITION_VALUE, JParse::Integer, GetVariableConst().m_int);
            break;
        }
    }
}

bool const TransitionCondition::ShouldSerializeOperation() const
{
    if (GetVariableType() == VariableType::Enum::TYPE_BOOL && GetConditionType() == OperatorType::Enum::EQUAL && GetVariableConst().m_bool == true)
    {
        // We are just a Boolean IsTrue, so recording the operation is redundant
        return false;
    }

    return true;
}

bool const TransitionCondition::ShouldSerializeValue() const
{
    if (GetVariableType() == VariableType::Enum::TYPE_BOOL && GetConditionType() == OperatorType::Enum::EQUAL && GetVariableConst().m_bool == true)
    {
        // We are just a Boolean IsTrue, so recording the value is redundant
        return false;
    }

    return true;
}

std::string const TransitionCondition::GetVariableNameStr() const
{
    return StringConvert::ToStr(GetVariableName());
}

std::string const TransitionCondition::GetConditionTypeStr() const
{
    switch (GetVariableType())
    {
    case VariableType::Enum::TYPE_BOOL:
        if (GetConditionType() == OperatorType::Enum::EQUAL && GetVariableConst().m_bool == true)
        {
            return CONDITION_BOOLEAN;
        }
        return CONDITION_OP_BOOL;
    case VariableType::Enum::TYPE_FLOAT:
        return CONDITION_OP_FLOAT;
    default:
    case VariableType::Enum::TYPE_INT:
        return CONDITION_OP_INT;
    }
}

std::string const TransitionCondition::GetOperatorTypeStr() const
{
    switch (GetConditionType())
    {
    default:
    case OperatorType::Enum::EQUAL:
        return OPERATOR_EQUAL;
    case OperatorType::Enum::NOT_EQUAL:
        return OPERATOR_NOT_EQUAL;
    case OperatorType::Enum::LESS:
        return OPERATOR_LESS;
    case OperatorType::Enum::LESS_EQUAL:
        return OPERATOR_LESS_EQUAL;
    case OperatorType::Enum::GREATER:
        return OPERATOR_GREATER;
    case OperatorType::Enum::GREATER_EQUAL:
        return OPERATOR_GREATER_EQUAL;
    }
}

void TransitionCondition::SetVariableName(std::wstring const & name)
{
    m_properties.GetPropertyPtr<StringProperty>(PropertyID_VariableName)->m_value = name;
}

void TransitionCondition::SetVariableType(VariableType::Enum const & type)
{
    m_properties.GetPropertyPtr<EnumProperty<VariableType, VariableType::Enum>>(PropertyID_ExpectedType)->m_value = type;
}

void TransitionCondition::SetConditionType(OperatorType::Enum const & type)
{
    m_properties.GetPropertyPtr<EnumProperty<OperatorType, OperatorType::Enum>>(PropertyID_ConditionType)->m_value = type;
}

void TransitionCondition::SetVariable(Variable const & variable)
{
    m_properties.GetPropertyPtr<VariableProperty>(PropertyID_Variable)->m_value = variable;
}

std::wstring const & TransitionCondition::GetVariableName() const
{
    return m_properties.GetPropertyPtr<StringProperty>(PropertyID_VariableName)->m_value;
}

VariableType::Enum const & TransitionCondition::GetVariableType() const
{
    return m_properties.GetPropertyPtr<EnumProperty<VariableType, VariableType::Enum>>(PropertyID_ExpectedType)->m_value;
}

OperatorType::Enum const & TransitionCondition::GetConditionType() const
{
    return m_properties.GetPropertyPtr<EnumProperty<OperatorType, OperatorType::Enum>>(PropertyID_ConditionType)->m_value;
}

Variable const & TransitionCondition::GetVariableConst() const
{
    return m_properties.GetPropertyPtr<VariableProperty>(PropertyID_Variable)->m_value;
}

Variable & TransitionCondition::GetVariable()
{
    return m_properties.GetPropertyPtr<VariableProperty>(PropertyID_Variable)->m_value;
}

void TransitionCondition::InitProperties()
{
    m_properties.RegisterProperty(std::make_shared<StringProperty>(L"VariableName"), PropertyID_VariableName);
    m_properties.RegisterProperty(std::make_shared<EnumProperty<VariableType, VariableType::Enum>>(VariableType::TYPE_INT), PropertyID_ExpectedType);
    m_properties.RegisterProperty(std::make_shared<EnumProperty<OperatorType, OperatorType::Enum>>(OperatorType::EQUAL), PropertyID_ConditionType);
    m_properties.RegisterProperty(std::make_shared<VariableProperty>(), PropertyID_Variable);
}