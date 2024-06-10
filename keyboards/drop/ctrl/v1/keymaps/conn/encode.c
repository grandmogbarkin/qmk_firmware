#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "secrets.h"

int modulo(int n,int M){
    return ((n % M) + M) % M;
}

int main(){
    char msg[256]; char key[33]; char dec_map[100];

    const int u_len = 95;
    const int u_base = 32;

    printf("Please Enter Message to be encrypted\n");//text to be encrypted

    scanf("%s",msg);

    printf("Please Enter keyword\n"); // key word

    scanf("%s",key);

    int msgLen = strlen(msg), keyLen = strlen(key), i, j;

    char encryptedMsg[msgLen], decryptedMsg[msgLen];

    //encryption
    printf("Encrypting..\n");
    for(i = 0; i < msgLen; ++i) {
        encryptedMsg[i] = ((msg[i]-u_base + key[i%keyLen]-u_base) % u_len) + u_base;
        //printf("%d + %d %% %d + %d = %c\n", msg[i]-u_base,key[i%keyLen]-u_base, u_len, u_base, encryptedMsg[i]);
    }

    encryptedMsg[i] = '\0';

    //decryption
    printf("Decrypting..\n");
    for(i = 0; i < msgLen; ++i) {
        decryptedMsg[i] = modulo(((encryptedMsg[i]-u_base) - (key[i%keyLen]-u_base)), u_len) + u_base;
        //printf("%d - %d = %d %% %d + %d = %d\n", encryptedMsg[i]-u_base,key[i%keyLen]-u_base, (encryptedMsg[i]-u_base) - (key[i%keyLen]-u_base), u_len, u_base, decryptedMsg[i]);
    }

    decryptedMsg[i] = '\0';

    printf("Original Message: %s", msg);
    printf("\nKey: %s", key);
    printf("\nEncrypted Message: %s", encryptedMsg);
    printf("\nDecrypted Message: %s", decryptedMsg);

    int dec_len = (msgLen+3)/4;
    int map_dec_len = msgLen/4;
    printf("\nPlease Enter key code (%d chars)\n", dec_len); // key word

    scanf("%s",dec_map);

    int decLen = strlen(dec_map);

    for(i = 0; i < map_dec_len; ++i) {
        int encJ = 0;
        for(j = 0; j < 4; ++j, ++encJ) {
            if (encryptedMsg[(i*4)+j] == '"' || encryptedMsg[(i*4)+j] == '\\') {
                keycode_vignere_enc[dec_map[i] - 33].str[encJ++] = '\\';
            }
            keycode_vignere_enc[dec_map[i] - 33].str[encJ] = encryptedMsg[(i*4)+j];
        }
    }

    // pretty print keycode_vignere_enc in its entirety so that I can copy/paste it back into secrets.h
    for(i = 0; i < 95; ++i) {
        printf("{\"");
        for(j = 0; j < 4; ++j) {
            if (keycode_vignere_enc[i].str[j] == '"' || keycode_vignere_enc[i].str[j] == '\\') {
                printf("\\%c", keycode_vignere_enc[i].str[j]);
            } else {
                printf("%c", keycode_vignere_enc[i].str[j]);
            }
        }
        printf("\"}, ");
    }



    int msg_len = dec_len * 4;
    char finalMsg[msg_len+1];
    int final_len;
    for (final_len = 0, j = 0; j < dec_len; ++j) {
        int kMax = 4;
        if (dec_map[j]-33 >= 15 && dec_map[j]-33 <= 18) {
            kMax = 2;
        } else if (dec_map[j]-33 >= 19 && dec_map[j]-33 <=24) {
            kMax = 1;
        }
        for (int k = 0; k < kMax; ++k) {
            finalMsg[(j*4)+k] = keycode_vignere_enc[dec_map[j] - 33].str[k];
            ++final_len;
        }
    }
    finalMsg[final_len] = '\0';
    // print finalMsg
    printf("\nDecoded Message: %s\n", finalMsg);
    if (strcmp(finalMsg, encryptedMsg) == 0) {
        printf("Final encoding matches original encoding\n");
    } else {
        printf("Final encoding DOES NOT MATCH original encoding!\n");
    }

	return 0;
}
