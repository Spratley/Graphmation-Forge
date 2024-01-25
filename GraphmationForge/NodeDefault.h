#pragma once

#include "Node.h"

class NodeDefault : public Node
{
public:
    NodeDefault(HWND mainWindowHandle, HWND nodeWindowHandle) : Node(mainWindowHandle, nodeWindowHandle) {}

    // Properties    
    void SetAnimationName(std::wstring const& animationName);

    std::wstring const& GetAnimationName() const;

    void InitProperties() override;

private:
    enum
    {
        PropertyID_AssociatedAnimation = NumBaseNodeProperties
    };
};