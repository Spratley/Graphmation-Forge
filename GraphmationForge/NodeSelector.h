#pragma once

#include "Node.h"
#include "Property.h"

#include <vector>
#include <memory.h>

class NodeSelector : public Node
{
public:
    NodeSelector(HWND mainWindowHandle, HWND nodeWindowHandle) : Node(mainWindowHandle, nodeWindowHandle) {}

    // Properties    
    std::vector<std::shared_ptr<StringProperty>>& GetAnimations();
    std::vector<std::shared_ptr<StringProperty>> const& GetAnimations() const;

    void SetVariableName(std::wstring const& variableName);
    std::wstring const& GetVariableName() const;

    void InitProperties() override;
private:
    enum
    {
        PropertyID_VariableName = NumBaseNodeProperties,
        PropertyID_AssociatedAnimations
    };
};