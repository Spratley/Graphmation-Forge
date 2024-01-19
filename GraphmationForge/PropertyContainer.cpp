#include "PropertyContainer.h"

PropertyContainer::~PropertyContainer()
{
    for (auto const& property : m_properties)
    {
        delete property.second;
    }
}

void PropertyContainer::RegisterProperty(Property * property, int const propertyID)
{
    if (m_properties.count(propertyID) > 0)
    {
        delete m_properties[propertyID];
    }
    m_properties[propertyID] = property;
}
 
Property * PropertyContainer::GetProperty(int const propertyID) const
{
    if (m_properties.count(propertyID) == 0)
    {
        return nullptr;
    }
    return m_properties.at(propertyID);
}
