#pragma once



/**
 * Manages a scoped pointer much like in the boost library. This object is non-copyable and as the 
 * name suggests it maintain the object within a scope and guarantees that it is deleted. This works 
 * with std::shared_ptrs maintaining them as internal class objects.
 **/
template<typename T>
public ref class scoped_ptr
{    
  
#pragma region copy constructors

public:  
 
  
  scoped_ptr( scoped_ptr<T> % )
  { throw gcnew System::InvalidOperationException("scoped_ptr is non-copyable"); }    
  
  template<typename U>    
  scoped_ptr( scoped_ptr<U> % ) 
  { throw gcnew System::InvalidOperationException("scoped_ptr is non-copyable"); }    
  
  scoped_ptr % operator=( scoped_ptr<T> % ) 
  { throw gcnew System::InvalidOperationException("scoped_ptr is non-copyable"); }    
  
  template<typename U>    
  scoped_ptr % operator=( scoped_ptr<U> % ) 
  { throw gcnew System::InvalidOperationException("scoped_ptr is non-copyable"); }

#pragma endregion


public:   

  /// Default constructor
  scoped_ptr( void ) : 
  m_native_ptr(nullptr) 
  {}    

  /// Templated constructor takes the pointer type and set the native pointer to it 
  template<typename U>    
  scoped_ptr( U ptr ) : 
  m_native_ptr(ptr) 
  {}    

  /// Constructor
  explicit scoped_ptr( T* ptr ) : 
  m_native_ptr(ptr) 
  {}    
  
  /// Finalizer (Resets the memory to NULL)
  !scoped_ptr( void ) 
  { reset(); }    
  
  /// Destructor
  ~scoped_ptr( void ) 
  { scoped_ptr::!scoped_ptr(); }    
  
  /// Resets the native pointer 
  void reset( T * ptr ) 
  { 
    /// Delete the old pointer which causes a shared_ptr to decrement the reference counter
    delete m_native_ptr; 

    /// Sets the new pointer
    m_native_ptr = ptr; 

  } // reset( T * ptr )   
  
  /// Resets the pointer to null causing the interal pointer to be released
  inline void reset( void ) 
  { 
    reset(nullptr); 
  }    
  
  /// Overrides the equal to reset the internal native pointer
  scoped_ptr % operator=( T * ptr ) 
  { Reset(ptr); }   
  
  /// Overides the equal to reset the interal native pointer
  template<typename U>    
  scoped_ptr % operator=( U ptr ) 
  { reset(ptr); }    
  
  
  // Swap the tehe contents of the pointers
  void swap( scoped_ptr<T> % other )    
  {        
    using std::swap;        
    swap(m_native_ptr, other->m_native_ptr);    
  } // swap()
  
  // ==============================================================================================
  // Releases the native pointer (unscoping) returning the pointer. This can be uncommented if a
  // transfer of ownership is required. Thus making it closer to an auto_ptr.
  // ==============================================================================================
  //T * release( ) 
  //{ 
  //  T * ptr = m_native_ptr; 
  //  m_native_ptr = nullptr; 
  //  return ptr; 
  //}   // Release() 
  
  /// Gets the pointer
  T * get( ) 
  { 
    return m_native_ptr; 
  } // Get()    
  
  //  Reference to the pointer
  static T * operator->( scoped_ptr<T> % ptr ) 
  { return ptr.get(); }    
  
  // Dereference operator
  static T & operator*( scoped_ptr<T> % ptr ) 
  { return *ptr.get(); }

private:

  /// Native pointer
  T * m_native_ptr;    
  
};  // class scoped_ptr
  


 