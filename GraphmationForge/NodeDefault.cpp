#include "NodeDefault.h"

#include "Property.h"

void NodeDefault::SetAnimationName(std::wstring const & animationName)
{
    m_properties.GetPropertyPtr<StringProperty>(PropertyID_AssociatedAnimation)->m_value = animationName;
}

std::wstring const & NodeDefault::GetAnimationName() const
{
    return m_properties.GetPropertyPtr<StringProperty>(PropertyID_AssociatedAnimation)->m_value;
}

void NodeDefault::InitProperties()
{
    Node::InitProperties();
    m_properties.RegisterProperty(std::make_shared<StringProperty>(L""), PropertyID_AssociatedAnimation, L"Anim");
}
