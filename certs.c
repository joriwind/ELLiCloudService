//#define HAVE_ECC
//#define WOLFSSL_CERT_GEN

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <stdio.h>                  /* standard in/out procedures */
#include <stdlib.h>                 /* defines system calls */
#include <string.h>                 /* necessary for memset */
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/asn.h>
#include <wolfssl/wolfcrypt/asn_public.h>
#include <wolfssl/wolfcrypt/ecc.h>

static char* eccCaKeyFile = "./certs/ca-key.der";
static char* eccCaCertFile = "./certs/ca-cert.pem";

int main(char** args);
int gen_cert(char* request, char* certificate);
int gen_key(char* keyFile);

int main(char** args){
   int ret;
   char* certName = "./certs/cert.pem";
   
   //ret = wolfSSL_KeyPemToDer(eccCaKeyFile, 
   
   ret = gen_cert("./certs/cert-req.pem", certName);
   if(ret != 0){
      printf("Error in main: %i\n", ret);
      return -1;
   }
   printf("Succeeded\n");
   return 0;
}

int gen_cert(char* request, char* certificate){
   
// Certificate Generation and signing by CA certificate
   RNG rng;
   
   ecc_key     caKey;
   Cert        myCert;
   //Cert        certReq;
   byte        derCert[4096];
   byte        pemCert[4096];
   FILE*       derFile;
   FILE*       pemFile;
   int         certSz;
   int         ret;

   byte        tmp[2048];
   byte        byte_certReq[2048];
   size_t      bytes;
   word32      idx = 0;
   
   
   
   FILE*  file = fopen(eccCaKeyFile, "rb");

   if (!file)
      return -412;

   bytes = fread(tmp, 1, sizeof(tmp), file);


   wc_ecc_init(&caKey);  
   ret = wc_EccPrivateKeyDecode(tmp, &idx, &caKey, (word32)bytes);
   if (ret != 0) 
   {
      printf("Problem decoding private key: %i\n", ret);    
      return -413;
   }
   
   ret = wc_InitRng(&rng);
   if(ret != 0){
      return -1;
   }
   
   
   /*FILE* file_certReq;
   file_certReq = fopen(request,"rb");
   
   if (!file)
      return -412;
   bytes = fread(byte_certReq, 1, sizeof(byte_certReq), file_certReq);*/
   
   //InitDecodedCert(certReq, byte_certReq, (word32)bytes, 0);
   wc_InitCert(&myCert);
   myCert.sigType = CTC_SHAwECDSA;

   strncpy(myCert.subject.country, "FI", CTC_NAME_SIZE);
   strncpy(myCert.subject.state, "Espoo", CTC_NAME_SIZE);
   strncpy(myCert.subject.locality, "Otanimie", CTC_NAME_SIZE);
   strncpy(myCert.subject.org, "", CTC_NAME_SIZE);
   strncpy(myCert.subject.unit, "", CTC_NAME_SIZE);
   strncpy(myCert.subject.commonName, "", CTC_NAME_SIZE);
   strncpy(myCert.subject.email, "", CTC_NAME_SIZE);
   
   myCert.daysValid = 1000;

   ret = wc_SetIssuer(&myCert,eccCaCertFile);
   if(ret < 0)
      printf("Problem setting issuer..\n");
   ret = wc_SetSubject(&myCert,request);
   if(ret < 0)
      printf("Problem setting subject..\n");

   certSz = wc_MakeCert(&myCert, derCert, sizeof(derCert),NULL,  &caKey, &rng); 
   if (certSz < 0)
   {
      printf("Problem with Make cert\n");            
      return -407;
   }

   certSz = wc_SignCert(myCert.bodySz, myCert.sigType, derCert, sizeof(derCert), NULL, &caKey, &rng);
   if (certSz < 0)
   {
   printf("Problem with signing certificate\n");            
   return -408;
   }

   int someSz = -1;
   someSz = wc_DerToPem(derCert,certSz,pemCert,sizeof(pemCert),CERT_TYPE);
   if (someSz < 0)
   {
      printf("Problem in conversion from DER to PEM\n");            
      return -409;
   }else{
   printf("Size is %d\n",someSz);
   }
   
   FILE* certfp;
   certfp = fopen(certificate,"w+");
   fwrite(pemCert,1,sizeof(pemCert),certfp);

   fclose(certfp);

   //certfp = fopen(certName + ".der","w+");
   //fwrite(derCert,1,sizeof(derCert),certfp);

   //fclose(certfp);
   
   printf("Cert has been generated\n");
   
   return 0;


}

int gen_key(char* keyFile){
   // Key generation
   ecc_key genkey;
   RNG rng;
   int ret;
   FILE* fp;

   ret = wc_InitRng(&rng);
   if(ret != 0){
      return -1;
   }
   wc_ecc_init(&genkey);

   ret = wc_ecc_make_key(&rng,32,&genkey);
   if(ret != 0){
      return -1;
   }
   
   byte der[4096];
   byte pem[4096];

   int derSz = wc_EccKeyToDer(&genkey,der,sizeof(der));

   if(derSz < 0)
     printf("DER error\n");

   int pemSz = wc_DerToPem(der,derSz,pem,sizeof(pem),PRIVATEKEY_TYPE);

   if(pemSz < 0)
     printf("PEM error\n");

   fp = fopen(keyFile,"w+");

   fwrite(pem,1,sizeof(pem),fp);

   fclose(fp);

   //fp = fopen(keyName + ".der","w+");

   //fwrite(der,1,sizeof(der),fp);

   //fclose(fp);

   printf("Now time for certificate..\n");
   
   return 0;
}