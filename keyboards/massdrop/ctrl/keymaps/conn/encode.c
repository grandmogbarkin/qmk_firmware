#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int modulo(int n,int M){
    return ((n % M) + M) % M;
}

int main(){
    char msg[256]; char key[33];

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

	return 0;
}
