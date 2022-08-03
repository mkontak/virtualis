using System;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;

using Viatronix.Imapi2.Interop;
using FileSystemInterop = Viatronix.Imapi2.FileSystem.Interop;

namespace Viatronix.Imapi
{
  /// <summary>
  /// Represents a file system that will be stored on the disc.
  /// </summary>
  public class FileSystemImage : IDisposable
  {

    #region DirectoryItem class

    /// <summary>
    /// Represents a directory item the user can add files to.
    /// </summary>
    public class DirectoryItem : IDisposable
    {

      #region dll imports

      [DllImport("shlwapi.dll", CharSet = CharSet.Unicode, ExactSpelling = true, PreserveSig = false, EntryPoint = "SHCreateStreamOnFileW")]
      public static extern void SHCreateStreamOnFile(string fileName, uint mode, ref System.Runtime.InteropServices.ComTypes.IStream stream);

      #endregion

      #region fields

      /// <summary>
      /// The directory item.
      /// </summary>
      private FileSystemInterop.IFsiDirectoryItem m_directoryItem = null;
    
      /// <summary>
      /// Has this instance been disposed of.
      /// </summary>
      private bool m_disposed = false;

      #endregion

      #region construction

      /// <summary>
      /// Constructor.
      /// </summary>
      /// <param name="directoryItem">The directory item.</param>
      internal DirectoryItem(FileSystemInterop.IFsiDirectoryItem directoryItem)
      {
        m_directoryItem = directoryItem;
      } // DirectoryItem( directoryItem )

      #endregion

      #region methods

      /// <summary>
      /// Adds a new sub-directory to the directory.
      /// </summary>
      /// <param name="name"></param>
      public void AddDirectory(DirectoryItem directory)
      {
        m_directoryItem.Add(directory.m_directoryItem);
      } // AddDirectory( name )


      /// <summary>
      /// Adds a file to the directory item.
      /// </summary>
      /// <param name="filename">The name of the file as it should appear on the disc.</param>
      /// <param name="fullpath">The full path to the file.</param>
      public void AddFile(string filename, string fullpath)
      {
        System.Runtime.InteropServices.ComTypes.IStream fileStream = null;
        SHCreateStreamOnFile(fullpath, 0x00000000 | 0x00000020, ref fileStream);
        m_directoryItem.AddFile(filename, (FileSystemInterop.FsiStream)fileStream);
      } // AddFile( filename, fullpath )

      #endregion

      #region IDisposable implementation

      /// <summary>
      /// Performs dispose.
      /// </summary>
      public void Dispose()
      {
        Dispose(true);
        GC.SuppressFinalize(this);
      } // Dispose()


      /// <summary>
      /// Disposes of this object.
      /// </summary>
      /// <param name="disposing">Disposing?</param>
      protected virtual void Dispose(bool disposing)
      {
        if (!m_disposed)
        {
          if (disposing)
          {
            // Dispose managed resources.
          }

          // Dispose unmanaged resources.
          Marshal.ReleaseComObject(m_directoryItem);
        }

        m_disposed = true;
      } // Dispose( disposing )

      #endregion

    } // class DirectoryItem

    #endregion

    #region FileSystemImageResult class

    /// <summary>
    /// The result of building up the file system.
    /// </summary>
    /// <remarks>Wraps the IMAPI IFileSystemImageResult interface.</remarks>
    public class FileSystemImageResult : IDisposable
    {

      #region fields

      /// <summary>
      /// The file system image result.
      /// </summary>
      private FileSystemInterop.IFileSystemImageResult m_imageResult = null;

      /// <summary>
      /// Has this instance been disposed?
      /// </summary>
      private bool m_disposed = false;

      #endregion

      #region construction

      /// <summary>
      /// Constructor.
      /// </summary>
      /// <param name="imageResult">The image result.</param>
      internal FileSystemImageResult(FileSystemInterop.IFileSystemImageResult imageResult)
      {
        m_imageResult = imageResult;
      } // FileSystemImageResult( imageResult )

      #endregion

      #region properties

      /// <summary>
      /// Gets the file system as a stream.
      /// </summary>
      public System.Runtime.InteropServices.ComTypes.IStream Stream
      {
        get { return (System.Runtime.InteropServices.ComTypes.IStream)m_imageResult.ImageStream; }
      } // Stream

      #endregion

      #region IDisposable implementation

      /// <summary>
      /// Performs dispose.
      /// </summary>
      public void Dispose()
      {
        Dispose(true);
        GC.SuppressFinalize(this);
      } // Dispose()


      /// <summary>
      /// Disposes of this object.
      /// </summary>
      /// <param name="disposing">Disposing?</param>
      protected virtual void Dispose(bool disposing)
      {
        if (!m_disposed)
        {
          if (disposing)
          {
            // Dispose managed resources.
          }

          // Dispose unmanaged resources.
          Marshal.ReleaseComObject(m_imageResult);
        }

        m_disposed = true;
      } // Dispose( disposing )

      #endregion

    } // class FileSystemImageResult

    #endregion

    #region fields

    /// <summary>
    /// The file system image.
    /// </summary>
    private FileSystemInterop.MsftFileSystemImage m_fileSystemImage = null;

    /// <summary>
    /// Was this object disposed?
    /// </summary>
    private bool m_disposed = false;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public FileSystemImage()
    {
      m_fileSystemImage = new FileSystemInterop.MsftFileSystemImage();
    } // FileSystemImage()


    /// <summary>
    /// Destructor.
    /// </summary>
    ~FileSystemImage()
    {
      Dispose(true);
    } // ~FileSystemImage()

    #endregion

    #region properties

    /// <summary>
    /// Gets the root directory.
    /// </summary>
    public DirectoryItem Root
    {
      get { return new DirectoryItem(m_fileSystemImage.Root); }
    } // Root


    /// <summary>
    /// Gets or sets the file systems to create.
    /// </summary>
    public FileSystemTypes FileSystemsToCreate
    {
      get { return (FileSystemTypes)m_fileSystemImage.FileSystemsToCreate; }
      set { m_fileSystemImage.FileSystemsToCreate = (FileSystemInterop.FsiFileSystems)value; }
    } // FileSystemsToCreate


    /// <summary>
    /// Gets the number of free media blocks.
    /// </summary>
    public Int64 FreeMediaBlocks
    {
      get { return m_fileSystemImage.FreeMediaBlocks; }
    } // FreeMediaBlocks

    #endregion

    #region methods

    /// <summary>
    /// Choose the image defaults based on the current media in the recorder.
    /// </summary>
    /// <param name="recorder">The recorder.</param>
    public void ChooseImageDefaults(DiscRecorder recorder)
    {
      m_fileSystemImage.ChooseImageDefaults((FileSystemInterop.IDiscRecorder2)recorder.ComRecorder);
    } // ChooseImageDefaults( recorder )


    /// <summary>
    /// Chooses the image defaults based on the media type.
    /// </summary>
    /// <param name="mediaType">The media type to choose defaults.</param>
    public void ChooseImageDefaultsForMediaType(MediaTypes mediaType)
    {
      m_fileSystemImage.ChooseImageDefaultsForMediaType((FileSystemInterop.IMAPI_MEDIA_PHYSICAL_TYPE)mediaType);
    } // ChooseImageDefaultsForMediaType( mediaType )


    /// <summary>
    /// Creates and adds a directory item with the name.
    /// </summary>
    /// <param name="name">The directory name.</param>
    public DirectoryItem CreateDirectory(string name)
    {
      return new DirectoryItem(m_fileSystemImage.CreateDirectoryItem(name));
    } // CreateDirectory( name )


    /// <summary>
    /// Creates the result image to be written.
    /// </summary>
    /// <returns></returns>
    public FileSystemImageResult CreateResultImage()
    {
      return new FileSystemImageResult(m_fileSystemImage.CreateResultImage());
    } // CreateResultImage()

    #endregion

    #region IDisposable implementation

    /// <summary>
    /// Performs dispose.
    /// </summary>
    public void Dispose()
    {
      Dispose(true);
      GC.SuppressFinalize(this);
    } // Dispose()


    /// <summary>
    /// Disposes of this object.
    /// </summary>
    /// <param name="disposing">Disposing?</param>
    protected virtual void Dispose(bool disposing)
    {
      if (!m_disposed)
      {
        if (disposing)
        {
          // Dispose managed resources.
        }

        // Dispose unmanaged resources.
        Marshal.ReleaseComObject(m_fileSystemImage);
      }

      m_disposed = true;
    } // Dispose( disposing )

    #endregion

  } // class FileSystemImage
} // namespace Viatronix.Imapi
