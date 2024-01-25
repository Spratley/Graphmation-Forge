#include "NodeSelector.h"

#include "Property.h"

void NodeSelector::InitProperties()
{
    m_type = NodeType::SELECTOR;
    m_properties.RegisterProperty(std::make_shared<VectorProperty<std::wstring>>(), PropertyID_AssociatedAnimations, L"Anim");
}

std::vector<std::wstring> const & NodeSelector::GetAnimations() const
{
    return m_properties.GetPropertyPtr<VectorProperty<std::wstring>>(PropertyID_AssociatedAnimations)->m_value;
}

std::vector<std::wstring>& NodeSelector::GetAnimations()
{
    return m_properties.GetPropertyPtr<VectorProperty<std::wstring>>(PropertyID_AssociatedAnimations)->m_value;
}
