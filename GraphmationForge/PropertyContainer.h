#pragma once

#include <unordered_map>
#include <memory>

struct Property;

class PropertyContainer
{
public:
    PropertyContainer() {}
    PropertyContainer(PropertyContainer const& other);
    ~PropertyContainer();

    void RegisterProperty(std::shared_ptr<Property> property, int const propertyID);
    std::shared_ptr<Property> GetProperty(int const propertyID) const;

    template <typename T>
    std::shared_ptr<T> GetPropertyPtr(int const propertyID) const;

    std::unordered_map<int, std::shared_ptr<Property>> const& GetProperties() const { return m_properties; }

private:
    std::unordered_map<int, std::shared_ptr<Property>> m_properties;
};

template<typename T>
inline std::shared_ptr<T> PropertyContainer::GetPropertyPtr(int const propertyID) const
{
    return std::static_pointer_cast<T>(GetProperty(propertyID));
}
