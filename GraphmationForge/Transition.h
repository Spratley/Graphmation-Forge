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

enum ConditionType
{
    EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    IS_TRUE,
    IS_FALSE
};

union Variable
{
    float m_float;
    int m_int;
    bool m_bool;
};

struct TransitionCondition
{
    std::wstring m_variableName;
    VariableType m_expectedType;
    ConditionType m_conditionType;
    Variable m_value;
};

class Transition : public ISelectable
{
public:
    Transition(HWND const parentWindowHandle);

    // Properties
    std::vector<TransitionCondition>& GetConditions() { return m_conditions; }
    std::vector<TransitionCondition> const& GetConditions() const { return m_conditions; }

    void SetFromNode(Node* const from) { m_fromNode = from; }
    void SetToNode(Node* const to) { m_toNode = to; }

    Node* const GetFromNode() const { return m_fromNode; }
    Node* const GetToNode() const { return m_toNode; }

    bool IsMouseOverlapping(POINT mousePos);

    HRGN const& GetPaintRegion() override;

    void Paint(HDC hdc, HBRUSH fillColor);
    
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