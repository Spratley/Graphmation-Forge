#pragma once

#include "Node.h"

#include <vector>

class NodeSelector : public Node
{
public:
    NodeSelector(HWND mainWindowHandle, HWND nodeWindowHandle) : Node(mainWindowHandle, nodeWindowHandle) {}

    // Properties    
    std::vector<std::wstring> const& GetAnimations() const;
    std::vector<std::wstring>& GetAnimations();


protected:
    void InitProperties() override;

private:
    enum
    {
        PropertyID_AssociatedAnimations
    };
};