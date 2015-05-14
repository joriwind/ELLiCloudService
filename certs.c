//#define HAVE_ECC
//#define WOLFSSL_CERT_GEN
#define WOLFSSL_CERT_REQ

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
int gen_cert_req(Cert* request, char* req_loc, char* key_loc);
int sign_csr(Cert* req, char* der_loc, char* cert_loc);
int gen_certFromReq(char* req_loc, char* cert_loc);

int main(char** args){
   int ret;
   char* certName = "./certs/cert.pem";
   Cert* req;
   Cert request;
   req = &request;
   
   //ret = wolfSSL_KeyPemToDer(eccCaKeyFile, 
   printf("Start of main\n");
   //ret = gen_cert("./certs/cert-req.pem", certName);
   ret = gen_cert_req(req, "./certs/test_req", "./certs/server-key.der");
   if(ret != 0){
      printf("Error in main gen_cert_req: %i\n", ret);
      return -1;
   }else{
      printf("Created req\n");
   }
   
   //ret = sign_csr(req, "./certs/test_req.der", "./certs/test_cert");
   ret = gen_certFromReq("./certs/test_req.der", "./certs/test_cert");
   if(ret != 0){
      printf("Error in main gen_certFromReq: %i\n", ret);
      return -1;
   }else{
      printf("Signed\n");
   }
   printf("Succeeded\n");
   return 0;
}

int gen_certFromReq(char* req_loc, char* cert_loc){
   
   DecodedCert certDecode;
   byte tmp[4096];
   byte der[4096];
   byte pem[4096];
   int derSz;
   int pemSz;
   int bytes;
   Cert myCert;
   ecc_key key;
   ecc_key caKey;
   word32      idx = 0;
   int ret;
   char       location[50];
   
   FILE* file;
   
   RNG rng;
   
   /* RND number */
   ret = wc_InitRng(&rng);
   if(ret != 0){
      return -1;
   }
   
   /* Read and parse CSR */
   file = fopen(req_loc, "rb");
   if (!file) {
     return -49;
   }

   bytes = fread(tmp, 1, sizeof(tmp), file);
   fclose(file);
   
   InitDecodedCert(&certDecode, tmp, (word32)bytes, 0);
   //Parsing CSR
   ret = ParseCert(&certDecode, CERTREQ_TYPE, NO_VERIFY, 0);
   if (ret != 0){
      printf("Error: %i", ret);
      return -491;
   }
   
   //Retrieve pub key
   ret = wc_EccPrivateKeyDecode(certDecode.publicKey, &idx, &key, certDecode.pubKeySize); //Did not work because only private key decode and need public key for info
   if (ret != 0) 
   {
      printf("Problem decoding private key: %i\n", ret);    
      return -413;
   }
   
   /* Read CA key */
   file = fopen(eccCaKeyFile, "rb");

   if (!file)
      return -412;

   bytes = fread(tmp, 1, sizeof(tmp), file);
   fclose(file);


   wc_ecc_init(&caKey);  
   ret = wc_EccPrivateKeyDecode(tmp, &idx, &caKey, (word32)bytes);
   if (ret != 0) 
   {
      printf("Problem decoding private key: %i\n", ret);    
      return -413;
   }
   
   /* Create certificate */
   wc_InitCert(&myCert);
   myCert.sigType = CTC_SHAwECDSA;

   strncpy(myCert.subject.country, certDecode.subjectC, CTC_NAME_SIZE);
   strncpy(myCert.subject.state, certDecode.subjectST, CTC_NAME_SIZE);
   strncpy(myCert.subject.locality, certDecode.subjectL, CTC_NAME_SIZE);
   
   myCert.daysValid = 1000;

   ret = wc_SetIssuer(&myCert,eccCaCertFile);
   if(ret < 0)
      printf("Problem setting issuer..\n");
   
   derSz = wc_MakeCert(&myCert, der, sizeof(der),NULL,  &key, &rng); 
   if (derSz < 0)
   {
      printf("Problem with Make cert\n");            
      return -407;
   }
   
   derSz = wc_SignCert(myCert.bodySz, myCert.sigType, der, sizeof(der), NULL, &caKey, &rng);
   if (derSz < 0)
   {
      printf("Problem with signing certificate\n");            
      return -408;
   }
   
   
   strcpy(location, cert_loc);
   strcat(location, ".der");
   
   file = fopen(location, "wb");
   if (!file) {
      return -468;
   }
   
   ret = (int)fwrite(der, 1, derSz, file);
   fclose(file);
   if (ret != derSz) {
      return -471;
   }
   
   pemSz = wc_DerToPem(der,derSz,pem,sizeof(pem),CERT_TYPE);
   if (pemSz < 0)
   {
      printf("Problem in conversion from DER to PEM\n");            
      return -409;
   }else{
   printf("Size is %d\n",pemSz);
   }
   
   strcpy(location, cert_loc);
   strcat(location, ".pem");
   file = fopen(location, "wb");
   if (!file) {
      return -469;
   }
   ret = (int)fwrite(pem, 1, pemSz, file);
   fclose(file);
   if (ret != pemSz) {
      return -470;
   }
   
   //Free
   FreeDecodedCert(&certDecode);
   return 0;
}

int gen_cert_req(Cert* request, char* req_loc, char* key_loc){
   //Cert        req;
   byte        der[4096];
   byte        pem[4096];
   int         derSz;
   int         pemSz;
   FILE*       reqFile;
   char       location[50];
   int         ret;
   
   ecc_key     key;
   word32      idx = 0;
   byte        tmp[2048];
   size_t      bytes;
   
   RNG rng;
   
   /* RND number */
   ret = wc_InitRng(&rng);
   if(ret != 0){
      return -1;
   }
   
   /* key read*/
   /*FILE*  file = fopen(key_loc, "rb");

   if (!file)
      return -412;

   bytes = fread(tmp, 1, sizeof(tmp), file);*/
   
   wc_ecc_init(&key);  
   /*ret = wc_EccPrivateKeyDecode(tmp, &idx, &key, (word32)bytes); //Did not work because only private key decode and need public key for info
   if (ret != 0) 
   {
      printf("Problem decoding private key: %i\n", ret);    
      return -413;
   }*/
   
   ret = wc_ecc_make_key(&rng,32,&key);
   if(ret != 0){
      return -1;
   }
   
   derSz = wc_EccKeyToDer(&key,der,sizeof(der));

   if(derSz < 0)
     printf("DER error\n");

   pemSz = wc_DerToPem(der,derSz,pem,sizeof(pem),PRIVATEKEY_TYPE);

   if(pemSz < 0)
     printf("PEM error\n");
   
   strcpy(location, key_loc);
   strcat(location, ".pem");
   
   FILE* fp = fopen(location,"w+");

   fwrite(pem,1,sizeof(pem),fp);

   fclose(fp);
   
   
   strcpy(location, key_loc);
   strcat(location, ".der");
   
   fp = fopen(location,"w+");
   
   fwrite(der,1,sizeof(der),fp);

   fclose(fp);

   printf("Now time for certificate..\n");
   
   
   
   /*Cert*/
   wc_InitCert(request);

   request->version = 0;
   request->isCA    = 1;
   strncpy(request->challengePw, "", CTC_NAME_SIZE);
   strncpy(request->subject.country, "FI", CTC_NAME_SIZE);
   strncpy(request->subject.state, "Espoo", CTC_NAME_SIZE);
   strncpy(request->subject.locality, "Otanimie", CTC_NAME_SIZE);
   strncpy(request->subject.org, "", CTC_NAME_SIZE);
   strncpy(request->subject.unit, "", CTC_NAME_SIZE);
   strncpy(request->subject.commonName, "", CTC_NAME_SIZE);
   strncpy(request->subject.email, "", CTC_NAME_SIZE);
   request->sigType = CTC_SHAwECDSA;

   derSz = wc_MakeCertReq(request, der, sizeof(der), NULL, &key);
   if (derSz < 0) {
      return -465;
   }
   
   derSz = wc_SignCert(request->bodySz, request->sigType, der, sizeof(der),
                    NULL, &key, &rng);
   if (derSz < 0) {
      return -466;
   }

   pemSz = wc_DerToPem(der, derSz, pem, sizeof(pem), CERTREQ_TYPE);
   if (pemSz < 0) {
      return -467;
   }
   
   strcpy(location, req_loc);
   strcat(location, ".der");
   
   reqFile = fopen(location, "wb");
   if (!reqFile) {
      return -468;
   }
   
   ret = (int)fwrite(der, 1, derSz, reqFile);
   fclose(reqFile);
   if (ret != derSz) {
      return -471;
   }
   
   strcpy(location, req_loc);
   strcat(location, ".pem");
   reqFile = fopen(location, "wb");
   if (!reqFile) {
      return -469;
   }
   ret = (int)fwrite(pem, 1, pemSz, reqFile);
   fclose(reqFile);
   if (ret != pemSz) {
      return -470;
   }
   
   printf("Succesfully created cert_req\n");
   
   return 0;
}

int sign_csr(Cert* req, char* der_loc, char* cert_loc){
   RNG rng;
   
   FILE*       reqFile;
   byte        der[4096];
   byte        pem[4096];
   int         derSz;
   int         pemSz;
   char       location[50];
   
   
   ecc_key     ca_key;
   word32      idx = 0;
   byte        tmp[2048];
   size_t      bytes;
   
   /* key read*/
   FILE*  file_der = fopen(der_loc, "rb");

   if (!file_der)
      return -412;

   bytes = fread(der, 1, sizeof(der), file_der);
   
   /* key read*/
   FILE*  file_key = fopen(eccCaKeyFile, "rb");

   if (!file_key)
      return -412;

   bytes = fread(tmp, 1, sizeof(tmp), file_key);
   
   wc_ecc_init(&ca_key);  
   int ret = wc_EccPrivateKeyDecode(tmp, &idx, &ca_key, (word32)bytes);
   if (ret != 0) 
   {
      printf("Problem decoding private key: %i\n", ret);    
      return -413;
   }
   
   
   /* RND number */
   ret = wc_InitRng(&rng);
   if(ret != 0){
      return -1;
   }
   
   
   int requestSz = req->bodySz;
   int sType = req->sigType;
   
   derSz = wc_SignCert(requestSz, sType, der, sizeof(der),
                          NULL, &ca_key, &rng);
   if (derSz < 0) {
      return -466;
   }
   
   strcpy(location, cert_loc);
   strcat(location, ".der");
   
   reqFile = fopen(location, "wb");
   if (!reqFile) {
      return -468;
   }
   
   ret = (int)fwrite(der, 1, derSz, reqFile);
   fclose(reqFile);
   if (ret != derSz) {
      return -471;
   }
   
   pemSz = wc_DerToPem(der,derSz,pem,sizeof(pem),CERT_TYPE);
   if (pemSz < 0)
   {
      printf("Problem in conversion from DER to PEM\n");            
      return -409;
   }else{
   printf("Size is %d\n",pemSz);
   }
   
   strcpy(location, cert_loc);
   strcat(location, ".pem");
   reqFile = fopen(location, "wb");
   if (!reqFile) {
      return -469;
   }
   ret = (int)fwrite(pem, 1, pemSz, reqFile);
   fclose(reqFile);
   if (ret != pemSz) {
      return -470;
   }
      
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