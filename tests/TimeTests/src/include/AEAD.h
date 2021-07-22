#ifndef AEAD_H
#define AEAD_H

#include "chacha.h"
#include "poly1305.h"

#include <stdint.h>


class AEADConstruct {
private:
	ChaChaEncryption cipher;
	Poly1305MAC mac;


public:
	AEADConstruct();
	~AEADConstruct();

	void encryptAndTagMessage();
	bool messageAuthentic();
	void decryptAuthenticatedMessage();
};


AEADConstruct::AEADConstruct() {

}


AEADConstruct::~AEADConstruct() {

}




#endif