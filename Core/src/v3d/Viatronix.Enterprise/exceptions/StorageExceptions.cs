using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Enterprise
{

  #region MissingInvalidDicomFieldException class

  /// <summary>
  /// Missing or invalid dicom field.
  /// </summary>
  public class MissingInvalidDicomFieldException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public MissingInvalidDicomFieldException(string message) : base(message, ErrorCodes.MissingInvalidDicomField)
    {
    } // MissingInvalidDicomFieldException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public MissingInvalidDicomFieldException(string message, Exception innerException) : base(message, innerException, ErrorCodes.MissingInvalidDicomField)
    {
    } // MissingInvalidDicomFieldException( message, innerException )

    #endregion

  } // class MissingInvalidDicomFieldException

  #endregion

  #region FailedSerializeException class

  /// <summary>
  /// Failed to serialize the specified entity.
  /// </summary>
  public class FailedSerializeException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public FailedSerializeException(string message) : base(message, ErrorCodes.FailedSerialize)
    {
    } // FailedSerializeException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public FailedSerializeException(string message, Exception innerException) : base(message, innerException, ErrorCodes.FailedSerialize)
    {
    } // FailedSerializeException( message, innerException )

    #endregion

  } // class FailedSerializeException

  #endregion

  #region DeleteDirectoryFailedException class

  /// <summary>
  /// Failed to delete the directory.
  /// </summary>
  public class DeleteDirectoryFailedException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public DeleteDirectoryFailedException(string message) : base(message, ErrorCodes.DeleteDirectoryFailed)
    {
    } // DeleteDirectoryFailedException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public DeleteDirectoryFailedException(string message, Exception innerException) : base(message, innerException, ErrorCodes.DeleteDirectoryFailed)
    {
    } // DeleteDirectoryFailedException( message, innerException )

    #endregion

  } // class DeleteDirectoryFailedException

  #endregion

  #region DeleteFileFailedException class

  /// <summary>
  /// Delete file failed.
  /// </summary>
  public class DeleteFileFailedException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public DeleteFileFailedException(string message) : base(message, ErrorCodes.DeleteFileFailed)
    {
    } // DeleteFileFailedException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public DeleteFileFailedException(string message, Exception innerException) : base(message, innerException, ErrorCodes.DeleteFileFailed)
    {
    } // DeleteFileFailedException( message, innerException )

    #endregion

  } // class DeleteFileFailedException

  #endregion

  #region SeriesContainsReferenceException class

  /// <summary>
  /// Cannot delete, series contains references.
  /// </summary>
  public class SeriesContainsReferenceException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public SeriesContainsReferenceException(string message) : base(message, ErrorCodes.SeriesContainsReference)
    {
    } // SeriesContainsReferenceException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public SeriesContainsReferenceException(string message, Exception innerException) : base(message, innerException, ErrorCodes.SeriesContainsReference)
    {
    } // SeriesContainsReferenceException( message, innerException )

    #endregion

  } // class SeriesContainsReferenceException

  #endregion

  #region SeriesContainsDependentsException class

  /// <summary>
  /// Series has dependents.
  /// </summary>
  public class SeriesContainsDependentsException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public SeriesContainsDependentsException(string message) : base(message, ErrorCodes.SeriesContainsDependents)
    {
    } // SeriesContainsDependentsException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public SeriesContainsDependentsException(string message, Exception innerException) : base(message, innerException, ErrorCodes.SeriesContainsDependents)
    {
    } // SeriesContainsDependentsException( message, innerException )

    #endregion

  } // class SeriesContainsDependentsException

  #endregion

  #region SeriesProcessingException class

  /// <summary>
  /// The series is currently processing.
  /// </summary>
  public class SeriesProcessingException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public SeriesProcessingException(string message) : base(message, ErrorCodes.SeriesProcessing)
    {
    } // SeriesProcessingException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public SeriesProcessingException(string message, Exception innerException) : base(message, innerException, ErrorCodes.SeriesProcessing)
    {
    } // SeriesProcessingException( message, innerException )

    #endregion

  } // class SeriesProcessingException

  #endregion

  #region SeriesContainsNoImagesException class

  /// <summary>
  /// The series contains no dicom images.
  /// </summary>
  public class SeriesContainsNoImagesException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public SeriesContainsNoImagesException(string message) : base(message, ErrorCodes.SeriesContainsNoImages)
    {
    } // SeriesContainsNoImagesException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public SeriesContainsNoImagesException(string message, Exception innerException) : base(message, innerException, ErrorCodes.SeriesContainsNoImages)
    {
    } // SeriesContainsNoImagesException( message, innerException )

    #endregion

  } // class SeriesContainsNoImagesException

  #endregion

  #region NoLaunchableDataFoundException class

  /// <summary>
  /// No launchable data was found.
  /// </summary>
  public class NoLaunchableDataFoundException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public NoLaunchableDataFoundException(string message) : base(message, ErrorCodes.NoLaunchableDataFound)
    {
    } // NoLaunchableDataFoundException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public NoLaunchableDataFoundException(string message, Exception innerException) : base(message, innerException, ErrorCodes.NoLaunchableDataFound)
    {
    } // NoLaunchableDataFoundException( message, innerException )

    #endregion

  } // class NoLaunchableDataFoundException

  #endregion

  #region CannotLaunchWhileProcessingException class

  /// <summary>
  /// Cannot launch because processing is occuring.
  /// </summary>
  public class CannotLaunchWhileProcessingException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public CannotLaunchWhileProcessingException(string message) : base(message, ErrorCodes.CannotLaunchWhileProcessing)
    {
    } // CannotLaunchWhileProcessingException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public CannotLaunchWhileProcessingException(string message, Exception innerException) : base(message, innerException, ErrorCodes.CannotLaunchWhileProcessing)
    {
    } // CannotLaunchWhileProcessingException( message, innerException )

    #endregion

  } // class CannotLaunchWhileProcessingException

  #endregion

  #region CannotLaunchSpecifiedTypeException class

  /// <summary>
  /// Cannot launch the specified type.
  /// </summary>
  public class CannotLaunchSpecifiedTypeException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public CannotLaunchSpecifiedTypeException(string message) : base(message, ErrorCodes.CannotLaunchSpecifiedType)
    {
    } // CannotLaunchSpecifiedTypeException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public CannotLaunchSpecifiedTypeException(string message, Exception innerException) : base(message, innerException, ErrorCodes.CannotLaunchSpecifiedType)
    {
    } // CannotLaunchSpecifiedTypeException( message, innerException )

    #endregion

  } // class CannotLaunchSpecifiedTypeException

  #endregion

  #region CannotLaunchWhileProcessingWEntityException class

  /// <summary>
  /// Cannot launch the entity while processing is occurring.
  /// </summary>
  public class CannotLaunchWhileProcessingWEntityException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public CannotLaunchWhileProcessingWEntityException(string message) : base(message, ErrorCodes.CannotLaunchWhileProcessingWEntity)
    {
    } // CannotLaunchWhileProcessingWEntityException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public CannotLaunchWhileProcessingWEntityException(string message, Exception innerException) : base(message, innerException, ErrorCodes.CannotLaunchWhileProcessingWEntity)
    {
    } // CannotLaunchWhileProcessingWEntityException( message, innerException )

    #endregion

  } // class CannotLaunchWhileProcessingWEntityException

  #endregion

  #region CannotLaunchFailedSeriesException class

  /// <summary>
  /// Cannot launch a failed series.
  /// </summary>
  public class CannotLaunchFailedSeriesException : EntityException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public CannotLaunchFailedSeriesException(string message) : base(message, ErrorCodes.CannotLaunchFailedSeries)
    {
    } // CannotLaunchFailedSeriesException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public CannotLaunchFailedSeriesException(string message, Exception innerException) : base(message, innerException, ErrorCodes.CannotLaunchFailedSeries)
    {
    } // CannotLaunchFailedSeriesException( message, innerException )

    #endregion

  } // class CannotLaunchFailedSeriesException

  #endregion

  #region CannotLaunchMultiSeriesException class

  /// <summary>
  /// Cannot launch multiple series with the same dicom root.
  /// </summary>
  public class CannotLaunchMultiSeriesLinkedToDicomException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public CannotLaunchMultiSeriesLinkedToDicomException(string message) : base(message, ErrorCodes.CannotLaunchMultiSeriesLinkedToDicom)
    {
    } // CannotLaunchMultiSeriesLinkedToDicomException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public CannotLaunchMultiSeriesLinkedToDicomException(string message, Exception innerException) : base(message, innerException, ErrorCodes.CannotLaunchMultiSeriesLinkedToDicom)
    {
    } // CannotLaunchMultiSeriesLinkedToDicomException( message, innerException )

    #endregion

  } // class CannotLaunchMultiSeriesLinkedToDicomException

  #endregion

  #region SessionDoesNotMatchApplicationException class

  /// <summary>
  /// The session does not match the application.
  /// </summary>
  public class SessionDoesNotMatchApplicationException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public SessionDoesNotMatchApplicationException(string message) : base(message, ErrorCodes.SessionDoesNotMatchApplication)
    {
    } // SessionDoesNotMatchApplicationException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public SessionDoesNotMatchApplicationException(string message, Exception innerException) : base(message, innerException, ErrorCodes.SessionDoesNotMatchApplication)
    {
    } // SessionDoesNotMatchApplicationException( message, innerException )

    #endregion

  } // class SessionDoesNotMatchApplicationException

  #endregion

  #region SeriesHasNotBeenProcessedForAppException class

  /// <summary>
  /// Series has not been processed for the specified application.
  /// </summary>
  public class SeriesHasNotBeenProcessedForAppException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public SeriesHasNotBeenProcessedForAppException(string message) : base(message, ErrorCodes.SeriesHasNotBeenProcessedForApp)
    {
    } // SeriesHasNotBeenProcessedForAppException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public SeriesHasNotBeenProcessedForAppException(string message, Exception innerException) : base(message, innerException, ErrorCodes.SeriesHasNotBeenProcessedForApp)
    {
    } // SeriesHasNotBeenProcessedForAppException( message, innerException )

    #endregion

  } // class SeriesHasNotBeenProcessedForAppException

  #endregion

  #region CannotSpecifyXAndYTogetherException class

  /// <summary>
  /// Cannot specify x and y together.
  /// </summary>
  public class CannotSpecifyXAndYTogetherException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public CannotSpecifyXAndYTogetherException(string message) : base(message, ErrorCodes.CannotSpecifyXAndYTogether)
    {
    } // CannotSpecifyXAndYTogetherException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public CannotSpecifyXAndYTogetherException(string message, Exception innerException) : base(message, innerException, ErrorCodes.CannotSpecifyXAndYTogether)
    {
    } // CannotSpecifyXAndYTogetherException( message, innerException )

    #endregion

  } // class CannotSpecifyXAndYTogetherException

  #endregion

  #region CannotSpecifyMoreThanXException class

  /// <summary>
  /// Cannot specify more than x.
  /// </summary>
  public class CannotSpecifyMoreThanXException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    public CannotSpecifyMoreThanXException(string message) : base(message, ErrorCodes.CannotSpecifyMoreThanX)
    {
    } // CannotSpecifyMoreThanXException( message )


    /// <summary>
    /// Constructor.
    /// <summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    public CannotSpecifyMoreThanXException(string message, Exception innerException) : base(message, innerException, ErrorCodes.CannotSpecifyMoreThanX)
    {
    } // CannotSpecifyMoreThanXException( message, innerException )

    #endregion

  } // class CannotSpecifyMoreThanException

  #endregion
  
} // namespace Viatronix.Enterprise
