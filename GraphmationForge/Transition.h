#pragma once

#include "ISelectable.h"
#include "PropertyContainer.h"
#include "EnumHelper.h"

#include <string>
#include <vector>

class Node;

union Variable;

namespace JParse
{
    struct Object;
}

struct TransitionCondition
{
    TransitionCondition();
    TransitionCondition(TransitionCondition const& other);

    void SetOperatorFromString(std::string const& op);
    void SetVariableTypeFromString(std::string const& type);
    bool const IsEqual(TransitionCondition const& other) const;

    void BuildJSON(JParse::Object* container) const;
    bool const ShouldSerializeOperation() const;
    bool const ShouldSerializeValue() const;

    std::string const GetVariableNameStr() const;
    std::string const GetConditionTypeStr() const;
    std::string const GetOperatorTypeStr() const;

    void SetVariableName(std::wstring const& name);
    void SetVariableType(VariableType::Enum const& type);
    void SetConditionType(OperatorType::Enum const& type);
    void SetVariable(Variable const& variable);

    std::wstring const& GetVariableName() const;
    VariableType::Enum const& GetVariableType() const;
    OperatorType::Enum const& GetConditionType() const;
    Variable const& GetVariableConst() const;
    Variable& GetVariable();

    // Properties
    void InitProperties();

    // std::wstring m_variableName;
    // VariableType m_expectedType;
    // OperatorType m_conditionType;
    // Variable m_value;
    PropertyContainer m_properties;
    enum
    {
        PropertyID_VariableName,
        PropertyID_ExpectedType,
        PropertyID_ConditionType,
        PropertyID_Variable
    };
};

class Transition : public ISelectable
{
public:
    Transition(HWND const parentWindowHandle);

    // Properties
    void SetName(std::wstring const& name); // { m_name = name; }
    std::wstring const& GetName() const; // { return m_name; }

    std::vector<TransitionCondition>& GetConditions(); // { return m_conditions; }
    std::vector<TransitionCondition> const& GetConditions() const; // { return m_conditions; }

    void SetFromNode(Node* const from) { m_fromNode = from; }
    void SetToNode(Node* const to) { m_toNode = to; }

    Node* const GetFromNode() const { return m_fromNode; }
    Node* const GetToNode() const { return m_toNode; }

    bool IsMouseOverlapping(POINT mousePos);

    HRGN const& GetPaintRegion() override;

    void Paint(HDC hdc, HBRUSH fillColor);

    bool const HasSameConditions(Transition* transition) const;

    void AddNewCondition();
    void DeleteCondition(int conditionIndex);
    
private:
    void UpdateRegion();

    void BuildPointArrow(POINT* outPoints, float directionX, float directionY);

    void InitProperties();

private:
    Node* m_fromNode;
    Node* m_toNode;

    HRGN m_paintRegion;

    // Properties
    /// std::wstring m_name;
    /// std::vector<TransitionCondition> m_conditions;

    enum
    {
        PropertyID_TransitionName,
        PropertyID_Conditions
    };
};