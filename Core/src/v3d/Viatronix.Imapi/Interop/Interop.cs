using System;
using System.Collections;
using System.Diagnostics;
using System.IO;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Threading;
 
namespace Viatronix.Imapi.Interop
{

  #region DDiscFormat2DataEvents
  /// <summary>
  /// Data Writer
  /// </summary>
  [ComImport]
  [Guid("2735413C-7F64-5B0F-8F00-5D77AFBE261E")]
  [TypeLibType(TypeLibTypeFlags.FNonExtensible | TypeLibTypeFlags.FOleAutomation | TypeLibTypeFlags.FDispatchable)]
  public interface DDiscFormat2DataEvents
  {
    // Update to current progress
    [DispId(0x200)]     // DISPID_DDISCFORMAT2DATAEVENTS_UPDATE
    [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
    void Update([In, MarshalAs(UnmanagedType.IDispatch)] object sender, [In, MarshalAs(UnmanagedType.IDispatch)] object progress);
  }

  [ComVisible(false)]
  [ComEventInterface(typeof(DDiscFormat2DataEvents), typeof(DiscFormat2Data_EventProvider))]
  [TypeLibType(TypeLibTypeFlags.FHidden)]
  public interface DiscFormat2Data_Event
  {
    // Events
    event DiscFormat2Data_EventHandler Update;
  }

  [ClassInterface(ClassInterfaceType.None)]
  internal sealed class DiscFormat2Data_EventProvider : DiscFormat2Data_Event, IDisposable
  {
    // Fields
    private Hashtable m_aEventSinkHelpers = new Hashtable();
    private IConnectionPoint m_connectionPoint = null;


    // Methods
    public DiscFormat2Data_EventProvider(object pointContainer)
    {
      lock (this)
      {
        Guid eventsGuid = typeof(DDiscFormat2DataEvents).GUID;
        IConnectionPointContainer connectionPointContainer = pointContainer as IConnectionPointContainer;

        connectionPointContainer.FindConnectionPoint(ref eventsGuid, out m_connectionPoint);
      }
    }

    public event DiscFormat2Data_EventHandler Update
    {
      add
      {
        lock (this)
        {
          DiscFormat2Data_SinkHelper helper =
              new DiscFormat2Data_SinkHelper(value);
          int cookie;

          m_connectionPoint.Advise(helper, out cookie);
          helper.Cookie = cookie;
          m_aEventSinkHelpers.Add(helper.UpdateDelegate, helper);
        }
      }

      remove
      {
        lock (this)
        {
          DiscFormat2Data_SinkHelper helper =
              m_aEventSinkHelpers[value] as DiscFormat2Data_SinkHelper;
          if (helper != null)
          {
            m_connectionPoint.Unadvise(helper.Cookie);
            m_aEventSinkHelpers.Remove(helper.UpdateDelegate);
          }
        }
      }
    }

    ~DiscFormat2Data_EventProvider()
    {
      Cleanup();
    }

    public void Dispose()
    {
      Cleanup();
      GC.SuppressFinalize(this);
    }

    private void Cleanup()
    {
      Monitor.Enter(this);
      try
      {
        foreach (DiscFormat2Data_SinkHelper helper in m_aEventSinkHelpers.Values)
        {
          m_connectionPoint.Unadvise(helper.Cookie);
        }

        m_aEventSinkHelpers.Clear();
        Marshal.ReleaseComObject(m_connectionPoint);
      }
      catch (SynchronizationLockException)
      {
        return;
      }
      finally
      {
        Monitor.Exit(this);
      }
    }
  }

  [ClassInterface(ClassInterfaceType.None)]
  [TypeLibType(TypeLibTypeFlags.FHidden)]
  public sealed class DiscFormat2Data_SinkHelper : DDiscFormat2DataEvents
  {
    // Fields
    private int m_dwCookie;
    private DiscFormat2Data_EventHandler m_UpdateDelegate;

    // Methods
    internal DiscFormat2Data_SinkHelper(DiscFormat2Data_EventHandler eventHandler)
    {
      if (eventHandler == null)
        throw new ArgumentNullException("Delegate (callback function) cannot be null");
      m_dwCookie = 0;
      m_UpdateDelegate = eventHandler;
    }

    public void Update(object sender, object args)
    {
      m_UpdateDelegate(sender, args);
    }

    public int Cookie
    {
      get
      {
        return m_dwCookie;
      }
      set
      {
        m_dwCookie = value;
      }
    }

    public DiscFormat2Data_EventHandler UpdateDelegate
    {
      get
      {
        return m_UpdateDelegate;
      }
      set
      {
        m_UpdateDelegate = value;
      }
    }
  }

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void DiscFormat2Data_EventHandler([In, MarshalAs(UnmanagedType.IDispatch)] object sender, [In, MarshalAs(UnmanagedType.IDispatch)] object progress);

  #endregion // DDiscFormat2DataEvents

  #region  DDiscFormat2EraseEvents
  /// <summary>
  /// Provides notification of media erase progress.
  /// </summary>
  [ComImport]
  [TypeLibType(TypeLibTypeFlags.FNonExtensible | TypeLibTypeFlags.FOleAutomation | TypeLibTypeFlags.FDispatchable)]
  [Guid("2735413A-7F64-5B0F-8F00-5D77AFBE261E")]
  public interface DDiscFormat2EraseEvents
  {
    // Update to current progress
    [DispId(0x200)]     // DISPID_IDISCFORMAT2ERASEEVENTS_UPDATE 
    [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
    void Update([In, MarshalAs(UnmanagedType.IDispatch)] object sender, [In] int elapsedSeconds, [In] int estimatedTotalSeconds);
  }

  [TypeLibType(TypeLibTypeFlags.FHidden)]
  [ComVisible(false)]
  [ComEventInterface(typeof(DDiscFormat2EraseEvents), typeof(DiscFormat2Erase_EventProvider))]
  public interface DiscFormat2Erase_Event
  {
    // Events
    event DiscFormat2Erase_EventHandler Update;
  }

  [ClassInterface(ClassInterfaceType.None)]
  internal sealed class DiscFormat2Erase_EventProvider : DiscFormat2Erase_Event, IDisposable
  {
    // Fields
    private Hashtable m_aEventSinkHelpers = new Hashtable();
    private IConnectionPoint m_connectionPoint = null;

    // Methods
    public DiscFormat2Erase_EventProvider(object pointContainer)
    {
      lock (this)
      {
        Guid eventsGuid = typeof(DDiscFormat2EraseEvents).GUID;
        IConnectionPointContainer connectionPointContainer = pointContainer as IConnectionPointContainer;

        connectionPointContainer.FindConnectionPoint(ref eventsGuid, out m_connectionPoint);
      }
    }

    public event DiscFormat2Erase_EventHandler Update
    {
      add
      {
        lock (this)
        {
          DiscFormat2Erase_SinkHelper helper =
              new DiscFormat2Erase_SinkHelper(value);
          int cookie = -1;

          m_connectionPoint.Advise(helper, out cookie);
          helper.Cookie = cookie;
          m_aEventSinkHelpers.Add(helper.UpdateDelegate, helper);
        }
      }

      remove
      {
        lock (this)
        {
          DiscFormat2Erase_SinkHelper helper =
              m_aEventSinkHelpers[value] as DiscFormat2Erase_SinkHelper;
          if (helper != null)
          {
            m_connectionPoint.Unadvise(helper.Cookie);
            m_aEventSinkHelpers.Remove(helper.UpdateDelegate);
          }
        }
      }
    }

    ~DiscFormat2Erase_EventProvider()
    {
      Cleanup();
    }

    public void Dispose()
    {
      Cleanup();
      GC.SuppressFinalize(this);
    }

    private void Cleanup()
    {
      Monitor.Enter(this);
      try
      {
        foreach (DiscFormat2Erase_SinkHelper helper in m_aEventSinkHelpers.Values)
        {
          m_connectionPoint.Unadvise(helper.Cookie);
        }

        m_aEventSinkHelpers.Clear();
        Marshal.ReleaseComObject(m_connectionPoint);
      }
      catch (SynchronizationLockException)
      {
        return;
      }
      finally
      {
        Monitor.Exit(this);
      }
    }
  }

  [TypeLibType(TypeLibTypeFlags.FHidden)]
  [ClassInterface(ClassInterfaceType.None)]
  public sealed class DiscFormat2Erase_SinkHelper : DDiscFormat2EraseEvents
  {
    // Fields
    private int m_dwCookie;
    private DiscFormat2Erase_EventHandler m_UpdateDelegate;

    public DiscFormat2Erase_SinkHelper(DiscFormat2Erase_EventHandler eventHandler)
    {
      if (eventHandler == null)
        throw new ArgumentNullException("Delegate (callback function) cannot be null");
      m_dwCookie = 0;
      m_UpdateDelegate = eventHandler;
    }

    public void Update(object sender, int elapsedSeconds, int estimatedTotalSeconds)
    {
      m_UpdateDelegate(sender, elapsedSeconds, estimatedTotalSeconds);
    }

    public int Cookie
    {
      get
      {
        return m_dwCookie;
      }
      set
      {
        m_dwCookie = value;
      }
    }

    public DiscFormat2Erase_EventHandler UpdateDelegate
    {
      get
      {
        return m_UpdateDelegate;
      }
      set
      {
        m_UpdateDelegate = value;
      }
    }
  }

  [UnmanagedFunctionPointer(CallingConvention.StdCall)]
  public delegate void DiscFormat2Erase_EventHandler([In, MarshalAs(UnmanagedType.IDispatch)]object sender, [In] int elapsedSeconds, [In] int estimatedTotalSeconds);
  #endregion  // DDiscFormat2EraseEvents

} // namespace Viatronix.Imapi.Interop
