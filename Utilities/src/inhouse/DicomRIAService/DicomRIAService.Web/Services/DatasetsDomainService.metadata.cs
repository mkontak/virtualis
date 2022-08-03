
namespace DicomRIAService.Web.Models
{
  using System;
  using System.Collections.Generic;
  using System.ComponentModel;
  using System.ComponentModel.DataAnnotations;
  using System.Linq;
  using System.ServiceModel.DomainServices.Hosting;
  using System.ServiceModel.DomainServices.Server;


  // The MetadataTypeAttribute identifies DatasetMetadata as the class
  // that carries additional metadata for the Dataset class.
  [MetadataTypeAttribute( typeof( Dataset.DatasetMetadata ) )]
  public partial class Dataset
  {

    // This class allows you to attach custom attributes to properties
    // of the Dataset class.
    //
    // For example, the following marks the Xyz property as a
    // required property and specifies the format for valid values:
    //    [Required]
    //    [RegularExpression("[A-Z][A-Za-z0-9]*")]
    //    [StringLength(32)]
    //    public string Xyz { get; set; }
    internal sealed class DatasetMetadata
    {

      // Metadata classes are not meant to be instantiated.
      private DatasetMetadata()
      {
      }

      public string app
      {
        get;
        set;
      }

      public bool cpd
      {
        get;
        set;
      }

      public string desc
      {
        get;
        set;
      }

      public int icnt
      {
        get;
        set;
      }

      public string man
      {
        get;
        set;
      }

      public string mod
      {
        get;
        set;
      }

      public string model
      {
        get;
        set;
      }

      public string name
      {
        get;
        set;
      }

      public string ort
      {
        get;
        set;
      }

      public bool qa
      {
        get;
        set;
      }

      public string type
      {
        get;
        set;
      }

      public string uid
      {
        get;
        set;
      }
    }
  }

  // The MetadataTypeAttribute identifies ProgressMetadata as the class
  // that carries additional metadata for the Progress class.
  [MetadataTypeAttribute( typeof( Progress.ProgressMetadata ) )]
  public partial class Progress
  {

    // This class allows you to attach custom attributes to properties
    // of the Progress class.
    //
    // For example, the following marks the Xyz property as a
    // required property and specifies the format for valid values:
    //    [Required]
    //    [RegularExpression("[A-Z][A-Za-z0-9]*")]
    //    [StringLength(32)]
    //    public string Xyz { get; set; }
    internal sealed class ProgressMetadata
    {

      // Metadata classes are not meant to be instantiated.
      private ProgressMetadata()
      {
      }

      public string aetitle
      {
        get;
        set;
      }

      public int port
      {
        get;
        set;
      }

      public int progress1
      {
        get;
        set;
      }

      public int total
      {
        get;
        set;
      }

      public string uid
      {
        get;
        set;
      }
    }
  }
}
