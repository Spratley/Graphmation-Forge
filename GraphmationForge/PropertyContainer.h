#pragma once

#include <unordered_map>

struct Property;

class PropertyContainer
{
public:
    ~PropertyContainer();

    void RegisterProperty(Property* property, int const propertyID);
    Property* GetProperty(int const propertyID) const;

    template <typename T>
    T* GetPropertyPtr(int const propertyID) const;

    std::unordered_map<int, Property*> const& GetProperties() const { return m_properties; }

private:
    std::unordered_map<int, Property*> m_properties;
};

template<typename T>
inline T * PropertyContainer::GetPropertyPtr(int const propertyID) const
{
    return static_cast<T*>(GetProperty(propertyID));
}
