#include "Transition.h"

#include "GraphmationForgeApp.h"
#include "Node.h"

#include "ParsingDefines.h"

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
    if (strcmp(str, OPERATOR_EQUAL))
    {
        m_conditionType = EQUAL;
    }
    else if (strcmp(str, OPERATOR_NOT_EQUAL))
    {
        m_conditionType = NOT_EQUAL;
    }
    else if (strcmp(str, OPERATOR_GREATER))
    {
        m_conditionType = GREATER;
    }
    else if (strcmp(str, OPERATOR_GREATER_EQUAL))
    {
        m_conditionType = GREATER_EQUAL;
    }
    else if (strcmp(str, OPERATOR_LESS))
    {
        m_conditionType = LESS;
    }
    else if (strcmp(str, OPERATOR_LESS_EQUAL))
    {
        m_conditionType = LESS_EQUAL;
    }
}

void TransitionCondition::SetVariableTypeFromString(std::string const & type)
{
    const char* str = type.c_str();
    if (strcmp(str, CONDITION_OP_INT))
    {
        m_expectedType = TYPE_INT;
    }
    else if (strcmp(str, CONDITION_OP_FLOAT))
    {
        m_expectedType = TYPE_FLOAT;
    }
    else if (strcmp(str, CONDITION_OP_BOOL) || strcmp(str, CONDITION_BOOLEAN))
    {
        m_expectedType = TYPE_BOOL;
    }
}
