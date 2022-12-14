/* C++ section */
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>

/* C section */
#include <cstdio>
#include <cstring>

/* OSSL section */
#include <openssl/crypto.h>
#include <openssl/engine.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/obj_mac.h>
#include <openssl/hmac.h>
#include <openssl/err.h>

/* STL section */
#include <vector>
#include <algorithm>
#include <string>

/* function to encrypt using OpenSSL + Gost28147-89 algorithm */
int encrypt(unsigned char* text, int text_len, unsigned char* key, unsigned char* cipher)
    {
    ENGINE* engine = ENGINE_by_id("gost");   
    
    if ( !engine ){
      perror("ENGINE_by_id() failed");
      exit(-1);
    }
    
    int cipher_len = 0;
    int len = 0;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx){
      perror("EVP_CIPHER_CTX_new() failed");
      exit(-1);
    }
    const EVP_CIPHER* ciph = EVP_get_cipherbyname("gost89");
    
    if (! ciph ){
      perror("EVP_get_cipherbyname() failed");
      exit(-1);
    }

    unsigned char iv[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    if ( !EVP_EncryptInit_ex(ctx, ciph, engine, key, iv)){
        int err = ERR_get_error();
        ERR_load_crypto_strings();
        std::cerr << ERR_error_string(err, NULL) << std::endl;
        perror("EVP_EncryptInit_ex() failed");
        exit(-1);
    }
    
    if ( !EVP_EncryptUpdate(ctx, cipher, &len, text, text_len)){
      perror("EVP_EncryptUpdate() failed");
      exit(-1);
    }
    
    cipher_len += len;

    if ( !EVP_EncryptFinal_ex(ctx, cipher + len, &len)){
      perror("EVP_EncryptFinal_ex() failed");
      exit(-1);
    }

    cipher_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    return cipher_len;

}

/* make bytearray from file contents */
static std::vector<char> readAllBytes(const std::string& filename){

        std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
        std::ifstream::pos_type pos = ifs.tellg();

        if (pos == 0) {
            return std::vector<char>{};
        }

        std::vector<char>  result(pos);

        ifs.seekg(0, std::ios::beg);
        ifs.read(&result[0], pos);

        return result;
}

int main(int argc, char** argv){
  OPENSSL_add_all_algorithms_conf(); 
  if (argc == 1){
    std::cerr << "Nothing provided\n";
    return -1;
  }
  
  std::vector<std::string> arguments (argv + 1, argv + argc);
  unsigned char* key = (unsigned char*) "abobaboba";

  for (auto arg : arguments){
    if (!std::filesystem::is_directory("./" + arg) && std::filesystem::exists("./" + arg)){
        std::ofstream out(arg + "_cipher", std::ofstream::out);
        if (out.is_open()){
          std::vector<char> res_bytes = readAllBytes("./" + arg);
          unsigned char* text = (unsigned char*)res_bytes.data();
          int text_len = strlen((const char*) text);
          unsigned char cipher[1000];
          int cipher_len = encrypt(text, text_len, (unsigned char*) key, cipher);
          for (int i = 0; i < cipher_len; ++i){
            out << std::hex << std::setw(2) << std::setfill('0') << cipher[i] << " ";
          }
          out << std::endl;
          out << "filename: " << arg << std::endl;
          out << "KEY: " <<  key << std::endl;
        }
    
        out.close();
  
    }
  
  }
  return 0;
}

