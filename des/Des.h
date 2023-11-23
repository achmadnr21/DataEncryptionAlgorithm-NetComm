#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string>
#include <stdint.h>
#include <string.h>

typedef uint64_t ints;

class DES{
    private:
    ints key;
    ints keys[16];

    ints* plaintext_bin;
    ints  plaintext_block;
    
    public:
    DES(){}
    
    DES(std::string Key){
        this->key = key_str2ints(Key);
        key_16generate(key);
    }
    DES(ints Key){
        this->key = Key;
        key_16generate(key);
    }

    // Get plaintext_bin
    ints* getPlaintextBin(){
        return plaintext_bin;
    }
    ints getPlaintextBlock(){
        return plaintext_block;
    }


    // Set plaintext_bin
    void setPlaintextBin(ints* plaintext_bin, ints plaintext_block){
        this->plaintext_bin = plaintext_bin;
        this->plaintext_block = plaintext_block;
    }
    // DES Setter For Key
    void setKeyStr(std::string Key){
        this->key = key_str2ints(Key);
        key_16generate(key);
    }
    void setKeyInt64(ints Key){
        this->key = Key;
        key_16generate(key);
    }
    void setKeyStrHex(std::string Key){
        ints keyz = std::stoull(Key, 0, 16);
        ints keysize = Key.size();
        ints lefts = 64-keysize % 16 * 4;
        keyz <<= lefts;
        setKeyInt64(keyz);
    }
    
    ints key_str2ints(std::string Keystr) {
        if (Keystr.size() > 8) {
            std::cout << "[Warning] :: Your Key is more than 8 characters! We will only take the first 8 characters!" << std::endl;
        }
        ints newkey = 0;
        for (int i = 0; i < 8; i++) {
            newkey <<= 8;
            if(i+1 > Keystr.size())continue;
            newkey |= static_cast<int>(Keystr[i]);
        }
        return newkey;
    }
    
    // DES encrypt a data
    // void encrypt(std::string plaintext){
    //     //setup preprocess the data
    //     setup_64block(plaintext);
    //     initial_permutation();
    //     feistel_encipher();
    //     final_permutation();
    //     print_result_hex();

    // }
    void encrypt(std::string plaintext){
        // printf("data pertama plain\t: %lu\n", *(plaintext_bin));
        //setup preprocess the data
        setup_64block(plaintext);
        initial_permutation();
        feistel_encipher();
        final_permutation();
    }
    void decrypt(ints dataInt){
        //setup preprocess the data
        
        dataInt = sub_initial_permutation(dataInt);
        dataInt = sub_feistel_decipher(dataInt);
        dataInt = sub_final_permutation(dataInt);
        print_result_asciiInt(dataInt);
        
    }
    
    void encrypt_strhex(std::string hextext){
        //setup preprocess the data
        setup_64block_strhex(hextext);
        initial_permutation();
        feistel_encipher();
        final_permutation();
        print_result_hex();
    }
    // DES decrypt
    void decrypt_strhex(std::string hextext){
        //setup preprocess the data
        setup_64block_strhex(hextext);
        initial_permutation();
        feistel_decipher();
        final_permutation();
        print_result_hex();
        print_result_ascii();
        
    }
    // Setting Up the text and store it 64 bit each
    void setup_64block(std::string plaintext){
      ints plaintext_size = plaintext.size();
      this->plaintext_block=plaintext_size/8; 
      (plaintext_size>(plaintext_block*8))?(this->plaintext_block)++:0;
      free(plaintext_bin);
      plaintext_bin = (ints*)malloc(plaintext_block*sizeof(ints));
      ints block_state = 0;
      for(int i = 0; i<plaintext_size; i++){
          if(i>0 && i%8==0) block_state++;
          *(plaintext_bin + block_state) <<= (8);
          *(plaintext_bin + block_state)|=plaintext[i];
      }
      if(plaintext_size%8 !=0){
          *(plaintext_bin+(plaintext_block-1)) <<= 8*(8 - plaintext_size%8);
      }
      
    }
    
    
    void setup_64block_strhex(std::string hextext){
        ints hextext_size = hextext.size();
        this->plaintext_block=hextext_size/16;
        (hextext_size>(plaintext_block*16))?(this->plaintext_block)++:0;
        free(plaintext_bin);
        plaintext_bin = (ints*)malloc(plaintext_block*sizeof(ints));
        ints block_state = 0;
        for (size_t i = 0; i < hextext_size; i += 16) {
            std::string substring = hextext.substr(i, 16);
            *(plaintext_bin + block_state) = std::stoull(substring,0,16);
            block_state++;
        }
        ints lefts = 64-hextext_size % 16 * 4;
        *(plaintext_bin + block_state - 1) = *(plaintext_bin + block_state - 1) << (lefts);
    }
    
    
    
    // DES INITIAL PERMUTATION
    void initial_permutation(){
        for(int i = 0; i<plaintext_block; i++){
            *(plaintext_bin+i) = sub_initial_permutation(*(plaintext_bin+i));
        }
    }
    ints sub_initial_permutation(ints data){
        ints data_size = 64;
        ints newdata = 0;
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                data_size--;
                ints bindata = (data >> (64-init_perm[i][j])) &0b1;
                newdata = newdata|(bindata<<data_size);
            }
        }
        return newdata;
    }

    // DES Key Generator
    ints key_permute1(ints data){
        ints data_size = 64;
        ints newdata = 0;
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 7; j++){
                data_size--;
                // printf("%u",(uint8_t)(data >> (64-pc1[i][j])) &0b1);
                ints bindata = (data >> (64-pc1[i][j])) &0b1;
                newdata = newdata|(bindata<<data_size);
            }//printf(" ");
        }
        return newdata;
    }
    ints key_permute2(ints data){
        ints data_size = 64;
        ints newdata = 0;
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 6; j++){
                data_size--;
                // printf("%u",(uint8_t)(data >> (64-pc1[i][j])) &0b1);
                ints bindata = (data >> (64-pc2[i][j])) &0b1;
                newdata = newdata|(bindata<<data_size);
            }//printf(" ");
        }
        return newdata;
    }
    void key_16generate(ints key){
        memset(keys, 0, sizeof(keys));
        ints keyp1 = key_permute1(key);
        ints C, D;
        C = (keyp1>>(64-28))<<(64-28);
        D = (keyp1>>(64-56))<<(64-28);
        for(int i = 0; i<16; i++){
            ints numleft = (i < 2 || (i-1)%7 == 0)?1:2;
            for(int j = 0; j < numleft; j++){
                C=circular_leftShift(C, 28);
                D=circular_leftShift(D, 28);
            }
            keys[i] = key_permute2(C | D>>28);
        }
    }

    // DES Circular Left Shift
    
    ints cleanup(ints data, ints bits){
        ints mask = 0;
        for(int i = 0; i<bits; i++){
            mask|= (0b1 << i);
        }
        // printbin(mask, 64);
        return data&mask;
    }
    ints circular_leftShift(ints data, ints bits){
        ints maxbit = sizeof(data)*8;
        if(bits > maxbit){
            printf("%lu is bigger than the max block size %lu. We did not process the data.\n", bits, sizeof(data)*8);
            return data;
        }
        data >>= (maxbit-bits);
        data = cleanup(data, bits);
        
        ints lefts = (data >> (bits-1))&0b1;
        
        data <<=(static_cast<ints>(1));
        data |=lefts;
        
        if(bits < maxbit){
            data &= ~(static_cast<ints>(1)<< (bits));
        }
        data <<=(maxbit-bits);
        return data;
    }
    
    // DES Feistel Enchiper
    void feistel_encipher(){
        for(int i = 0; i<plaintext_block; i++){
            *(plaintext_bin+i) = sub_feistel_encipher(*(plaintext_bin+i));
        }
    }
    ints sub_feistel_encipher(ints data){
        ints L, R;
        L = (data>>(64-32))<<(64-32);
        R = (data>>(64-64))<<(64-32);
        for( int i = 0; i < 16; i++){
            ints Rnew = L^f_function(R, i);
            L = R;
            R = Rnew;
        }
        //SWAP - Feistel
        ints tempL = L;
        L = R;
        R = tempL;
        ints newdata = L | (R>>32);
        return newdata;
    }

    ints expand32_48(ints data){
        ints data_size = 64;
        ints newdata = 0;
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 6; j++){
                data_size--;
                ints bindata = (data >> (64-expand[i][j])) &0b1;
                newdata = newdata|(bindata<<data_size);
            }
        }
        return newdata;
    }

    ints sboxing(ints data){
        ints newdata = 0;
        for(int i = 0; i < 8; i++){
            ints tempdata = data<<(6*i);
            tempdata >>=(58);
            uint8_t x = ((tempdata >> 5 &0b1) << 1) | (tempdata&0b1);
            uint8_t y = (tempdata&~(0b1 << 5)) >> 1;
            newdata <<= (4);
            newdata |= sbox[i][x][y];
        }
        return newdata<<32;
    }

    ints pboxing(ints data){
        ints data_size = 64;
        ints newdata = 0;
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 4; j++){
                data_size--;
                ints bindata = (data >> (64-pbox[i][j])) &0b1;
                newdata = newdata|(bindata<<data_size);
            }
        }
        return newdata;
    }

    ints f_function(ints data, ints kn){
        // printf("data start\t: "); printbin(data,64); printf("\n");
        data = expand32_48(data);
        // printf("data 32-48\t: "); printbin(data,64); printf("\n");
        data = data^keys[kn];
        // printf("data XOR key-n\t: "); printbin(data,64); printf("\n");
        data = sboxing(data);
        // printf("data SBOX key-n\t: "); printbin(data,64); printf("\n");
        data = pboxing(data);
        // printf("data PBOX key-n\t: "); printbin(data,64); printf("\n");
        return data;
    }

    
    // DES Feistel Decipher
    void feistel_decipher(){
        for(int i = 0; i<plaintext_block; i++){
            *(plaintext_bin+i) = sub_feistel_decipher(*(plaintext_bin+i));
        }
    }
    ints sub_feistel_decipher(ints data){
        ints L, R;
        L = (data>>(64-32))<<(64-32);
        R = (data>>(64-64))<<(64-32);
        for( int i = 15; i >= 0; i--){
            ints Rnew = L^f_function(R, i);
            L = R;
            R = Rnew;

        }
        //SWAP - Feistel
        ints tempL = L;
        L = R;
        R = tempL;
        ints newdata = L | (R>>32);
        return newdata;
    }

    // DES Final Permutation
    void final_permutation(){
        for(int i = 0; i<plaintext_block; i++){
            *(plaintext_bin+i) = sub_final_permutation(*(plaintext_bin+i));
        }
    }
    
    ints sub_final_permutation(ints data){
        ints newdata = 0;
        ints data_size = 64;
        //final permutation
        for (int i = 0, num = 40; i < 8; i++, num--) {
            ints pum = num;
            for(int j = 0; j < 8; j++){
                data_size--;
                ints bindata = (data >> (64-pum)) & 0b1;
                newdata = newdata|(bindata<<data_size);
                pum = (j&0b1) ?pum + 40 : pum-32 ;
            }
        }
        return newdata;
    }

    //UTILS
    void printbin(ints data, ints bits){
        // printf("bin\t: ");
        for(int i = 0; i < bits; i++){
            if(i%8 == 0 && i > 0) printf(" ");
            printf("%u", (uint8_t)((data >> (bits-i-1))&0b1));
        }
    }
    void printhex(ints data, ints size){
        data >>=size;
        std::cout << std::setfill('0')<<std::setw(16) << std::hex;
        std::cout << data;
        std::cout << std::dec;
    }
    
    //RESETS
    void reset_all(){
        free(this->plaintext_bin);
        this->key = 0u;
        this->plaintext_block = 0u;
        memset(this->keys, 0, sizeof(this->keys));
        printf("\n[RESET] ALL ATTRIBUTE\n");
        
    }
    
    //DES PRINT BINARY
    void print_block_data(std::string state){
        printf("\n@@ DATA in block state - %s\n", state.c_str());
        printf("     hex data\t\t:\t\tbin data\n");
        for(int i = 0; i < plaintext_block; i++){
            printhex(*(plaintext_bin+i), 64);printf("\t: ");printbin(*(plaintext_bin+i), 64);printf("\n");
        }
    }
    void print_block_data_bin(std::string state){
        printf("@@ binary in block state - %s\n", state.c_str());
        printf("     uint 64 bit\t:\t\tbinary data\n");
        for(int i = 0; i < plaintext_block; i++){
            printf("%lu\t: ", *(plaintext_bin+i));
            for(int j = 0; j<64; j++){
                if(j>0 && j%8==0)printf(" ");
                // uint16_t data = (datas[i][j] >> (15-k)) & 0b1;
                printf("%u", (uint8_t) (*(plaintext_bin+i)>>(63-j))&0b1);
            }printf("\n");
        }
    }
    void print_block_data_hex(std::string state){
        printf("@@ HEX in block state - %s\n", state.c_str());
        printf("     uint 64 bit\t:\thex data\n");
        for(int i = 0; i < plaintext_block; i++){
            printf("%lu\t: ", *(plaintext_bin+i));
            printhex(*(plaintext_bin+i), 64);printf("\n");
        }
    }
    
    void print_result_hex(){
        printf("\nHEX combined Result\t:\n");
        for(int i = 0; i < plaintext_block; i++){
            printhex(*(plaintext_bin+i), 64);
        }printf("\n");
    }
    void print_result_ascii(){
        for(int i = 0; i < plaintext_block; i++){
            for(int j = 0; j < 8; j++){
                printf("%c", (char)(plaintext_bin[i] >> (56-(8*j))));
            }
        }printf("\n");
    }
    void print_result_asciiInt(ints data){
        for(int j = 0; j < 8; j++){
            printf("%c", (char)(data >> (56-(8*j))));
        }
    }

    private:
    const uint8_t init_perm[8][8] = {
        {58, 50, 42, 34, 26, 18, 10, 2},
        {60, 52, 44, 36, 28, 20, 12, 4},
        {62, 54, 46, 38, 30, 22, 14, 6},
        {64, 56, 48, 40, 32, 24, 16, 8},
        {57, 49, 41, 33, 25, 17, 9, 1},
        {59, 51, 43, 35, 27, 19, 11, 3},
        {61, 53, 45, 37, 29, 21, 13, 5},
        {63, 55, 47, 39, 31, 23, 15, 7}
    };
    const uint8_t pc1[8][7]={
        {57, 49, 41, 33, 25, 17, 9},
        {1, 58, 50, 42, 34, 26, 18},
        {10, 2, 59, 51, 43, 35, 27},
        {19, 11, 3, 60, 52, 44, 36},
        {63, 55, 47, 39, 31, 23, 15},
        {7, 62, 54, 46, 38, 30, 22},
        {14, 6, 61, 53, 45, 37, 29},
        {21, 13, 5, 28, 20, 12, 4}
    };
    const uint8_t pc2[8][6]={
        {14, 17, 11, 24, 1, 5},
        {3, 28, 15, 6, 21, 10},
        {23, 19, 12, 4, 26, 8},
        {16, 7, 27, 20, 13, 2},
        {41, 52, 31, 37, 47, 55},
        {30, 40, 51, 45, 33, 48},
        {44, 49, 39, 56, 34, 53},
        {46, 42, 50, 36, 29, 32}
    };
    const ints expand[8][6]={
    {32,1,2,3,4,5},
    {4,5,6,7,8,9},
    {8,9,10,11,12,13},
    {12,13,14,15,16,17},
    {16,17,18,19,20,21},
    {20,21,22,23,24,25},
    {24,25,26,27,28,29},
    {28,29,30,31,32,1}
    };

    const ints sbox[8][4][16]={
        {{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
        
        {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
        {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
        {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
        {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
        
        {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
        {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
        {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
        {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
        
        {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
        {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
        {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
        {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
        
        {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
        {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
        {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
        {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
        
        {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
        {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
        {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
        {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
        
        {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
        {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
        {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
        {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
        
        {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
        {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
        {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
        {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}
    };

    const ints pbox[8][4] = {
        {16,  7, 20, 21},
        {29, 12, 28, 17},
        {1, 15, 23, 26},
        {5, 18, 31, 10},
        {2,  8, 24, 14},
        {32, 27,  3,  9},
        {19, 13, 30,  6},
        {22, 11,  4, 25}
    };
};
