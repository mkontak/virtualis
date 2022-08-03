#pragma once

namespace dcm
{

/**
 * Collection interface
 */
template<typename T>
class DCM_DLL ICollection
{
protected:

  ICollection() {}

public:
    /// Clears the collection
  virtual void Clear() = 0;

  /// Gets the current object
  virtual std::shared_ptr<T> Current() = 0;

  /// If not at the end moves indexer to next object and returns true, otherwise returns false
  virtual bool MoveNext() = 0;

  /// Resets the indexer
  virtual void Reset() = 0;

  /// Removes the specified object if it exists and returns true, otherwise retuns false
  virtual bool Remove(const std::shared_ptr<T> & item) = 0;
 
  /// Determines if the item specified exists in the collection and returns true if found, flase if not
  virtual bool Contains(const std::shared_ptr<T> & item) = 0;
  
  /// Returns true if the collection is empty
  virtual bool IsEmpty() = 0;

  /// Returns the number of object in collection
  virtual unsigned int Count() = 0;

  /// Adds the item to the collection
  virtual void Add(std::shared_ptr<T> & item) = 0;

}; // class ICollection

} // namespace dcm