using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Security;
using System.Security.Cryptography;

namespace Viatronix.Launching
{

  #region class AESAlgorithm

  /// <summary>
  /// Wraps an AES Algorithm
  /// </summary>
  internal class AESAlgorithm : IDisposable
  {

    #region fields

     /// <summary>
    /// AES Algrothim object
    /// </summary>
    private RijndaelManaged m_algorithm = new RijndaelManaged();

    /// <summary>
    /// Dispoed flag
    /// </summary>
    private bool m_disposed = false;


    #endregion

    #region constructors/destructors 
    

    /// <summary>
    /// Dispose 
    /// </summary>
    public void Dispose()
    {
      Dispose(true);
    }

    /// <summary>
    /// Dispose
    /// </summary>
    /// <param name="disposing">Disposing flag</param>
    private void Dispose(bool disposing)
    {

      if (! m_disposed)
      {
        Clear();
      }

      m_disposed = true;

    } // Dispose(bool disposing)

    #endregion

    #region methods


    /// <summary>
    /// Clears the algorithm
    /// </summary>
    public void Clear()
    {
      if ( m_algorithm != null )
        m_algorithm.Clear();

      m_algorithm = null;
    }

    /// <summary>
    /// Creates an encrypto
    /// </summary>
    public ICryptoTransform CreateEncryptor()
    {
      return m_algorithm.CreateEncryptor(m_algorithm.Key, m_algorithm.IV);
    } // CreateEncryptor()

    /// <summary>
    /// Creates an encrypto
    /// </summary>
    public ICryptoTransform CreateDecryptor()
    {
      return m_algorithm.CreateDecryptor(m_algorithm.Key, m_algorithm.IV);
    } // CreateDecryptor()

    
    /// <summary>
    /// Crates the AESAlgorithm object
    /// </summary>
    /// <param name="key">Key used to set up algorithm</param>
    /// <returns></returns>
    public static AESAlgorithm Create(Rfc2898DeriveBytes key)
    {
      AESAlgorithm algorithm = new AESAlgorithm();

      algorithm.m_algorithm.Key = key.GetBytes(algorithm.m_algorithm.KeySize / 8);
      algorithm.m_algorithm.IV = key.GetBytes(algorithm.m_algorithm.BlockSize / 8);

      return algorithm;
    }  // Create(Rfc2898DeriveBytes key)


    /// <summary>
    /// Crates the AESAlgorithm object
    /// </summary>
    /// <param name="key">Key used to set up algorithm</param>
    /// <returns></returns>
    public static AESAlgorithm Create(byte [] key)
    {
      AESAlgorithm algorithm = new AESAlgorithm();

      byte[] aKey = new byte[algorithm.m_algorithm.KeySize / 8];
      byte[] aIv = new byte[algorithm.m_algorithm.BlockSize / 8];

      Array.Copy(key, aKey, algorithm.m_algorithm.KeySize / 8);
      Array.Copy(key, aIv, algorithm.m_algorithm.BlockSize / 8);

      algorithm.m_algorithm.Key = aKey;
      algorithm.m_algorithm.IV = aIv;

      return algorithm;
    }  // Create(Rfc2898DeriveBytes key)

  #endregion

  };  // class AESAlgorithm

  #endregion


  #region class Crypto


  /// <summary>
  /// Implements encryption and decryption for internal use
  /// </summary>
  public class Crypto
  {

    #region fields


    /// <summary>
    /// Salt 
    /// </summary>
    private static byte[] _salt = Encoding.ASCII.GetBytes("o6806642kbM7c5");


    /// <summary>
    /// Standard password
    /// </summary>
    private static string m_password = "paxvobiscum!";

    #endregion


    #region methods

    /// <summary>
    /// Encrypts the string using the standard password
    /// </summary>
    /// <param name="planText">Text to be encripted</param>
    /// <returns>Returns the encrypted string</returns>
    public static string Encrypt(string plainText)
    { return Encrypt(plainText, m_password); }

     /// <summary>    
    /// Encrypt the string in the plainText using the sharedSecret string. To decryt you must supply the same string.
    /// </summary>    
    /// <param name="plainText">The text to encrypt.</param>    
    /// <param name="password">A password used to generate a key for encryption.</param>    
    public static string Encrypt(string plainText, string password)    
    {        

      // =============================================
      // Make sure the string to encrypt was supplied
      // =============================================
      if(string.IsNullOrEmpty(plainText))            
        throw new ArgumentNullException("Invalid arguement missing string to encrypt");


      // ======================================
      // Make sure the password was supplied
      // ======================================
      if (string.IsNullOrEmpty(password))
        throw new ArgumentNullException("Invalid arguement missing password");

      // Encrypted string to return       
      string cipherText = null;                        

      
        
      // AES Algorithm
      AESAlgorithm alg = AESAlgorithm.Create(new Rfc2898DeriveBytes(password, _salt));
                
      // Mempry stream
      using (MemoryStream msEncrypt = new MemoryStream())            
      {                

        // Encryption stream
        using (CryptoStream csEncrypt = new CryptoStream(msEncrypt, alg.CreateEncryptor(), CryptoStreamMode.Write))               
        {                   
 
          // Stream writer
          using (StreamWriter swEncrypt = new StreamWriter(csEncrypt))                    
          {                                      
            // Write the plain text to be encrypted
            swEncrypt.Write(plainText);                    
          } // END ... Using StreamWriter
 
        } // END ... Using CryptoStream

        // Convert to Base64
        cipherText = Convert.ToBase64String(msEncrypt.ToArray());            

      } // END ... Using MemoryStream       
       
      
      // Return the encrypted bytes from the memory stream.        
      return cipherText;    
    
    }

    /// <summary>    
    /// Encrypt the string in the plainText using the sharedSecret string. To decryt you must supply the same string.
    /// </summary>    
    /// <param name="plainText">The text to encrypt.</param>    
    /// <param name="password">A password used to generate a key for encryption.</param>    
    public static string Encrypt(string plainText, byte [] key)
    {

      // =============================================
      // Make sure the string to encrypt was supplied
      // =============================================
      if (key.Length < 32)
        throw new ArgumentNullException("Invalid arguement key is too short");


      // Encrypted string to return       
      string cipherText = null;



      // AES Algorithm
      AESAlgorithm alg = AESAlgorithm.Create(key);

      // Mempry stream
      using (MemoryStream msEncrypt = new MemoryStream())
      {

        // Encryption stream
        using (CryptoStream csEncrypt = new CryptoStream(msEncrypt, alg.CreateEncryptor(), CryptoStreamMode.Write))
        {

          // Stream writer
          using (StreamWriter swEncrypt = new StreamWriter(csEncrypt))
          {
            // Write the plain text to be encrypted
            swEncrypt.Write(plainText);
          } // END ... Using StreamWriter

        } // END ... Using CryptoStream

        // Convert to Base64
        cipherText = Convert.ToBase64String(msEncrypt.ToArray());

      } // END ... Using MemoryStream       


      // Return the encrypted bytes from the memory stream.        
      return cipherText;

    }


    /// <summary>
    /// Decrypt using the standard password
    /// </summary>
    /// <param name="cipherText">String to decrypt</param>
    /// <returns>Decrypted string</returns>
    public static string Decrypt(string cipherText)
    { return Decrypt(cipherText, m_password);  }
    
    /// <summary>    
    /// Decrypt the given string supplied. The password must match the password used to encrypt
    /// </summary>    
    /// <param name="cipherText">The text to decrypt.</param>    
    /// <param name="password">A password used to generate a key for decryption.</param>
    /// <returns>Decrypted string</returns>
    public static string Decrypt(string cipherText, string password)    
    {

      // =============================================
      // Make sure the string to encrypt was supplied
      // =============================================
      if (string.IsNullOrEmpty(cipherText))
        throw new ArgumentNullException("Invalid arguement missing string to decrypt");


      // ======================================
      // Make sure the password was supplied
      // ======================================
      if (string.IsNullOrEmpty(password))
        throw new ArgumentNullException("Invalid arguement missing password");        

      // Declare the string used to hold the decrypted text.        
      string plaintext = null;        
      

      // AES Algorithm
      AESAlgorithm alg = AESAlgorithm.Create(new Rfc2898DeriveBytes(password, _salt));
                
      // Create the streams used for decryption.                            
      byte[] bytes = Convert.FromBase64String(cipherText);            
        
      // MemoryStream
      using (MemoryStream msDecrypt = new MemoryStream(bytes))            
      {

        // CrytoStream
        using (CryptoStream csDecrypt = new CryptoStream(msDecrypt, alg.CreateDecryptor(), CryptoStreamMode.Read))                
        {

          // StreamReader
          using (StreamReader srDecrypt = new StreamReader(csDecrypt))
          {
            // Read the decrypted bytes from the decrypting stream and place them in a string.                        
            plaintext = srDecrypt.ReadToEnd();
          } // END ... Using StreanReader

        } // END ... Using CryptoStream

      } // END ... Using MemoryStream       
       
      // Returns the decrypted string
      return plaintext;

    } // Decrypt(string cipherText, string password)


    /// <summary>    
    /// Decrypt the given string supplied. The password must match the password used to encrypt
    /// </summary>    
    /// <param name="cipherText">The text to decrypt.</param>    
    /// <param name="password">A password used to generate a key for decryption.</param>
    /// <returns>Decrypted string</returns>
    public static string Decrypt(string cipherText, byte [] key)
    {

      // =============================================
      // Make sure the string to encrypt was supplied
      // =============================================
      if (key.Length < 32 )
        throw new ArgumentNullException("Invalid arguement missing key is too short");



      // Declare the string used to hold the decrypted text.        
      string plaintext = null;


      // AES Algorithm
      AESAlgorithm alg = AESAlgorithm.Create(key);

      // Create the streams used for decryption.                            
      byte[] bytes = Convert.FromBase64String(cipherText);

      // MemoryStream
      using (MemoryStream msDecrypt = new MemoryStream(bytes))
      {

        // CrytoStream
        using (CryptoStream csDecrypt = new CryptoStream(msDecrypt, alg.CreateDecryptor(), CryptoStreamMode.Read))
        {

          // StreamReader
          using (StreamReader srDecrypt = new StreamReader(csDecrypt))
          {
            // Read the decrypted bytes from the decrypting stream and place them in a string.                        
            plaintext = srDecrypt.ReadToEnd();
          } // END ... Using StreanReader

        } // END ... Using CryptoStream

      } // END ... Using MemoryStream       

      // Returns the decrypted string
      return plaintext;

    } // Decrypt(string cipherText, string password)

    #endregion



  }; // class Crypto

  #endregion

} // namespace Viatronix.Encryption
    
