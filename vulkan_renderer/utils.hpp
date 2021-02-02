#pragma once

template <class T> class SingleThreaded
{
public:
  typedef T VolatileType;
};

template
<
  class T//,
  //template <class> class CreationPolicy = CreateUsingNew,
  //template <class> class LifetimePolicy = DefaultLifetime,
  //template <class> class ThreadingModel = SingleThreaded
>
class SingeltonHolder
{
public:
  static T& Instance()
  {
    if (!pInstance_)
    {
      //typename ThreadingModel<T>::Lock guard;
      if (!pInstance_)
      {
        if (destroyed_)
        {
          //LifetimePolicy<T>::OnDeadReference();
          destroyed_ = false;
        }
        pInstance_ = new T;// CreationPolicy<T>::Create();
        //LifetimePolicy<T>::ScheduleCall(&destroySingelton);
      }
    }
    return pInstance_;
  }

private:
  //helpers
  static void destroySingelton()
  {
    assert(!destroyed_);
    //CreationPolicy<T>::Destroy(pInstance_);
    delete T;
    pInstance_ = 0;
    destroyed_ = true;
  }

  //protection
  SingeltonHolder();

  //data
  //typedef ThreadingModel<T>::VotileType InstanceType;
  static T* pInstance_;
  static bool destroyed_;
};

