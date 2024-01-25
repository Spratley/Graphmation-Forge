#include "NodeSelector.h"

#include "Property.h"

void NodeSelector::InitProperties()
{
    Node::InitProperties();
    m_type = NodeType::SELECTOR;
    m_properties.RegisterProperty(std::make_shared<StringProperty>(L"NULL"), PropertyID_VariableName, L"Var");
    m_properties.RegisterProperty(std::make_shared<VectorProperty<std::shared_ptr<StringProperty>>>(), PropertyID_AssociatedAnimations, L"Anim");
}

std::vector<std::shared_ptr<StringProperty>> const & NodeSelector::GetAnimations() const
{
    return m_properties.GetPropertyPtr<VectorProperty<std::shared_ptr<StringProperty>>>(PropertyID_AssociatedAnimations)->m_value;
}

std::vector<std::shared_ptr<StringProperty>>& NodeSelector::GetAnimations()
{
    return m_properties.GetPropertyPtr<VectorProperty<std::shared_ptr<StringProperty>>>(PropertyID_AssociatedAnimations)->m_value;
}
