#pragma once

#include "ISelectable.h"

#include <string>
#include <vector>

class Node;

enum VariableType
{
    TYPE_BOOL,
    TYPE_FLOAT,
    TYPE_INT
};

enum OperatorType
{
    EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL
};

union Variable
{
    float m_float;
    int m_int;
    bool m_bool;
};

namespace JParse
{
    struct Object;
}

struct TransitionCondition
{
    void SetOperatorFromString(std::string const& op);
    void SetVariableTypeFromString(std::string const& type);
    bool const IsEqual(TransitionCondition const& other) const;

    void BuildJSON(JParse::Object* container) const;
    bool const ShouldSerializeOperation() const;
    bool const ShouldSerializeValue() const;

    std::string const GetVariableNameStr() const;
    std::string const GetConditionTypeStr() const;
    std::string const GetOperatorTypeStr() const;

    std::wstring m_variableName;
    VariableType m_expectedType;
    OperatorType m_conditionType;
    Variable m_value;
};

class Transition : public ISelectable
{
public:
    Transition(HWND const parentWindowHandle);

    // Properties
    void SetName(std::wstring const& name) { m_name = name; }
    std::wstring const& GetName() const { return m_name; }

    std::vector<TransitionCondition>& GetConditions() { return m_conditions; }
    std::vector<TransitionCondition> const& GetConditions() const { return m_conditions; }

    void SetFromNode(Node* const from) { m_fromNode = from; }
    void SetToNode(Node* const to) { m_toNode = to; }

    Node* const GetFromNode() const { return m_fromNode; }
    Node* const GetToNode() const { return m_toNode; }

    bool IsMouseOverlapping(POINT mousePos);

    HRGN const& GetPaintRegion() override;

    void Paint(HDC hdc, HBRUSH fillColor);

    bool const HasSameConditions(Transition* transition) const;
    
private:
    void UpdateRegion();

    void BuildPointArrow(POINT* outPoints, float directionX, float directionY);

private:
    Node* m_fromNode;
    Node* m_toNode;

    HRGN m_paintRegion;

    // Properties
    std::wstring m_name;
    std::vector<TransitionCondition> m_conditions;
};