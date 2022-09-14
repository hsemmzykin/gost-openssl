#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>

#include <cstdio>
#include <cstring>

#include <openssl/crypto.h>
#include <openssl/engine.h>
#include <openssl/conf.h>
#include <openssl/evp.h>


#include <vector>
#include <algorithm>
#include <string>


int encrypt(unsigned char* text, int text_len, unsigned char* key, unsigned char* cipher)
    {
    
    int cipher_len = 0;
    int len = 0;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx){
      perror("EVP_CIPHER_CTX_new() failed");
      exit(-1);
    }
    
    EVP_CIPHER* ciph = (EVP_CIPHER*)EVP_get_cipherbyname("GOST2001-GOST89-GOST89");
    if ( !EVP_EncryptInit_ex(ctx, ciph, NULL, key, NULL)){
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
  if (argc == 1){
    std::cerr << "Nothing provided\n";
    return -1;
  }
  std::vector<std::string> arguments (argv + 1, argv + argc);
  unsigned char* key = (unsigned char*) "MARTINALEXEEVICH1337";

  for (auto arg : arguments){
    if (!std::filesystem::is_directory("./" + arg) && std::filesystem::exists("./" + arg)){
        std::ofstream out(arg + "_cipher", std::ofstream::out);
        if (out.is_open()){
          std::vector<char> res_bytes = readAllBytes("./" + arg);
          unsigned char* text = (unsigned char*)res_bytes.data();
          int text_len = strlen((const char*) text);
          unsigned char cipher[64];
          int cipher_len = encrypt(text, text_len, key, cipher);
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

