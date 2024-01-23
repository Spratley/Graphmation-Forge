#include "PropertyContainer.h"

#include "Property.h"

PropertyContainer::PropertyContainer(PropertyContainer const& other)
: m_properties(other.m_properties)
{ 

}

PropertyContainer::~PropertyContainer()
{
    // Now that we're using std::unique_ptr we don't have to delete properties
    // for (auto const& property : m_properties)
    // {
    //     delete property.second;
    // }
}

void PropertyContainer::RegisterProperty(std::shared_ptr<Property> property, int const propertyID, std::wstring const& propertyName)
{
    /*if (m_properties.count(propertyID) > 0)
    {
        delete m_properties[propertyID];
    }*/
    property->m_name = propertyName;
    m_properties[propertyID] = property;
}
 
std::shared_ptr<Property> PropertyContainer::GetProperty(int const propertyID) const
{
    if (m_properties.count(propertyID) == 0)
    {
        return std::shared_ptr<Property>(nullptr);
    }
    return m_properties.at(propertyID);
}
