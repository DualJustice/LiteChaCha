#ifndef CIPHERSETUP_H
#define CIPHERSETUP_H

#include "rng.h"


class CipherSuiteSetup {
private:
	RNGen rand;

	char dumb[6] = {9, 9, 9, 9, 9, 9};
	unsigned short dumber = 42;
public:
	CipherSuiteSetup();
	~CipherSuiteSetup();

	void generateRandomKey();

	void generateRandomUserID(); // Used as the fixed nonce in ChaCha.

	bool compareIDs();
};


CipherSuiteSetup::CipherSuiteSetup() {

}


CipherSuiteSetup::~CipherSuiteSetup() {

}


void CipherSuiteSetup::generateRandomKey() {
	rand.generateBytes(dumb, dumber);
}

#endif