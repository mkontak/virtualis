// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include <functional>
#include <vector>
#include <algorithm>



template <typename T>
class IObserver
{
public:

  /// destructor
  virtual ~IObserver() {}

  /// notification handler
  virtual void OnNotified( const T & args ) = 0;

};



template <typename T>
class ObservableObject 
{
// functions
public:

  virtual ~ObservableObject<T>() {}
 
public:

  /// registers an observer to receive notification
  virtual void Connect( IObserver<T> * pObserver )
  {
    if( std::find( m_observers.begin(), m_observers.end(), pObserver ) == m_observers.end() )
      m_observers.push_back( pObserver );
  }

  /// unregisters an observer from receiving notification
  virtual void Disconnect( IObserver<T> * pObserver )
  {
    m_observers.erase( std::remove( m_observers.begin(), m_observers.end(), pObserver ), m_observers.end() );
  }

  /// notifies all registered observers
  virtual void Notify( const T & args )
  {
    std::for_each( m_observers.begin(), m_observers.end(), [&args]( IObserver<T> * pObserver ) { pObserver->OnNotified( args ); });
  }

private:

  // collection of observers
  std::vector< IObserver<T> * > m_observers;

};



template <typename T, typename Args = T>
class Observable : public ObservableObject< Args >, public T
{
// functions
public:

  /// default constructor
  explicit Observable<T, Args>() {};

  /// constructor
  explicit Observable<T, Args>(const T & other)
  {
    *dynamic_cast<T*>(this) = other;
  }

  /// assignment operator
  Observable<T, Args> & operator=(const Observable<T, Args> & other) 
  { 
    *dynamic_cast<T*>(this) = *dynamic_cast<const T *>(&other);
    m_clients = other.m_clients;
    return *this; 
  }

  /// assignment operator FOR OBJECT ONLY
  Observable<T, Args> & operator=(const T & other) 
  { 
    *dynamic_cast<T*>(this) = other;
    return *this; 
  }


private:

  // copy constructor
  Observable<T, Args>(const Observable<T, Args> & other); // should only be public if implemented

};
