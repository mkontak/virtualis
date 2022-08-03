using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Security;
using System.Security.Cryptography;


namespace Viatronix.Encryption
{

  #region class AESAlgorithm

  /// <summary>
  /// Wraps an AES Algorithm
  /// </summary>
  internal class AESAlgorithm : IDisposable
  {

    #region fields

    /// <summary>
    /// AES provider
    /// </summary>
    private AesCryptoServiceProvider m_provider = new AesCryptoServiceProvider();

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
      if (m_provider != null)
        m_provider.Clear();

      m_provider = null;
    }

    /// <summary>
    /// Creates an encrypto
    /// </summary>
    public ICryptoTransform CreateEncryptor()
    {
      return m_provider.CreateEncryptor(m_provider.Key, m_provider.IV);
    } // CreateEncryptor()

    /// <summary>
    /// Creates an encrypto
    /// </summary>
    public ICryptoTransform CreateDecryptor()
    {
      return m_provider.CreateDecryptor(m_provider.Key, m_provider.IV);
    } // CreateDecryptor()

    
    /// <summary>
    /// Crates the AESAlgorithm object
    /// </summary>
    /// <param name="key">Key used to set up algorithm</param>
    /// <returns></returns>
    public static AESAlgorithm Create(Rfc2898DeriveBytes key)
    {
      AESAlgorithm algorithm = new AESAlgorithm();

      algorithm.m_provider.Key = key.GetBytes(algorithm.m_provider.KeySize / 8);
      algorithm.m_provider.IV = key.GetBytes(algorithm.m_provider.BlockSize / 8);

      return algorithm;
    }  // Create(Rfc2898DeriveBytes key)

    /// <summary>
    /// Crates the AESAlgorithm object
    /// </summary>
    /// <param name="key">Key used to set up algorithm</param>
    /// <returns></returns>
    public static AESAlgorithm Create(byte[] keyhash)
    {
      AESAlgorithm algorithm = new AESAlgorithm();

      int keysize = algorithm.m_provider.KeySize / 8;
      byte [] key = new byte[keysize];
      Array.Copy(keyhash, key, keysize);
      algorithm.m_provider.Key = key;

      int ivsize = algorithm.m_provider.BlockSize / 8;
      byte [] iv = new byte[ivsize];
      Array.Copy(keyhash, keysize, iv, 0, ivsize);
      algorithm.m_provider.IV = iv;

      return algorithm;
    }  // Create(Rfc2898DeriveBytes key)

 #endregion


    #region properties

    /// <summary>
    /// Gets the provider
    /// </summary>
    public AesCryptoServiceProvider Provider
    { get { return m_provider; } }

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
    /// Key Hash (Default)
    /// </summary>
    private readonly static byte[] m_keyhash = new byte[] { 0x17, 0xee, 0x25, 0xb8, 0x7b, 0x24, 0x7e, 0x4b, 0x64, 0x1a, 0xd5, 0x94, 0x61, 0xd6, 0x57, 0x42, 0x25, 0xb5, 
                                                            0x1e, 0x28, 0x95, 0x5f, 0x38, 0x9e, 0xd1, 0x74, 0x8b, 0x7d, 0x2b, 0x3f, 0x91, 0xdf, 0x29, 0x18, 0x64, 0xf2, 
                                                            0x80, 0xfd, 0x4b, 0xdd, 0xe0, 0x1d, 0xcb, 0xe1, 0x91, 0x9c, 0x5e, 0x4e };

    /// <summary>
    /// Salt  (Default)
    /// </summary>
    private readonly static byte[] m_salt = new byte [] { 0x6F,0x36,0x38,0x30,0x36,0x36,0x34,0x32,0x6B,0x62,0x4D,0x37,0x63,0x35 };                              // o6806642kbM7c5


    /// <summary>
    /// Password (Default)
    /// </summary>
    private readonly static byte[] m_password = new byte[] { 0x65, 0x70, 0x6C, 0x75, 0x72, 0x69, 0x62, 0x75, 0x73, 0x55, 0x6E, 0x75, 0x6D, 0x39, 0x39, 0x21 }; // epluribusUnum99!

    #endregion


    #region methods

    /// <summary>
    /// Encrypts the string using the standard password
    /// </summary>
    /// <param name="planText">Text to be encripted</param>
    /// <returns>Returns the encrypted string</returns>
    public static string Encrypt(string plainText)
    { return Encrypt(plainText, Encoding.ASCII.GetString(m_password)); }

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
      if (password.Length == 0)
        throw new ArgumentNullException("Invalid arguement missing password");

      // Encrypted string to return       
      string cipherText = null;                        

      
        
      // AES Algorithm
      AESAlgorithm alg = AESAlgorithm.Create(m_keyhash);
                
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
    { return Decrypt(cipherText, Encoding.ASCII.GetString(m_password));  }
    
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
        throw new ArgumentNullException("Invalid argument missing string to decrypt");


      // ======================================
      // Make sure the password was supplied
      // ======================================
      if (string.IsNullOrEmpty(password))
        throw new ArgumentNullException("Invalid argument missing password");        

      // Declare the string used to hold the decrypted text.        
      string plaintext = null;        
      

      // AES Algorithm
      AESAlgorithm alg = AESAlgorithm.Create(new Rfc2898DeriveBytes(password, m_salt));
                
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

    } // Decrypt(string cipherText, byte key[])


    public static byte[] GenerateKey()
    { return GenerateKey(System.Text.ASCIIEncoding.ASCII.GetString(m_password), m_salt); }

    public static byte [] GenerateKey(string password, string salt)
    { return GenerateKey(password, System.Text.ASCIIEncoding.ASCII.GetBytes(salt)); }


    public static byte[] GenerateKey(string password, byte [] salt)
    {
      AESAlgorithm algorithm = new AESAlgorithm();

      Rfc2898DeriveBytes rfc = new Rfc2898DeriveBytes(password, salt);


      int keysize = algorithm.Provider.KeySize / 8;
      int ivsize = algorithm.Provider.BlockSize / 8;

      byte[] key = new byte[keysize + ivsize];

      Array.Copy(rfc.GetBytes(keysize), key, keysize);
      Array.Copy(rfc.GetBytes(ivsize), 0, key, keysize, ivsize);


      return key;
    }




    #endregion



  }; // class Crypto

  #endregion

} // namespace Viatronix.Encryption
    
